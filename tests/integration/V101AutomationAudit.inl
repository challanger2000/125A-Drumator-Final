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
 check(f.c->getParameterCount()==66,
       "Automation expected Pattern+16 Volume+16 Pan+16 Mute+16 Pitch+Master = 66");
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
   check(info.stepCount==(a.kind==AutomationKind::Mute?1:
                          a.kind==AutomationKind::Pitch?48:0),
         "Automation pitch/mute not discrete or mix not continuous");
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
 check(f.c->adjustRowPitch(2,12)&&
       recorded.lastId==kRowPitchBase+2&&
       f.c->rowPitch(2)==12&&
       std::abs(recorded.lastValue-.75)<1e-8,
       "Automation GUI Pitch not reflected in DAW host");
 check(recorded.beginCount==5&&recorded.endCount==5&&
       recorded.performCount==5,
       "Automation GUI edit gestures not balanced");
 check(f.c->setParamNormalized(kRowVolumeBase+15,.75)==kResultOk&&
       f.c->rowGain(15)==150&&
       f.c->setParamNormalized(kRowPanBase,0.)==kResultOk&&
       f.c->rowPan(0)==-100&&
       f.c->setParamNormalized(kRowMuteBase+1,0.)==kResultOk&&
       !f.c->isRowMuted(1)&&
       f.c->setParamNormalized(kMasterVolumeParam,.25)==kResultOk&&
       f.c->masterVolume()==50&&
       f.c->setParamNormalized(kRowPitchBase+15,0.)==kResultOk&&
       f.c->rowPitch(15)==-24,
       "Automation host->Controller value feedback disconnected");
 check(automationAddress(kRowPitchBase).kind==AutomationKind::Pitch&&
       automationAddress(kRowPitchBase+15).row==15,
       "Automation missing row-specific pitch address");
 evidence("AUT01 VST3 66 stable parameters, 16x gain/pan/mute/pitch, master, GUI host gestures and feedback PASS");
}

// AUT08: field regression for Studio One: host Pitch works, then WAV Editor
// Fade Out APPLY, then automation value and GUI still move but DSP fails.
// Prove the fix by observing the actual rendered source-frame advance.
void testV101PitchSurvivesWaveEditorApply(){
 using namespace drumator;
 Fixture f;
 KitState kit{};kit.visible=1;kit.grid.fill(0);
 kit.names[0]="fade-apply-pitch.wav";
 kit.wav[0]=constantStereo(.5f,-.25f); // 1024 frames, 44.1/48k fixture
 f.kit(kit);prepareAudio(*f.p);
 AuditAudioBlock b;
 EventList notes;Event note{};note.type=Event::kNoteOnEvent;
 note.noteOn.pitch=36;note.noteOn.velocity=1.f;note.noteOn.noteId=-1;
 check(notes.addEvent(note)==kResultOk,"AUT08 MIDI trigger preparation");
 ParameterChanges up;int32 i=0;
 up.addParameterData(kRowPitchBase,i)->addPoint(0,.75,i); // +12 st
 b.data.inputEvents=&notes;b.data.inputParameterChanges=&up;
 check(f.p->process(b.data)==kResultOk&&std::abs(b.left[100]-.5f)<1e-4f,
       "AUT08 baseline +12 host Pitch did not render");
 b.data.inputEvents=nullptr;b.data.inputParameterChanges=nullptr;
 check(f.p->process(b.data)==kResultOk,"AUT08 baseline second block");
 check(f.p->process(b.data)==kResultOk&&std::abs(b.left[100])<1e-5f,
       "AUT08 baseline +12 pitch not twice playback rate");
 ParameterChanges neutral;i=0;
 neutral.addParameterData(kRowPitchBase,i)->addPoint(0,.5,i);
 b.data.inputParameterChanges=&neutral;
 check(f.p->process(b.data)==kResultOk,"AUT08 return host Pitch to 0 st");
 b.data.inputParameterChanges=nullptr;
 const SampleEdit fade{0,1024,0,128,1.f};
 check(f.c->commitSampleEdit(0,fade),"AUT08 WAV Editor Fade Out Apply rejected");
 ResizableMemoryIBStream saved;KitState state{};
 check(f.p->getState(&saved)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),
                       saved.getCursor(),state)&&
       state.sampleEdits[0].fadeOut==128&&state.pitch[0]==24,
       "AUT08 Fade Out Apply failed to save edit and neutral pitch");
 // Real bug: post-Apply auto value becomes +12, but WAV remains at unity.
 b.data.inputEvents=&notes;b.data.inputParameterChanges=&up;
 allocationFail=0;
 const auto result=f.p->process(b.data);
 allocationFail=-1;
 check(result==kResultOk&&std::abs(b.left[100]-.5f)<1e-4f,
       "AUT08 post-Apply +12 realtime allocation/PCM failure");
 b.data.inputEvents=nullptr;b.data.inputParameterChanges=nullptr;
 check(f.p->process(b.data)==kResultOk,"AUT08 post-Apply +12 second block");
 check(f.p->process(b.data)==kResultOk&&
       std::abs(b.left[100])<1e-5f&&std::abs(b.right[100])<1e-5f,
       "AUT08 regression: Wave Editor Apply disabled audible host Pitch automation");
 ResizableMemoryIBStream after;KitState stored{};
 check(f.p->getState(&after)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(after.getData()),
                        after.getCursor(),stored)&&
       stored.sampleEdits[0].fadeOut==128&&stored.pitch[0]==36,
       "AUT08 audio Pitch and Fade Out were not serialized together");
 // The new filter bank must also survive automation back down to -12.
 ParameterChanges down;i=0;
 down.addParameterData(kRowPitchBase,i)->addPoint(0,.25,i);
 b.data.inputEvents=&notes;b.data.inputParameterChanges=&down;
 check(f.p->process(b.data)==kResultOk,"AUT08 post-Apply -12 first block");
 b.data.inputEvents=nullptr;b.data.inputParameterChanges=nullptr;
 check(f.p->process(b.data)==kResultOk,"AUT08 post-Apply -12 second block");
 check(f.p->process(b.data)==kResultOk&&b.left[100]>.45f,
       "AUT08 post-Apply -12 should lengthen audible sample");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("AUT08 Studio One sequence: Fade Out APPLY preserves +12/-12 Pitch PCM, state and allocation-free callback PASS");
}

