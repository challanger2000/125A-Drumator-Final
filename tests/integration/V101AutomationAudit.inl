// Automated VST3 mix controls: metadata, Controller/host edits, real
// Processor PCM with sample-offset host points and no callback allocations.
// Pitch is intentionally excluded until its filter-bank RT design is proven.
struct AutoHostRecorder final : Steinberg::Vst::IComponentHandler {
 unsigned beginCount=0,endCount=0,performCount=0;
 Steinberg::Vst::ParamID lastId=0;
 Steinberg::Vst::ParamValue lastValue=-1;
 tresult PLUGIN_API queryInterface(const TUID interfaceId,void** out) override {
   if(!out)return kInvalidArgument;
   *out=nullptr;
   if(FUnknownPrivate::iidEqual(interfaceId,FUnknown::iid)||
      FUnknownPrivate::iidEqual(interfaceId,IComponentHandler::iid)){
     *out=static_cast<IComponentHandler*>(this);addRef();return kResultOk;
   }
   return kNoInterface;
 }
 uint32 PLUGIN_API addRef() override{return 1;}
 uint32 PLUGIN_API release() override{return 1;}
 tresult PLUGIN_API beginEdit(ParamID) override{++beginCount;return kResultOk;}
 tresult PLUGIN_API performEdit(ParamID id,ParamValue value) override{
   ++performCount;lastId=id;lastValue=value;return kResultOk;
 }
 tresult PLUGIN_API endEdit(ParamID) override{++endCount;return kResultOk;}
 tresult PLUGIN_API restartComponent(int32) override{return kResultOk;}
};

void testV101HostMixAutomationContract(){
 using namespace drumator;
 AutoHostRecorder recorded;
 Fixture f;
 check(f.c->setComponentHandler(&recorded)==kResultOk,
       "Automation host handler rejected");
 check(f.c->getParameterCount()==50,
       "Automation expected Pattern+16 Volume+16 Pan+16 Mute+Master = 50");
 std::array<bool,kAutomatedMixLanes> found{};
 bool patternFound=false;
 for(int32 i=0;i<f.c->getParameterCount();++i){
   ParameterInfo info{};
   check(f.c->getParameterInfo(i,info)==kResultOk,
         "Automation parameter info failed");
   if(info.id==kPatternParam){patternFound=true;continue;}
   const auto a=automationAddress(info.id);
   const auto lane=automationLane(a);
   check(lane<kAutomatedMixLanes&&!found[lane]&&
         (info.flags&ParameterInfo::kCanAutomate)!=0,
         "Automation missing/duplicate/non-automatable parameter ID");
   found[lane]=true;
   check(info.stepCount==(a.kind==AutomationKind::Mute?1:0),
         "Automation mute not discrete or mix not continuous");
   const double expected=a.kind==AutomationKind::Mute?0.0:0.5;
   check(std::abs(info.defaultNormalizedValue-expected)<1e-8,
         "Automation parameter default mismatch");
 }
 check(patternFound&&std::all_of(found.begin(),found.end(),[](bool x){return x;}),
       "Automation missing row or master parameter");
 check(f.c->adjustRowMix(0,false,25)&&
       recorded.lastId==kRowVolumeBase&&
       std::abs(recorded.lastValue-.625)<1e-8&&
       f.c->rowGain(0)==125,
       "Automation GUI Volume did not publish host performEdit");
 check(f.c->adjustRowMix(15,true,30)&&
       recorded.lastId==kRowPanBase+15&&
       f.c->rowPan(15)==30,
       "Automation GUI Pan row 16 not recorded");
 check(f.c->toggleRowGate(1,false)&&
       recorded.lastId==kRowMuteBase+1&&recorded.lastValue==1.0&&
       f.c->isRowMuted(1),
       "Automation GUI Mute did not publish discrete host state");
 check(f.c->adjustMasterVolume(25)&&
       recorded.lastId==kMasterVolumeParam&&
       f.c->masterVolume()==125,
       "Automation GUI Master did not publish host state");
 check(recorded.beginCount==4&&recorded.endCount==4&&
       recorded.performCount==4,
       "Automation GUI edit gestures not balanced");
 check(f.c->setParamNormalized(kRowVolumeBase+15,.75)==kResultOk&&
       f.c->rowGain(15)==150&&
       f.c->setParamNormalized(kRowPanBase,0.)==kResultOk&&
       f.c->rowPan(0)==-100&&
       f.c->setParamNormalized(kRowMuteBase+1,0.)==kResultOk&&
       !f.c->isRowMuted(1)&&
       f.c->setParamNormalized(kMasterVolumeParam,.25)==kResultOk&&
       f.c->masterVolume()==50,
       "Automation host->Controller value feedback disconnected");
 check(automationAddress(kRowPitchBase).kind==AutomationKind::None,
       "Automation premature unsafe pitch host parameter");
 evidence("AUT01 VST3 50 stable parameters, 16x gain/pan/mute, master, GUI host gestures and feedback PASS");
}

