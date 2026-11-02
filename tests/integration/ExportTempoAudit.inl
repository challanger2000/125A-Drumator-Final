#include "../ExportTimingOracle.h"
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
 for(unsigned steps:{16u,32u})for(double bpm:{90.,100.,120.,128.,140.}){
  KitState kit;kit.visible=1;kit.grid.fill(0);
  kit.patternLengths.fill(static_cast<std::uint8_t>(steps));
  kit.names[0]="quarter-impulse.wav";kit.wav[0]=impulse();
  for(unsigned step=0;step<steps;step+=4)kit.grid[step]=127;
  f.kit(kit);prepareAudio(*f.p);
  const auto total=frames(steps,bpm);std::vector<float> liveL(total),liveR(total);
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
  const auto cell=UiV101Popup::cell(geometry,1);
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
  for(unsigned beat=0;beat<steps/4;++beat){
   const auto at=onset(beat*4,bpm);
   check(std::abs(wave.sample(at)-.01f)<2e-6f,"EXPTEMPO wrong audible quarter-note onset");
   if(at)check(std::abs(wave.sample(at-1))<1e-7f,"EXPTEMPO early quarter-note onset");
  }
  check(f.c->exportMidi(false,midiPath.string()),"EXPTEMPO connected MIDI export failed");
  const Midi midi(read(midiPath));
  check(midi.micros==static_cast<std::uint32_t>(std::lround(60000000./bpm)),
        "EXPTEMPO WAV/MIDI tempo source differs");
  drumator::qa::result("EXPTEMPO positive: host="+std::to_string(bpm)+" Controller="+
    std::to_string(f.c->exportTempoBpm())+" steps="+std::to_string(steps)+
    " PCM frames="+std::to_string(total)+"; every L/R frame equals live VST3");
  std::error_code cleanup;std::filesystem::remove(dragged,cleanup);
  std::filesystem::remove(dragged.parent_path(),cleanup);
  f.p->setProcessing(false);f.p->setActive(false);
 }
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
  drumator::qa::result("EXPTEMPO REPRO: actual host=120, stale Controller="+std::to_string(f.c->exportTempoBpm())+
    "; accepted WAV frames="+std::to_string(wrong.frames)+" instead of 96000");
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