// Codex AV01/AV02/AV03/AV04/AV05 release-blocker regressions.
// Each test checks a real state transition or PCM value, not a label.
void testV101AutomationAuditAV01HighRateRecall(){
 using namespace drumator;
 auto highRate=constantStereo(.5f,-.25f);
 const auto write32=[&](std::size_t pos,std::uint32_t v){
   for(unsigned i=0;i<4;++i)highRate[pos+i]=static_cast<std::uint8_t>(v>>(8*i));
 };
 write32(24,192000);write32(28,192000*8);
 KitState kit{};kit.visible=1;kit.grid.fill(0);
 kit.names[0]="192k-pitch24.wav";kit.wav[0]=highRate;kit.pitch[0]=48;
 auto setup44=[](Processor& p){
   std::array<SpeakerArrangement,kRows> outputs{};
   outputs.fill(SpeakerArr::kStereo);
   check(p.setBusArrangements(nullptr,0,outputs.data(),
      static_cast<Steinberg::int32>(outputs.size()))==kResultOk,
      "AV01 16 stereo buses rejected");
   ProcessSetup s{};s.sampleRate=44100.;s.maxSamplesPerBlock=256;
   s.symbolicSampleSize=kSample32;s.processMode=kRealtime;
   check(p.setupProcessing(s)==kResultOk&&
         p.setActive(true)==kResultOk&&p.setProcessing(true)==kResultOk,
         "AV01 +24 192k sample rejected at 44.1k host");
 };
 Fixture f;f.kit(kit);setup44(*f.p);
 ResizableMemoryIBStream saved;
 check(f.p->getState(&saved)==kResultOk,"AV01 cannot save +24 192k project");
 Fixture recalled;setup44(*recalled.p);
 saved.rewind();
 check(recalled.p->setState(&saved)==kResultOk,
       "AV01 recorded high-rate pitch cannot be recalled");
 AuditAudioBlock b;EventList notes;Event note{};
 note.type=Event::kNoteOnEvent;note.noteOn.pitch=36;
 note.noteOn.velocity=1.f;note.noteOn.noteId=-1;
 check(notes.addEvent(note)==kResultOk,"AV01 trigger setup failed");
 b.data.inputEvents=&notes;
 check(recalled.p->process(b.data)==kResultOk&&
       std::abs(b.left[15])>.1f&&
       std::abs(b.left[160])<1e-5f,
       "AV01 recalled 192k pitch did not render/finish at +24");
 check(f.c->commitSampleEdit(0,SampleEdit{0,1024,0,64,1.f}),
       "AV01 Wave Editor Apply rejected previously playing +24 setting");
 f.p->setProcessing(false);f.p->setActive(false);
 recalled.p->setProcessing(false);recalled.p->setActive(false);
 evidence("AV01 192k WAV / 44.1k host / +24 pitch / save-recall PCM / editor Apply PASS");
}