void testV101HostMixAutomationPcm(){
 using namespace drumator;
 // Never rely on a mock renderer: actual Controller+Processor, real
 // embedded stereo WAV, and real VST3 ParameterChanges queues.
 const auto makeBlock=[&](AuditAudioBlock& b,EventList& notes){
   Event hit{};hit.type=Event::kNoteOnEvent;
   hit.noteOn.pitch=36;hit.noteOn.velocity=1.f;hit.noteOn.noteId=-1;
   check(notes.addEvent(hit)==kResultOk,"AUT02 MIDI note-on creation");
   b.data.inputEvents=&notes;
 };
 {
   Fixture f;
   KitState kit{};kit.visible=1;kit.grid.fill(0);
   kit.names[0]="host-autom.wav";kit.wav[0]=constantStereo(.5f,-.25f);
   f.kit(kit);prepareAudio(*f.p);
   AuditAudioBlock b;EventList notes;makeBlock(b,notes);
   ParameterChanges changed;int32 index=0;
   changed.addParameterData(kRowPanBase,index)->addPoint(64,1.,index);
   changed.addParameterData(kRowMuteBase,index)->addPoint(176,1.,index);
   b.data.inputParameterChanges=&changed;
   allocationFail=0;
   const auto processed=f.p->process(b.data);
   allocationFail=-1;
   check(processed==kResultOk,"AUT02 host Pan/Mute PCM process/RT allocation");
   check(std::abs(b.left[30]-.5f)<1e-5f&&
         std::abs(b.right[30]+.25f)<1e-5f,
         "AUT02 host automation activated before its sample offset");
   check(std::abs(b.left[170])<1e-4f&&
         std::abs(b.right[170]+.25f)<1e-4f,
         "AUT02 Pan 100% did not reach right-only PCM before Mute");
   check(std::abs(b.right[255])<.08f&&
         std::abs(b.right[255])>.001f,
         "AUT02 Mute ramp missing/instant cut after offset 176");
   ResizableMemoryIBStream saved;KitState actual{};
   check(f.p->getState(&saved)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),
                         saved.getCursor(),actual)&&
         actual.pan[0]==200&&(actual.muted&1u)!=0,
         "AUT02 host Pan/Mute not serialized in kit state");
   f.p->setProcessing(false);f.p->setActive(false);
 }
 {
   Fixture f;
   KitState kit{};kit.visible=1;kit.grid.fill(0);
   kit.names[0]="host-vol.wav";kit.wav[0]=constantStereo(.5f,-.25f);
   f.kit(kit);prepareAudio(*f.p);
   AuditAudioBlock b;EventList notes;makeBlock(b,notes);
   ParameterChanges changed;int32 index=0;
   changed.addParameterData(kRowVolumeBase,index)->addPoint(32,0.,index);
   b.data.inputParameterChanges=&changed;
   check(f.p->process(b.data)==kResultOk,"AUT03 host row volume failed");
   check(std::abs(b.left[30]-.5f)<1e-5f&&
         std::abs(b.left[200])<1e-5f,
         "AUT03 host Volume not applied at offset 32 with smooth fade");
   f.p->setProcessing(false);f.p->setActive(false);
 }
 {
   Fixture f;
   KitState kit{};kit.visible=1;kit.grid.fill(0);
   kit.names[0]="host-master.wav";kit.wav[0]=constantStereo(.5f,-.25f);
   f.kit(kit);prepareAudio(*f.p);
   AuditAudioBlock b;EventList notes;makeBlock(b,notes);
   ParameterChanges changed;int32 index=0;
   changed.addParameterData(kMasterVolumeParam,index)->addPoint(64,0.,index);
   b.data.inputParameterChanges=&changed;
   check(f.p->process(b.data)==kResultOk,"AUT04 host Master failed");
   check(std::abs(b.left[30]-.5f)<1e-5f&&
         b.left[180]<b.left[70]&&
         b.left[180]>.01f,
         "AUT04 Master ramp missing or started before offset 64");
   f.p->setProcessing(false);f.p->setActive(false);
 }
 evidence("AUT02-04 actual VST3 host sample points Pan/Mute/Volume/Master, fade PCM, persistence, no audio-thread allocations PASS");
}
