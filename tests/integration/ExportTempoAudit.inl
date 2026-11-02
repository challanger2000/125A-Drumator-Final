#include "../ExportTimingOracle.h"
#include "core/BeatCreator.h"
void exportTimingEvidence(const std::string& text){
 auto* prior=drumator::qa::collectedNotices;
 drumator::qa::collectedNotices=nullptr;
 drumator::qa::result(text);
 drumator::qa::collectedNotices=prior;
}
// Exact current Controller -> Processor -> IPC -> WAV/MIDI export path.
void testExportTempoHostRoundTrip(){
 using namespace export_timing;
 Fixture f;f.openV101();
 auto* view=IntegrationAccess::view(*f.c,0);
 const auto base=std::filesystem::temp_directory_path();
 const auto wavPath=base/"125a-host-tempo-audit.wav";
 const auto midiPath=base/"125a-host-tempo-audit.mid";
 std::array<float,256> left{},right{};float* channels[]{left.data(),right.data()};
 AudioBusBuffers bus{};bus.numChannels=2;bus.channelBuffers32=channels;
 ProcessContext hostClock{};hostClock.state=ProcessContext::kPlaying|
   ProcessContext::kProjectTimeMusicValid|ProcessContext::kTempoValid;
 ProcessData io{};io.symbolicSampleSize=kSample32;io.numOutputs=1;
 io.outputs=&bus;io.processContext=&hostClock;
 const auto evidenceDir=std::filesystem::current_path()/"export-timing";
 std::filesystem::create_directories(evidenceDir);
 std::ofstream measurements(evidenceDir/"live-versus-temp-wav.csv");
 measurements<<"style,steps,song,bpm,dsp_profile,frames,seconds,pcm_comparison\n";
 unsigned cases=0;
 for(unsigned style=0;style<=10;++style)for(unsigned steps:{16u,32u})
 for(double bpm:{90.,100.,120.,128.,140.})for(bool song:{false,true})
 for(unsigned profile=0;profile<2;++profile){
  if(style<10&&(bpm!=120.||song))continue;
  if(profile&&(style!=10||bpm!=120.))continue;
  KitState kit;kit.visible=1;kit.grid.fill(0);
  kit.patternLengths.fill(static_cast<std::uint8_t>(steps));
  kit.names[0]="quarter-impulse.wav";kit.wav[0]=impulse();
  for(unsigned step=0;step<steps;step+=4)kit.grid[step]=127;
  if(style<10){
   BeatCreatorSettings settings{static_cast<BeatStyle>(style),35,static_cast<std::uint8_t>(steps)};
   const auto generated=BeatCreator::preview(settings,42);
   check(generated.valid,"EXPTEMPO native genre fixture invalid");
   kit.visible=8;kit.grid.fill(0);
   std::copy(generated.a.begin(),generated.a.end(),kit.grid.begin());
   for(unsigned row=0;row<8;++row){kit.names[row]="genre-impulse.wav";kit.wav[row]=impulse(row);}
  }
  kit.songEnabled=song?1:0;
  check(kit.chain.setCount(2)&&kit.chain.configure(0,0,static_cast<std::uint8_t>(steps))&&
    kit.chain.configure(1,0,static_cast<std::uint8_t>(steps)),"EXPTEMPO native Song fixture");
  if(profile){
   kit.visible=3;kit.swing=25;kit.humanize=60;kit.masterVolume=150;kit.limiter=1;
   kit.wav[0]=constantStereo(.7f,-.3f);kit.pitch[0]=36;kit.pan[0]=140;kit.gain[0]=140;kit.choke[0]=1;
   kit.names[1]="choked-low.wav";kit.wav[1]=constantStereo(.6f,-.4f);
   kit.pitch[1]=12;kit.pan[1]=60;kit.choke[1]=1;
   kit.names[2]="muted.wav";kit.wav[2]=impulse(2);kit.muted=4;
   for(unsigned step=1;step<steps;step+=4)kit.grid[kSteps+step]=100;
   for(unsigned step=0;step<steps;step+=4)kit.grid[2*kSteps+step]=127;
  }
  f.kit(kit);prepareAudio(*f.p);
  const auto total=frames(steps*(song?2u:1u),bpm);std::vector<float> liveL(total),liveR(total);
  for(std::size_t first=0;first<total;){
   const auto n=static_cast<int32>(std::min<std::size_t>(256,total-first));
   hostClock.tempo=bpm;hostClock.projectTimeMusic=static_cast<double>(first)*bpm/(60.*48000.);
   io.numSamples=n;check(f.p->process(io)==kResultOk,"EXPTEMPO actual live process failed");
   std::copy_n(left.begin(),static_cast<std::size_t>(n),liveL.begin()+first);
   std::copy_n(right.begin(),static_cast<std::size_t>(n),liveR.begin()+first);
   first+=static_cast<std::size_t>(n);
  }
  f.c->requestMeterSnapshot();
  check(std::abs(f.c->exportTempoBpm()-bpm)<1e-9,"EXPTEMPO host -> Processor -> Controller BPM mismatch");
  // Mouse press -> drag threshold -> real temp render -> actual CFilePath package.
  // Intercept only the final OS handoff: this is not a Studio One import test.
  std::filesystem::path dragged;
  struct ResetObserver {~ResetObserver(){drumator::qa::exportDragObserver={};}} resetObserver;
  drumator::qa::exportDragObserver=[&](VSTGUI::IDataPackage* package){
   check(package&&package->getCount()==1,"EXPTEMPO drag package missing/ambiguous");
   const void* raw=nullptr;VSTGUI::IDataPackage::Type type=VSTGUI::IDataPackage::kError;
   const auto size=package->getData(0,raw,type);
   check(type==VSTGUI::IDataPackage::kFilePath&&raw&&size>1&&
     static_cast<const char*>(raw)[size-1]==0,"EXPTEMPO drag path is not UTF-8/NUL CFilePath");
   dragged=pathFromUtf8(static_cast<const char*>(raw));return true;
  };
  IntegrationAccess::exportMenu(*view);
  const auto geometry=UiV101Popup::geometry(UiV101Popup::Kind::Export,
    UiV101Layout::header,view->getViewSize().getHeight(),false);
  const auto cell=UiV101Popup::cell(geometry,song?3:1);
  VSTGUI::CPoint down{(cell.x0+cell.x1)/2.,(cell.y0+cell.y1)/2.};
  check(view->onMouseDown(down,VSTGUI::CButtonState(VSTGUI::kLButton))==VSTGUI::kMouseEventHandled,
        "EXPTEMPO actual export-menu press not handled");
  VSTGUI::CPoint move{down.x+30.,down.y};
  (void)view->onMouseMoved(move,VSTGUI::CButtonState(VSTGUI::kLButton));
  check(!dragged.empty()&&std::filesystem::is_regular_file(dragged),
        "EXPTEMPO real drag did not create a temporary WAV before OS handoff");
  const Wav wave(dragged);
  drumator::qa::exportDragObserver={};check(wave.frames==total,"EXPTEMPO connected WAV duration mismatch");
  for(std::size_t frame=0;frame<total;++frame)
   check(std::abs(wave.sample(frame)-liveL[frame])<2e-6f&&
         std::abs(wave.sample(frame,1)-liveR[frame])<2e-6f,
         "EXPTEMPO exported PCM differs from live host clock");
  if(style==10&&!profile)for(unsigned beat=0;beat<(steps*(song?2u:1u))/4;++beat){
   const auto at=onset(beat*4,bpm);
   check(std::abs(wave.sample(at)-.01f)<2e-6f,"EXPTEMPO wrong audible quarter-note onset");
   if(at)check(std::abs(wave.sample(at-1))<1e-7f,"EXPTEMPO early quarter-note onset");
  }
  check(f.c->exportMidi(song,midiPath.string()),"EXPTEMPO connected MIDI export failed");
  const Midi midi(read(midiPath));
  check(midi.micros==static_cast<std::uint32_t>(std::lround(60000000./bpm)),
        "EXPTEMPO WAV/MIDI tempo source differs");
  measurements<<style<<','<<steps<<','<<song<<','<<bpm<<','<<profile<<','<<total<<','
    <<static_cast<double>(total)/48000.<<",every_L_R_frame_equal\n";
  ++cases;
  if(style==10&&bpm==120.&&!profile){
   exportTimingEvidence("EXPTEMPO direct D&D before DAW import: host=120, Controller=120, renderer=120, steps="+
      std::to_string(steps)+", song="+std::to_string(song)+", 48k/24-bit/stereo frames="+
      std::to_string(wave.frames)+", duration="+std::to_string(static_cast<double>(wave.frames)/48000.)+
      "s, quarter-note distance=24000 PCM frames, every L/R PCM frame equals live VST3");
   std::filesystem::copy_file(dragged,evidenceDir/(std::string{song?"song-":"pattern-"}+
     std::to_string(steps)+"-steps-120bpm-before-daw-import.wav"),
     std::filesystem::copy_options::overwrite_existing);
  }
  std::error_code cleanup;std::filesystem::remove(dragged,cleanup);
  std::filesystem::remove(dragged.parent_path(),cleanup);
  f.p->setProcessing(false);f.p->setActive(false);
 }
 measurements.close();
 check(cases==44&&measurements.good(),"EXPTEMPO native matrix incomplete or evidence write failed");
 exportTimingEvidence("EXPTEMPO 44 real native mouse-drag/live-PCM cases PASS: 10 styles at120/16+32, quarter grid Pattern/Song at90/100/120/128/140 + four swing/humanize/pitch/choke/limiter/mute profiles; complete CSV retained");
 std::error_code ec;std::filesystem::remove(wavPath,ec);std::filesystem::remove(midiPath,ec);
}