void testV101AutomationAuditAV02RecallGate(){
 using namespace drumator;
 Fixture f;auto old=namedKit('X');f.kit(old);prepareAudio(*f.p);
 auto next=namedKit('Y');next.grid.fill(0);
 next.gain[0]=100;next.pan[0]=100;next.pitch[0]=24;
 next.masterVolume=100;next.muted=0;
 next.wav[0]=constantStereo(.25f,-.125f);
 auto bytes=Fixture::encode(next);
 arm(qa::restoreMidCommit);
 tresult recalled=kResultFalse;
 std::jthread thread([&]{
   qa::ByteStream state(bytes.data(),bytes.size());
   recalled=f.p->setState(&state);
 });
 waitEntered();
 ProcessData flush{};flush.numSamples=0;flush.numOutputs=0;
 flush.symbolicSampleSize=kSample32;
 tresult result=kResultOk;
 // A -> B -> A catches identical-value and ABA loss at the restore gate.
 for(unsigned pass=0;pass<3;++pass) {
   ParameterChanges changes;int32 index=0;
   const auto gain=pass==1?.3:.75;
   const auto pan=pass==1?.25:.8;
   const auto mute=pass==1?0.:1.;
   const auto pitch=pass==1?.5:.75;
   const auto master=pass==1?.85:.25;
   changes.addParameterData(kRowVolumeBase,index)->addPoint(0,gain,index);
   changes.addParameterData(kRowPanBase,index)->addPoint(0,pan,index);
   changes.addParameterData(kRowMuteBase,index)->addPoint(0,mute,index);
   changes.addParameterData(kRowPitchBase,index)->addPoint(0,pitch,index);
   changes.addParameterData(kMasterVolumeParam,index)->addPoint(0,master,index);
   flush.inputParameterChanges=&changes;
   if(f.p->process(flush)!=kResultOk)result=kResultFalse;
 }
 flush.inputParameterChanges=nullptr;releaseProbe();thread.join();
 check(result==kResultOk&&recalled==kResultOk&&!probeTimeout,
       "AV02 gated automation or KitState restore failed");
 ResizableMemoryIBStream state;KitState actual{};
 check(f.p->getState(&state)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(state.getData()),
                        state.getCursor(),actual)&&
       actual.gain[0]==150&&actual.pan[0]==160&&
       (actual.muted&1u)!=0&&actual.pitch[0]==36&&
       actual.masterVolume==50,
       "AV02 gated Volume/Pan/Mute/Pitch/Master were discarded");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("AV02 Kit-Recall gate preserves all five parameter groups and ABA host events PASS");
}

void testV101AutomationAuditAV03PackedMix(){
 using namespace drumator;
 AtomicRowMix mixed;
 mixed.store(100,100);
 std::jthread volume([&]{
   for(int i=0;i<20000;++i)mixed.updateGain(static_cast<std::uint8_t>(i%201));
   mixed.updateGain(150);
 });
 std::jthread pan([&]{
   for(int i=0;i<20000;++i)mixed.updatePan(static_cast<std::uint8_t>(i%201));
   mixed.updatePan(160);
 });
 volume.join();pan.join();
 const auto value=mixed.snapshot();
 check(AtomicRowMix::gainPercent(value)==150&&
       AtomicRowMix::panEncoded(value)==160,
       "AV03 concurrent gain and pan updates lost the other field");
 evidence("AV03 concurrent packed Volume/Pan field changes preserve both values PASS");
}

void testV101AutomationAuditAV04FractionalUnity(){
 using namespace drumator;
 constexpr std::size_t n=2048;
 std::array<float,n> signal{};
 constexpr double pi=3.14159265358979323846;
 for(std::size_t i=0;i<n;++i)
   signal[i]=static_cast<float>(std::sin(2*pi*10000.*i/48000.));
 PitchKernelBank bank;
 check(bank.prepare(48000,48000.),"AV04 sinc bank preparation failed");
 SampleVoiceEngine engine;
 check(engine.setOutputRate(48000.),"AV04 output rate rejected");
 RowSound sound{};
 sound.sample={signal.data(),nullptr,n};
 sound.edit=SampleEdit::reset(static_cast<std::uint32_t>(n));
 sound.pitchBank=&bank;sound.pitchIndex=36;sound.kernel=bank.kernel(36);
 sound.gain=1.f;sound.pan=0.f;
 check(engine.setRow(0,sound),"AV04 pitched voice setup rejected");
 Trigger hit{};hit.row=0;hit.pattern=0;hit.step=0;
 hit.velocity=127;hit.sampleOffset=0;
 std::array<float,128> earlyL{},earlyR{};
 check(engine.render(&hit,1,earlyL.data(),earlyR.data(),128).status==RenderStatus::Ok,
       "AV04 +12 source advance failed");
 check(engine.setRowPitch(0,24),"AV04 glide back to neutral rejected");
 std::array<float,288> glideL{},glideR{};
 check(engine.render(nullptr,0,glideL.data(),glideR.data(),288).status==RenderStatus::Ok,
       "AV04 glide to zero failed");
 // First 128 frames at ratio 2 -> source position 256; after the
 // first 287 glide frames the last source position is 686.5.
 const auto* unity=bank.kernel(24);
 check(unity&&unity->ready(),"AV04 unity kernel missing");
 // A 288-frame pitch glide finishes mathematically at position 686.5.
 // Repeated binary64 additions may place it infinitesimally below 686.5,
 // mapping to phase 511 rather than 512 under the engine's floor quantizer.
 // Both adjacent sinc phases are valid; neither is the old raw-frame bug.
 const std::int64_t center=686;
 const auto first=center-static_cast<std::int64_t>(unity->taps()/2-1);
 const auto interpolate=[&](std::size_t phase) {
   const auto* weights=unity->phase(phase);
   double sum=0.;
   for(std::size_t j=0;j<unity->taps();++j)
     sum+=static_cast<double>(signal[static_cast<std::size_t>(first+j)])*
          static_cast<double>(weights[j]);
   return sum;
 };
 const auto referenceBelow=interpolate(kSincPhases/2-1);
 const auto referenceAbove=interpolate(kSincPhases/2);
 check(std::min(std::abs(glideL[287]-referenceBelow),
                std::abs(glideL[287]-referenceAbove))<2e-4&&
       std::abs(glideL[287]-signal[686])>.02,
       "AV04 neutral glide must use fractional sinc, never raw floor sample");
 evidence("AV04 10-kHz sinus +12 to 0 fractional-unity sine interpolation oracle PASS");
}