void testExportTempoFailedTransfer(){
 using namespace export_timing;
 Fixture f;KitState kit;kit.visible=1;kit.grid.fill(0);kit.patternLengths.fill(16);
 kit.names[0]="tempo-fault-impulse.wav";kit.wav[0]=impulse();
 for(unsigned step=0;step<16;step+=4)kit.grid[step]=127;
 f.kit(kit);prepareAudio(*f.p);
 std::array<float,256> left{},right{};float* channels[]{left.data(),right.data()};
 AudioBusBuffers bus{};bus.numChannels=2;bus.channelBuffers32=channels;
 ProcessContext hostClock{};hostClock.state=ProcessContext::kPlaying|
   ProcessContext::kProjectTimeMusicValid|ProcessContext::kTempoValid;
 hostClock.tempo=90.;hostClock.projectTimeMusic=0.;
 ProcessData io{};io.symbolicSampleSize=kSample32;io.numSamples=256;
 io.numOutputs=1;io.outputs=&bus;io.processContext=&hostClock;
 check(f.p->process(io)==kResultOk,"EXPTEMPO old project/tempo setup failed");
 f.c->requestMeterSnapshot();
 check(f.c->exportTempoBpm()==90.,"EXPTEMPO stale-cache positive control not 90 BPM");
 check(f.c->disconnect(f.p)==kResultOk,"EXPTEMPO disconnect fault injection failed");
 hostClock.tempo=120.;
 check(f.p->process(io)==kResultOk,"EXPTEMPO new actual 120 BPM callback failed");
 f.c->requestMeterSnapshot(); // Must NOT silently retain a usable 90-BPM export.
 const auto wavPath=std::filesystem::temp_directory_path()/"125a-tempo-failure.wav";
 const auto midiPath=std::filesystem::temp_directory_path()/"125a-tempo-failure.mid";
 const std::vector<std::uint8_t> sentinel{'p','r','e','s','e','r','v','e'};
 for(const auto& path:{wavPath,midiPath}){
  std::ofstream out(path,std::ios::binary);out.write(reinterpret_cast<const char*>(sentinel.data()),
    static_cast<std::streamsize>(sentinel.size()));check(out.good(),"EXPTEMPO sentinel write failed");
 }
 const bool accepted=f.c->exportWav(false,wavPath.string());
 if(accepted){
  const Wav wrong(wavPath);
  exportTimingEvidence("EXPTEMPO REPRO: actual host=120, stale Controller="+std::to_string(f.c->exportTempoBpm())+
    "; accepted WAV frames="+std::to_string(wrong.frames)+" instead of 96000");
  check(wrong.frames==128000&&std::abs(wrong.sample(32000)-.01f)<2e-6f&&
    std::abs(wrong.sample(24000))<1e-7f,
    "EXPTEMPO stale-tempo mutation must actually move the second beat to frame32000");
  std::filesystem::create_directories("export-timing");
  std::filesystem::copy_file(wavPath,"export-timing/negative-control-stale90-actual120.wav",
    std::filesystem::copy_options::overwrite_existing);
 }
 check(!accepted&&read(wavPath)==sentinel,
       "EXPTEMPO failed tempo IPC accepted a stale-BPM WAV and overwrote destination");
 check(!f.c->exportMidi(false,midiPath.string())&&read(midiPath)==sentinel,
       "EXPTEMPO failed tempo IPC accepted stale MIDI metadata");
 check(f.c->connect(f.p)==kResultOk,"EXPTEMPO reconnect failed");
 f.c->requestMeterSnapshot();
 check(f.c->exportTempoBpm()==120.&&f.c->exportWav(false,wavPath.string())&&
       Wav(wavPath).frames==96000,"EXPTEMPO recovery did not produce 2.000s at 120 BPM");
 std::error_code ec;std::filesystem::remove(wavPath,ec);std::filesystem::remove(midiPath,ec);
 f.p->setProcessing(false);f.p->setActive(false);
}