void testV101AutomationAuditAV05TouchGesture(){
 using namespace drumator;
 using UI=UiV101Interaction;
 using L=UiV101Layout;
 AutoHostRecorder recorder;Fixture f;prepareAudio(*f.p);
 check(f.c->setComponentHandler(&recorder)==kResultOk,
       "AV05 host handler rejected");
 UI ui(*f.c);
 const auto x=L::knobCenters[0],y=static_cast<double>(L::header)+24.;
 check(ui.down(x,y)==UI::Result::Handled&&
       recorder.beginCount==1&&recorder.endCount==0,
       "AV05 drag did not begin one sustained host gesture");
 check(ui.move(x,y-8.)==UI::Result::Applied&&
       ui.move(x,y-16.)==UI::Result::Applied&&
       recorder.performCount>=2&&recorder.endCount==0,
       "AV05 host gesture ended during held rotary movement");
 check(ui.up()==UI::Result::Handled&&recorder.endCount==1&&
       recorder.beginCount==1,
       "AV05 host gesture did not finish on mouse-up");
 check(ui.down(x,y)==UI::Result::Handled&&recorder.beginCount==2,
       "AV05 second physical drag failed to start");
 ui.cancel();
 check(recorder.endCount==2,
       "AV05 cancelled drag leaked host touch gesture");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("AV05 sustained Touch gesture Down/Move/Pause/Move/Up and Cancel PASS");
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
 // Pitch +12 semitones doubles the source-frame advance rate. The same
 // 1024-frame WAV therefore ends in two 256-frame audio blocks, whereas
 // neutral pitch would still be audible. Check real rendered PCM and state.
 {
   Fixture f;
   KitState kit{};kit.visible=1;kit.grid.fill(0);
   kit.names[0]="host-pitch.wav";kit.wav[0]=constantStereo(.5f,-.25f);
   f.kit(kit);prepareAudio(*f.p);
   AuditAudioBlock b;EventList notes;makeBlock(b,notes);
   ParameterChanges changed;int32 index=0;
   changed.addParameterData(kRowPitchBase,index)->addPoint(0,.75,index);
   b.data.inputParameterChanges=&changed;
   allocationFail=0;
   const auto first=f.p->process(b.data);
   allocationFail=-1;
   check(first==kResultOk&&std::abs(b.left[100]-.5f)<1e-5f,
         "AUT05 pitch bank allocation or audible sample failed");
   b.data.inputEvents=nullptr;b.data.inputParameterChanges=nullptr;
   check(f.p->process(b.data)==kResultOk,
         "AUT05 pitch+12 second block failed");
   check(f.p->process(b.data)==kResultOk&&
         std::abs(b.left[100])<1e-5f&&std::abs(b.right[100])<1e-5f,
         "AUT05 +12 semitone pitch did not double playback rate");
   ResizableMemoryIBStream saved;KitState state{};
   check(f.p->getState(&saved)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),
                          saved.getCursor(),state)&&state.pitch[0]==36,
         "AUT05 host Pitch automation not persisted at +12 semitones");
   saved.rewind();
   check(f.c->setComponentState(&saved)==kResultOk&&
         f.c->rowPitch(0)==12,
         "AUT05 Pitch project restore not reflected in Controller");
   f.p->setProcessing(false);f.p->setActive(false);
 }
 // Flush paths: Studio One may send parameter-only callbacks with no
 // samples or temporarily disable outputs. The last valid values must not
 // disappear while the audio renderer is inactive.
 {
   Fixture f;KitState kit{};kit.visible=1;kit.grid.fill(0);f.kit(kit);
   prepareAudio(*f.p);
   ParameterChanges changes;int32 index=0;
   changes.addParameterData(kRowVolumeBase,index)->addPoint(0,.75,index);
   changes.addParameterData(kRowPanBase,index)->addPoint(0,1.,index);
   changes.addParameterData(kRowMuteBase,index)->addPoint(0,1.,index);
   changes.addParameterData(kRowPitchBase,index)->addPoint(0,.75,index);
   changes.addParameterData(kMasterVolumeParam,index)->addPoint(0,.25,index);
   ProcessData flush{};flush.numSamples=0;flush.numOutputs=0;
   flush.inputParameterChanges=&changes;
   allocationFail=0;const auto zeroResult=f.p->process(flush);allocationFail=-1;
   check(zeroResult==kResultOk,
         "AUT06 zero-frame host mix automation failed or allocated");
   ResizableMemoryIBStream saved;KitState state{};
   check(f.p->getState(&saved)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),
                           saved.getCursor(),state)&&
         state.gain[0]==150&&state.pan[0]==200&&state.pitch[0]==36&&
         (state.muted&1u)!=0&&state.masterVolume==50,
         "AUT06 zero-frame parameter-only host updates did not persist");
   ParameterChanges outputless;index=0;
   outputless.addParameterData(kRowPitchBase,index)->addPoint(192,.5,index);
   flush.numSamples=256;flush.inputParameterChanges=&outputless;
   check(f.p->process(flush)==kResultOk,
         "AUT06 256-frame inactive-output host automation failed");
   ResizableMemoryIBStream second;KitState secondState{};
   check(f.p->getState(&second)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(second.getData()),
                           second.getCursor(),secondState)&&
         secondState.pitch[0]==24&&secondState.gain[0]==150,
         "AUT06 outputless automation lost new Pitch or other parameters");
   f.p->setProcessing(false);f.p->setActive(false);
 }
 // Editor/headphone audition must use the live host Pitch, not the
 // last control-thread kit value. Regression: Pitch +12 (ratio 2.0)
 // should end a 1024-frame WAV in 512 output samples.
 {
   Fixture f;KitState kit{};kit.visible=1;kit.grid.fill(0);
   kit.names[0]="autom-preview.wav";kit.wav[0]=constantStereo(.5f,-.25f);
   f.kit(kit);prepareAudio(*f.p);
   AuditAudioBlock b;ParameterChanges pitch;int32 index=0;
   pitch.addParameterData(kRowPitchBase,index)->addPoint(0,.75,index);
   b.data.inputParameterChanges=&pitch;
   check(f.p->process(b.data)==kResultOk,
         "AUT07 host Pitch automation did not reach Processor");
   b.data.inputParameterChanges=nullptr;
   check(f.c->previewSampleEdit(0,SampleEdit::reset(1024)),
         "AUT07 audition after host Pitch rejected");
   check(f.p->process(b.data)==kResultOk&&
         std::abs(b.left[100]-.5f)<1e-4f,
         "AUT07 MIDI-independent editor audition silent/wrong pitch");
   check(f.p->process(b.data)==kResultOk,
         "AUT07 audition second 256-frame block failed");
   check(f.p->process(b.data)==kResultOk&&
         std::abs(b.left[100])<1e-5f&&
         std::abs(b.right[100])<1e-5f,
         "AUT07 editor preview ignored host Pitch and played at old rate");
   check(f.c->stopSampleEditPreview(),
         "AUT07 audition STOP cleanup failed");
   f.p->setProcessing(false);f.p->setActive(false);
 }
 evidence("AUT02-07 actual VST3 mix/Pitch PCM, sample-accurate points, zero/outputless flush, editor audition, project recall and no audio allocations PASS");
}