void testExportTempoMutation(){
 struct ResetMutation {~ResetMutation(){drumator::qa::auditMutation=drumator::qa::AuditMutation::none;}} reset;
 drumator::qa::auditMutation=drumator::qa::AuditMutation::keepStaleExportTempo;
 bool detected=false;
 try{testExportTempoFailedTransfer();}
 catch(const std::runtime_error& error){
  detected=std::string(error.what())==
    "EXPTEMPO failed tempo IPC accepted a stale-BPM WAV and overwrote destination";
 }
 check(detected,"EXPTEMPO oracle did not kill the reintroduced stale-tempo export defect");
 exportTimingEvidence("EXPTEMPO negative control killed: actual host120/stale90 rendered128000 frames (2.666667s), second beat32000 instead of24000; same regression assertion rejects it");
}

struct ExportTempoReplyQueue final:IConnectionPoint {
 Fixture& fixture;
 std::vector<IMessage*> replies;
 explicit ExportTempoReplyQueue(Fixture& f):fixture(f){
  check(f.p->disconnect(f.c)==kResultOk&&f.p->connect(this)==kResultOk,
    "EXPTEMPO reply proxy connection failed");
 }
 ~ExportTempoReplyQueue(){
  fixture.p->disconnect(this);fixture.p->connect(fixture.c);
  for(auto* message:replies)if(message)message->release();
 }
 tresult PLUGIN_API queryInterface(const TUID requestedIid,void** object)override{
  if(FUnknownPrivate::iidEqual(requestedIid,IConnectionPoint::iid)||
     FUnknownPrivate::iidEqual(requestedIid,FUnknown::iid)){
   *object=static_cast<IConnectionPoint*>(this);return kResultOk;
  }*object=nullptr;return kNoInterface;
 }
 uint32 PLUGIN_API addRef()override{return 1;}
 uint32 PLUGIN_API release()override{return 1;}
 tresult PLUGIN_API connect(IConnectionPoint*)override{return kResultOk;}
 tresult PLUGIN_API disconnect(IConnectionPoint*)override{return kResultOk;}
 tresult PLUGIN_API notify(IMessage* message)override{
  if(std::strcmp(message->getMessageID(),"Drumator.MeterSnapshot")!=0)
    return fixture.c->notify(message);
  replies.push_back(message);message->addRef();return kResultOk;
 }
 void deliver(std::size_t index){
  check(index<replies.size()&&replies[index],"EXPTEMPO missing queued reply");
  auto* message=replies[index];replies[index]=nullptr;
  const auto status=fixture.c->notify(message);message->release();
  check(status==kResultOk,"EXPTEMPO queued snapshot rejected meter attributes");
 }
};

void testExportTempoLifecycle(){
 using namespace export_timing;
 Fixture f;KitState kit;kit.visible=1;kit.grid.fill(0);kit.grid[0]=127;
 kit.patternLengths.fill(16);kit.wav[0]=impulse();kit.names[0]="lifecycle.wav";
 f.kit(kit);prepareAudio(*f.p);
 const auto path=std::filesystem::temp_directory_path()/"125a-tempo-lifecycle.wav";
 const std::vector<std::uint8_t> sentinel{'p','r','e','s','e','r','v','e'};
 {std::ofstream file(path,std::ios::binary);file.write(reinterpret_cast<const char*>(sentinel.data()),
    static_cast<std::streamsize>(sentinel.size()));check(file.good(),"EXPTEMPO lifecycle sentinel");}
 check(!f.c->hasExportTempo()&&!f.c->exportWav(false,path.string())&&read(path)==sentinel,
       "EXPTEMPO unknown initial host tempo silently defaulted to120");
 confirmExportHostTempo(f,90.);
 // No UI poll between the new actual host tempo and export: export refreshes it.
 ProcessContext context{};context.state=ProcessContext::kTempoValid;context.tempo=120.;
 ProcessData io{};io.symbolicSampleSize=kSample32;io.processContext=&context;
 check(f.p->process(io)==kResultOk&&f.c->exportWav(false,path.string())&&
       f.c->exportTempoBpm()==120.&&Wav(path).frames==96000,
       "EXPTEMPO zero-frame Stop's valid tempo not refreshed by actual WAV export");
 // Rendering disabled outputs must still publish genuine host context.
 io.numSamples=256;context.tempo=128.;
 check(f.p->process(io)==kResultOk,"EXPTEMPO outputless host callback failed");
 f.c->requestMeterSnapshot();check(f.c->exportTempoBpm()==128.,"EXPTEMPO outputless tempo lost");
 io.numSamples=0;
 context.state=ProcessContext::kTempoValid|ProcessContext::kPlaying;
 context.tempo=140.;check(f.p->process(io)==kResultOk,"EXPTEMPO Stop/Play context failed");
 f.c->requestMeterSnapshot();check(f.c->exportTempoBpm()==140.,"EXPTEMPO restart tempo lost");
 IntegrationAccess::expireExportTempo(*f.c);
 f.c->requestMeterSnapshot();
 check(!f.c->hasExportTempo()&&!f.c->exportWav(false,path.string()),
       "EXPTEMPO repeated meter replies renewed an old host observation");
 confirmExportHostTempo(f,120.);
 {
  ExportTempoReplyQueue queued(f);
  context.tempo=90.;check(f.p->process(io)==kResultOk,"EXPTEMPO delayed90 setup");
  f.c->requestMeterSnapshot();
  context.tempo=140.;check(f.p->process(io)==kResultOk,"EXPTEMPO delayed140 setup");
  f.c->requestMeterSnapshot();
  check(queued.replies.size()==2,"EXPTEMPO expected two real delayed snapshots");
  queued.deliver(1);queued.deliver(0);
  check(f.c->exportTempoBpm()==140.,"EXPTEMPO older delayed reply reverted tempo");
  // Missing tempo is not permission to reuse140. Deliver a real malformed reply.
  f.c->requestMeterSnapshot();
  check(queued.replies.back()->getAttributes()->setFloat("ExportTempo",0.)==kResultOk,
        "EXPTEMPO invalid tempo injection failed");
  queued.deliver(2);check(!f.c->hasExportTempo(),"EXPTEMPO invalid tempo retained usable old value");
  context.tempo=90.;check(f.p->process(io)==kResultOk,"EXPTEMPO pre-recall delayed setup");
  f.c->requestMeterSnapshot();
  f.kit(kit);queued.deliver(3);
  check(!f.c->hasExportTempo(),"EXPTEMPO old project's late reply survived component recall");
 }
 check(!f.c->exportWav(false,path.string()),"EXPTEMPO recalled project reused previous host tempo");
 confirmExportHostTempo(f,120.);
 const std::vector<std::uint8_t> corrupt{'b','a','d'};
 drumator::qa::ByteStream bad(const_cast<std::uint8_t*>(corrupt.data()),corrupt.size());
 check(f.c->setComponentState(&bad)!=kResultOk&&f.c->hasExportTempo(),
       "EXPTEMPO rejected component recall invalidated valid old host tempo");
 // Materialize the test argument BEFORE injecting OOM. Otherwise the
 // temporary path.string() allocation throws outside the product boundary.
 const auto destination=path.string();
 allocationFail=0;
 const bool accepted=f.c->exportWav(false,destination);
 const bool injected=allocationFail==-1;allocationFail=-1;
 check(injected&&!accepted&&!f.c->hasExportTempo(),"EXPTEMPO allocation failure reused cached export tempo");
 confirmExportHostTempo(f,120.);
 check(f.c->exportWav(false,path.string())&&Wav(path).frames==96000,
       "EXPTEMPO lifecycle recovery WAV timing incorrect");
 f.openV101();IntegrationAccess::expireExportTempo(*f.c);
 auto* view=IntegrationAccess::view(*f.c,0);
 IntegrationAccess::exportMenu(*view);
 const auto geometry=UiV101Popup::geometry(UiV101Popup::Kind::Export,
   UiV101Layout::header,view->getViewSize().getHeight(),false);
 const auto cell=UiV101Popup::cell(geometry,1);
 VSTGUI::CPoint down{(cell.x0+cell.x1)/2.,(cell.y0+cell.y1)/2.};
 (void)view->onMouseDown(down,VSTGUI::CButtonState(VSTGUI::kLButton));
 VSTGUI::CPoint move{down.x+30.,down.y};
 (void)view->onMouseMoved(move,VSTGUI::CButtonState(VSTGUI::kLButton));
 check(IntegrationAccess::sampleEditorStatus(*view)==
   "EXPORT: HOST TEMPO UNAVAILABLE - PLAY HOST AND RETRY",
   "EXPTEMPO real drag did not explain unavailable/stale host tempo");
 std::error_code ec;std::filesystem::remove(path,ec);
 f.p->setProcessing(false);f.p->setActive(false);
 exportTimingEvidence("EXPTEMPO actual Controller/Processor lifecycle PASS: unknown/expired/invalid tempo, zero-frame Stop, restart, outputless callback, reordered replies, project recall, OOM and native drag error status");
}
