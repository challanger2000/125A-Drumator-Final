// Windows-only real Controller/View/Processor integration. Probe calls are
// inserted in generated copies; shipped sources and VST3 ABI remain untouched.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <objbase.h>
#include <malloc.h>
#include "Probe.h"
#include "ByteStream.h"
#include "ResultLog.h"
#include "core/PatternQuantization.h"
#include "core/Utf8Path.h"
#include "core/FlacImport.h"
#include "vst3/DrumatorController.h"
#include "vst3/DrumatorView.h"
#include "core/UiDetailGeometry.h"
#include "core/UiRowControls.h"
#include "core/UiCompactLayout.h"
#include "core/UiSongLayout.h"
#include "vst3/UiV101Scene.h"
#include "vst3/UiV101WaveLayout.h"
#include "UiV101NativePreview.h"
#include "vst3/UiV101ControllerBridge.h"
#include "vst3/DrumatorProcessor.h"
#include "vst3/DrumatorEditor.h"
#include "vst3/PluginIDs.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/parameterchanges.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/utility/memoryibstream.h"
#include "vstgui/lib/vstguiinit.h"
#include "vstgui/lib/platform/platformfactory.h"
#include "vstgui/lib/platform/win32/win32factory.h"
#include "vstgui/lib/controls/ctextedit.h"
#include "vstgui/lib/platform/iplatformframe.h"
#include "vstgui/lib/cbitmap.h"
#include "vstgui/lib/coffscreencontext.h"
#include "vstgui/lib/idatapackage.h"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <array>
#include <limits>
#include <atomic>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <new>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <cmath>

void* moduleHandle=nullptr; // SDK moduleinit platform handle, EXE fixture only.
namespace {
thread_local long allocationFail=-1;
thread_local long allocationCount=0;
void allocationPoint() {
 ++allocationCount;
 if(allocationFail==0){allocationFail=-1;throw std::bad_alloc{};}
 if(allocationFail>0)--allocationFail;
}
void check(bool v,const char* msg){if(!v)throw std::runtime_error(msg);}
std::atomic<int> stopPoint{drumator::qa::none};
std::atomic<bool> probeTimeout{false};
HANDLE entered=nullptr,resume=nullptr;
void arm(drumator::qa::Point p) {
 ResetEvent(entered);ResetEvent(resume);probeTimeout=false;stopPoint=p;
}
void waitEntered(){check(WaitForSingleObject(entered,10000)==WAIT_OBJECT_0,"probe not reached");}
void releaseProbe(){stopPoint=drumator::qa::none;SetEvent(resume);}
void pump() {
 MSG m{};while(PeekMessageW(&m,nullptr,0,0,PM_REMOVE)){TranslateMessage(&m);DispatchMessageW(&m);}
}
void evidence(const std::string& line) {
 std::cout<<line<<'\n';
 char file[32768]{};
 const auto n=GetEnvironmentVariableA("GITHUB_STEP_SUMMARY",file,static_cast<DWORD>(sizeof(file)));
 if(n>0&&n<sizeof(file)){std::ofstream f(file,std::ios::app);f<<"- "<<line<<'\n';}
}
}
void* operator new(std::size_t n){allocationPoint();if(auto* p=std::malloc(n?n:1))return p;throw std::bad_alloc{};}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
namespace drumator::qa {
thread_local bool legacyTriggerOrdering=false;
std::atomic<AuditMutation> auditMutation{AuditMutation::none};
std::atomic<bool> suppressRotaryFace{false};
std::atomic<unsigned> auditSongScheduleCalls{0};
std::atomic<unsigned> auditSongPatternSnapshots{0};
std::atomic<unsigned> auditViewRefreshFaultHits{0};
std::atomic<unsigned> auditWaveDecodeMisses{0},auditWaveFullReplies{0};
std::atomic<unsigned> auditWaveIndexedCalls{0};
std::atomic<const void*> auditImportTarget{nullptr};
std::atomic<unsigned> auditImportCalls{0},auditDelayCalls{0};
void probe(Point p) noexcept {
 if(stopPoint.load(std::memory_order_acquire)!=p)return;
 SetEvent(entered);
 if(WaitForSingleObject(resume,10000)!=WAIT_OBJECT_0)probeTimeout=true;
}
}
using namespace Steinberg;
using namespace Steinberg::Vst;
namespace drumator {
// Added as a friend in generated TEST headers only. Observes real state,
// including Undo/clipboard/paths, rather than mirroring the commit algorithm.
struct IntegrationAccess {
 static bool sampleEditorOpen(const DrumatorView& view){return view.sampleEditorOpen_;}
 static bool songEditorOpen(const DrumatorView& view){return view.songEditorOpen_;}
 static std::string sampleEditorStatus(const DrumatorView& v){return v.status_;}
 static bool openSampleEditorForCapture(DrumatorView& view,std::size_t row){return view.openSampleEditor(row);}
 static bool importPathViaView(DrumatorView& view,std::size_t row,const std::string& path){return view.loadPath(row,path);}
 static std::pair<std::uint32_t,std::uint32_t> editorViewport(const DrumatorView& view){return {view.sampleEditorFirst_,view.sampleEditorLast_};}
 static const auto& editorLeft(const DrumatorView& view){return view.sampleEditorWave_;}
 static const auto& editorRight(const DrumatorView& view){return view.sampleEditorWaveRight_;}
 static bool editorStereo(const DrumatorView& view){return view.sampleEditorStereo_;}
 static std::shared_ptr<OutputMeter> processorMeter(const Processor& p){return p.meter_;}
 static void pollMeter(DrumatorView& view){view.pollMeter();}
 static std::array<float,4> meterPaintState(const DrumatorView& v){return {v.mainMeterL_,v.mainMeterR_,v.holdMeterL_,v.holdMeterR_};}
 static std::uint32_t editorFrameCount(const DrumatorView& view){return view.sampleEditorFrames_;}
 static std::array<double,3> rotaryImageStatus(const DrumatorView& view){
   const auto* bmp=view.knobBitmap_;
   return {bmp&&bmp->isLoaded()?1.:0.,bmp?bmp->getWidth():-1.,bmp?bmp->getHeight():-1.};
 }
 static bool bundledBrandingLoaded(const DrumatorView& v){
   return v.productLogoBitmap_&&v.productLogoBitmap_->isLoaded()&&
          v.productLogoBitmap_->getWidth()==2172.&&
          v.productLogoBitmap_->getHeight()==724.&&
          v.backgroundBitmap_&&v.backgroundBitmap_->isLoaded()&&
          v.backgroundBitmap_->getWidth()==1671.&&
          v.backgroundBitmap_->getHeight()==941.;
 }
 static SampleEdit sampleEditorDraft(const DrumatorView& view){return view.sampleEditorEdit_;}
 static void setSampleEditorDraft(DrumatorView& view,const SampleEdit& edit){view.sampleEditorEdit_=edit;}
 static void setStatusText(DrumatorView& view,const std::string& status){view.status_=status;}
 static void seedTransportClock(Processor& p) {
   p.haveExpectedPpq_=true;
   p.expectedNextPpq_=19.5;
   p.phaseOriginStep_=76;
 }
 static bool clockReset(const Processor& p) {
   return !p.haveExpectedPpq_&&p.expectedNextPpq_==0.0&&p.phaseOriginStep_==0;
 }
 static std::uint32_t activeAudioVoices(const Processor& p) {
   return p.voices_.active();
 }
 static std::uint64_t overflowEvents(const Processor& p){return p.droppedTriggers_.load(std::memory_order_acquire);}
 static void setProcessorPattern(Processor& p,unsigned n){p.selected_.hostSubmit(n);}
 static auto snapshot(Controller& c) {
   std::array<PatternSteps,kPatterns> undo{};
   std::array<bool,kPatterns> available{};
   std::array<std::uint8_t,kPatterns> undoLengths{};
   for(std::size_t p=0;p<kPatterns;++p){
     available[p]=c.undo_.available(p);
     undoLengths[p]=c.undo_.snapshotLength(p);
     if(auto* x=c.undo_.snapshot(p))undo[p]=*x;
   }
   std::array<std::pair<std::uint8_t,std::uint8_t>,SongChain::kCapacity> chain{};
   for(std::size_t n=0;n<SongChain::kCapacity;++n){const auto e=c.songChain_.entry(n);chain[n]={e.pattern,e.steps};}
   return std::tuple{c.grid_,c.patternLengths_,c.labels_,c.customNames_,c.samplePaths_,
     c.muted_,c.solo_,c.gain_,c.pan_,c.pitch_,c.choke_,c.output_,c.visibleRows_,
     c.swingPercent_,c.humanizePercent_,c.masterVolume_,c.randomVelocity_,c.songEnabled_,
     c.clipboard_,c.clipboardLength_,c.clipboardValid_,c.strokeBefore_,c.strokePattern_,c.strokeGeneration_,
     c.editGeneration(),available,undo,undoLengths,chain,c.songChain_.count(),c.velocityVariation_,c.preferredZoom_,c.getParamNormalized(kPatternParam),
     c.originIdentity_,c.originIdentityValid_,c.pathsRestoredBeforeComponent_,c.strokeOwner_};
 }
 static auto viewSnapshot(DrumatorView& v) {
   VSTGUI::IPlatformTextEdit* platform=nullptr;
   if(v.nameEdit_)platform=v.nameEdit_->getPlatformTextEdit();
   return std::tuple{v.laneDragActive_,v.laneDragGeneration_,v.editingNameRow_,v.nameEditGeneration_,
     v.fillMenuRow_,v.getViewSize(),static_cast<const void*>(platform),v.laneDragPattern_,v.laneErase_,
      v.detailDragControl_,v.detailDragRow_,
     v.velocityRow_,v.velocityStep_,v.firstRow_,v.songSlot_,v.songEditorOpen_,
     v.compactViewport_,v.compactPageRows_,v.velocityExpanded_,v.popup_,v.popupRow_,v.activeKnob_,v.knobRow_,v.knobStartY_,v.knobStartValue_,v.knobGeneration_,v.status_};
 }
 static std::size_t viewCount(const Controller& c){return c.views_.size();}
 static DrumatorView* view(Controller& c,std::size_t n){check(n<c.views_.size(),"real custom view absent");return c.views_[n];}
 static VSTGUI::CTextEdit* edit(DrumatorView& v){return v.nameEdit_;}
  static std::size_t firstRow(DrumatorView& v){return v.firstRow_;}
  static std::size_t selectedRow(DrumatorView& v){return v.velocityRow_;}
  static double rowTop(DrumatorView& v,std::size_t i){return v.rowTopOnPage(i);}
  static bool velocityExpanded(DrumatorView& v,std::size_t row){return v.velocityExpanded_[row];}
  static int activeKnob(DrumatorView& v){return v.activeKnob_;}
  static int popup(DrumatorView& v){return v.popup_;}
  static std::size_t popupRow(DrumatorView& v){return v.popupRow_;}
 static std::size_t songPage(DrumatorView& v){return v.songPage_;}
 static std::size_t songSlot(DrumatorView& v){return v.songSlot_;}
 static const auto& paths(Controller& c){return c.samplePaths_;}
 static bool pendingDirty(const Controller& c){return c.dirtyPending_;}
 static bool zoomSample(DrumatorView& v,bool in,bool full=false){return v.zoomSampleEditor(in,full);}
 static bool scrollSample(DrumatorView& v,double x){return v.scrollEditorWave(x);}
 static void moveEditorMarker(DrumatorView& v,double x,bool end){v.moveSampleMarker(x,end);}
 static std::uint64_t requestId(const Controller& c,const DrumatorView* v){
   for(const auto& q:c.waveRequests_)if(q.view==v)return q.id;
   return 0;
 }
 static std::size_t outstandingWaves(const Controller& c){return c.waveRequests_.size();}
 static std::uint64_t hostInspected(const Processor& p){return p.inspectedHostEvents_.load(std::memory_order_acquire);}
 static std::uint64_t hostValid(const Processor& p){return p.validHostNotes_.load(std::memory_order_acquire);}
 static std::uint64_t hostUninspected(const Processor& p){return p.uninspectedHostEvents_.load(std::memory_order_acquire);}
 static void pendingPreview(Processor& p,unsigned row){p.previewPending_.fetch_or(1u<<row,std::memory_order_release);}
 static bool waveCacheEmpty(const Controller& c){return c.editorWaveRow_==kRows;}
 static std::size_t cachedWaveRow(const Controller& c){return c.editorWaveRow_;}
};
}
namespace {
using namespace drumator;
struct ResizeHost final:IPlugFrame {
 bool accept=true;unsigned calls=0;HWND hwnd=nullptr;
 tresult PLUGIN_API queryInterface(const TUID requestedIid,void** obj) override {
   if(FUnknownPrivate::iidEqual(requestedIid,IPlugFrame::iid)||FUnknownPrivate::iidEqual(requestedIid,FUnknown::iid)){
     *obj=static_cast<IPlugFrame*>(this);addRef();return kResultOk;
   }*obj=nullptr;return kNoInterface;
 }
 uint32 PLUGIN_API addRef() override{return 1;}
 uint32 PLUGIN_API release() override{return 1;}
 tresult PLUGIN_API resizeView(IPlugView* view,ViewRect* rect) override {
   ++calls;if(!accept)return kResultFalse;
   if(hwnd)SetWindowPos(hwnd,nullptr,0,0,rect->getWidth(),rect->getHeight(),SWP_NOMOVE|SWP_NOZORDER);
   return view->onSize(rect);
 }
};
struct Fixture {
 HostApplication host;
 Controller* c=new Controller;
 Processor* p=new Processor;
 std::vector<DrumatorEditor*> editors;
 std::array<ResizeHost,3> frames{};
 Fixture(){
   check(c->initialize(&host)==kResultOk&&p->initialize(&host)==kResultOk,"initialize real components");
   check(c->connect(p)==kResultOk&&p->connect(c)==kResultOk,"connect real components");
 }
 ~Fixture(){
   allocationFail=-1;releaseProbe();
   while(!editors.empty())closeLast();
   c->disconnect(p);p->disconnect(c);c->terminate();p->terminate();c->release();p->release();
 }
 void closeLast(){
   const auto n=editors.size()-1;auto* e=editors.back();editors.pop_back();
   static_cast<IPlugView*>(e)->removed();static_cast<IPlugView*>(e)->setFrame(nullptr);e->release();DestroyWindow(frames[n].hwnd);frames[n].hwnd=nullptr;
 }
 DrumatorEditor& open(){
   const auto n=editors.size();check(n<frames.size(),"too many fixture views");
   // Legacy regression fixtures continue exercising the original V1.0.0
   // template. Do not weaken or rewrite their historic hit coordinates.
   auto* e=new DrumatorEditor(c,"DrumatorView","Drumator.uidesc");
   e->setAllowedZoomFactors({});
   e->setZoomChecked(c->preferredZoom());
   auto& h=frames[n];h.hwnd=CreateWindowExW(0,L"STATIC",L"Drumator native QA",WS_OVERLAPPEDWINDOW|WS_VISIBLE,
     0,0,1280,850,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
   check(h.hwnd!=nullptr,"create parent HWND");static_cast<IPlugView*>(e)->setFrame(&h);
   check(static_cast<IPlugView*>(e)->attached(h.hwnd,kPlatformTypeHWND)==kResultOk,"attach actual VSTGUI editor");
   editors.push_back(e);pump();return *e;
 }
 // Attach the V1.0.1 template through a real Win32 IPlugFrame. This is
 // deliberately a separate test editor until DAW-facing activation passes.
 DrumatorEditor& openV101(){
   const auto n=editors.size();check(n<frames.size(),"too many V101 fixture editors");
   // This is the actual currently selected factory default, not a separate
   // test-only editor constructor.
   auto* e=dynamic_cast<DrumatorEditor*>(c->createView(ViewType::kEditor));
   check(e!=nullptr,"V101 production default factory returned no editor");
   auto& h=frames[n];
   h.hwnd=CreateWindowExW(0,L"STATIC",L"Drumator V1.0.1 native QA",
                           WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,1280,850,
                           nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
   check(h.hwnd!=nullptr,"create V101 native parent HWND");
   static_cast<IPlugView*>(e)->setFrame(&h);
   check(static_cast<IPlugView*>(e)->attached(h.hwnd,kPlatformTypeHWND)==kResultOk,
         "attach native V101 editor template to actual Windows host frame");
   editors.push_back(e);pump();return *e;
 }
 static std::vector<std::uint8_t> encode(const KitState& kit){std::vector<std::uint8_t> b;check(KitCodec::encode(kit,b),"valid kit encode");return b;}
 tresult recall(const std::vector<std::uint8_t>& bytes){
   drumator::qa::ByteStream stream(const_cast<std::uint8_t*>(bytes.data()),bytes.size());return c->setComponentState(&stream);
 }
 void kit(const KitState& kit){
   auto b=encode(kit);drumator::qa::ByteStream stream(b.data(),b.size());
   check(p->setState(&stream)==kResultOk&&recall(b)==kResultOk,"load real processor/controller");
 }
};
KitState namedKit(char ch,std::uint8_t rows=4) {
 KitState k;k.visible=rows;k.grid.fill(0);k.grid[0]=ch=='X'?37:93;
 if(ch!='X'){k.gain[0]=130;k.pan[0]=125;k.pitch[0]=22;k.masterVolume=75;}
 for(std::size_t r=0;r<kRows;++r){k.displayNames[r]=std::string(40,ch);k.names[r]=std::string(48,ch)+".wav";}
 return k;
}
tresult zoom(Controller& c,bool large){
 std::array<std::uint8_t,5> bytes{{0x44,0x5a,0x4d,0x31,static_cast<std::uint8_t>(large)}};
 drumator::qa::ByteStream s(bytes.data(),bytes.size());return c.setState(&s);
}
void click(DrumatorView& v,double x,double y,int buttons=VSTGUI::kLButton){
 VSTGUI::CPoint at(x,y);v.onMouseDown(at,VSTGUI::CButtonState(buttons));
}
HWND nameEditor(Fixture& f,std::size_t view=0,std::size_t row=0){
 SetActiveWindow(f.frames[view].hwnd);SetForegroundWindow(f.frames[view].hwnd);
 SetFocus(static_cast<HWND>(f.editors[view]->getFrame()->getPlatformFrame()->getPlatformRepresentation()));pump();
 auto* v=IntegrationAccess::view(*f.c,view);
 while(IntegrationAccess::firstRow(*v)+UiCompactLayout::perPage<=row){
    const auto bottom=v->getViewSize().getHeight()-UiCompactLayout::footer;
    click(*v,1190,bottom+17.);
  }
  check(row>=IntegrationAccess::firstRow(*v)&&
        row<IntegrationAccess::firstRow(*v)+UiCompactLayout::perPage,
        "name editor row not in viewport");
  click(*v,110,IntegrationAccess::rowTop(*v,row-IntegrationAccess::firstRow(*v))+14.,
        VSTGUI::kLButton|VSTGUI::kShift);
 pump();auto h=GetFocus();wchar_t cls[32]{};GetClassNameW(h,cls,32);
 check(h&&lstrcmpiW(cls,L"EDIT")==0,"native EDIT HWND/focus missing");
 check(IntegrationAccess::edit(*v)->getPlatformTextEdit()!=nullptr,"CTextEdit platform absent");return h;
}
void testPatternOverlap(){
 AtomicPatternBank bank;std::array<std::uint8_t,kRows*kSteps> x{},y{},actual{};x.fill(15);y.fill(115);
 check(bank.replacePattern(0,x.data(),x.size()),"T03 initial pattern");
 for(int i=0;i<16;++i){
   arm(qa::patternMidWrite);
   std::jthread writer([&]{bank.replacePattern(0,y.data(),y.size());});waitEntered();
   const bool accepted=bank.coherentPattern(0,actual);
   std::array<std::uint8_t,kRows*kSteps> torn{};
   for(std::size_t n=0;n<torn.size();++n)torn[n]=bank.get(0,n/kSteps,n%kSteps);
   const bool oracleRejects=torn!=x&&torn!=y;
   releaseProbe();writer.join();
   check(!accepted,"T03 read accepted an in-progress writer");
   check(oracleRejects,"T03 torn-reader negative was not truly mixed");
   check(bank.coherentPattern(0,actual)&&actual==y,"T03 completed generation absent");
   arm(qa::patternReadStarted);bool got=true;
   std::jthread reader([&]{got=bank.coherentPattern(0,actual);});waitEntered();
   bank.replacePattern(0,x.data(),x.size());releaseProbe();reader.join();
   check(!got,"T03 reader accepted changed generation");
   check(bank.coherentPattern(0,actual)&&actual==x,"T03 quiescent X missing");
 }
 evidence("T03 PASS: 16 mid-write rejections, 16 interrupted-read rejections, 32 complete reads, 16 real torn-reader negatives");
}
void prepareAudio(Processor& p){
 std::array<SpeakerArrangement,kRows> allOutputs{};
 allOutputs.fill(SpeakerArr::kStereo);
 check(p.setBusArrangements(nullptr,0,allOutputs.data(),
                             static_cast<Steinberg::int32>(allOutputs.size()))==kResultOk,
       "16 fixed stereo output arrangements");
 ProcessSetup s{};s.sampleRate=48000;s.maxSamplesPerBlock=256;s.symbolicSampleSize=kSample32;s.processMode=kRealtime;
 check(p.setupProcessing(s)==kResultOk&&p.setActive(true)==kResultOk&&p.setProcessing(true)==kResultOk,"prepare actual audio");
}
std::vector<std::uint8_t> constantStereo(float l,float r){
 std::vector<std::uint8_t> b(44+1024*8,0);
 auto put=[&](std::size_t at,std::uint32_t x,unsigned n){for(unsigned i=0;i<n;++i)b[at+i]=static_cast<std::uint8_t>(x>>(i*8));};
 std::memcpy(b.data(),"RIFF",4);put(4,static_cast<std::uint32_t>(b.size()-8),4);
 std::memcpy(b.data()+8,"WAVEfmt ",8);put(16,16,4);put(20,3,2);put(22,2,2);
 put(24,48000,4);put(28,48000*8,4);put(32,8,2);put(34,32,2);
 std::memcpy(b.data()+36,"data",4);put(40,1024*8,4);
 for(std::size_t n=0;n<1024;++n){std::memcpy(b.data()+44+n*8,&l,4);std::memcpy(b.data()+48+n*8,&r,4);}
 return b;
}
void testWavEditorPreviewAndRecall(){
 Fixture f;
 KitState initial;initial.visible=2;initial.grid.fill(0);
 initial.names[0]="sample.wav";initial.wav[0]=constantStereo(.5f,-.25f);
 f.kit(initial);prepareAudio(*f.p);
 std::array<float,256> left{},right{};float* channels[]{left.data(),right.data()};
 AudioBusBuffers out{};out.numChannels=2;out.channelBuffers32=channels;
 ProcessData block{};block.numSamples=256;block.numOutputs=1;block.outputs=&out;
 block.symbolicSampleSize=kSample32;
 auto getKit=[&]{
   ResizableMemoryIBStream stream;KitState kit;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                          static_cast<std::size_t>(stream.getCursor()),kit),
         "WE01 processor save/decode failed");
   return kit;
 };
 SampleEdit candidate{64,512,0,0,.5f};
 SampleEdit observed{};std::uint32_t frames=0;
 std::array<WaveformRange,256> peaks{};
 check(f.c->sampleEditorSnapshot(0,observed,frames,peaks)&&frames==1024&&
       observed.start==0&&observed.end==1024,"WE01 embedded editor snapshot");
 check(f.c->previewSampleEdit(0,candidate),"WE01 provisional preview rejected");
 check(f.p->process(block)==kResultOk,"WE01 stopped-transport preview render");
 check(std::abs(left[100]-.25f)<.005f&&
       std::abs(right[100]+.125f)<.005f,
       "WE01 provisional START/GAIN not audible");
 check(getKit().sampleEdits[0].end==0,"WE01 preview dirtied project");
 f.c->stopSampleEditPreview();
 check(f.p->process(block)==kResultOk,"WE01 preview stop render");
 check(std::abs(left[100])<1e-6f&&std::abs(right[100])<1e-6f,
       "WE01 cancel left provisional audio playing");
 check(getKit().sampleEdits[0].end==0,"WE01 cancel changed saved edit");
 check(f.c->commitSampleEdit(0,candidate),"WE01 commit rejected");
 const auto committed=getKit();
 check(committed.sampleEdits[0].start==64&&committed.sampleEdits[0].end==512&&
       committed.sampleEdits[0].gain==.5f,"WE01 edit missing from kit state");
 const auto encoded=Fixture::encode(committed);
 drumator::qa::ByteStream input(const_cast<std::uint8_t*>(encoded.data()),encoded.size());
 check(f.p->setState(&input)==kResultOk&&f.recall(encoded)==kResultOk,
       "WE01 processor/controller recall rejected");
 check(f.c->sampleEditorSnapshot(0,observed,frames,peaks)&&
       observed.start==64&&observed.end==512&&observed.gain==.5f,
       "WE01 recalled editor state differs");
 // Repeat native editor attach/detach while snapshots and previews are active.
 for(unsigned cycle=0;cycle<3;++cycle){
   f.open();
   check(f.c->sampleEditorSnapshot(0,observed,frames,peaks),
         "WE02 editor re-open lost WAV snapshot");
   check(f.c->previewSampleEdit(0,candidate),"WE02 repeated preview rejected");
   check(f.p->process(block)==kResultOk,"WE02 repeated preview render");
   f.closeLast();
   f.c->stopSampleEditPreview();
   check(f.p->process(block)==kResultOk,"WE02 stopped preview after detach");
   check(std::abs(left[100])<1e-6f,"WE02 orphaned preview after editor close");
 }
 // Valid host transport clock must not suppress a provisional audition.
 ProcessContext clock{};clock.tempo=120.;clock.projectTimeMusic=0.;
 clock.state=ProcessContext::kPlaying|ProcessContext::kTempoValid|
             ProcessContext::kProjectTimeMusicValid;
 block.processContext=&clock;
 check(f.c->previewSampleEdit(0,candidate),"WE03 transport preview rejected");
 check(f.p->process(block)==kResultOk,"WE03 transport preview render");
 check(std::abs(left[100]-.25f)<.005f&&
       std::abs(right[100]+.125f)<.005f,
       "WE03 running transport provisional output wrong");
 check(getKit().sampleEdits[0].start==64,"WE03 preview changed saved kit");
 f.c->stopSampleEditPreview();clock.projectTimeMusic+=256./48000.;
 check(f.p->process(block)==kResultOk,"WE03 transport preview stop");
 check(std::abs(left[100])<1e-6f,"WE03 stopped preview still audible");
 block.processContext=nullptr;
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("WE01-03 provisional audition stopped/running transport, 3 view cycles and DM16 recall PASS");
}
void testArbitration(bool zero,bool busy,const std::vector<unsigned>& points){
 Fixture f;auto old=namedKit('X');old.selected=2;f.kit(old);prepareAudio(*f.p);
 auto next=namedKit('Y');next.selected=6;next.grid.fill(0);
 next.gain[0]=100;next.pan[0]=100;next.pitch[0]=24;next.masterVolume=100;
 next.wav[0]=constantStereo(.4f,-.2f);
 for(std::size_t p=0;p<kPatterns;++p)next.grid[p*kRows*kSteps]=static_cast<std::uint8_t>(12+p*16);
 auto bytes=Fixture::encode(next);
 arm(busy?qa::restoreMidCommit:qa::restoreSnapshot);
 tresult restored=kResultFalse;
 std::jthread restore([&]{drumator::qa::ByteStream s(bytes.data(),bytes.size());restored=f.p->setState(&s);});waitEntered();
 std::array<float,256> left{},right{};float* ptrs[]{left.data(),right.data()};
 AudioBusBuffers out{};out.numChannels=2;out.channelBuffers32=ptrs;
 ProcessData data{};data.numSamples=zero?0:256;data.symbolicSampleSize=kSample32;
 data.numOutputs=zero?0:1;data.outputs=zero?nullptr:&out;
 tresult processed=kResultOk;
 // Distinct process calls are essential: SDK queues coalesce equal offsets.
 // In a zero-frame flush every valid point is at offset zero.
 for(auto point:points){
   ParameterChanges changes;int32 index=0;
   changes.addParameterData(kPatternParam,index)->addPoint(0,static_cast<double>(point)/7.,index);
   data.inputParameterChanges=&changes;
   if(f.p->process(data)!=kResultOk)processed=kResultFalse;
 }
 data.inputParameterChanges=nullptr;releaseProbe();restore.join();
 check(processed==kResultOk&&restored==kResultOk&&!probeTimeout,"O01 execution/controlled barrier failed");
 ResizableMemoryIBStream saved;KitState observed;
 check(f.p->getState(&saved)==kResultOk&&KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),saved.getCursor(),observed),"O01 returned state");
 // Documented AtomicPatternSelection contract: host points after restore's
 // selection snapshot must not be overwritten, including equal values/ABA.
 check(observed.selected==points.back(),"O01 later host automation overwritten by restore");
 saved.rewind();
 check(f.c->setComponentState(&saved)==kResultOk&&f.c->currentPattern()==points.back(),"O01 controller/host recall mismatch");
 String128 label{};check(f.c->getParamStringByValue(kPatternParam,f.c->getParamNormalized(kPatternParam),label)==kResultOk&&
   label[0]==static_cast<TChar>('A'+points.back()),"O01 host label differs");
 // Read the audible selected pattern independently of the returned state.
 ProcessContext clock{};clock.tempo=120;clock.projectTimeMusic=0;
 clock.state=ProcessContext::kPlaying|ProcessContext::kTempoValid|ProcessContext::kProjectTimeMusicValid;
 data.numSamples=256;data.numOutputs=1;data.outputs=&out;data.processContext=&clock;
 check(f.p->process(data)==kResultOk,"O01 follow-up render");
 const double velocity=static_cast<double>(12+points.back()*16)/127.;
 for(std::size_t n=80;n<180;++n)
   check(std::abs(left[n]-.4*velocity)<2e-4&&std::abs(right[n]+.2*velocity)<2e-4,
         "O01 audible selected pattern differs from state/host");
 f.p->setProcessing(false);f.p->setActive(false);
}
void testKitGate(){
 Fixture f;auto old=namedKit('X');old.wav[0]=constantStereo(.25f,-.35f);
 old.grid.fill(0);old.grid[0]=127;f.kit(old);prepareAudio(*f.p);
 auto next=namedKit('Y');next.selected=1;next.grid.fill(0);next.grid[kRows*kSteps]=64;
 next.gain[0]=100;next.pan[0]=100;next.pitch[0]=24;next.masterVolume=100;
 next.wav[0]=constantStereo(-.4f,.1f);auto bytes=Fixture::encode(next);
 arm(qa::restoreMidCommit);tresult loaded=kResultFalse;
 std::jthread control([&]{qa::ByteStream s(bytes.data(),bytes.size());loaded=f.p->setState(&s);});waitEntered();
 std::array<float,256> l{},r{};l.fill(19);r.fill(19);float* ptr[]{l.data(),r.data()};
 AudioBusBuffers bus{};bus.numChannels=2;bus.channelBuffers32=ptr;
 ProcessContext clock{};clock.tempo=120;clock.state=ProcessContext::kPlaying|
   ProcessContext::kTempoValid|ProcessContext::kProjectTimeMusicValid;
 ProcessData audio{};audio.symbolicSampleSize=kSample32;audio.numSamples=256;
 audio.numOutputs=1;audio.outputs=&bus;audio.processContext=&clock;
 const auto result=f.p->process(audio);releaseProbe();control.join();
 check(result==kResultOk&&loaded==kResultOk&&!probeTimeout,"T04 forced mid-commit execution");
 for(std::size_t n=0;n<256;++n)check(l[n]==0&&r[n]==0,"T04 busy gate leaked partially installed kit");
 check(f.p->process(audio)==kResultOk,"T04 after gate render");
 for(std::size_t n=80;n<180;++n)
   check(std::abs(l[n]+.4*64./127.)<2e-4&&std::abs(r[n]-.1*64./127.)<2e-4,
     "T04 first audible block mixed sample/pattern/mix generations");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("T04 controlled gate PASS: real process paused inside restore, exact silence then coherent new-kit audio");
}
void activeEdits(Fixture& f){
 auto* v=IntegrationAccess::view(*f.c,0);
 // Actual Undo and clipboard plus an active drag in a real view.
 check(f.c->setStepVelocity(0,0,3,91)&&f.c->copyCurrentPattern(),"Z01 create Undo/clipboard");
 click(*v,1248,UiCompactLayout::header+88); // expand velocity only for first instrument
 check(IntegrationAccess::velocityExpanded(*v,0),"Z01 Velocity row did not expand");
 click(*v,630,UiCompactLayout::header+UiCompactLayout::baseRow+12); // inline velocity for first instrument
 check(std::get<0>(IntegrationAccess::viewSnapshot(*v)),"Z01 fixture did not start actual view drag");
 check(f.c->setStepVelocity(0,0,4,85),"Z01 active stroke change");
}
void testRecallOOM(){
 // Count actual allocations in the real setComponentState, then fault each
 // reached position. Use fresh controllers so previous failures cannot hide one.
 long sites=0;auto y=namedKit('Y');auto bytes=Fixture::encode(y);
 {Fixture f;f.kit(namedKit('X'));f.open();f.open();activeEdits(f);SetWindowTextW(nameEditor(f),L"OLD PENDING BEFORE RECALL");
  allocationCount=0;check(f.recall(bytes)==kResultOk,"Z01 count recall");sites=allocationCount;}
 check(sites>=32,"Z01 injection did not reach allocating controller path");
 unsigned rejected=0,accepted=0;
 for(long fault=0;fault<sites;++fault){
   Fixture f;f.kit(namedKit('X'));f.open();f.open();activeEdits(f);const auto h=nameEditor(f);SetWindowTextW(h,L"OLD PENDING BEFORE RECALL");
   auto* v=IntegrationAccess::view(*f.c,0);auto before=IntegrationAccess::snapshot(*f.c);
   auto viewBefore=IntegrationAccess::viewSnapshot(*v);
   allocationFail=fault;tresult result=kResultFalse;bool escaped=false;
   try {result=f.recall(bytes);}catch(const std::bad_alloc&){escaped=true;}
   allocationFail=-1;
   check(!escaped,"Z01 bad_alloc escaped the actual controller recall");
   if(result!=kResultOk){
     ++rejected;check(IntegrationAccess::snapshot(*f.c)==before,"Z01 rejected recall changed data/Undo/gesture");
     check(IntegrationAccess::viewSnapshot(*v)==viewBefore&&IsWindow(h)&&GetFocus()==h,"Z01 rejected recall changed view/native edit");
     // Cancel must still restore the old, valid kit gesture; Undo remains usable.
     v->onMouseCancel();check(f.c->canUndoCurrentPattern(),"Z01 rejected recall lost Undo");
   }else{
     ++accepted;check(f.c->customName(0)==y.displayNames[0]&&!f.c->canUndoCurrentPattern(),"Z01 accepted recall not committed");
     const auto after=IntegrationAccess::snapshot(*f.c);
     VSTGUI::CPoint pt(630,400);v->onMouseMoved(pt,VSTGUI::CButtonState(VSTGUI::kLButton));v->onMouseUp(pt,{});v->onMouseCancel();
     if(auto* edit=IntegrationAccess::edit(*v))v->valueChanged(edit);
     check(IntegrationAccess::snapshot(*f.c)==after,"Z01 late callback changed accepted kit");
   }
 }
 check(rejected>0,"Z01 no real rejected allocation");
 evidence("Z01 PASS: actual Controller OOM sites="+std::to_string(sites)+", rejected="+std::to_string(rejected)+", postcommit accepted="+std::to_string(accepted));
}
void testLegacyReject(){
 for(std::uint32_t magic:{0x31524d44u,0x32524d44u,0x33524d44u})for(long fault=0;fault<16;++fault){
   Fixture f;f.kit(namedKit('X'));f.open();f.open();activeEdits(f);
   std::vector<std::uint8_t> b;
   for(auto n:{magic,std::uint32_t{0}})for(int i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(n>>(i*8)));
   if(magic==0x33524d44u)b.push_back(4);
   if(magic!=0x31524d44u)b.insert(b.end(),kCells,19);
   const auto before=IntegrationAccess::snapshot(*f.c);
   allocationFail=fault;const auto result=f.recall(b);allocationFail=-1;
   check(result!=kResultOk&&IntegrationAccess::snapshot(*f.c)==before,"Z01 rejected Legacy changed state");
   check(f.recall(b)==kResultOk&&!f.c->canUndoCurrentPattern(),"Z01 successful Legacy failed");
 }
 evidence("Z01 Legacy PASS: DMR1/2/3 48 actual default-name OOM rejections and successful recalls");
}
void testMultiViewLifecycleNotifications(){
 // V18: attached native views receive controller notifications while
 // both exist; removing one must leave no dangling registration.
 Fixture f;
 f.kit(namedKit('X'));
 f.open();f.open();
 check(IntegrationAccess::viewCount(*f.c)==2,"V18 both views not registered");
 auto* original=IntegrationAccess::view(*f.c,0);
 auto* second=IntegrationAccess::view(*f.c,1);
 check(original!=second,"V18 duplicate view registration");
 check(f.c->setParamNormalized(kPatternParam,3.0/7.0)==kResultOk,
       "V18 dual-view parameter notification");
 check(IntegrationAccess::viewCount(*f.c)==2&&
       IntegrationAccess::view(*f.c,0)==original&&
       IntegrationAccess::view(*f.c,1)==second,
       "V18 view collection corrupted by notification");
 f.closeLast();
 check(IntegrationAccess::viewCount(*f.c)==1&&IntegrationAccess::view(*f.c,0)==original,
       "V18 removed view still registered");
 check(f.c->setParamNormalized(kPatternParam,4.0/7.0)==kResultOk,
       "V18 notification after second view removal");
 check(IntegrationAccess::viewCount(*f.c)==1,"V18 stale view after notification");
 f.open();
 check(IntegrationAccess::viewCount(*f.c)==2&&IntegrationAccess::view(*f.c,0)==original,
       "V18 reopened editor lost original view");
 check(f.c->setParamNormalized(kPatternParam,5.0/7.0)==kResultOk,
       "V18 notification after reopening");
 evidence("V18 two native views, notification, removal, reopening PASS");
}
void testZoom(){
 Fixture f;f.kit(namedKit('X'));auto& e=f.open();auto* v=IntegrationAccess::view(*f.c,0);
 for(double dpi:{1.,1.5,2.}){
   check(e.setContentScaleFactor(dpi)==kResultOk,"Z02 attached DPI rejected");
   for(bool large:{true,false}){
     const auto before=e.getFrame()->getViewSize();const auto transform=e.getFrame()->getTransform();
     const auto preference=f.c->preferredZoom();f.frames[0].accept=false;
     check(zoom(*f.c,large)!=kResultOk,"Z02 host rejection accepted");
     check(e.getFrame()->getViewSize()==before&&e.getFrame()->getTransform()==transform&&
       f.c->preferredZoom()==preference,"Z02 host rejection partially changed zoom");
     f.frames[0].accept=true;check(zoom(*f.c,large)==kResultOk,"Z02 accepted recall failed");
     check(v->zoomMatches(large?1.5:1.)&&std::abs(e.getFrame()->getTransform().m11-dpi*(large?1.5:1.))<.001,"Z02 transform mismatch");
   }
 }
 // DZM2 preserves intermediate user zoom; DZM1 remains backward compatible.
 for(const auto entry : {std::pair<double,std::uint8_t>{1.2,1},
                         std::pair<double,std::uint8_t>{1.3,2}}) {
   std::array<std::uint8_t,5> v2{{'D','Z','M','2',entry.second}};
   drumator::qa::ByteStream in(v2.data(),v2.size());
   check(f.c->setState(&in)==kResultOk&&v->zoomMatches(entry.first),"Z02 DZM2 intermediate zoom recall");
   ResizableMemoryIBStream written;
   check(f.c->getState(&written)==kResultOk&&written.getCursor()==5+2*kRows,
         "Z02 DZM3 state length with empty sample paths");
   const auto* b=static_cast<const std::uint8_t*>(written.getData());
   check(b[0]=='D'&&b[1]=='Z'&&b[2]=='M'&&b[3]=='3'&&b[4]==entry.second,
         "Z02 intermediate zoom not persisted in DZM3");
 }
 check(zoom(*f.c,true)==kResultOk,"Z02 saved preference set");
 ResizableMemoryIBStream saved;
 check(f.c->getState(&saved)==kResultOk&&saved.getCursor()==5+2*kRows&&
   static_cast<const std::uint8_t*>(saved.getData())[4]==3,
   "Z02 saved DZM3 zoom bytes disagree");
 f.closeLast();
 auto& reopened=f.open();check(std::abs(reopened.getZoomFactor()-1.5)<.001,"Z02 reopen lost preference");
 // Both views need a change: policy must reject before any native host request.
 f.open();const auto calls=f.frames[0].calls+f.frames[1].calls;
 check(zoom(*f.c,false)!=kResultOk&&f.c->preferredZoom()==1.5,"Z02 multi-view recall not rejected");
 check(f.frames[0].calls+f.frames[1].calls==calls&&
   IntegrationAccess::view(*f.c,0)->zoomMatches(1.5)&&IntegrationAccess::view(*f.c,1)->zoomMatches(1.5),"Z02 partial multi-view change");
 evidence("Z02 PASS: attached 100/150, DPI 100/150/200, host refusals, saved reopen and atomic two-view rejection");
}
// Real VSTGUI offscreen render, exported as top-down 24-bit Windows BMP.
// The QA artifact provides pixels (including native Windows font rendering),
// not an invented screenshot or a simulation of the CSS layout.
void captureVstgui(DrumatorView& v,const std::string& title,double scale){
 const auto rect=v.getViewSize();
 check(rect.getWidth()>0&&rect.getHeight()>0,"invalid native GUI dimensions");
 auto bitmap=VSTGUI::renderBitmapOffscreen(
     VSTGUI::CPoint(rect.getWidth(),rect.getHeight()),scale,
     [&](VSTGUI::CDrawContext& ctx){v.draw(&ctx);});
 check(bitmap!=nullptr,"VSTGUI offscreen bitmap creation failed");
 auto* access=VSTGUI::CBitmapPixelAccess::create(bitmap.get(),false);
 check(access!=nullptr,"VSTGUI native bitmap pixel read failed");
 const auto width=access->getBitmapWidth(),height=access->getBitmapHeight();
 check(width>0&&height>0&&width<=4096&&height<=4096,"invalid VSTGUI screenshot pixel dimensions");
 if(title=="packaged-vst3-resources-silver-100"){
    unsigned bright=0,dark=0;
    for(std::uint32_t y=0;y<std::min(height,80u);++y)
      for(std::uint32_t x=450;x<std::min(width,825u);++x){
        check(access->setPosition(x,y),"packaged silver pixel out of bounds");
        VSTGUI::CColor p{};access->getColor(p);
        if(p.red>140&&p.green>140&&p.blue>140&&
           std::abs(int(p.red)-int(p.green))<45)++bright;
        if(int(p.red)+int(p.green)+int(p.blue)<270)++dark;
      }
    check(bright>500&&dark>4000,
          "real packaged resource VSTGUI image lacks dark header/silver logo");
 }
 if(title=="v2-song-board"){
   // Regression: the Song overlay must erase the entire row UI, INCLUDING
   // the 28-pixel strip to the right of the former 1252px background.
   check(width==1280&&height>200&&access->setPosition(1260,UiCompactLayout::header+32.),
         "Song overlay offscreen oracle pixel missing");
   VSTGUI::CColor pixel{};access->getColor(pixel);
   check(pixel.red==26&&pixel.green==35&&pixel.blue==45,
         "Song board leaked the underlying VELO button on its right edge");
 }

 const std::size_t stride=(static_cast<std::size_t>(width)*3u+3u)&~std::size_t{3u};
 std::array<unsigned char,54> hdr{};
 auto le16=[&](std::size_t at,std::uint16_t v){
  hdr[at]=static_cast<unsigned char>(v);hdr[at+1]=static_cast<unsigned char>(v>>8);
 };
 auto le32=[&](std::size_t at,std::uint32_t v){
  for(int b=0;b<4;++b)hdr[at+static_cast<std::size_t>(b)]=static_cast<unsigned char>(v>>(8*b));
 };
 hdr[0]='B';hdr[1]='M';
 le32(2,static_cast<std::uint32_t>(54u+stride*height));le32(10,54);
 le32(14,40);le32(18,width);
 le32(22,static_cast<std::uint32_t>(-static_cast<std::int32_t>(height)));
 le16(26,1);le16(28,24);le32(34,static_cast<std::uint32_t>(stride*height));
 std::filesystem::create_directories("gui-shots");
 std::ofstream output("gui-shots/"+title+".bmp",std::ios::binary);
 check(output.good(),"could not write native GUI screenshot");
 output.write(reinterpret_cast<const char*>(hdr.data()),hdr.size());
 std::vector<unsigned char> row(stride,0);
 std::uint64_t checksum=0;
 for(std::uint32_t y=0;y<height;++y){
  for(std::uint32_t x=0;x<width;++x){
    check(access->setPosition(x,y),"screenshot bitmap out of bounds");
    VSTGUI::CColor color{};access->getColor(color);
    const auto at=static_cast<std::size_t>(x)*3u;
    row[at]=color.blue;row[at+1]=color.green;row[at+2]=color.red;
    checksum+=color.red+color.green+color.blue;
  }
  output.write(reinterpret_cast<const char*>(row.data()),
               static_cast<std::streamsize>(row.size()));
 }
 access->forget();
 check(output.good()&&checksum>0,"blank or incomplete VSTGUI screenshot");
 evidence("Native VSTGUI render "+title+" "+std::to_string(width)+"x"+
          std::to_string(height)+" pixel checksum "+std::to_string(checksum));
}

// One-directional V1.0.1 view model: real controller getters only.
// Does not mutate the released controller, audio or original editor.
drumator::qa::UiV101NativePreview::Scene captureV101ControllerScene(Controller& c){
 return drumator::v101::captureControllerScene(c);
}

// Deterministic actual embedded-kit fixture. All visible labels and parameter
// readouts flow through real KitCodec -> real Controller -> Scene -> VSTGUI.
KitState v101PreviewKit(){
 KitState kit;
 constexpr std::array<const char*,10> names{{
   "Kick 01","Snare 01","HiHat Closed","HiHat Open","Tom Low",
   "Tom Mid","Tom High","Clap 01","Rim 01","Perc 01"
 }};
 kit.visible=10;
 kit.selected=2; // pattern C, assertion checks C is lit, not hard-coded A
 kit.swing=15;
 kit.humanize=10;
 kit.masterVolume=105;
 kit.gain[0]=135;
 kit.pan[1]=65;
 kit.pitch[2]=30; // +6 semitones
 kit.muted=std::uint16_t{1u<<5};
 kit.solo=std::uint16_t{1u<<8};
 const auto wav=constantStereo(.12f,-.10f);
 for(std::size_t row=0;row<names.size();++row){
   kit.displayNames[row]=names[row];
   kit.names[row]=std::string(names[row])+".wav";
   kit.wav[row]=wav;
   for(std::size_t step=0;step<kSteps;++step){
     const bool active=(row%3==0&&step%4==0)||
                       (row%3==1&&step%4==2)||
                       (row%3==2&&step%8==4);
     kit.grid[2*kRows*kSteps+row*kSteps+step]=active?static_cast<std::uint8_t>(95+row):0;
   }
 }
 return kit;
}
void testV101ControllerScene(){
 Fixture f;
 const auto kit=v101PreviewKit();
 f.kit(kit);
 const auto s=captureV101ControllerScene(*f.c);
 check(s.pattern==2&&s.visibleRows==10&&s.patternLength==32,
       "V101 controller scene pattern / viewport not real");
 check(s.swing==15&&s.humanize==10&&s.master==105,
       "V101 controller scene global knobs not real");
 check(s.rows[0].name=="Kick 01"&&s.rows[9].name=="Perc 01",
       "V101 controller scene sample labels not real");
 check(s.rows[0].hasSample&&s.rows[9].hasSample,
       "V101 controller scene embedded sample presence lost");
 check(s.rows[5].muted&&s.rows[8].solo&&
       !s.rows[4].muted&&!s.rows[4].solo,
       "V101 controller scene gate states wrong");
 check(s.rows[0].gain==135&&s.rows[1].pan==-35&&s.rows[2].pitch==6,
       "V101 controller scene mix controls wrong");
 check(s.rows[0].velocities[0]==95&&s.rows[0].velocities[1]==0&&
       s.rows[1].velocities[2]==96&&s.rows[2].velocities[4]==97,
       "V101 controller scene step data mismatch");
 check(f.c->toggleStep(2,0,0),"V101 controller step toggle failed");
 const auto updated=captureV101ControllerScene(*f.c);
 check(updated.rows[0].velocities[0]==0&&s.rows[0].velocities[0]==95,
       "V101 scene stale or mutates previous snapshot");
 // The new geometry -> bridge -> actual Controller path must work with real
 // mouse coordinates; popup-only controls must NOT silently mutate state.
 using L=drumator::UiV101Layout;
 using Bridge=drumator::UiV101ControllerBridge;
 constexpr L::Expansions noExpansion{};
 const auto stepHit=L::hit(L::stepX+L::stepPitch+8.,L::header+20.,0,10,noExpansion);
 check(stepHit.row==0&&stepHit.step==1&&stepHit.action==L::Action::Step,
       "V101 pixel-row step hit geometry incorrect");
 check(Bridge::apply(*f.c,stepHit)==Bridge::Route::Applied&&
       f.c->velocity(2,0,1)==100,
       "V101 step hit -> real Controller toggle path failed");
 const auto muteHit=L::hit(30.,L::header+5.*L::rowHeight+20.,0,10,noExpansion);
 check(muteHit.row==5&&muteHit.action==L::Action::Mute,
       "V101 mute physical hit geometry wrong");
 check(Bridge::apply(*f.c,muteHit)==Bridge::Route::Applied&&
       !f.c->isRowMuted(5),
       "V101 mute hit -> real Controller state failed");
 const auto fillHit=L::hit(90.,L::header+20.,0,10,noExpansion);
 check(fillHit.action==L::Action::Fill&&
       Bridge::apply(*f.c,fillHit)==Bridge::Route::DeferredToView,
       "V101 FILL popup must be left to the actual view");
 const auto gainBefore=f.c->rowGain(0);
 const auto rotaryHit=L::hit(L::knobCenters[0],L::header+24.,0,10,noExpansion);
 check(rotaryHit.action==L::Action::Volume&&
       Bridge::apply(*f.c,rotaryHit)==Bridge::Route::DeferredToView&&
       f.c->rowGain(0)==gainBefore,
       "V101 rotary requires mouse-drag lifecycle and must not change gain on click");
 check(Bridge::apply(*f.c,{0,L::Action::Step,32})==Bridge::Route::Ignored,
       "V101 out-of-range step bridge should not mutate Controller");
 check(Bridge::apply(*f.c,{10,L::Action::Mute,-1})==Bridge::Route::Ignored,
       "V101 hidden row bridge must not mutate Controller");

 // A genuine VSTGUI pixel must change when a real controller Step flips.
 // Checking the Scene struct alone would miss a disconnected paint function.
 auto* knob=new VSTGUI::CMultiFrameBitmap(
   VSTGUI::CResourceDescription("125A_Drumator_Knob.png"),{{125.,125.},101,1});
 check(knob->getWidth()==125.&&knob->getHeight()==12625.,
       "V101 controller pixel test original knob resource decode failed");
 auto* pngTitle=new VSTGUI::CBitmap(
   VSTGUI::CResourceDescription("Futuristisches Metalllogo DRUMATOR.png"));
 const auto titleBounds=drumator::qa::UiV101NativePreview::measureTitle(pngTitle);
 check(titleBounds.valid()&&titleBounds.width>100.&&titleBounds.height>10.,
       "V101 native controller scene must load actual new futuristic logo PNG");
 const auto stepPixel=[&](const drumator::qa::UiV101NativePreview::Scene& state){
   auto bmp=VSTGUI::renderBitmapOffscreen({1280.,686.},1.,
     [&](VSTGUI::CDrawContext& dc){
       drumator::qa::UiV101NativePreview::draw(dc,knob,pngTitle,titleBounds,state);
     });
   check(bmp!=nullptr,"V101 controller pixel test native raster missing");
   auto* a=VSTGUI::CBitmapPixelAccess::create(bmp.get(),false);
   check(a!=nullptr&&a->setPosition(428,178),
         "V101 controller pixel test key pixel missing");
   VSTGUI::CColor color{};a->getColor(color);a->forget();
   return std::array<int,3>{{color.red,color.green,color.blue}};
 };
 const auto beforePixel=stepPixel(s),afterPixel=stepPixel(updated);
 // Dynamic L/R and clip LEDs must be driven by numeric scene values.
 // A decorative/static meter would fail this exact same native paint test.
 const auto meterPixels=[&](float gain,bool clip){
   auto measured=s;
   measured.meterLeft=gain;
   measured.meterRight=gain;
   measured.clipped=clip;
   auto bmp=VSTGUI::renderBitmapOffscreen({1280.,686.},1.,
     [&](VSTGUI::CDrawContext& dc){
       drumator::qa::UiV101NativePreview::draw(dc,knob,pngTitle,titleBounds,
                                                 measured);
     });
   check(bmp!=nullptr,"V101 real meter bitmap unavailable");
   auto* a=VSTGUI::CBitmapPixelAccess::create(bmp.get(),false);
   check(a!=nullptr,"V101 meter pixels unavailable");
   VSTGUI::CColor level{},clipLed{};
   check(a->setPosition(1139,106),"V101 L meter pixel out of bounds");
   a->getColor(level);
   check(a->setPosition(1234,109),"V101 clip LED pixel out of bounds");
   a->getColor(clipLed);a->forget();
   return std::array<int,2>{{level.green,clipLed.red}};
 };
 const auto meterOff=meterPixels(0.f,false);
 const auto meterOn=meterPixels(1.f,true);
 check(meterOn[0]>meterOff[0]+80&&meterOn[1]>meterOff[1]+120,
       "V101 L/R or CLIP painter is still decorative instead of live");
 knob->forget();
 pngTitle->forget();
 check(beforePixel!=afterPixel&&beforePixel[0]>afterPixel[0]+70,
       "V101 actual Controller -> renderer step pixel did not change");
 evidence("V101 controller-backed GUI model: PCM kit/labels, pattern C, step mutation, gates, actual mix and globals PASS");
}

// Minimal VST3 host edit handler, used only by the isolated GUI input test.
// Pattern selector changes are host-automatable, and require a valid
// IComponentHandler::performEdit. The generic Fixture does not install one.
struct V101PatternEditHost final : Steinberg::Vst::IComponentHandler {
 bool editing=false;
 unsigned calls=0;
 Steinberg::Vst::ParamValue last=-1.;
 Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID interfaceId,void** ptr) override{
   if(!ptr)return Steinberg::kInvalidArgument;
   *ptr=nullptr;
   if(Steinberg::FUnknownPrivate::iidEqual(interfaceId,Steinberg::FUnknown::iid)||
      Steinberg::FUnknownPrivate::iidEqual(interfaceId,Steinberg::Vst::IComponentHandler::iid)){
     *ptr=static_cast<Steinberg::Vst::IComponentHandler*>(this);
     addRef();
     return Steinberg::kResultOk;
   }
   return Steinberg::kResultFalse;
 }
 Steinberg::uint32 PLUGIN_API addRef() override{return 1;}
 Steinberg::uint32 PLUGIN_API release() override{return 1;}
 Steinberg::tresult PLUGIN_API beginEdit(Steinberg::Vst::ParamID id) override {
   if(id!=drumator::kPatternParam)return Steinberg::kResultFalse;
   editing=true;return Steinberg::kResultOk;
 }
 Steinberg::tresult PLUGIN_API performEdit(Steinberg::Vst::ParamID id,
                                         Steinberg::Vst::ParamValue value) override {
   if(!editing||id!=drumator::kPatternParam)return Steinberg::kResultFalse;
   ++calls;last=value;return Steinberg::kResultOk;
 }
 Steinberg::tresult PLUGIN_API endEdit(Steinberg::Vst::ParamID id) override {
   if(!editing||id!=drumator::kPatternParam)return Steinberg::kResultFalse;
   editing=false;return Steinberg::kResultOk;
 }
 Steinberg::tresult PLUGIN_API restartComponent(Steinberg::int32) override{
   return Steinberg::kResultOk;
 }
};

void testV101Interaction(){
 using L=drumator::UiV101Layout;
 using UI=drumator::UiV101Interaction;
 V101PatternEditHost hostHandler;
 Fixture f;f.kit(v101PreviewKit());
 // PITCH is a real PreparedSample rebuild. The processor deliberately rejects
 // pitch changes until a valid output sample rate is established; unlike
 // VOL/PAN, pitch cannot be tested on an unprepared audio component.
 prepareAudio(*f.p);
 check(f.c->setComponentHandler(&hostHandler)==kResultOk,
       "V101 test host automation callback installation failed");
 UI ui(*f.c);
 check(ui.shownRows()==10&&ui.height()==686.,
       "V101 initial page size/150-percent height incorrect");
 // Every raised key uses identical physical y-bounds and the old low
 // positions must not receive invisible clicks under the hardware bezel.
 check(L::patternButton(0).y0==L::barsButton.y0&&
       L::patternMenu.y0==L::barsButton.y0&&
       L::headerGroup(0).y0==L::barsButton.y0&&
       L::headerGroup(1).y1==L::barsButton.y1&&
       L::patternButton(7).y1==L::barsButton.y1,
       "V101 raised PATTERN/BARS/SONG button family misaligned");
 check(ui.down(40.,135.)==UI::Result::Ignored,
       "V101 obsolete low header click area remains active");
 check(ui.down(382.,115.)==UI::Result::DeferredToView,
       "V101 raised PTNR MENU must defer to a real popup");
 check(ui.takeDeferredRequest().kind==UI::DeferredKind::PatternMenu&&
       ui.takeDeferredRequest().kind==UI::DeferredKind::None,
       "V101 pattern popup request must be precise and consume once");
 // The four large actions now have one 83px family with 8px gutters.
 // The wider MENU must activate at the new far-right position; the
 // obsolete irregular spacing must not leave invisible hot zones.
 check(ui.down(440.,115.)==UI::Result::DeferredToView &&
       ui.takeDeferredRequest().kind==UI::DeferredKind::PatternMenu,
       "V101 83px MENU right edge does not open real popup");
 check(ui.down(451.,115.)==UI::Result::Ignored &&
       ui.down(542.,115.)==UI::Result::Ignored &&
       ui.down(633.,115.)==UI::Result::Ignored,
       "V101 8px uniform header key gaps remain clickable");
 // Real host/controller pattern and song commands, not UI-only highlights.
 check(ui.down(170.,120.)==UI::Result::Applied&&f.c->currentPattern()==3&&
       hostHandler.calls==1&&std::abs(hostHandler.last-3./7.)<.00001,
       "V101 header pattern D did not commit through host edit callback");
 check(ui.down(130.,120.)==UI::Result::Applied&&f.c->currentPattern()==2,
       "V101 header pattern C restore failed");
 check(ui.down(460.,120.)==UI::Result::Applied&&f.c->patternLength(2)==16,
       "V101 one-bar header button did not reach controller");
 const auto protectedNote=f.c->velocity(2,0,16);
 check(ui.down(L::stepX+16*L::stepPitch+8.,L::header+20.)==
          UI::Result::Ignored&&f.c->velocity(2,0,16)==protectedNote,
       "V101 Step beyond 1-bar length must be inert");
 check(ui.down(460.,120.)==UI::Result::Applied&&f.c->patternLength(2)==32,
       "V101 two-bar restore failed");
 const auto song=f.c->songEnabled();
 check(ui.down(560.,120.)==UI::Result::Applied&&f.c->songEnabled()!=song,
       "V101 SONG button not routed to Controller");
 check(ui.down(650.,120.)==UI::Result::DeferredToView,
       "V101 SONG EDIT must defer to actual editor");
 check(ui.takeDeferredRequest().kind==UI::DeferredKind::SongEditor,
       "V101 SONG EDIT view intent was lost");
 const auto beforeStep=f.c->velocity(2,0,0);
 check(ui.down(428.,L::header+20.)==UI::Result::Applied&&
       f.c->velocity(2,0,0)!=(beforeStep),
       "V101 Step from physical hit not applied");
 check(ui.down(30.,L::header+5*L::rowHeight+20.)==UI::Result::Applied&&
       !f.c->isRowMuted(5),"V101 physical Mute hit failed");
 check(ui.down(60.,L::header+8*L::rowHeight+20.)==UI::Result::Applied&&
       !f.c->isRowSolo(8),"V101 physical Solo hit failed");
 // Pointer captures real hardware rotaries; mouse down must not change value.
 const auto oldSwing=f.c->swingPercent();
 check(ui.down(L::headerKnobCenters[0],L::headerKnobY)==UI::Result::Handled&&
       f.c->swingPercent()==oldSwing,"V101 knob down changed value");
 check(ui.move(L::headerKnobCenters[0],L::headerKnobY-8.)==UI::Result::Applied&&
       f.c->swingPercent()==oldSwing+2,"V101 SWING drag wrong");
 check(ui.up()==UI::Result::Handled,"V101 knob release not handled");
 const auto oldGain=f.c->rowGain(0);
 check(ui.down(L::knobCenters[0],L::header+24.)==UI::Result::Handled,
       "V101 row volume capture failed");
 check(ui.move(L::knobCenters[0],L::header+8.)==UI::Result::Applied&&
       f.c->rowGain(0)==oldGain+4,"V101 volume gesture didn't reach Controller");
 (void)ui.up();
 const auto oldPan=f.c->rowPan(1);
 check(ui.down(L::knobCenters[1],L::header+L::rowHeight+24.)==UI::Result::Handled,
       "V101 row pan capture failed");
 check(ui.move(L::knobCenters[1],L::header+L::rowHeight+16.)==UI::Result::Applied&&
       f.c->rowPan(1)==oldPan+2,"V101 PAN drag wrong");
 (void)ui.up();
 const auto oldPitch=f.c->rowPitch(2);
 check(ui.down(L::knobCenters[2],L::header+2*L::rowHeight+24.)==UI::Result::Handled,
       "V101 row pitch capture failed");
 check(ui.move(L::knobCenters[2],L::header+2*L::rowHeight+18.)==UI::Result::Applied&&
       f.c->rowPitch(2)==oldPitch+1,"V101 PITCH drag wrong");
 (void)ui.up();
 // Expand the tenth row: viewport scrolls one row to stay <=1080 at 150%.
 check(ui.down(1230.,L::header+9*L::rowHeight+12.)==UI::Result::Handled,
       "V101 VEL open failed");
 check(ui.state().expandedRow==9&&ui.state().firstRow==1&&
       ui.shownRows()==9&&ui.height()==686.,
       "V101 expanded last row hidden or exceeds 150-percent ceiling");
 const double expandedTop=L::rowTop(ui.state().firstRow,8,ui.expandedRows());
 const auto initialVel=f.c->velocity(2,9,0);
 check(initialVel>0&&f.c->velocity(2,9,1)==0,
       "V101 expected source notes for velocity gesture absent");
 check(ui.down(L::stepX+8.,expandedTop+72.)==UI::Result::Applied&&
       f.c->velocity(2,9,0)!=initialVel,
       "V101 Velocity lane did not change occupied step");
 ui.cancel();
 check(f.c->velocity(2,9,0)==initialVel,
       "V101 cancel must rollback entire velocity gesture");
 check(ui.down(L::stepX+L::stepPitch+8.,expandedTop+72.)==
       UI::Result::Ignored&&f.c->velocity(2,9,1)==0,
       "V101 lane activated empty note");
 check(ui.down(L::stepX+8.,expandedTop+72.)==UI::Result::Applied,
       "V101 second velocity begin failed");
 check(ui.up()==UI::Result::Handled&&f.c->velocity(2,9,0)==64,
       "V101 velocity commit did not retain edited value");
 // Exactly one VEL/SET expansion; view actions do not invent menus.
 check(ui.down(1230.,expandedTop+36.)==UI::Result::Handled&&
       ui.state().expansion==L::Expansion::Settings,
       "V101 VEL/SET exclusion failed");
 const unsigned outBefore=f.c->rowOutput(9);
 check(ui.down(151.,expandedTop+69.)==UI::Result::DeferredToView&&
       f.c->rowOutput(9)==outBefore,
       "V101 OUTPUT selector silently mutated Controller");
 const auto outputRequest=ui.takeDeferredRequest();
 check(outputRequest.kind==UI::DeferredKind::OutputMenu&&outputRequest.row==9,
       "V101 OUTPUT view request lost exact instrument target");
 check(ui.down(280.,expandedTop+69.)==UI::Result::DeferredToView,
       "V101 CHOKE menu not deferred");
 const auto chokeRequest=ui.takeDeferredRequest();
 check(chokeRequest.kind==UI::DeferredKind::ChokeMenu&&chokeRequest.row==9,
       "V101 CHOKE view request lost exact instrument target");
 const auto randomRealtime=f.c->rowRandomVelocity(9);
 check(ui.down(1100.,expandedTop+69.)==UI::Result::Applied&&
       f.c->rowRandomVelocity(9)==randomRealtime+10,
       "V101 realtime random increase not connected");
 check(ui.down(900.,expandedTop+69.)==UI::Result::Applied&&
       f.c->rowRandomVelocity(9)==randomRealtime,
       "V101 realtime random decrease not connected");
 check(ui.down(1000.,expandedTop+69.)==UI::Result::Handled,
       "V101 realtime percent field did not start direct mouse drag");
 check(ui.move(1000.,expandedTop+57.)==UI::Result::Applied&&
       f.c->rowRandomVelocity(9)==3,
       "V101 realtime percent drag cannot reach exact sub-10 values");
 check(ui.up()==UI::Result::Handled,
       "V101 realtime drag failed to release");
 check(ui.down(401.,expandedTop+69.)==UI::Result::Applied,
       "V101 SET copy row failed");
 const double row8Top=L::rowTop(ui.state().firstRow,7,ui.expandedRows());
 check(ui.down(1230.,row8Top+36.)==UI::Result::Handled&&
       ui.state().expandedRow==8,"V101 settings switch row failed");
 check(ui.down(540.,row8Top+69.)==UI::Result::Applied&&
       f.c->velocity(2,8,0)==64,
       "V101 SET paste row did not modify pattern");
 check(ui.down(652.,row8Top+69.)==UI::Result::Applied&&
       f.c->velocity(2,8,0)==0,"V101 SET clear row failed");
 // Modal zoom selection cannot click through; host rejection rolls back.
 check(ui.down(1200.,24.)==UI::Result::Handled&&ui.state().zoomOpen,
       "V101 zoom menu did not open");
 check(ui.down(1200.,139.)==UI::Result::DeferredToView&&
       ui.userZoom()==1.5,"V101 150 percent selection not proposed");
 check(ui.takeDeferredRequest().kind==UI::DeferredKind::Zoom,
       "V101 zoom view request not distinguishable from popup actions");
 ui.confirmZoom(false);
 check(ui.userZoom()==1.0,"V101 host-rejected zoom did not rollback");
 check(ui.down(1200.,24.)==UI::Result::Handled,
       "V101 zoom menu didn't reopen");
 check(ui.down(1200.,139.)==UI::Result::DeferredToView,
       "V101 zoom option not routed");
 ui.confirmZoom(true);
 check(ui.userZoom()==1.5,"V101 host-approved zoom not retained");
 check(ui.down(1200.,110.)==UI::Result::Applied,
       "V101 real main meter clip-reset hit not wired to Controller");
 // Page navigation across all 16 rows must return to the original first page.
 Fixture all;auto kit=v101PreviewKit();kit.visible=16;all.kit(kit);
 UI pages(*all.c);
 // Compact 95x25 caps: the renderer and hit handler share these bounds.
 static_assert(L::footerPrev.x1-L::footerPrev.x0==95.&&
               L::footerNext.x1-L::footerNext.x0==95.&&
               L::footerPrev.y1-L::footerPrev.y0==25.&&
               L::footerNext.y1-L::footerNext.y0==25.,
               "V101 footer buttons have mismatched or oversized hit rectangles");
 const double footerY=pages.height()-L::footer;
 check(pages.down(1175.,footerY+22.)==UI::Result::Handled&&
       pages.state().firstRow==10&&pages.shownRows()==6,
       "V101 compact next page did not reveal final six rows");
 check(pages.down(985.,footerY+22.)==UI::Result::Ignored&&
       pages.state().firstRow==10,
       "V101 old oversized PREV click hotspot still active");
 // Page two contains only six rows, so its footer moves UP by
 // four row-heights. Always click the CURRENT viewport's footer.
 const double secondFooterY=pages.height()-L::footer;
 check(pages.down(1070.,secondFooterY+22.)==UI::Result::Handled&&
       pages.state().firstRow==0&&pages.shownRows()==10,
       "V101 compact previous page not symmetrical");
 Fixture emptyRows;KitState noSamples{};noSamples.visible=9;emptyRows.kit(noSamples);
 UI countControls(*emptyRows.c);
 check(countControls.height()==L::editorHeight(0,9,L::Expansions{}),
       "V101 empty 9-row height contract");
 check(countControls.down(95.,countControls.height()-L::footer+22.)==
          UI::Result::Applied&&emptyRows.c->visibleRows()==10,
       "V101 ADD ROW did not change Controller count");
 check(countControls.down(195.,countControls.height()-L::footer+22.)==
          UI::Result::Applied&&emptyRows.c->visibleRows()==9,
       "V101 REMOVE ROW did not restore Controller count");
 evidence("V101 interactive controller routing: header, patterns, knob drag, VEL rollback, SETTINGS, zoom modal and 16-row paging PASS");
}

// This is the actual CView attached through a Windows VST3 editor and host
// frame. The earlier V101 tests cover only a detached interaction object.
void testV101LiveEditor(){
 using L=drumator::UiV101Layout;
 V101PatternEditHost automation;
 Fixture f;auto kit=v101PreviewKit();kit.visible=16;f.kit(kit);
 prepareAudio(*f.p);
 check(f.c->setComponentHandler(&automation)==kResultOk,
       "V101 host test cannot install automatable Pattern callback");
 auto& hostEditor=f.openV101();
 check(IntegrationAccess::viewCount(*f.c)==1,
       "V101 native host did not register exactly one Controller view");
 auto* view=IntegrationAccess::view(*f.c,0);
 check(view->isV101(),"V101 host loaded legacy painter instead of candidate");
 check(view->getViewSize().getWidth()==1280.&&
       view->getViewSize().getHeight()==686.,
       "V101 native host has wrong initial editor bounds");
 check(view->sampleDropTargetAt(200.,L::header+22.),
       "V101 real view did not expose sample field for WAV/FLAC drop");
 check(!view->sampleDropTargetAt(420.,L::header+22.),
       "V101 real view accepted sample drop on Step bank");
 // Real VSTGUI::CView pixels, not the isolated hardware fixture.
 const auto currentStepPixel=[&](){
   const auto bmp=VSTGUI::renderBitmapOffscreen({1280.,686.},1.,
       [&](VSTGUI::CDrawContext& dc){view->draw(&dc);});
   check(bmp!=nullptr,"V101 live attached editor offscreen draw failed");
   auto* px=VSTGUI::CBitmapPixelAccess::create(bmp.get(),false);
   check(px&&px->setPosition(428,178),"V101 live key raster unavailable");
   VSTGUI::CColor color{};px->getColor(color);px->forget();
   return std::array<int,3>{{color.red,color.green,color.blue}};
 };
 const auto pixelBefore=currentStepPixel();
 const auto original=f.c->velocity(2,0,0);
 click(*view,428.,L::header+20.);
 check(f.c->velocity(2,0,0)!=original,
       "V101 actual attached mouse Step did not reach Controller");
 const auto pixelAfter=currentStepPixel();
 check(pixelBefore!=pixelAfter&&pixelBefore[0]>pixelAfter[0]+70,
       "V101 attached real painter did not follow mouse Step edit");
 click(*view,L::knobCenters[0],L::header+24.,
       VSTGUI::kLButton|VSTGUI::kControl);
 check(f.c->rowGain(0)==100,
       "V101 real Ctrl-click Volume default reset lost");
 VSTGUI::CPoint knobStart(L::knobCenters[0],L::header+24.);
 (void)view->onMouseDown(knobStart,
        VSTGUI::CButtonState(VSTGUI::kLButton|VSTGUI::kShift));
 VSTGUI::CPoint knobMove(L::knobCenters[0],L::header+12.);
 (void)view->onMouseMoved(knobMove,
        VSTGUI::CButtonState(VSTGUI::kLButton|VSTGUI::kShift));
 (void)view->onMouseUp(knobMove,VSTGUI::CButtonState(VSTGUI::kLButton));
 check(f.c->rowGain(0)==101,
       "V101 real Shift-fine Volume drag wrong or silently cancelled");
 click(*view,170.,115.);
 check(f.c->currentPattern()==3&&automation.calls>=1,
       "V101 actual host Pattern D did not execute automation");
 click(*view,130.,115.);
 check(f.c->currentPattern()==2,"V101 actual host Pattern C restore failed");
 click(*view,90.,L::header+20.);
 const auto fillGeometry=drumator::UiV101Popup::geometry(
     drumator::UiV101Popup::Kind::Fill,L::header,view->getViewSize().getHeight());
 const auto every2=drumator::UiV101Popup::cell(fillGeometry,0);
 click(*view,(every2.x0+every2.x1)/2.,(every2.y0+every2.y1)/2.);
 check(f.c->velocity(2,0,2)>0,
       "V101 attached FILL menu did not edit the real row");
 // Mouse-only SET row; OUTPUT and CHOKE popups use the painted hit geometry.
 click(*view,1230.,L::header+36.);
 click(*view,151.,L::header+69.);
 const auto outputGeometry=drumator::UiV101Popup::geometry(
     drumator::UiV101Popup::Kind::Output,L::header,view->getViewSize().getHeight());
 const auto output2=drumator::UiV101Popup::cell(outputGeometry,2);
 click(*view,(output2.x0+output2.x1)/2.,(output2.y0+output2.y1)/2.);
 check(f.c->rowOutput(0)==2,"V101 host OUTPUT menu did not set bus 2");
 click(*view,280.,L::header+69.);
 const auto chokeGeometry=drumator::UiV101Popup::geometry(
     drumator::UiV101Popup::Kind::Choke,L::header,view->getViewSize().getHeight());
 const auto choke3=drumator::UiV101Popup::cell(chokeGeometry,3);
 click(*view,(choke3.x0+choke3.x1)/2.,(choke3.y0+choke3.y1)/2.);
 check(f.c->rowChoke(0)==3,"V101 host CHOKE menu did not set group 3");
 // Sample options are reachable without holding Shift/Alt.
 // SET expansion is 48 px high, so the second physical row
 // begins after both the first 48 px lane and its expanded controls.
 L::Expansions openSet{};
 openSet[0]=L::Expansion::Settings;
 const double secondRowTop=L::rowTop(0,1,openSet);
 click(*view,267.,secondRowTop+20.);
 const auto sampleGeometry=drumator::UiV101Popup::geometry(
     drumator::UiV101Popup::Kind::Sample,secondRowTop,
     view->getViewSize().getHeight());
 const auto remove=drumator::UiV101Popup::cell(sampleGeometry,2);
 click(*view,(remove.x0+remove.x1)/2.,(remove.y0+remove.y1)/2.);
 check(!f.c->hasSample(1),"V101 host sample-menu REMOVE targeted wrong row");
 const auto rowFooterY=view->getViewSize().getHeight()-L::footer;
 check(f.c->canHideLast(),"V101 host fixture last instrument unexpectedly occupied");
 click(*view,195.,rowFooterY+22.);
 check(f.c->visibleRows()==15,"V101 real host REMOVE ROW not connected");
 click(*view,95.,rowFooterY+22.);
 check(f.c->visibleRows()==16,"V101 real host ADD ROW not connected");
 // V1.0.1 real SONG editor uses the shared hardware paint family but
 // the exact original UiSongLayout click targets (no behavior regression).
 click(*view,650.,115.);
 check(IntegrationAccess::songEditorOpen(*view)&&
       view->getViewSize().getHeight()==UiCompactLayout::songEditorHeight,
       "V101 SONG board did not open at its real native height");
 captureVstgui(*view,"v101-song-board-hardware-100",1.0);
 captureVstgui(*view,"v101-song-board-hardware-130",1.3);
 const auto songCount=f.c->songCount();
 click(*view,181.,352.); // Pattern B for selected entry
 check(f.c->songEntry(0).pattern==1,
       "V101 SONG Pattern B hardware button disconnected");
 click(*view,716.,352.); // 1/2 BAR for same entry
 check(f.c->songEntry(0).steps==8,
       "V101 SONG 1/2 BAR hardware button disconnected");
 check(UiSongLayout::previousRight-UiSongLayout::previousLeft==100.&&
       UiSongLayout::nextRight-UiSongLayout::nextLeft==100.&&
       UiSongLayout::navBottom-UiSongLayout::navTop==26.&&
       UiSongLayout::startBottom-UiSongLayout::startTop==26.&&
       UiSongLayout::choiceBottom-UiSongLayout::choiceTop==26.,
       "V101 SONG keys must share 26px height and equal nav widths");
 check(!UiSongLayout::previousHit(1032.,181.)&&
       UiSongLayout::previousHit(1086.,181.)&&
       UiSongLayout::nextHit(1194.,181.),
       "V101 SONG navigation mouse zones are not aligned with narrow keys");
 check(UiSongLayout::addRight-UiSongLayout::addLeft==98.&&
       UiSongLayout::removeRight-UiSongLayout::removeLeft==98.,
       "V101 SONG action keys have unequal widths");
 check(UiSongLayout::removeLastHit(1175.,352.)&&
       !UiSongLayout::removeLastHit(1220.,352.),
       "V101 SONG REMOVE LAST retained its old oversized hit zone");
 click(*view,1050.,352.);
 check(f.c->songCount()==songCount+1,
       "V101 SONG ADD ENTRY lost count or layout");
 click(*view,1220.,352.);
 check(f.c->songCount()==songCount+1,
       "V101 SONG REMOVE LAST obsolete wide hot zone still removes an entry");
 click(*view,1175.,352.);
 check(f.c->songCount()==songCount,
       "V101 SONG REMOVE LAST failed to undo added position");
 const auto startBar=f.c->songStartBar();
 click(*view,606.,180.);
 check(f.c->songStartBar()==startBar+1,
       "V101 SONG START + lost exact increment");
 click(*view,675.,180.);
 check(f.c->songStartBar()==1,
       "V101 SONG START RESET not connected");
 click(*view,650.,115.);
 check(!IntegrationAccess::songEditorOpen(*view)&&view->isV101()&&
       view->getViewSize().getHeight()==686.,
       "V101 SONG close did not return to native main GUI and height");
 // Compact 95x25 footer keys must page in the actual host, resizing
 // from ten to six rows and back without stale click areas.
 const auto initialHeight=view->getViewSize().getHeight();
 click(*view,1175.,initialHeight-L::footer+22.);
 check(view->getViewSize().getHeight()==
           L::editorHeight(10,6,L::Expansions{}),
       "V101 attached host NEXT PAGE did not resize to six rows");
 click(*view,1070.,view->getViewSize().getHeight()-L::footer+22.);
 check(view->getViewSize().getHeight()==686.,
       "V101 attached host PREV PAGE did not return to ten rows");
 // 80x28 real zoom, with preset selection and host resize negotiation.
 click(*view,1200.,25.);click(*view,1200.,139.);
 check(std::abs(hostEditor.getZoomFactor()-1.5)<0.0001,
       "V101 host zoom selector did not apply the 150 percent zoom");
 check(f.frames[0].calls>0,"V101 native host did not receive any resize calls");
 // Wave editor: a direct EDIT click must request the correct real sample,
 // either immediately or through the existing async snapshot pipeline.
 click(*view,380.,L::header+24.);
 check(IntegrationAccess::sampleEditorOpen(*view)||
       f.c->sampleEditorRequestPending(0,view),
       "V101 live EDIT click did not reach sample editor snapshot");
 if(!IntegrationAccess::sampleEditorOpen(*view))
   check(IntegrationAccess::openSampleEditorForCapture(*view,0),
         "V101 WAV editor source not ready for native rendering verification");
 check(IntegrationAccess::sampleEditorOpen(*view),
       "V101 real Wave editor did not open for rendering verification");
 captureVstgui(*view,"v101-wave-editor-hardware-100",1.0);
 captureVstgui(*view,"v101-wave-editor-hardware-130",1.3);
 // Mounting groups are a unified paint-only 41px-deep navy family;
 // zoom/fit, preview/stop, reset and cancel/apply share one blue border.
 const auto& bezelGroups=drumator::UiV101WaveLayout::toolbarGroups;
 check(bezelGroups.size()==4,
       "V101 Wave toolbar must have four logical mounting bezels");
 for(std::size_t k=0;k<bezelGroups.size();++k){
   const auto& rect=bezelGroups[k];
   check(rect.bottom-rect.top==41.&&rect.top==453.&&
         rect.bottom<drumator::UiV101WaveLayout::readout(0).top,
         "V101 Wave group bezel overlaps value displays");
   if(k)
     check(rect.left-bezelGroups[k-1].right==2.,
           "V101 Wave blue group bezels must have identical separation");
 }
 // Every original key is still in exactly one display bezel.
 for(std::size_t k=0;k<drumator::UiV101WaveLayout::toolbar.size();++k){
   const auto& key=drumator::UiV101WaveLayout::toolbar[k];
   const std::size_t group=k<3?0:k<5?1:k==5?2:3;
   const auto& border=bezelGroups[group];
   check(border.left<key.left&&border.right>key.right&&
         border.top<key.top&&border.bottom>key.bottom,
         "V101 Wave button has no matching blue mounting bezel");
 }
 // On the real native editor, every new Wave key must agree with its
 // exact hitbox; clicks in the eight-pixel gutters must be inert.
 check(drumator::UiV101WaveLayout::buttonAt(360.,473.)==-1&&
       drumator::UiV101WaveLayout::buttonAt(971.,473.)==7,
       "V101 Wave hardware toolbar hit map differs from the painted keys");
 const auto beforeGap=IntegrationAccess::sampleEditorDraft(*view);
 click(*view,360.,473.);
 check(IntegrationAccess::sampleEditorOpen(*view)&&
       IntegrationAccess::sampleEditorDraft(*view).start==beforeGap.start&&
       IntegrationAccess::sampleEditorDraft(*view).end==beforeGap.end&&
       IntegrationAccess::sampleEditorDraft(*view).fadeIn==beforeGap.fadeIn&&
       IntegrationAccess::sampleEditorDraft(*view).fadeOut==beforeGap.fadeOut&&
       IntegrationAccess::sampleEditorDraft(*view).gain==beforeGap.gain,
       "V101 Wave obsolete toolbar gap still changes the edit");
 const auto preEdit=IntegrationAccess::sampleEditorDraft(*view);
 click(*view,300.,210.);
 check(IntegrationAccess::sampleEditorDraft(*view).start!=preEdit.start,
       "V101 Wave START marker stopped responding after redraw");
 click(*view,875.,473.); // CANCEL
 check(!IntegrationAccess::sampleEditorOpen(*view)&&view->isV101(),
       "V101 Wave CANCEL failed to return to hardware main GUI");
 f.closeLast();
 check(IntegrationAccess::viewCount(*f.c)==0,
       "V101 real host closing editor leaked a Controller view");
 // Host resize refusal must rollback BOTH the native window and the
 // processor/Controller instrument count (not just the UI's page state).
 Fixture refusal;KitState empty{};empty.visible=9;refusal.kit(empty);
 (void)refusal.openV101();
 auto* refusedView=IntegrationAccess::view(*refusal.c,0);
 const double nineRowHeight=L::editorHeight(0,9,L::Expansions{});
 check(refusedView->getViewSize().getHeight()==nineRowHeight,
       "V101 native nine-row fixture wrong initial height");
 refusal.frames[0].accept=false;
 click(*refusedView,95.,nineRowHeight-L::footer+22.);
 check(refusal.c->visibleRows()==9&&
       refusedView->getViewSize().getHeight()==nineRowHeight,
       "V101 rejected host ADD ROW left changed processor or view state");
 refusal.frames[0].accept=true;
 click(*refusedView,95.,nineRowHeight-L::footer+22.);
 check(refusal.c->visibleRows()==10&&
       refusedView->getViewSize().getHeight()==686.,
       "V101 accepted host ADD ROW failed after rollback");
 refusal.closeLast();
 check(IntegrationAccess::viewCount(*refusal.c)==0,
       "V101 rejected-resize fixture leaked editor view");
 evidence("V101 attached native VST3 editor: live raster, Step/Pattern/FILL, OUTPUT/CHOKE, sample menu, SONG return, 16-row paging, zoom, WAV edit request and teardown PASS");
}

enum class V101CaptureMode { Normal, Velocity, Settings };
void captureV101Preview(const char* title,double scale,bool zoomMenuOpen=false,
                        V101CaptureMode mode=V101CaptureMode::Normal){
 using L=drumator::UiV101Layout;
 Fixture fixture;
 fixture.kit(v101PreviewKit());
 drumator::UiV101Interaction ui(*fixture.c);
 if(mode!=V101CaptureMode::Normal){
   check(ui.down(1230.,L::header+9*L::rowHeight+12.)==
           drumator::UiV101Interaction::Result::Handled,
         "V101 screenshot did not open velocity expansion through UI hit");
   if(mode==V101CaptureMode::Settings){
     const double top=L::rowTop(ui.state().firstRow,8,ui.expandedRows());
     check(ui.down(1230.,top+36.)==
             drumator::UiV101Interaction::Result::Handled,
           "V101 screenshot failed to select settings expansion");
   }
 }
 const auto scene=captureV101ControllerScene(*fixture.c);
 check(scene.pattern==2&&scene.rows[0].hasSample,
       "V1.0.1 capture did not use an actual decoded controller kit");
 const double h=ui.height();
 check(h*1.5<=1080.,
       "V101 expanded-row screenshot would exceed 1080 at 150 percent");
 // Load the original 125A 101-frame knob filmstrip and the *new*
 // user-supplied transparent PNG with the filename preserved verbatim.
 // The old silver engraved per-glyph text is never drawn as a fallback.
 auto* knob=new VSTGUI::CMultiFrameBitmap(
   VSTGUI::CResourceDescription("125A_Drumator_Knob.png"),{{125.,125.},101,1});
 auto* logo=new VSTGUI::CBitmap(
   VSTGUI::CResourceDescription("Futuristisches Metalllogo DRUMATOR.png"));
 check(knob->getWidth()==125.&&knob->getHeight()==12625.,
       "V1.0.1 preview did not load real V1.0 knob filmstrip");
 const auto bounds=drumator::qa::UiV101NativePreview::measureTitle(logo);
 check(bounds.valid()&&bounds.width>100.&&bounds.height>10.,
       "V1.0.1 logo PNG unreadable or lacks alpha-visible content");
 auto* logoPixels=VSTGUI::CBitmapPixelAccess::create(logo,false);
 check(logoPixels!=nullptr&&logoPixels->setPosition(0,0),
       "V1.0.1 logo source PNG alpha inspection failed");
 VSTGUI::CColor corner{};logoPixels->getColor(corner);
 logoPixels->forget();
 check(corner.alpha<24,
       "V1.0.1 title has an opaque rectangular backdrop instead of transparent PNG");
 const auto box=drumator::qa::UiV101NativePreview::titleTarget(bounds);
 check(box.left>=191.&&box.right<=1119.&&box.top>=6.&&box.bottom<=77.,
       "V1.0.1 title/lightbars overlap 125A badge, header controls or ZOOM");
 auto bitmap=VSTGUI::renderBitmapOffscreen(
   VSTGUI::CPoint(L::width,h),scale,
   [knob,logo,bounds,&scene,&ui,zoomMenuOpen,scale](VSTGUI::CDrawContext& context){
     drumator::qa::UiV101NativePreview::draw(context,knob,logo,bounds,scene,zoomMenuOpen,
       scale>1.2?std::size_t{2}:std::size_t{0},ui.state());
   });
 knob->forget();
 logo->forget();
 check(bitmap!=nullptr,"V1.0.1 native VSTGUI prototype render failed");
 auto* access=VSTGUI::CBitmapPixelAccess::create(bitmap.get(),false);
 check(access!=nullptr,"V1.0.1 native pixels unavailable");
 const auto width=access->getBitmapWidth(),height=access->getBitmapHeight();
 check(width>=1280&&height>=686&&width<=2400&&height<=1300,
       "V1.0.1 render pixel dimensions wrong");
 // Native pixel contract: the footer must not mask either case screw.
 // Geometry-only checks would miss the former opaque paint overlay.
 if(scale==1.&&mode==V101CaptureMode::Normal&&!zoomMenuOpen){
   const auto screwY=static_cast<std::uint32_t>(h-15.);
   const auto sampleAt=[&](std::uint32_t x,std::uint32_t y){
     check(access->setPosition(x,y),"V101 footer screw pixel missing");
     VSTGUI::CColor color{};access->getColor(color);return color;
   };
   const auto left=sampleAt(14,screwY),right=sampleAt(1266,screwY);
   const auto metalLeft=sampleAt(28,screwY),metalRight=sampleAt(1253,screwY);
   check(left.red<150&&right.red<150&&
         metalLeft.red>left.red+30&&metalRight.red>right.red+30,
         "V101 footer overlay concealed the left or right case screw");
 }
 // Reference-material contract at native 100%. The supplied hardware
 // reference has warm steel (R>G>B) and a midnight-navy instrument body,
 // not cool-grey chrome or an almost-black row.
 if(scale==1.&&mode==V101CaptureMode::Normal&&!zoomMenuOpen){
   // The header must contain visible orange flashes *from the loaded PNG*,
   // not just the lower orange pattern caps (which start at y=99).
   std::size_t titleAmber=0;
   for(std::uint32_t yy=17;yy<70;yy+=2)
     for(std::uint32_t xx=200;xx<1100;xx+=3){
       check(access->setPosition(xx,yy),"V101 title alpha-screen coordinate invalid");
       VSTGUI::CColor light{};access->getColor(light);
       if(light.red>150&&light.red>light.green+45&&
          light.green>35&&light.blue<125)++titleAmber;
     }
   check(titleAmber>20,
         "V101 PNG lightbars absent from real native header pixel rendering");
   check(access->setPosition(250,50),
         "V101 hardware palette steel coordinate inaccessible");
   VSTGUI::CColor steel{};access->getColor(steel);
   check(steel.red>=185&&steel.red<=220&&
         steel.red>=steel.green+5&&steel.green>=steel.blue+6,
         "V101 hardware palette: faceplate lost reference warm-steel hue");
   check(access->setPosition(410,189),
         "V101 hardware palette navy coordinate inaccessible");
   VSTGUI::CColor navy{};access->getColor(navy);
   check(navy.red>=15&&navy.red<=45&&
         navy.green>=navy.red+9&&navy.blue>=navy.green+9,
         "V101 hardware palette: instrument chassis lost deep-blue hue");
 }
 if(mode!=V101CaptureMode::Normal){
   check(scale==1.&&ui.state().firstRow==1&&
         ui.state().expandedRow==9&&
         ui.height()==686.,"V101 expanded native raster wrong viewport/zoom");
   check(access->setPosition(mode==V101CaptureMode::Velocity?428:480,620),
         "V101 expanded row sample pixel inaccessible");
   VSTGUI::CColor sample{};access->getColor(sample);
   if(mode==V101CaptureMode::Velocity){
     check(sample.red>175&&sample.red>sample.green+55&&
           sample.red>sample.blue+80,
           "V101 Velocity screenshot lacks actual bright orange velocity bar");
   }else{
     check(sample.red>120&&sample.green>120&&sample.blue>105&&
           std::abs(static_cast<int>(sample.red)-static_cast<int>(sample.green))<28,
           "V101 Settings screenshot did not display grey metal COPY ROW button");
   }
 }
 unsigned light=0,dark=0,orange=0;
 for(std::uint32_t y=0;y<height;y+=7)
   for(std::uint32_t x=0;x<width;x+=7){
     check(access->setPosition(x,y),"V1.0.1 sample pixel out of range");
     VSTGUI::CColor pixel{};access->getColor(pixel);
     if(pixel.red>185&&pixel.green>180&&pixel.blue>170)++light;
     if(pixel.red<64&&pixel.green<75&&pixel.blue<93)++dark;
     if(pixel.red>165&&pixel.red>pixel.green+60&&pixel.green>40)++orange;
   }
 check(light>400&&dark>600&&orange>25,"V1.0.1 pixel oracle: missing chassis, rows or lit steps");
 const std::size_t stride=(static_cast<std::size_t>(width)*3u+3u)&~std::size_t{3u};
 std::array<unsigned char,54> hdr{};
 auto le16=[&](std::size_t at,std::uint16_t v){
   hdr[at]=static_cast<unsigned char>(v);
   hdr[at+1]=static_cast<unsigned char>(v>>8);
 };
 auto le32=[&](std::size_t at,std::uint32_t v){
   for(int i=0;i<4;++i)hdr[at+static_cast<std::size_t>(i)]=
     static_cast<unsigned char>(v>>(8*i));
 };
 hdr[0]='B';hdr[1]='M';le32(2,static_cast<std::uint32_t>(54+stride*height));
 le32(10,54);le32(14,40);le32(18,width);
 le32(22,static_cast<std::uint32_t>(-static_cast<std::int32_t>(height)));
 le16(26,1);le16(28,24);le32(34,static_cast<std::uint32_t>(stride*height));
 std::filesystem::create_directories("gui-shots");
 std::ofstream out(std::string("gui-shots/")+title+".bmp",std::ios::binary);
 check(out.good(),"V1.0.1 native BMP path not writable");
 out.write(reinterpret_cast<const char*>(hdr.data()),static_cast<std::streamsize>(hdr.size()));
 std::vector<unsigned char> scanline(stride,0);
 for(std::uint32_t y=0;y<height;++y){
   for(std::uint32_t x=0;x<width;++x){
     check(access->setPosition(x,y),"V1.0.1 output pixel out of range");
     VSTGUI::CColor pixel{};access->getColor(pixel);
     const auto at=static_cast<std::size_t>(x)*3u;
     scanline[at]=pixel.blue;scanline[at+1]=pixel.green;scanline[at+2]=pixel.red;
   }
   out.write(reinterpret_cast<const char*>(scanline.data()),
             static_cast<std::streamsize>(scanline.size()));
 }
 access->forget();
 check(out.good(),"V1.0.1 native BMP write failed");
 evidence(std::string("V1.0.1 native VSTGUI design prototype ")+title+" "+
          std::to_string(width)+"x"+std::to_string(height)+
          " light="+std::to_string(light)+" dark="+std::to_string(dark)+
          " active="+std::to_string(orange)+" PASS");
}

void testGuiPhysicalStepsRaster(){
 Fixture f;f.kit(namedKit('X',4));f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 auto sample=[](DrumatorView& view,int step){
  auto rect=view.getViewSize();
  auto bmp=VSTGUI::renderBitmapOffscreen(
   VSTGUI::CPoint(rect.getWidth(),rect.getHeight()),1.,
   [&](VSTGUI::CDrawContext& dc){view.draw(&dc);});
  check(bmp!=nullptr,"GUI physical-key bitmap missing");
  auto* a=VSTGUI::CBitmapPixelAccess::create(bmp.get(),false);
  check(a!=nullptr,"GUI physical-key pixel access missing");
  std::array<std::uint32_t,17*34> pixels{};
  std::size_t k=0;
  const int left=623+19*step;
  for(int yy=static_cast<int>(UiCompactLayout::header)+69;yy<static_cast<int>(UiCompactLayout::header)+103;++yy)for(int xx=left;xx<left+17;++xx){
   check(a->setPosition(xx,yy),"GUI key raster out of view");
   VSTGUI::CColor c{};a->getColor(c);
   pixels[k++]=(std::uint32_t(c.red)<<16)|(std::uint32_t(c.green)<<8)|c.blue;
  }
  a->forget();return pixels;
 };
 // Test normal keys, quarter markers, and final key. A newly enabled
 // step gets velocity 100, not any historical value.
 for(const int step:{0,4,15,31}){
  const double x=631.+19.*step;
  if(f.c->velocity(f.c->currentPattern(),0,step)!=0)
     click(*v,x,UiCompactLayout::header+88.);
  check(f.c->velocity(f.c->currentPattern(),0,step)==0,"GUI blank baseline");
  const auto before=sample(*v,step);
  click(*v,x,UiCompactLayout::header+88.);
  check(f.c->velocity(f.c->currentPattern(),0,step)==100,
        "GUI newly activated step must have velocity 100");
  const auto after=sample(*v,step);
  std::size_t diff=0;
  for(std::size_t i=0;i<before.size();++i)if(before[i]!=after[i])++diff;
  check(diff>=65,"GUI physical keycap did not visually respond to toggle");
  click(*v,x,UiCompactLayout::header+88.);
  check(f.c->velocity(f.c->currentPattern(),0,step)==0,"GUI step did not clear");
  check(sample(*v,step)==before,"GUI keycap raster did not restore blank appearance");
 }
 evidence("GUI keycaps 0/4/15/31 active/inactive/revert native raster PASS");
}
void testWaveEditorDesignCaptures(){
 // Test fixture contains genuine 48kHz IEEE float32 stereo WAVE bytes,
 // imported through the normal encoded kit/controller/processor state.
 for(const double factor:{1.0,1.3}){
   Fixture f;
   auto kit=namedKit('X',4);
   kit.names[0]="Wave_Editor_Demo_48k_stereo.wav";
   kit.wav[0]=constantStereo(0.f,0.f);
   // Retain valid RIFF/WAVE headers, replace PCM with a transient-rich,
   // amplitude-modulated real stereo signal (not a painted waveform).
   for(std::size_t n=0;n<1024;++n){
     const double t=static_cast<double>(n)/1024.;
     const float envelope=static_cast<float>((0.15+0.75*std::abs(std::sin(t*11.0)))*
         (0.35+0.65*std::abs(std::sin(t*73.0))));
     const float l=envelope*static_cast<float>(std::sin(t*91.0));
     const float r=envelope*static_cast<float>(std::sin(t*67.0+0.6));
     std::memcpy(kit.wav[0].data()+44+n*8,&l,4);
     std::memcpy(kit.wav[0].data()+48+n*8,&r,4);
   }
   kit.sampleEdits[0]=SampleEdit{90,940,110,140,1.25f};
   f.kit(kit);
   f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   const std::uint8_t zoomCode=factor==1.0?0:2;
   std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',zoomCode}};
   drumator::qa::ByteStream input(zoomState.data(),zoomState.size());
   check(f.c->setState(&input)==kResultOk&&v->zoomMatches(factor),
         "Wave editor native capture zoom mismatch");
   check(IntegrationAccess::openSampleEditorForCapture(*v,0)&&
         IntegrationAccess::sampleEditorOpen(*v),
         "Wave editor did not open loaded WAV sample");
   const auto draft=IntegrationAccess::sampleEditorDraft(*v);
   check(draft.start==90&&draft.end==940&&draft.fadeIn==110&&
         draft.fadeOut==140&&std::abs(draft.gain-1.25f)<0.001f,
         "Wave editor visible marker/fade/gain state differs from WAV test fixture");
   captureVstgui(*v,factor==1.0?"wave-editor-100":"wave-editor-130",factor);
 }
 evidence("Wave editor native captures 100/130: actual encoded stereo float32 WAV, start/end, fade-in/out and gain visible");
}
void testWaveEditor11FFlacAndStatus(){
 // Real libFLAC encoded stereo, 48kHz 16-bit, 4096 frames:
 // L=16384/32768, R=-8192/32768, independently verified source PCM.
 const std::uint8_t encodedFlac[]={0x66,0x4c,0x61,0x43,0x00,0x00,0x00,0x22,0x10,0x00,0x10,0x00,0x00,0x00,0x0e,0x00,0x00,0x0e,0x0b,0xb8,0x02,0xf0,0x00,0x00,0x10,0x00,0xa5,0x1e,0x81,0x86,0x53,0x2b,0x44,0x89,0x3f,0x27,0x7f,0x81,0x57,0xea,0x57,0xdc,0x84,0x00,0x00,0x28,0x20,0x00,0x00,0x00,0x72,0x65,0x66,0x65,0x72,0x65,0x6e,0x63,0x65,0x20,0x6c,0x69,0x62,0x46,0x4c,0x41,0x43,0x20,0x31,0x2e,0x35,0x2e,0x30,0x20,0x32,0x30,0x32,0x35,0x30,0x32,0x31,0x31,0x00,0x00,0x00,0x00,0xff,0xf8,0xca,0x18,0x00,0x7f,0x01,0x00,0x05,0x01,0x00,0x0f,0x92,0x34};
 const auto folder=std::filesystem::current_path()/"125A-W11F-import";
 std::filesystem::create_directories(folder);
 const auto path=folder/"stereo-reference.flac";
 const auto bad=folder/"broken.flac";
 const auto wav=folder/"stereo-reference.wav";
 auto write=[&](const std::filesystem::path& dest,const std::uint8_t* data,std::size_t n){
   std::ofstream file(dest,std::ios::binary);
   file.write(reinterpret_cast<const char*>(data),static_cast<std::streamsize>(n));
   check(file.good(),"W11F could not create real import fixture");
 };
 write(path,encodedFlac,sizeof(encodedFlac));
 // Independent reference check: this exact embedded FLAC passes
 // 'flac -t' with libFLAC 1.5.0 (4096 x 48kHz, 16-bit stereo).
 // Exercise converter separately before Controller/View, to locate failures.
 std::vector<std::uint8_t> converted;
 check(flacToWav(encodedFlac,sizeof(encodedFlac),converted),
       "W11G standalone dr_flac conversion failed on reference-validated FLAC");
 DecodedWav decoded;
 check(WavPcm16Decoder::decode(converted.data(),converted.size(),decoded)==WavError::Ok,
       "W11G converted float WAV rejected by existing decoder");
 check(decoded.sampleRate==48000&&decoded.channels==2&&decoded.left.size()==4096&&
       decoded.right.size()==4096,"W11G FLAC metadata/frame count incorrect");
 for(std::size_t i=0;i<4096;++i)
   check(std::abs(decoded.left[i]-.5f)<1e-6f&&
         std::abs(decoded.right[i]+.25f)<1e-6f,"W11G FLAC PCM independent channel reference");

 const std::uint8_t corrupted[]{'f','L','a','C',0,0,0,0};
 write(bad,corrupted,sizeof(corrupted));
 const auto wavBytes=constantStereo(.5f,-.25f);
 write(wav,wavBytes.data(),wavBytes.size());
 Fixture f;f.kit(namedKit('X',4));prepareAudio(*f.p);f.open();
 auto* view=IntegrationAccess::view(*f.c,0);
 // Use actual View loadPath -> Controller::loadWav -> Processor, not kit recall.
 for(const auto& imported:{wav,path}){
   if(!IntegrationAccess::importPathViaView(*view,0,pathToUtf8(imported)))
     throw std::runtime_error(std::string("W11F real import rejected: ")+
       imported.filename().string()+" / "+IntegrationAccess::sampleEditorStatus(*view));
   check(IntegrationAccess::sampleEditorStatus(*view).find("SAMPLE LOADED")!=std::string::npos,
         "W11F normal import status missing");
   check(IntegrationAccess::openSampleEditorForCapture(*view,0),
         "W11F imported sample editor could not open");
   check(IntegrationAccess::sampleEditorStatus(*view)=="SAMPLE READY FOR EDITING",
         "W11F imported editor displayed stale status");
   check(IntegrationAccess::editorFrameCount(*view)==(imported==wav?1024u:4096u)&&
         IntegrationAccess::editorStereo(*view),"W11F imported sample length/stereo");
   for(std::size_t bin=0;bin<256;++bin){
     const auto l=IntegrationAccess::editorLeft(*view)[bin];
     const auto r=IntegrationAccess::editorRight(*view)[bin];
     check(std::abs(l.minimum-.5f)<1e-4f&&std::abs(l.maximum-.5f)<1e-4f&&
           std::abs(r.minimum+.25f)<1e-4f&&std::abs(r.maximum+.25f)<1e-4f,
           "W11F decoded stereo waveform differs from independent PCM reference");
   }
   click(*view,875,473);
   check(!IntegrationAccess::sampleEditorOpen(*view),"W11F close editor after real import");
 }
 // Rejected import must preserve the previously accepted stereo sample.
 check(!IntegrationAccess::importPathViaView(*view,0,pathToUtf8(bad)),
       "W11G corrupt FLAC erroneously replaced loaded sample");
 check(IntegrationAccess::openSampleEditorForCapture(*view,0),
       "W11G corrupt FLAC damaged previous successful WAV/FLAC sample");
 check(IntegrationAccess::editorFrameCount(*view)==4096&&
       IntegrationAccess::editorStereo(*view),"W11G previous asset changed after failed FLAC");
 click(*view,875,473);
 const auto truncated=folder/"truncated.flac";
 write(truncated,encodedFlac,sizeof(encodedFlac)-8);
 check(!IntegrationAccess::importPathViaView(*view,0,pathToUtf8(truncated)),
       "W11G truncated FLAC erroneously accepted");
 check(!IntegrationAccess::importPathViaView(*view,1,pathToUtf8(bad)),
       "W11F corrupt FLAC accepted");
 check(IntegrationAccess::sampleEditorStatus(*view).find("SAMPLE LOADED")==std::string::npos,
       "W11F failed FLAC import falsely announced success");
 check(!IntegrationAccess::openSampleEditorForCapture(*view,1),
       "W11F failed FLAC imported audio in empty row");
 evidence("W11F real filesystem WAV (1024 frames) + libFLAC (4096 frames) stereo import, 256x2 PCM envelope bins, normal View status and corrupt FLAC rejection PASS");
}
void testWaveEditor11FScrollNumeric(){
 constexpr std::size_t frames=4096;
 Fixture f;auto kit=namedKit('X',4);
 auto& bytes=kit.wav[0];bytes.resize(44+frames*8);
 auto put=[&](std::size_t at,std::uint32_t v,unsigned n){
   for(unsigned k=0;k<n;++k)bytes[at+k]=static_cast<std::uint8_t>(v>>(8*k));
 };
 std::memcpy(bytes.data(),"RIFF",4);put(4,static_cast<std::uint32_t>(bytes.size()-8),4);
 std::memcpy(bytes.data()+8,"WAVEfmt ",8);
 put(16,16,4);put(20,3,2);put(22,2,2);put(24,48000,4);
 put(28,48000*8,4);put(32,8,2);put(34,32,2);
 std::memcpy(bytes.data()+36,"data",4);put(40,frames*8,4);
 std::array<float,frames> left{},right{};
 for(std::size_t i=0;i<frames;++i){
   // Unique channel- and position-dependent values; extrema are independently
   // calculated from the PCM source rather than Controller's cached envelopes.
   left[i]=static_cast<float>((i%223)/223.0-.5);
   right[i]=static_cast<float>((i%137)/137.0-.65);
   std::memcpy(bytes.data()+44+i*8,&left[i],4);
   std::memcpy(bytes.data()+48+i*8,&right[i],4);
 }
 f.kit(kit);f.open();auto* view=IntegrationAccess::view(*f.c,0);
 check(IntegrationAccess::openSampleEditorForCapture(*view,0),"W11F scroll editor open");
 auto validate=[&](std::uint32_t expectedFirst,std::uint32_t expectedLast){
   const auto [first,last]=IntegrationAccess::editorViewport(*view);
   check(first==expectedFirst&&last==expectedLast,
         "W11F scroll viewport frame boundaries incorrect");
   for(std::size_t bin=0;bin<256;++bin){
     const auto a=first+(static_cast<std::uint64_t>(last-first)*bin)/256;
     const auto b=first+(static_cast<std::uint64_t>(last-first)*(bin+1))/256;
     float minL=left[a],maxL=left[a],minR=right[a],maxR=right[a];
     for(std::size_t i=a+1;i<std::max<std::size_t>(a+1,b);++i){
       minL=std::min(minL,left[i]);maxL=std::max(maxL,left[i]);
       minR=std::min(minR,right[i]);maxR=std::max(maxR,right[i]);
     }
     const auto l=IntegrationAccess::editorLeft(*view)[bin];
     const auto r=IntegrationAccess::editorRight(*view)[bin];
     check(std::abs(l.minimum-minL)<1e-6f&&std::abs(l.maximum-maxL)<1e-6f&&
           std::abs(r.minimum-minR)<1e-6f&&std::abs(r.maximum-maxR)<1e-6f,
           "W11F scrolled stereo viewport extrema differ from independent source PCM");
   }
 };
 validate(0,4096);
 click(*view,300,473);
 validate(1024,3072);
 auto drag=[&](double from,double to){
   VSTGUI::CPoint p(from,395.);
   view->onMouseDown(p,VSTGUI::CButtonState(VSTGUI::kLButton));
   p.x=to;view->onMouseMoved(p,VSTGUI::CButtonState(VSTGUI::kLButton));
   view->onMouseUp(p,VSTGUI::CButtonState{});
 };
 drag(640,994);
 validate(2048,4096);
 drag(850,284);
 validate(0,2048);
 click(*view,380,473);
 validate(0,4096);
 click(*view,300,473);
 click(*view,470,473);
 validate(0,4096);
 evidence("W11F actual scroll thumb drags: FIT/ZOOM, both rail ends, 256x2 independently computed PCM bin extrema each viewport PASS");
}
void testWaveEditor11FStereoFadePcm(){
 Fixture f;auto kit=namedKit('X',2);
 kit.wav[0]=constantStereo(.5f,-.25f);
 f.kit(kit);prepareAudio(*f.p);
 const SampleEdit edit{64,192,32,32,1.f};
 check(f.c->previewSampleEdit(0,edit),"W11F stereo fade preview rejected");
 std::array<float,256> left{},right{};float* channels[]{left.data(),right.data()};
 AudioBusBuffers output{};output.numChannels=2;output.channelBuffers32=channels;
 ProcessData data{};data.numSamples=256;data.numOutputs=1;data.outputs=&output;
 data.symbolicSampleSize=kSample32;
 check(f.p->process(data)==kResultOk,"W11F stereo fade PCM process failure");
 for(unsigned n=0;n<256;++n){
   double envelope=n>=128?0.:1.;
   if(n<32)envelope*=static_cast<double>(n)/32.;
   if(n>=96&&n<128)envelope*=static_cast<double>(127-n)/32.;
   check(std::abs(left[n]-.5*envelope)<.006&&
         std::abs(right[n]+.25*envelope)<.006,
         "W11F stereo PCM differs from independently calculated START/END/fade reference");
 }
 check(f.c->stopSampleEditPreview(),"W11F stop stereo fade");
 evidence("W11F PCM independently calculated 256 frames x 2 channels: START 64 END 192 FADE-IN/OUT 32, silence boundaries PASS");
}
void testWaveEditorRealMouseControls(){
 Fixture f;auto kit=namedKit('X',4);
 kit.wav[0]=constantStereo(.5f,-.25f);
 kit.sampleEdits[0]=SampleEdit{90,940,110,140,1.25f};
 f.kit(kit);prepareAudio(*f.p);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 check(IntegrationAccess::openSampleEditorForCapture(*v,0),"W11D open native editor");
 auto draft=[&]{return IntegrationAccess::sampleEditorDraft(*v);};
 auto drag=[&](double x,double y,double dest){
   VSTGUI::CPoint pos(x,y);
   v->onMouseDown(pos,VSTGUI::CButtonState(VSTGUI::kLButton));
   pos.x=dest;
   v->onMouseMoved(pos,VSTGUI::CButtonState(VSTGUI::kLButton));
   v->onMouseUp(pos,VSTGUI::CButtonState{});
 };
 // Full-screen sample viewport, select START and END at distinct x coordinates.
 drag(284.+710.*90./1024.,310.,284.+710.*140./1024.);
 check(draft().start>=138&&draft().start<=142,"W11D START drag frame mismatch");
 drag(284.+710.*940./1024.,310.,284.+710.*900./1024.);
 check(draft().end>=898&&draft().end<=902,"W11D END drag frame mismatch");
 // Upper fade handles must modify the underlying frame durations.
 const auto beforeIn=draft().fadeIn;
 drag(284.+710.*(draft().start+beforeIn)/1024.,182.,
      284.+710.*(draft().start+beforeIn+40)/1024.);
 check(draft().fadeIn>beforeIn,"W11D fade-in handle failed");
 const auto beforeOut=draft().fadeOut;
 drag(284.+710.*(draft().end-beforeOut)/1024.,182.,
      284.+710.*(draft().end-beforeOut-40)/1024.);
 check(draft().fadeOut>beforeOut,"W11D fade-out handle failed");
 // Zoom +, pan via actual mouse drag, zoom -, FIT.
 click(*v,300,473);
 check(IntegrationAccess::zoomSample(*v,false),"W11D zoom-out from button state");
 click(*v,300,473);
 drag(400,395,760);
 click(*v,380,473);
 click(*v,470,473);
 // Gain slider change and Ctrl-reset, then RESET toolbar.
 drag(680,421,815);
 check(draft().gain>1.f,"W11D gain drag failed");
 click(*v,740,421,VSTGUI::kLButton|VSTGUI::kControl);
 check(std::abs(draft().gain-1.f)<.001f,"W11D gain control-reset failed");
 click(*v,770,473);
 check(draft().start==0&&draft().end==1024&&draft().fadeIn==0&&
       draft().fadeOut==0&&std::abs(draft().gain-1.f)<.001f,
       "W11D RESET did not restore full sample");
 // Repeat preview/stop through actual controls; cancelled edits must not persist.
 for(int i=0;i<3;++i){click(*v,560,473);click(*v,665,473);}
 click(*v,350,310);
 check(draft().start>0,"W11D modified state before cancel");
 click(*v,875,473);
 check(!IntegrationAccess::sampleEditorOpen(*v),"W11D CANCEL did not close");
 check(IntegrationAccess::openSampleEditorForCapture(*v,0),"W11D reopen after cancel");
 check(draft().start==90&&draft().end==940,"W11D CANCEL persisted unsaved edit");
 // APPLY must persist changed Start when dialog is reopened.
 click(*v,380,310);
 const auto applyStart=draft().start;
 check(applyStart!=90,"W11D change before APPLY");
 click(*v,970,473);
 check(!IntegrationAccess::sampleEditorOpen(*v),"W11D APPLY did not close");
 check(IntegrationAccess::openSampleEditorForCapture(*v,0),"W11D reopen after apply");
 check(draft().start==applyStart,"W11D APPLY failed to persist edit");
 evidence("W11D real native VSTGUI mouse controls: marker/fade drag, zoom/scroll, gain/reset, preview/stop, apply/cancel PASS");
}
void testWaveEditorLongCaptures(){
 constexpr std::uint32_t rate=48000;
 constexpr std::size_t frames=rate*2;
 for(double factor:{1.0,1.3}){
   Fixture f;
   auto kit=namedKit('X',4);
   auto& wav=kit.wav[0];
   wav.resize(44+frames*8);
   auto put=[&](std::size_t at,std::uint32_t value,unsigned count){
     for(unsigned n=0;n<count;++n)wav[at+n]=static_cast<std::uint8_t>(value>>(8*n));
   };
   std::memcpy(wav.data(),"RIFF",4);put(4,static_cast<std::uint32_t>(wav.size()-8),4);
   std::memcpy(wav.data()+8,"WAVEfmt ",8);
   put(16,16,4);put(20,3,2);put(22,2,2);put(24,rate,4);
   put(28,rate*8,4);put(32,8,2);put(34,32,2);
   std::memcpy(wav.data()+36,"data",4);put(40,static_cast<std::uint32_t>(frames*8),4);
   for(std::size_t i=0;i<frames;++i){
     const double t=static_cast<double>(i)/rate;
     const float env=static_cast<float>((0.3+0.6*std::abs(std::sin(t*7.7)))*
                            (0.5+0.5*std::abs(std::sin(t*49.))));
     const float l=env*static_cast<float>(std::sin(t*730.));
     const float r=env*static_cast<float>(std::sin(t*920.+.4));
     std::memcpy(wav.data()+44+i*8,&l,4);
     std::memcpy(wav.data()+48+i*8,&r,4);
   }
   // Deliberately do not invent a sample name: embedded audio alone is sufficient.
   kit.names[0]="wave-qa-synthetic.wav"; // Required internal KitCodec identity; never shown as an imported filename.
   kit.sampleEdits[0]=SampleEdit{4800,86400,4800,7200,1.25f};
   f.kit(kit);f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',
      static_cast<std::uint8_t>(factor==1.?0:2)}};
   drumator::qa::ByteStream input(zoomState.data(),zoomState.size());
   check(f.c->setState(&input)==kResultOk&&v->zoomMatches(factor),
         "Wave long capture editor zoom");
   check(IntegrationAccess::openSampleEditorForCapture(*v,0),
         "Wave long capture loaded audio unavailable");
   check(IntegrationAccess::sampleEditorStatus(*v).find("EMPTY")==std::string::npos,
         "Wave editor incorrectly shows EMPTY for loaded audio");
   check(f.c->editorWaveSampleRate()==rate,"Wave editor source rate mismatch");
   captureVstgui(*v,factor==1.?"wave-long-fit-100":"wave-long-fit-130",factor);
   check(IntegrationAccess::zoomSample(*v,true),"Wave long zoom-in failed");
   captureVstgui(*v,factor==1.?"wave-long-zoom-100":"wave-long-zoom-130",factor);
 }
 evidence("Wave long sample: native 2s 48kHz stereo WAV, zoomed/full 100/130 and no EMPTY PASS");
}
void testVelocityDesignCaptures(){
 // Native GUI design fixture only. No production implementation changes.
 constexpr std::array<std::size_t,6> steps{{0,4,8,12,16,20}};
 constexpr std::array<std::uint8_t,6> values{{20,40,64,90,110,127}};
 for(const auto zoomFactor: {1.0,1.3}){
   Fixture f;
   auto kit=namedKit('X',4);
   kit.grid.fill(0);
   for(std::size_t n=0;n<steps.size();++n)kit.grid[steps[n]]=values[n];
   f.kit(kit);
   f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   const std::uint8_t zoomCode=zoomFactor==1.0?0:2;
   std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',zoomCode}};
   drumator::qa::ByteStream input(zoomState.data(),zoomState.size());
   check(f.c->setState(&input)==kResultOk&&v->zoomMatches(zoomFactor),
         "Velocity visual QA: actual host/editor zoom mismatch");
   click(*v,1248.,UiCompactLayout::header+88.);
   check(IntegrationAccess::velocityExpanded(*v,0),
         "Velocity visual QA: first row did not expand");
   for(std::size_t n=0;n<kSteps;++n){
     std::uint8_t expected=0;
     for(std::size_t i=0;i<steps.size();++i)if(steps[i]==n)expected=values[i];
     check(f.c->velocity(0,0,n)==expected,
           "Velocity visual QA: fixture values differ from actual controller state");
   }
   captureVstgui(*v,zoomFactor==1.0?"velocity-design-100":"velocity-design-130",zoomFactor);
 }
 evidence("Velocity design reference: six native velocity levels 20/40/64/90/110/127 at real 100% and 130% editor zoom");
}
void testGui49MusicalFourSampleCapture(){
 Fixture f;KitState kit;kit.visible=4;kit.selected=0;kit.grid.fill(0);
 constexpr const char* labels[]={"KICK","SNARE","CLOSED HI-HAT","OPEN HI-HAT"};
 constexpr const char* names[]={"Kick.wav","Snare.wav","Closed_HiHat.wav","Open_HiHat.wav"};
 for(std::size_t row=0;row<4;++row){
   kit.names[row]=names[row];kit.displayNames[row]=labels[row];
   auto wav=constantStereo(0.f,0.f);
   constexpr float pi=3.14159265358979323846f;
   for(std::size_t n=0;n<1024;++n){
     const float time=static_cast<float>(n)/48000.f;
     const float decay=std::exp(-time*(row==0?110.f:row==1?170.f:row==2?260.f:85.f));
     const float freq=row==0?65.f:row==1?180.f:row==2?2200.f:1250.f;
     const float signal=decay*std::sin(2.f*pi*freq*time);
     const float l=signal*.68f,r=signal*.61f;
     std::memcpy(wav.data()+44+n*8,&l,4);
     std::memcpy(wav.data()+48+n*8,&r,4);
   }
   kit.wav[row]=std::move(wav);
 }
 // Two 4/4 bars of 16th notes. Velocity is the actual occupied-step value.
 auto set=[&](std::size_t row,std::size_t step,unsigned velocity){
    kit.grid[row*kSteps+step]=static_cast<std::uint8_t>(velocity);
 };
 for(auto step:{0u,8u,16u,24u})set(0,step,115);
 for(auto step:{4u,12u,20u,28u})set(1,step,112);
 for(unsigned step=0;step<32;step+=2)set(2,step,(step%8==6)?72:98);
 for(auto step:{14u,30u})set(3,step,86);
 kit.patternLengths[0]=32;
 f.kit(kit);prepareAudio(*f.p);f.open();
 auto* view=IntegrationAccess::view(*f.c,0);
 const auto folder=std::filesystem::current_path()/"125A-GUI49-real-import-capture";
 std::filesystem::create_directories(folder);
 for(std::size_t row=0;row<4;++row){
   const auto file=folder/names[row];
   std::ofstream stream(file,std::ios::binary|std::ios::trunc);
   stream.write(reinterpret_cast<const char*>(kit.wav[row].data()),
                static_cast<std::streamsize>(kit.wav[row].size()));
   check(stream.good(),"GUI49 fixture WAV could not be written");
   stream.close();
   check(IntegrationAccess::importPathViaView(*view,row,pathToUtf8(file)),
         "GUI49 sample not accepted through normal WAV import");
 }
 check(IntegrationAccess::sampleEditorStatus(*view).find("SAMPLE LOADED")!=std::string::npos,
       "GUI49 screenshot must show the authentic loaded-sample status");
 for(std::size_t row=0;row<4;++row){
   check(f.c->hasSample(row)&&f.c->label(row)==labels[row],
         "GUI49 musical capture must contain four actual loaded stereo samples");
 }
 for(std::size_t row=0;row<4;++row)
   for(std::size_t step=0;step<32;++step)
     check(f.c->velocity(0,row,step)==kit.grid[row*kSteps+step],
           "GUI49 musical capture actual 32-step state mismatch");
 const std::array<std::uint8_t,5> z{{'D','Z','M','2',2}};
 drumator::qa::ByteStream state(const_cast<std::uint8_t*>(z.data()),z.size());
 check(f.c->setState(&state)==kResultOk&&view->zoomMatches(1.3),
       "GUI49 musical screenshot must use real attached 130% zoom");
 captureVstgui(*view,"gui49-four-real-samples-musical-130",1.3);
 evidence("GUI49 musical native 130pct: four decoded stereo WAVs, true names, occupied 32-step velocities and attached zoom PASS");
}
void test13MeterTransportAndMultiEditor(){
 // A transient between UI polls must survive a subsequent silent block.
 OutputMeter accumulator;
 std::array<float,8> transientL{{.9f,.6f,0.f,0.f,0.f,0.f,0.f,0.f}};
 std::array<float,8> transientR{{.25f,0.f,0.f,0.f,0.f,0.f,0.f,0.f}};
 std::array<float,8> silenceL{},silenceR{};
 float* hotBuffers[]{transientL.data(),transientR.data()};
 float* quietBuffers[]{silenceL.data(),silenceR.data()};
 AudioBusBuffers meterBus{};meterBus.numChannels=2;meterBus.channelBuffers32=hotBuffers;
 ProcessData meterData{};meterData.numSamples=8;meterData.numOutputs=1;
 meterData.outputs=&meterBus;
 accumulator.observe(meterData,0);
 meterBus.channelBuffers32=quietBuffers;
 accumulator.observe(meterData,0);
 check(OutputMeter::decode(accumulator.mainL.exchange(0))>.89f&&
       OutputMeter::decode(accumulator.mainR.exchange(0))>.24f&&
       accumulator.blockSequence.load()==2,
       "S13 short real peak lost between UI polls after silent block");
 // Distinguish an exact 0dBFS stem from two simultaneous overrange stems.
 OutputMeter multi;
 std::array<float,8> stemA{},stemB{},stemRight{};
 stemA[0]=1.f;stemB[0]=1.4f;
 float* channelsA[]{stemA.data(),stemRight.data()};
 float* channelsB[]{stemB.data(),stemRight.data()};
 AudioBusBuffers multiBuses[3]{};
 multiBuses[0]=meterBus;multiBuses[0].channelBuffers32=quietBuffers;
 multiBuses[1].numChannels=2;multiBuses[1].channelBuffers32=channelsA;
 multiBuses[2].numChannels=2;multiBuses[2].channelBuffers32=channelsB;
 ProcessData stemData{};stemData.numSamples=8;stemData.numOutputs=3;
 stemData.outputs=multiBuses;
 multi.observe(stemData,(1u<<1)|(1u<<2));
 check(multi.clipMask.load()==(1u<<2)&&
       multi.fullScaleMask.load()==((1u<<1)|(1u<<2)),
       "S13 exact 0dBFS stem incorrectly classified as CLIP");
 stemA[0]=1.25f;
 multi.clearClips();
 multi.observe(stemData,(1u<<1)|(1u<<2));
 check(multi.clipMask.load()==((1u<<1)|(1u<<2)),
       "S13 simultaneous active OUT2/OUT3 clip mask lost");
 multi.clearClips();
 multi.observe(stemData,1u<<1);
 check(multi.clipMask.load()==(1u<<1),
       "S13 deactivated OUT3 must not set CLIP");
 Fixture f;auto kit=namedKit('Q',2);
 kit.wav[0]=constantStereo(.8f,-.3f);kit.masterVolume=200;
 f.kit(kit);prepareAudio(*f.p);
 f.open();f.open();
 auto* a=IntegrationAccess::view(*f.c,0);
 auto* b=IntegrationAccess::view(*f.c,1);
 check(f.c->previewSample(0),"S13 meter first preview");
 std::array<float,256> testLeft{},testRight{};
 float* channels[]{testLeft.data(),testRight.data()};
 AudioBusBuffers bus{};bus.numChannels=2;bus.channelBuffers32=channels;
 ProcessData rendered{};rendered.numSamples=256;rendered.numOutputs=1;
 rendered.outputs=&bus;rendered.symbolicSampleSize=kSample32;
 check(f.p->process(rendered)==kResultOk,"S13 processor render failed");
 // Force an unavailable process-local meter registry on the Controller
 // while keeping the real two-component VST3 connection intact.
 auto* linkMessage=f.c->allocateMessage();
 check(linkMessage!=nullptr,"S13 meter link message allocation");
 linkMessage->setMessageID("Drumator.OutputMeterLink");
 check(linkMessage->getAttributes()->setInt("MeterId",922337)==kResultOk,
       "S13 invalid link injection");
 check(f.c->notify(linkMessage)==kResultOk&&f.c->meterLink()==nullptr,
       "S13 null address-space fallback did not activate");
 linkMessage->release();
 IntegrationAccess::pollMeter(*a);
 IntegrationAccess::pollMeter(*b);
 const auto first=IntegrationAccess::meterPaintState(*a);
 const auto second=IntegrationAccess::meterPaintState(*b);
 check(first[0]>.5f&&second[0]>.5f&&
       std::abs(first[0]-second[0])<.001f&&
       std::abs(first[1]-second[1])<.001f,
       "S13 both editors must display same real MAIN L/R without shared pointer");
 check((f.c->meterState().clip&1u)!=0,"S13 cross-process-compatible CLIP missing");
 // Force an allocation failure on the real GUI meter callback. A thrown
 // bad_alloc would otherwise escape the noexcept VSTGUI timer and terminate.
 bool meterCallbackEscaped=false;
 allocationFail=0;
 try {IntegrationAccess::pollMeter(*a);}
 catch(...) {meterCallbackEscaped=true;}
 const bool meterFaultInjected=allocationFail==-1;
 allocationFail=-1;
 check(meterFaultInjected&&!meterCallbackEscaped,
       "S13 meter snapshot allocation failure escaped noexcept GUI callback");
 bool clearEscaped=false;
 allocationFail=0;
 try {(void)f.c->clearMeterClips();}
 catch(...) {clearEscaped=true;}
 const bool clearFaultInjected=allocationFail==-1;
 allocationFail=-1;
 check(clearFaultInjected&&!clearEscaped,
       "S13 CLIP reset allocation failure escaped host callback");
 click(*b,UiCompactLayout::meterResetLeft+2.,80.);
 check(f.c->meterState().clip==0,"S13 GUI IPC clip reset failed");
 // Inject real C++ allocation failures in the Processor's reply, not
 // only when the Controller allocates its outgoing MeterRequest. Even an
 // unsuccessful later setInt/sendMessage must leave both peaks pending.
 auto* meterQuery=f.c->allocateMessage();
 check(meterQuery!=nullptr,"S13 fault query allocation");
 meterQuery->setMessageID("Drumator.MeterRequest");
 auto processorMeter=IntegrationAccess::processorMeter(*f.p);
 check(processorMeter!=nullptr,"S13 real Processor meter missing");
 const auto expectedLeft=OutputMeter::encode(.83f);
 const auto expectedRight=OutputMeter::encode(.41f);
 unsigned laterReplyFailures=0;
 for(long faultIndex=0;faultIndex<32;++faultIndex) {
   processorMeter->mainL.store(expectedLeft,std::memory_order_release);
   processorMeter->mainR.store(expectedRight,std::memory_order_release);
   processorMeter->blockSequence.fetch_add(1,std::memory_order_release);
   allocationFail=faultIndex;
   Steinberg::tresult replyStatus=Steinberg::kResultFalse;
   bool escaped=false;
   try {replyStatus=f.p->notify(meterQuery);}
   catch(...) {escaped=true;}
   const bool injected=allocationFail==-1;
   allocationFail=-1;
   if(escaped)
     throw std::runtime_error("S13 Processor meter reply fault escaped host notify at allocation index "+
                              std::to_string(faultIndex));
   if(injected&&replyStatus!=Steinberg::kResultOk) {
     check(processorMeter->mainL.load(std::memory_order_acquire)>=expectedLeft&&
           processorMeter->mainR.load(std::memory_order_acquire)>=expectedRight,
           "S13 failed reply consumed undelivered transients");
     if(faultIndex>=3)++laterReplyFailures;
   }
 }
 check(laterReplyFailures>0,"S13 later reply allocation fault not exercised");
 // Positive control: a successful response presents the peaks in the real
 // controller cache, then consumes them. It must not resend stale levels.
 processorMeter->mainL.store(expectedLeft,std::memory_order_release);
 processorMeter->mainR.store(expectedRight,std::memory_order_release);
 processorMeter->blockSequence.fetch_add(1,std::memory_order_release);
 check(f.p->notify(meterQuery)==Steinberg::kResultOk&&
       f.c->meterState().left>=expectedLeft&&
       f.c->meterState().right>=expectedRight&&
       processorMeter->mainL.load(std::memory_order_acquire)==0&&
       processorMeter->mainR.load(std::memory_order_acquire)==0,
       "S13 successful reply did not deliver and consume the peaks");
 meterQuery->release();
 evidence("S13 failed Processor meter reply retains peaks; successful delivery consumes PASS");
 for(int i=0;i<48;++i){IntegrationAccess::pollMeter(*a);IntegrationAccess::pollMeter(*b);}
 check(IntegrationAccess::meterPaintState(*a)[0]<.01f&&
       IntegrationAccess::meterPaintState(*b)[0]<.01f,
       "S13 stop with no callback must decay both GUI meters");
 evidence("S13 UI meter IPC without local registry, independent two-view peaks, sticky CLIP/clear, stop decay PASS");
}
void test11IMeterStereoAndStems(){
 // Independent exact buffer oracle: MAIN channel asymmetry, 0dBFS versus
 // true over-range, stem-only clipping and disabled stem guard.
 OutputMeter measured;
 std::array<float,8> left{{0.f,.125f,.5f,.99f,1.f,1.25f,.125f,0.f}};
 std::array<float,8> right{{0.f,.25f,.125f,.0625f,.5f,.0625f,0.f,0.f}};
 std::array<float,8> quiet{},quietR{},stemL{},stemR{};
 stemL.fill(1.375f);stemR.fill(.1f);
 float* mainChannels[]{left.data(),right.data()};
 float* stemChannels[]{stemL.data(),stemR.data()};
 AudioBusBuffers bus[2]{};
 bus[0].numChannels=2;bus[0].channelBuffers32=mainChannels;
 bus[1].numChannels=2;bus[1].channelBuffers32=stemChannels;
 ProcessData data{};data.numSamples=8;data.numOutputs=2;data.outputs=bus;
 measured.observe(data,0);
 check(measured.clipMask.load()==1u&&measured.fullScaleMask.load()==1u,
       "M11I MAIN exact/above-0dBFS distinctions or disabled STEM");
 check(std::abs(OutputMeter::decode(measured.mainL.exchange(0))-1.25f)<.0002f&&
       std::abs(OutputMeter::decode(measured.mainR.exchange(0))-.5f)<.0002f,
       "M11I independent stereo MAIN sample peaks");
 measured.clearClips();
 // Boundary-only MAIN exactly reaches 0 dBFS without exceeding full scale.
 std::array<float,8> thresholdL{{0.f,.2f,1.f,.4f,0.f,0.f,0.f,0.f}};
 float* thresholdChannels[]{thresholdL.data(),right.data()};
 bus[0].channelBuffers32=thresholdChannels;
 measured.observe(data,0);
 check(measured.clipMask.load()==0u&&measured.fullScaleMask.load()==1u,
       "M11I exact 0dBFS must not be classified as above-full-scale CLIP");
 measured.clearClips();(void)measured.mainL.exchange(0);(void)measured.mainR.exchange(0);
 float* silentChannels[]{quiet.data(),quietR.data()};
 bus[0].channelBuffers32=silentChannels;
 measured.observe(data,1u<<1);
 check(measured.clipMask.load()==2u&&
       measured.mainL.exchange(0)==0u&&measured.mainR.exchange(0)==0u,
       "M11I stem-only overrange must warn with silent MAIN");
 measured.clearClips();
 check(measured.clipMask.load()==0&&measured.fullScaleMask.load()==0,
       "M11I manual CLIP reset");
 bus[0].channelBuffers32=mainChannels;
 data.numSamples=0;measured.observe(data,0);
 check(measured.clipMask.load()==0,"M11I zero-frame callback changed latch");

 // Actual Processor -> post-master output PCM -> atomic meter -> Controller link.
 Fixture f;auto kit=namedKit('X',2);
 kit.wav[0]=constantStereo(.75f,-.375f);kit.masterVolume=200;
 f.kit(kit);prepareAudio(*f.p);
 auto meter=f.c->meterLink();
 check(meter!=nullptr,"M11I Processor-to-Controller meter connection missing");
 check(f.c->previewSample(0),"M11I real row preview failed");
 std::array<float,256> actualL{},actualR{};
 float* actualChannels[]{actualL.data(),actualR.data()};
 AudioBusBuffers out{};out.numChannels=2;out.channelBuffers32=actualChannels;
 ProcessData block{};block.numSamples=256;block.numOutputs=1;block.outputs=&out;
 block.symbolicSampleSize=kSample32;
 check(f.p->process(block)==kResultOk,"M11I actual audio process failed");
 float expectedL=0.f,expectedR=0.f;
 for(unsigned i=0;i<256;++i) {
   expectedL=std::max(expectedL,std::abs(actualL[i]));
   expectedR=std::max(expectedR,std::abs(actualR[i]));
 }
 check(expectedL>1.f&&expectedR>0.f,"M11I actual master gain/preview not audible");
 const float reportedL=OutputMeter::decode(meter->mainL.exchange(0));
 const float reportedR=OutputMeter::decode(meter->mainR.exchange(0));
 check(std::abs(expectedL-reportedL)<.001f&&std::abs(expectedR-reportedR)<.001f,
       "M11I meter does not match actual post-master PCM");
 check(meter->clipMask.load()&1u,"M11I actual MAIN overrange was not latched");
 meter->clearClips();
 f.open();auto* v=IntegrationAccess::view(*f.c,0);
 check(f.c->previewSample(0),"M11I second preview for native GUI meter");
 check(f.p->process(block)==kResultOk,"M11I second real master render");
 IntegrationAccess::pollMeter(*v);
 check(IntegrationAccess::meterPaintState(*v)[0]>1.f,
       "M11I GUI does not receive real MAIN peak");
 for(double zoomFactor:{1.,1.3}){
   const std::uint8_t z=zoomFactor==1.?0:2;
   std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',z}};
   drumator::qa::ByteStream in(zoomState.data(),zoomState.size());
   check(f.c->setState(&in)==kResultOk&&v->zoomMatches(zoomFactor),
         "M11I native GUI zoom contract");
   captureVstgui(*v,zoomFactor==1.?"gui49-11i-main-meter-100":"gui49-11i-main-meter-130",zoomFactor);
 }
 click(*v,1264,79);
 check(meter->clipMask.load()==0,"M11I GUI left-click failed to clear clip");

 // Render a real independently routed stem with MAIN silent.
 Fixture stems;auto stemKit=namedKit('X',2);
 stemKit.wav[1]=constantStereo(.75f,-.375f);stemKit.masterVolume=200;
 stemKit.output[1]=1;
 stems.kit(stemKit);prepareAudio(*stems.p);
 check(stems.p->activateBus(MediaTypes::kAudio,BusDirections::kOutput,1,true)==kResultOk,
       "M11I stem host activation failed");
 check(stems.c->previewSample(1),"M11I stem preview rejected");
 std::array<float,256> mainL{},mainR{},outL{},outR{};
 float* mainOut[]{mainL.data(),mainR.data()};
 float* stemOut[]{outL.data(),outR.data()};
 AudioBusBuffers buses[2]{};buses[0].numChannels=2;buses[0].channelBuffers32=mainOut;
 buses[1].numChannels=2;buses[1].channelBuffers32=stemOut;
 ProcessData multi{};multi.numSamples=256;multi.numOutputs=2;
 multi.outputs=buses;multi.symbolicSampleSize=kSample32;
 check(stems.p->process(multi)==kResultOk,"M11I actual stem process failed");
 float maxStem=0.f,maxMain=0.f;
 for(unsigned i=0;i<256;++i){
   maxStem=std::max(maxStem,std::abs(outL[i]));
   maxMain=std::max(maxMain,std::abs(mainL[i]));
 }
 auto stemMeter=stems.c->meterLink();
 check(stemMeter&&maxStem>1.f&&maxMain<1e-5f&&
       (stemMeter->clipMask.load()&(1u<<1))&&
       stemMeter->mainL.exchange(0)==0,
       "M11I OUT 2-only true overrange with silent MAIN missing");
 stems.open();auto* vs=IntegrationAccess::view(*stems.c,0);
 IntegrationAccess::pollMeter(*vs);
 const std::array<std::uint8_t,5> scale{{'D','Z','M','2',2}};
 drumator::qa::ByteStream state(const_cast<std::uint8_t*>(scale.data()),scale.size());
 check(stems.c->setState(&state)==kResultOk&&vs->zoomMatches(1.3),
       "M11I stem CLIP capture 130pct");
 captureVstgui(*vs,"gui49-11i-out2-clip-130",1.3);
 evidence("M11I MAIN L/R real post-ramp samples, separate 0dBFS/overrange, 16-bus inactive mask, OUT2-only clip and click reset PASS");
}
void test11IHeaderPatternCentered(){
  // Fixed centered product title is the alignment datum for eight A-H keys.
  constexpr double patternAxis=UiCompactLayout::patternStart+
      (7.*UiCompactLayout::patternPitch+UiCompactLayout::patternButtonWidth)/2.;
  check(patternAxis==UiCompactLayout::width/2.,
        "GUI 11I PATTERN A-H group not centered below DRUMATOR");
  check(UiCompactLayout::barTwo.right<UiCompactLayout::patternStart&&
        UiCompactLayout::patternStart+7.*UiCompactLayout::patternPitch+
          UiCompactLayout::patternButtonWidth<UiCompactLayout::patternTools.left&&
        UiCompactLayout::patternTools.right<UiCompactLayout::songToggle.left&&
        UiCompactLayout::songToggle.right<UiCompactLayout::songEdit.left&&
        UiCompactLayout::songEdit.right<UiCompactLayout::globalKnobCenters[2]-50.,
        "GUI 11I moved header controls collide");
  check(UiCompactLayout::meterRight+7.<=UiCompactLayout::width-12.&&
        UiCompactLayout::meterClipRight<=UiCompactLayout::width-12.&&
        UiCompactLayout::meterLeft>UiCompactLayout::globalKnobCenters[2]+72.,
        "GUI 11I meter clips chassis or MASTER readout");
  check(UiCompactLayout::meterResetLeft<UiCompactLayout::meterLeft&&
        UiCompactLayout::meterResetRight>UiCompactLayout::meterRight+7.,
        "GUI 11I new meter reset click zone misses LED columns");
  evidence("GUI 11I centered PATTERN A-H, following groups, MASTER and meter margins PASS");
}
void test11IRotaryMinCenterMax(){
 struct ControlState {
   const char* name;
   std::uint8_t swing,humanize,master,volume,pan,pitch;
 };
 constexpr ControlState cases[]{
   {"minimum",0,0,0,0,0,0},
   {"center",25,50,100,100,100,24},
   {"maximum",50,100,200,200,200,48}
 };
 for(const auto& state:cases){
   Fixture f;auto kit=namedKit('X',4);
   kit.swing=state.swing;kit.humanize=state.humanize;
   kit.masterVolume=state.master;kit.gain[0]=state.volume;
   kit.pan[0]=state.pan;kit.pitch[0]=state.pitch;
   f.kit(kit);f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   check(f.c->swingPercent()==state.swing&&
         f.c->humanizePercent()==state.humanize&&
         f.c->masterVolume()==state.master&&
         f.c->rowGain(0)==state.volume&&
         f.c->rowPan(0)==static_cast<int>(state.pan)-100&&
         f.c->rowPitch(0)==static_cast<int>(state.pitch)-24,
         "M11I rotary numeric min/center/max does not match kit/controller state");
   for(double factor:{1.,1.3}){
     const std::uint8_t z=factor==1.?0:2;
     std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',z}};
     drumator::qa::ByteStream in(zoomState.data(),zoomState.size());
     check(f.c->setState(&in)==kResultOk&&v->zoomMatches(factor),
           "M11I rotary actual native zoom invalid");
     captureVstgui(*v,std::string("gui49-11i-rotary-")+state.name+
                     (factor==1.?"-100":"-130"),factor);
   }
 }
 Fixture f;auto kit=namedKit('X',4);
 kit.swing=40;kit.humanize=80;kit.masterVolume=175;
 kit.gain[0]=180;kit.pan[0]=170;kit.pitch[0]=41;
 f.kit(kit);prepareAudio(*f.p);f.open();auto* v=IntegrationAccess::view(*f.c,0);
 for(const auto x:{UiCompactLayout::globalKnobCenters[0],
                    UiCompactLayout::globalKnobCenters[1],
                    UiCompactLayout::globalKnobCenters[2]})
   click(*v,x,UiCompactLayout::globalKnobY,VSTGUI::kLButton|VSTGUI::kDoubleClick);
 const auto rowY=UiCompactLayout::header+UiCompactLayout::rowKnobY;
 for(std::size_t i=0;i<3;++i)
   click(*v,UiCompactLayout::rowKnobCenters[i],rowY,
         VSTGUI::kLButton|VSTGUI::kDoubleClick);
 const auto resetDiagnostic=std::string("SWING=")+std::to_string(f.c->swingPercent())+
   " HUMANIZE="+std::to_string(f.c->humanizePercent())+
   " MASTER="+std::to_string(f.c->masterVolume())+
   " VOL="+std::to_string(f.c->rowGain(0))+
   " PAN="+std::to_string(f.c->rowPan(0))+
   " PITCH="+std::to_string(f.c->rowPitch(0));
 check(f.c->swingPercent()==0&&f.c->humanizePercent()==0&&
       f.c->masterVolume()==100&&f.c->rowGain(0)==100&&
       f.c->rowPan(0)==0&&f.c->rowPitch(0)==0,
       (std::string("M11I double-click reset values: ")+resetDiagnostic).c_str());
 evidence("M11I six rotary types min/center/max, exact numeric state, real native 100/130 screenshots, six double-click defaults PASS");
}
void test11IMonoFlacImport(){
 static constexpr std::uint8_t monoFlac[]={0x66,0x4c,0x61,0x43,0x00,0x00,0x00,0x22,0x10,0x00,0x10,0x00,0x00,0x00,0x0b,0x00,0x00,0x0b,0x0b,0xb8,0x00,0xf0,0x00,0x00,0x04,0x00,0xba,0x31,0x49,0x4f,0xa5,0x46,0x04,0x02,0x5c,0x12,0x84,0x74,0xa9,0x79,0x53,0xe4,0x84,0x00,0x00,0x28,0x20,0x00,0x00,0x00,0x72,0x65,0x66,0x65,0x72,0x65,0x6e,0x63,0x65,0x20,0x6c,0x69,0x62,0x46,0x4c,0x41,0x43,0x20,0x31,0x2e,0x35,0x2e,0x30,0x20,0x32,0x30,0x32,0x35,0x30,0x32,0x31,0x31,0x00,0x00,0x00,0x00,0xff,0xf8,0xaa,0x08,0x00,0xed,0x01,0x00,0x09,0xa4,0xb8};
 // The exact 97-byte file passes independent libFLAC 1.5.0 'flac -t'.
 const auto path=std::filesystem::current_path()/"125A-11I-mono.flac";
 {std::ofstream file(path,std::ios::binary|std::ios::trunc);
  file.write(reinterpret_cast<const char*>(monoFlac),sizeof(monoFlac));
  check(file.good(),"M11I failed to create Mono FLAC fixture");}
 Fixture f;f.kit(namedKit('X',2));prepareAudio(*f.p);f.open();
 auto* view=IntegrationAccess::view(*f.c,0);
 check(IntegrationAccess::importPathViaView(*view,0,pathToUtf8(path)),
       "M11I normal mono-FLAC file import rejected");
 check(IntegrationAccess::openSampleEditorForCapture(*view,0),
       "M11I mono-FLAC editor open failed");
 check(IntegrationAccess::editorFrameCount(*view)==1024&&
       !IntegrationAccess::editorStereo(*view),"M11I mono-FLAC channels/frames incorrect");
 for(const auto& v:IntegrationAccess::editorLeft(*view))
   check(std::abs(v.minimum-.25f)<1e-6f&&std::abs(v.maximum-.25f)<1e-6f,
         "M11I decoded mono-FLAC PCM level incorrect");
 evidence("M11I libFLAC validated mono 48kHz/16bit/1024 frames, 256 PCM waveform bins, normal import and editor PASS");
}
void testVisualRender(){
 for(const auto zoom: {1.0,1.2,1.3,1.5}){
   Fixture f;auto kit=namedKit('X',4);
   kit.displayNames[0]="KICK_LONG_AUDIO_SAMPLE_NAME_IN_2026.wav";
   f.kit(kit);f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   // Raster scaling alone previously produced a misleading 150%-named
   // screenshot whose actual plugin zoom indicator still read 100%.
   // Exercise the real attached editor zoom before taking each native render.
   const std::uint8_t zoomCode=zoom==1.?0:zoom==1.2?1:zoom==1.3?2:3;
   std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',zoomCode}};
   drumator::qa::ByteStream zoomInput(zoomState.data(),zoomState.size());
   check(f.c->setState(&zoomInput)==kResultOk&&v->zoomMatches(zoom),
         "native screenshot zoom state does not match raster scale");
   captureVstgui(*v,zoom==1.?"v2-four-rows-100":zoom==1.2?"v2-four-rows-120":
        zoom==1.3?"v2-four-rows-130":"v2-four-rows-150",zoom);
   if(zoom==1.||zoom==1.3){
     const auto top=IntegrationAccess::rowTop(*v,0);
     click(*v,61.,top+89.);
     check(IntegrationAccess::popup(*v)==6&&IntegrationAccess::popupRow(*v)==0&&
           v->zoomMatches(zoom),"native ROW menu wrong zoom or hitbox");
     captureVstgui(*v,zoom==1.?"v1-row-menu-100":"v1-row-menu-130",zoom);
     click(*v,220.,top+95.);
     check(IntegrationAccess::popup(*v)==0,"ROW menu did not dismiss");
   }
 }
 {
   Fixture f;f.kit(namedKit('X',16));f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   captureVstgui(*v,"v2-16-rows-page-1",1.0);
   // GUI46: inspect bottom fascia and group navigation with 16 instruments
   // at actual host zoom (not merely an upscaled raster).
   std::array<std::uint8_t,5> footerZoom{{'D','Z','M','2',2}};
   drumator::qa::ByteStream footerZoomInput(footerZoom.data(),footerZoom.size());
   check(f.c->setState(&footerZoomInput)==kResultOk&&v->zoomMatches(1.3),
         "GUI46 native 16-row footer capture must use true 130pct zoom");
   captureVstgui(*v,"gui46-16-rows-130",1.3);
   // GUI47: actual 5-8 navigation state at 130%; footer counter,
   // viewport and vertical scrollbar all read the same firstRow_.
   const auto bottomPage1=v->getViewSize().getHeight()-UiCompactLayout::footer;
   click(*v,1190.,bottomPage1+17.);
   check(IntegrationAccess::firstRow(*v)==4&&IntegrationAccess::selectedRow(*v)==4,
         "GUI47 right footer arrow did not select instrument group 5-8");
   captureVstgui(*v,"gui47-instruments-5-8-130",1.3);
   const auto bottomPage2=v->getViewSize().getHeight()-UiCompactLayout::footer;
   click(*v,1100.,bottomPage2+17.);
   check(IntegrationAccess::firstRow(*v)==0,
         "GUI47 left footer arrow did not return to instruments 1-4");

   std::array<std::uint8_t,5> defaultZoom{{'D','Z','M','2',0}};
   drumator::qa::ByteStream defaultZoomInput(defaultZoom.data(),defaultZoom.size());
   check(f.c->setState(&defaultZoomInput)==kResultOk&&v->zoomMatches(1.0),
         "GUI46 reset editor zoom after footer capture");
   click(*v,900.,114.);
   captureVstgui(*v,"v2-pattern-actions-menu",1.0);
   click(*v,15.,390.);
   click(*v,1150.,20.);
   captureVstgui(*v,"v2-zoom-menu",1.0);
   click(*v,15.,390.);
   click(*v,1248,UiCompactLayout::header+88.);
   check(IntegrationAccess::velocityExpanded(*v,0),"V2 row 1 expansion failed");
   const auto row2=IntegrationAccess::rowTop(*v,1);
   click(*v,1248,row2+88.);
   check(IntegrationAccess::velocityExpanded(*v,0)&&
         IntegrationAccess::velocityExpanded(*v,1),"V2 expansion changed another row");
   captureVstgui(*v,"v2-two-velocity-rows",1.0);
   click(*v,289.,UiCompactLayout::header+19.); // explicit SAMPLE actions menu
   captureVstgui(*v,"v2-sample-actions-menu",1.0);
   click(*v,15.,400.); // outside dismisses without triggering a step
   click(*v,575.,UiCompactLayout::header+19.); // MAIN / OUT selector
   captureVstgui(*v,"v2-output-menu",1.0);
   click(*v,15.,400.);
   // The CHOKE label is at rowTop+52..73, but the clickable selector is
   // rowTop+73..105. The former +69 capture silently recorded a CLOSED
   // popup identical to v2-two-velocity-rows; reject that false visual PASS.
   click(*v,592.,UiCompactLayout::header+88.); // center of actual CHOKE selector
   check(IntegrationAccess::popup(*v)==4&&IntegrationAccess::popupRow(*v)==0,
         "V2 choke visual oracle: menu did not open from actual selector");
   captureVstgui(*v,"v2-choke-menu",1.0);
   click(*v,15.,400.);
   check(IntegrationAccess::popup(*v)==0,"V2 choke popup did not dismiss");
   click(*v,425.,UiCompactLayout::header+19.); // row-local Fill menu
   captureVstgui(*v,"v2-fill-menu",1.0);
   click(*v,15.,400.);
   for(int i=0;i<2;++i){
     const auto bottom=v->getViewSize().getHeight()-UiCompactLayout::footer;
     click(*v,1190,bottom+17.);
   }
   check(IntegrationAccess::firstRow(*v)==8&&IntegrationAccess::selectedRow(*v)==8,
         "V2 third page selection/paging failed");
   captureVstgui(*v,"v2-16-rows-page-3",1.0);
   // Scrollbar must reach the last four actual instruments by thumb drag.
   click(*v,1271.,UiCompactLayout::scrollTop+1.);
   check(IntegrationAccess::firstRow(*v)==0,"scrollbar top");
   const double rail=IntegrationAccess::rowTop(*v,4)-8.-UiCompactLayout::scrollTop;
   VSTGUI::CPoint thumb(1271.,UiCompactLayout::scrollTop+rail/8.);
   v->onMouseDown(thumb,VSTGUI::CButtonState(VSTGUI::kLButton));
   thumb.y=UiCompactLayout::scrollTop+rail-2.;
   v->onMouseMoved(thumb,VSTGUI::CButtonState(VSTGUI::kLButton));
   v->onMouseUp(thumb,{});
   check(IntegrationAccess::firstRow(*v)==12,"scrollbar last page");
   captureVstgui(*v,"v2-16-rows-scrolled",1.0);
 }
 {
   Fixture f;f.kit(namedKit('X',4));f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   click(*v,1077,114.);
   captureVstgui(*v,"v2-song-board",1.0);
 }
 {
   // GUI39: actual editor state and native captured inactive/active indicators.
   Fixture f;f.kit(namedKit('X',4));f.open();
   auto* v=IntegrationAccess::view(*f.c,0);
   constexpr double zoom=1.3;
   std::array<std::uint8_t,5> zoomState{{'D','Z','M','2',2}};
   drumator::qa::ByteStream zoomInput(zoomState.data(),zoomState.size());
   check(f.c->setState(&zoomInput)==kResultOk&&v->zoomMatches(zoom),
         "GUI39 status raster requires actual 130pct editor zoom");
   captureVstgui(*v,"gui39-inactive-130",zoom);
   const double top=UiCompactLayout::header;
   click(*v,473.,top+22.); // MUTE row 0
   click(*v,513.,top+22.); // SOLO row 0
   click(*v,1246.,top+88.); // VEL row 0
   check(f.c->isRowMuted(0)&&f.c->isRowSolo(0)&&
         IntegrationAccess::velocityExpanded(*v,0),
         "GUI39 active lamps not backed by real controller/view states");
   captureVstgui(*v,"gui39-active-130",zoom);
 }
 evidence("V2 native VSTGUI vector controls and per-row Velocity windows captured");
}
void testPrecisionPanel(){
 Fixture f;f.kit(namedKit('X'));
 ProcessSetup setup{};
 setup.sampleRate=48000.;setup.maxSamplesPerBlock=256;
 setup.symbolicSampleSize=kSample32;setup.processMode=kRealtime;
 check(f.p->setupProcessing(setup)==kResultOk,"V2 pitch rate not initialized");
 f.open();auto* v=IntegrationAccess::view(*f.c,0);
 auto drag=[&](double x,double y,double dy){
   click(*v,x,y);
   VSTGUI::CPoint at(x,y+dy);
   v->onMouseMoved(at,VSTGUI::CButtonState(VSTGUI::kLButton));
   v->onMouseUp(at,{});
 };
 const double top=UiCompactLayout::header;
 drag(133.,top+UiCompactLayout::rowKnobY,-25.);
 check(f.c->rowGain(0)==117,"UX rotary gain 25px must increase 17%");
 click(*v,133.,top+UiCompactLayout::rowKnobY,VSTGUI::kLButton|VSTGUI::kControl);
 check(f.c->rowGain(0)==100,"V2 rotary Ctrl default not 100");
 {
   VSTGUI::CPoint pt(133.,top+UiCompactLayout::rowKnobY);
   v->onMouseDown(pt,VSTGUI::CButtonState(VSTGUI::kLButton|VSTGUI::kShift));
   pt.y-=15.;
   v->onMouseMoved(pt,VSTGUI::CButtonState(VSTGUI::kLButton));
   v->onMouseUp(pt,{});
   check(f.c->rowGain(0)==103,"UX fine knob sensitivity not latched");
   click(*v,133.,top+UiCompactLayout::rowKnobY,VSTGUI::kLButton|VSTGUI::kControl);
   check(f.c->rowGain(0)==100,"UX fine knob Ctrl reset failed");
 }
 // P11: actual VSTGUI PAN +100 must be reachable AND reversible;
 // regress the original owner report where a fully right-panned knob stuck.
 drag(254.,top+UiCompactLayout::rowKnobY,-150.);
 check(f.c->rowPan(0)==100,"P11 PAN +100 rotary endpoint unreachable");
 {
   ResizableMemoryIBStream saved;KitState state;
   check(f.p->getState(&saved)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),
             static_cast<std::size_t>(saved.getCursor()),state)&&state.pan[0]==200,
         "P11 +100 panorama not persisted by actual processor");
 }
 drag(254.,top+UiCompactLayout::rowKnobY,30.);
 check(f.c->rowPan(0)==80,"P11 PAN jammed at +100 on reverse drag");
 drag(254.,top+UiCompactLayout::rowKnobY,300.);
 check(f.c->rowPan(0)==-100,"P11 PAN -100 endpoint unreachable");
 {
   ResizableMemoryIBStream saved;KitState state;
   check(f.p->getState(&saved)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(saved.getData()),
             static_cast<std::size_t>(saved.getCursor()),state)&&state.pan[0]==0,
         "P11 -100 panorama not persisted by actual processor");
 }
 click(*v,254.,top+UiCompactLayout::rowKnobY,VSTGUI::kLButton|VSTGUI::kControl);
 check(f.c->rowPan(0)==0,"P11 PAN Ctrl reset to center failed");
 // Volume must also leave its maximum, and default must stay at 100%.
 drag(133.,top+UiCompactLayout::rowKnobY,-150.);
 check(f.c->rowGain(0)==200,"P11 VOL 200% endpoint unreachable");
 drag(133.,top+UiCompactLayout::rowKnobY,30.);
 check(f.c->rowGain(0)==180,"P11 VOL stuck at 200% after reversing");
 click(*v,133.,top+UiCompactLayout::rowKnobY,VSTGUI::kLButton|VSTGUI::kControl);
 check(f.c->rowGain(0)==100,"P11 VOL Ctrl reset not 100%");
 drag(375.,top+UiCompactLayout::rowKnobY,-60.);
 check(f.c->rowPitch(0)==24,"V2 pitch rotary maximum failed");
 drag(375.,top+UiCompactLayout::rowKnobY,30.);
 check(f.c->rowPitch(0)==12,"P11 PITCH stuck at +24 semitones");
 click(*v,375.,top+UiCompactLayout::rowKnobY,VSTGUI::kLButton|VSTGUI::kControl);
 check(f.c->rowPitch(0)==0,"P11 PITCH Ctrl reset failed");
 drag(496.,top+UiCompactLayout::rowKnobY,-250.);
 check(f.c->rowRandomVelocity(0)==0,"V2 button unexpectedly enabled legacy realtime random");
 // Existing projects with stored nonzero RT retain their usable old control.
 {
  Fixture legacy;
  auto oldKit=namedKit('X');
  oldKit.randomVelocity[0]=30;
  legacy.kit(oldKit);legacy.open();
  auto* oldView=IntegrationAccess::view(*legacy.c,0);
  VSTGUI::CPoint first(496.,top+UiCompactLayout::rowKnobY);
  oldView->onMouseDown(first,VSTGUI::CButtonState(VSTGUI::kLButton));
  VSTGUI::CPoint last(496.,top-182.);
  oldView->onMouseMoved(last,VSTGUI::CButtonState(VSTGUI::kLButton));
  oldView->onMouseUp(last,{});
  check(legacy.c->rowRandomVelocity(0)==100,
        "V2 legacy realtime random knob failed");
 }
 click(*v,597.,top+20.);
 click(*v,560.,top+36.+13.);
 check(f.c->rowOutput(0)==8,"V2 direct output selection OUT 9 failed");
 click(*v,590.,top+89.); // choke is beside the row's random-velocity knob
 click(*v,554.,top+36.+4.*27.+12.);
 check(f.c->rowChoke(0)==4,"V2 Choke 4 direct selection failed");
 click(*v,45.,top+UiCompactLayout::baseRow+12.);
 check(IntegrationAccess::selectedRow(*v)==1,
       "V2 instrument header selection failed");
 drag(133.,top+UiCompactLayout::baseRow+UiCompactLayout::rowKnobY,-25.);
 check(f.c->rowGain(0)==100&&f.c->rowGain(1)==117,
       "V2 rotary changed wrong instrument");
 click(*v,133.,top+UiCompactLayout::baseRow+UiCompactLayout::rowKnobY);
 check(IntegrationAccess::activeKnob(*v)==0,"V2 knob capture not active");
 check(f.recall(Fixture::encode(namedKit('Y')))==kResultOk,"V2 recall failed");
 VSTGUI::CPoint stale(133.,0.);
 v->onMouseMoved(stale,VSTGUI::CButtonState(VSTGUI::kLButton));
 v->onMouseUp(stale,{});
 check(f.c->rowGain(0)==130,"V2 stale knob edited restored kit");
 evidence("V2 rotary drag, exact defaults, menus, selection and recall PASS");
}
void testVelocityLaneExistingStepsOnly(){
 Fixture f;
 auto kit=namedKit('X',4);
 kit.grid.fill(0);
 kit.grid[0]=100;kit.grid[2]=90;
 f.kit(kit);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 const double top=UiCompactLayout::header;
 const double barY=top+UiCompactLayout::baseRow+4.;
 const auto target=UiCompactLayout::velocityAt(barY,top);
 check(target!=100&&target!=90,"UX fixture must choose a velocity distinct from both source notes");
 const auto col=[](std::size_t step)->double {
   return UiCompactLayout::stepX+static_cast<double>(step)*UiCompactLayout::stepWidth+8.;
 };
 click(*v,1248.,top+88.);
 check(IntegrationAccess::velocityExpanded(*v,0),
       "Velocity blank-gate fixture lane did not expand");
 click(*v,col(1),barY);
 check(f.c->velocity(0,0,1)==0&&
       !f.c->canUndoCurrentPattern(),
       "UX blank velocity bar created an unintended step or Undo");
 {
   VSTGUI::CPoint point(col(0),barY);
   v->onMouseDown(point,VSTGUI::CButtonState(VSTGUI::kLButton));
   point.x=col(1);
   v->onMouseMoved(point,VSTGUI::CButtonState(VSTGUI::kLButton));
   point.x=col(2);
   v->onMouseMoved(point,VSTGUI::CButtonState(VSTGUI::kLButton));
   v->onMouseUp(point,{});
 }
 check(f.c->velocity(0,0,0)==target&&
       f.c->velocity(0,0,2)==target,
       "UX occupied velocity bars did not change during row drag");
 check(f.c->velocity(0,0,1)==0,
       "UX dragging across empty column activated a new note");
 check(f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern(),
       "UX occupied velocity drag not undoable as one action");
 check(f.c->velocity(0,0,0)==100&&f.c->velocity(0,0,1)==0&&
       f.c->velocity(0,0,2)==90,
       "UX velocity drag Undo did not restore exact original steps");
 // Standard Step buttons remain the explicit way to add notes.
 click(*v,col(1),top+88.);
 check(f.c->velocity(0,0,1)>0,
       "UX standard Step click no longer enables a blank note");
 evidence("UX velocity lane edits occupied notes only; drag/Undo and explicit Step creation PASS");
}
void testRateChangeTransportReset(){
 Fixture f;
 ProcessSetup first{};
 first.sampleRate=48000.;
 first.maxSamplesPerBlock=256;
 first.symbolicSampleSize=kSample32;
 first.processMode=kRealtime;
 check(f.p->setupProcessing(first)==kResultOk,"T28 initial rate setup");
 IntegrationAccess::seedTransportClock(*f.p);
 first.sampleRate=44100.;
 check(f.p->setupProcessing(first)==kResultOk,"T28 rate reconfiguration");
 check(IntegrationAccess::clockReset(*f.p),
       "T28 rate change retained old expected PPQ or phase origin");
 evidence("T28 rate reconfiguration clears expected host PPQ and phase origin PASS");
}
void testRandomVelocityOneShot(){
 Fixture f;
 auto kit=namedKit('X',4);
 kit.grid.fill(0);
 kit.patternLengths[0]=16;
 kit.grid[0]=127;kit.grid[4]=90;kit.grid[12]=1;
 kit.grid[20]=64; // inactive second bar: must NOT change in 1T
 kit.grid[kSteps]=88; // different instrument: untouched
 kit.grid[kRows*kSteps]=100; // different pattern: untouched
 f.kit(kit);f.open();
 check(!f.c->randomizeRowVelocity(2),"RANDOM empty row should be inert");
 check(!f.c->canUndoCurrentPattern(),"RANDOM empty row incorrectly added undo");
 check(!f.c->randomizeRowVelocity(4),"RANDOM row outside visible kit allowed");
 check(f.c->randomizeRowVelocity(0),"RANDOM occupied row was rejected");
 for(const std::size_t step:{0u,4u,12u}){
   const auto before=kit.grid[step];
   const auto after=f.c->velocity(0,0,step);
   check(after>0&&after<=127&&after!=before,
         "RANDOM occupied velocity did not change visibly");
 }
 for(std::size_t step=0;step<kSteps;++step) {
   if(step!=0&&step!=4&&step!=12)
     check(f.c->velocity(0,0,step)==kit.grid[step],
           "RANDOM changed empty or hidden step");
 }
 check(f.c->velocity(0,1,0)==88&&f.c->velocity(1,0,0)==100,
       "RANDOM changed a different instrument or pattern");
 check(f.c->canUndoCurrentPattern(),"RANDOM missing single-pattern Undo");
 check(f.c->undoCurrentPattern(),"RANDOM cannot Undo action");
 for(std::size_t step=0;step<kSteps;++step)
   check(f.c->velocity(0,0,step)==kit.grid[step],
         "RANDOM undo did not restore complete source row");
 check(!f.c->canUndoCurrentPattern(),"RANDOM should consume exactly one undo");
 check(f.c->togglePatternLength(0),"RANDOM setup 2T");
 check(f.c->randomizeRowVelocity(0)&&f.c->velocity(0,0,20)!=64,
       "RANDOM 2T failed to edit second bar");
 check(f.c->undoCurrentPattern()&&f.c->velocity(0,0,20)==64,
       "RANDOM second-bar undo failed");
 check(f.c->togglePatternLength(0),"Button test return to one bar");
 auto* view=IntegrationAccess::view(*f.c,0);
 const auto before=f.c->velocity(0,0,0);
 click(*view,496.,UiCompactLayout::header+89.);
 check(f.c->velocity(0,0,0)!=before,"Button click did not randomize velocity");
 check(f.c->velocity(0,0,1)==0&&f.c->velocity(0,0,20)==64,"Button changed empty/inactive step");
 check(f.c->undoCurrentPattern()&&f.c->velocity(0,0,0)==before,"Button Undo failed");
 evidence("RANDOM one-shot occupied-only, 1T/2T, isolated row/pattern and Undo PASS");
}
void testFillActivePatternLength(){
 // V17: all nine FILL variants, including CLEAR, modify only the visible
 // musical bar in 1T while preserving stored bar-two notes for later 2T.
 Fixture f;
 KitState kit=namedKit('X',4);
 kit.grid.fill(0);
 kit.grid[kSteps+0]=90;kit.grid[kSteps+4]=100;
 kit.grid[kSteps+19]=37;kit.grid[kSteps+30]=65;
 kit.grid[0]=75; // other row, must be untouched
 kit.grid[kRows*kSteps+kSteps+2]=61; // other pattern, untouched
 const std::array<RowFill,9> modes{
   RowFill::Every2,RowFill::Every4,RowFill::Every8,
   RowFill::Snare24,RowFill::All,RowFill::Clear,RowFill::Offbeat8,
   RowFill::Gallop,RowFill::SnareGhost
 };
 auto state=[&]{
   ResizableMemoryIBStream stream;KitState saved;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                          static_cast<std::size_t>(stream.getCursor()),saved),
         "V17 actual processor project snapshot");
   return saved;
 };
 for(const auto active:{16u,32u}) {
   kit.patternLengths[0]=static_cast<std::uint8_t>(active);
   for(const auto mode:modes) {
     f.kit(kit);
     check(f.c->patternLength(0)==active,"V17 incorrect 1T/2T restore");
     check(f.c->fillRow(1,mode),"V17 actual controller FILL rejected");
     const auto saved=state();
     const auto expected=filledRow(mode);
     for(std::size_t index=0;index<kit.grid.size();++index){
       const auto row=index%(kRows*kSteps)/kSteps;
       const auto step=index%kSteps;
       const auto wanted=index<kRows*kSteps&&row==1&&step<active?
            expected[step]:kit.grid[index];
       check(saved.grid[index]==wanted,
             "V17 FILL/CLEAR modified hidden bar, other row or pattern");
     }
     check(f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern()&&
           state().grid==kit.grid,
           "V17 FILL/CLEAR Undo failed to restore complete original pattern");
   }
 }
 // Switch from 1T to 2T without clearing the stored second-bar material.
 kit.patternLengths[0]=16;
 f.kit(kit);
 check(f.c->fillRow(1,RowFill::Clear),"V17 1T CLEAR rejected");
 check(f.c->togglePatternLength(0)&&f.c->patternLength(0)==32,
       "V17 cannot restore 2T after FILL/CLEAR");
 check(f.c->velocity(0,1,19)==37&&f.c->velocity(0,1,30)==65,
       "V17 1T FILL/CLEAR destroyed previously hidden bar two");
 evidence("V17 nine row FILL/CLEAR variants preserve inactive steps, Undo and 2T return PASS");
}
void testVelocityInactiveBarProtection(){
 // V16: 1T editing operations must never change stored steps 17-32.
 // These steps must reappear at their original velocities on returning to 2T.
 Fixture f;auto kit=namedKit('X',4);
 kit.grid.fill(0);kit.patternLengths[0]=16;
 kit.grid[0]=90;kit.grid[4]=110;kit.grid[20]=64;
 kit.grid[kSteps+1]=60;kit.grid[kSteps+21]=33;
 kit.grid[kRows*kSteps]=77; // other pattern must remain unchanged
 f.kit(kit);f.open();
 auto gridAt=[&]{
   ResizableMemoryIBStream b;KitState state;
   check(f.p->getState(&b)==kResultOk&&
      KitCodec::decode(static_cast<const std::uint8_t*>(b.getData()),
                       static_cast<std::size_t>(b.getCursor()),state),
      "V16 actual processor state");
   return state;
 };
 check(f.c->patternLength(0)==16,"V16 1T fixture lost pattern length");
 check(f.c->varyPatternVelocity(),"V16 1T velocity vary rejected");
 const auto varied=gridAt();
 for(std::size_t row=0;row<kRows;++row)
   for(std::size_t step=16;step<kSteps;++step)
     check(varied.grid[row*kSteps+step]==kit.grid[row*kSteps+step],
           "V16 VELO VARIIEREN silently changed hidden bar two");
 check(varied.grid[kRows*kSteps]==77&&
       varied.grid[1]==0&&varied.grid[kSteps]==0,
       "V16 vary modified another pattern or introduced a note");
 check(f.c->undoCurrentPattern(),"V16 vary not undoable");
 check(gridAt().grid==kit.grid,"V16 vary Undo failed to restore full pattern");
 check(f.c->resetPatternVelocity(),"V16 1T velocity reset rejected");
 const auto reset=gridAt();
 check(reset.grid[0]==100&&reset.grid[4]==100&&
       reset.grid[kSteps+1]==100&&
       reset.grid[20]==64&&reset.grid[kSteps+21]==33&&
       reset.grid[kRows*kSteps]==77,
       "V16 1T reset touched bar 2/other pattern or missed active notes");
 check(f.c->undoCurrentPattern()&&gridAt().grid==kit.grid,
       "V16 1T reset Undo failed");
 check(f.c->togglePatternLength(0)&&f.c->patternLength(0)==32,
       "V16 could not return to two bars");
 check(f.c->velocity(0,0,20)==64&&f.c->velocity(0,1,21)==33,
       "V16 hidden notes changed when restored to 2T");
 check(f.c->resetPatternVelocity(),"V16 2T reset rejected");
 const auto reset2=gridAt();
 check(reset2.grid[20]==100&&reset2.grid[kSteps+21]==100,
       "V16 2T reset failed to reach second bar");
 check(f.c->undoCurrentPattern()&&gridAt().grid[20]==64&&
       gridAt().grid[kSteps+21]==33,
       "V16 2T undo did not recover hidden velocities");
 evidence("V16 active 1T VELO VARIIEREN/100 preserves second bar; 2T and Undo PASS");
}
void testNewStepDefault100(){
 Fixture f;
 auto kit=namedKit('X',4);
 kit.grid.fill(0);
 f.kit(kit);f.open();
 check(f.c->velocity(0,0,2)==0,"default step fixture not empty");
 check(f.c->toggleStep(0,0,2),"new step toggle rejected");
 check(f.c->velocity(0,0,2)==100,"new step did not receive Velocity 100");
 check(f.c->toggleStep(0,0,2)&&f.c->velocity(0,0,2)==0,
       "second step click must disable note");
 check(f.c->setStepVelocity(0,0,2,37),"setup explicit Velocity 37 failed");
 check(f.c->toggleStep(0,0,2)&&f.c->velocity(0,0,2)==0,
       "click on existing non-default Velocity must disable note");
 check(f.c->toggleStep(0,0,2)&&f.c->velocity(0,0,2)==100,
       "recreated step must use default Velocity 100");
 check(f.c->velocity(0,1,2)==0&&f.c->velocity(1,0,2)==0,
       "toggle changed another row or pattern");
 evidence("SOR01 real Controller toggles 0 -> 100 -> 0; previous non-default 37 -> 0 -> 100 PASS");
}
void testVelocityResetPattern(){
 Fixture f;
 auto kit=namedKit('X',4);
 kit.grid.fill(0);
 kit.grid[0]=1;kit.grid[4]=127;kit.grid[20]=64;
 kit.grid[kSteps]=88; // second instrument, same pattern
 kit.grid[kRows*kSteps]=72; // next pattern must remain untouched
 f.kit(kit);f.open();
 check(f.c->resetPatternVelocity(),"VELO 100 reset rejected");
 for(const std::size_t index : {std::size_t{0},std::size_t{4},std::size_t{20},std::size_t{kSteps}})
   check(f.c->velocity(0,index/kSteps,index%kSteps)==100,
         "VELO 100 did not reset occupied step to 100");
 check(f.c->velocity(0,0,1)==0&&f.c->velocity(0,1,1)==0,
       "VELO 100 created notes on empty steps");
 check(f.c->velocity(1,0,0)==72,"VELO 100 modified another pattern");
 check(f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern(),
       "VELO 100 must be one undoable operation");
 check(f.c->velocity(0,0,0)==1&&f.c->velocity(0,0,4)==127&&
       f.c->velocity(0,0,20)==64&&f.c->velocity(0,1,0)==88,
       "VELO 100 Undo did not restore exact prior velocities");
 check(!f.c->canUndoCurrentPattern(),"VELO 100 left extra undo operations");
 // Exercise actual native popup hit zones, not just the Controller API.
 click(*IntegrationAccess::view(*f.c,0),900.,114.); // PATTERN menu
 click(*IntegrationAccess::view(*f.c,0),900.,281.); // VELO 100 (item six)
 check(f.c->velocity(0,0,0)==100&&f.c->velocity(0,0,4)==100&&
       f.c->velocity(0,1,0)==100&&f.c->velocity(0,0,1)==0,
       "VELO 100 popup did not reset occupied notes only");
 click(*IntegrationAccess::view(*f.c,0),900.,114.);
 click(*IntegrationAccess::view(*f.c,0),900.,229.); // Undo (item four)
 check(f.c->velocity(0,0,0)==1&&f.c->velocity(0,0,4)==127&&
       f.c->velocity(0,1,0)==88&&f.c->velocity(0,0,1)==0,
       "VELO 100 popup Undo did not restore previous velocities");
 evidence("VELO 100 all occupied notes, no new notes, isolated pattern and single Undo PASS");
}
void testPatternDurationClipboard(){
 // P19: COPY/PASTE and DUPLIZIEREN must transfer both stored steps
 // and their musical length, and Undo must restore both in one operation.
 Fixture f;
 KitState kit=namedKit('X',4);
 kit.grid.fill(0);
 kit.grid[0]=91;kit.grid[20]=64;
 kit.grid[kRows*kSteps]=42;kit.grid[kRows*kSteps+20]=37;
 kit.patternLengths[0]=16;kit.patternLengths[1]=32;kit.patternLengths[2]=32;
 f.kit(kit);
 auto state=[&] {
   ResizableMemoryIBStream stream;KitState value;
   check(f.p->getState(&stream)==kResultOk&&
      KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                       static_cast<std::size_t>(stream.getCursor()),value),
      "P19 processor state decode");
   return value;
 };
 check(f.c->copyCurrentPattern(),"P19 copy one-bar A");
 check(f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk,
       "P19 select B");
 check(f.c->pasteCurrentPattern(),"P19 paste one bar into 2T");
 auto saved=state();
 check(saved.patternLengths[1]==16&&f.c->patternLength(1)==16&&
       saved.grid[kRows*kSteps]==91&&saved.grid[kRows*kSteps+20]==64,
       "P19 paste failed to atomically transfer length and hidden steps");
 check(f.c->undoCurrentPattern(),"P19 paste Undo");
 saved=state();
 check(saved.patternLengths[1]==32&&f.c->patternLength(1)==32&&
       saved.grid[kRows*kSteps]==42&&saved.grid[kRows*kSteps+20]==37,
       "P19 paste Undo failed to restore B's original 2T grid and duration");
 check(f.c->pasteCurrentPattern()&&f.c->duplicateCurrentPattern(),
       "P19 paste and duplicate to empty C");
 saved=state();
 check(saved.patternLengths[2]==16&&
       saved.grid[2*kRows*kSteps]==91&&saved.grid[2*kRows*kSteps+20]==64,
       "P19 duplicate failed to copy 1T musical length");
 check(f.c->setParamNormalized(kPatternParam,2./7.)==kResultOk&&
       f.c->undoCurrentPattern(),"P19 duplicate Undo");
 saved=state();
 check(saved.patternLengths[2]==32&&
       saved.grid[2*kRows*kSteps]==0&&saved.grid[2*kRows*kSteps+20]==0,
       "P19 duplicate Undo failed to restore previous empty 2T pattern");
 // Check other direction: 2T copied over an existing 1T pattern.
 KitState reverse=kit;reverse.patternLengths[0]=32;reverse.patternLengths[1]=16;
 f.kit(reverse);
 // Selecting A explicitly is required: the preceding host-selected C can
 // legitimately survive Component recall (RE07). The clipboard source
 // must not depend on the host's prior active pattern.
 check(f.c->setParamNormalized(kPatternParam,0.)==kResultOk&&
       f.c->currentPattern()==0,"P19 explicitly select source A after kit recall");
 check(f.c->copyCurrentPattern()&&
       f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk&&
       f.c->pasteCurrentPattern(),"P19 2T->1T paste");
 saved=state();
 check(saved.patternLengths[1]==32&&f.c->patternLength(1)==32&&
       saved.grid[kRows*kSteps+20]==64,
       "P19 paste 2T did not activate second bar");
 check(f.c->undoCurrentPattern(),"P19 2T->1T Undo");
 saved=state();
 check(saved.patternLengths[1]==16&&saved.grid[kRows*kSteps+20]==37,
       "P19 2T->1T undo duration or step failure");
 // Identical note contents still require the length change and Undo.
 KitState matching=kit;
 matching.grid[kRows*kSteps]=matching.grid[0];
 matching.grid[kRows*kSteps+20]=matching.grid[20];
 f.kit(matching);
 check(f.c->setParamNormalized(kPatternParam,0.)==kResultOk&&
       f.c->currentPattern()==0,"P19 explicitly select matching source A");
 check(f.c->copyCurrentPattern()&&
       f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk&&
       f.c->pasteCurrentPattern(),"P19 length-only paste rejected");
 check(state().patternLengths[1]==16,"P19 length-only edit not persisted");
 check(f.c->undoCurrentPattern()&&state().patternLengths[1]==32,
       "P19 length-only undo failed");
 // A malformed combined control message must be rejected *before* changing
 // either pattern bytes or the musical duration.
 const auto unaffected=state();
 PatternSteps malformedSteps{};malformedSteps[0]=125;
 HostMessage malformed;malformed.setMessageID("Drumator.PatternReplace");
 auto* bad=malformed.getAttributes();
 check(bad&&bad->setInt("Pattern",1)==kResultOk&&
       bad->setBinary("Steps",malformedSteps.data(),
                     static_cast<uint32>(malformedSteps.size()))==kResultOk&&
       bad->setInt("Length",24)==kResultOk,
       "P19 invalid-length message setup");
 check(f.p->notify(&malformed)==kInvalidArgument,
       "P19 malformed length not rejected");
 check(state().grid==unaffected.grid&&
       state().patternLengths==unaffected.patternLengths,
       "P19 malformed length partially committed pattern");
 // Legacy hosts/controllers have always sent PatternReplace with Steps only:
 // they must continue working without resetting any pattern duration.
 KitState oldProtocol=matching;
 oldProtocol.patternLengths[7]=16;
 f.kit(oldProtocol);
 HostMessage legacy;legacy.setMessageID("Drumator.PatternReplace");
 auto* attrs=legacy.getAttributes();
 check(attrs&&attrs->setInt("Pattern",7)==kResultOk&&
       attrs->setBinary("Steps",malformedSteps.data(),
                        static_cast<uint32>(malformedSteps.size()))==kResultOk,
       "P19 legacy message setup");
 check(f.p->notify(&legacy)==kResultOk&&
       state().patternLengths[7]==16&&
       state().grid[7*kRows*kSteps]==125,
       "P19 legacy Steps-only message broke duration or edit compatibility");
 evidence("P19 COPY/PASTE/DUPLICATE length 1T/2T, Undo, legacy and invalid-length PASS");
}
void testPatternDuplicateMenu(){
 Fixture f;
 KitState kit=namedKit('X',4);
 kit.grid.fill(0);
 kit.grid[0]=90;kit.grid[4]=60;kit.grid[20]=25;
 kit.patternLengths[0]=32;
 f.kit(kit);f.open();
 auto* view=IntegrationAccess::view(*f.c,0);
 check(f.c->canDuplicateCurrentPattern(),"P18 initial pattern not duplicable");
 auto getGrid=[&]{
   ResizableMemoryIBStream stream;KitState data;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                          static_cast<std::size_t>(stream.getCursor()),data),
         "P18 decode actual processor project");
   return data.grid;
 };
 const auto before=getGrid();
 click(*view,900.,114.); // PATTERN popup
 click(*view,900.,307.); // seventh and final action: DUPLIZIEREN
 const auto after=getGrid();
 for(std::size_t i=0;i<kRows*kSteps;++i){
   check(after[i]==before[i],"P18 duplicate changed source pattern");
   check(after[kRows*kSteps+i]==before[i],
         "P18 native DUPLIZIEREN menu did not copy entire source to pattern B");
 }
 check(after[2*kRows*kSteps]==0,"P18 duplicate wrote a third pattern");
 check(f.c->velocity(1,0,20)==25,"P18 controller not updated for copied pattern");
 // Existing action returns success if the host refuses selecting the copy:
 // source remains valid, and manually selecting B still permits Undo.
 check(f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk,
       "P18 cannot navigate to duplicated pattern B");
 check(f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern(),
       "P18 duplicate did not create undo history for target");
 check(getGrid()==before,"P18 target undo did not restore completely empty B");
 // With all eight patterns occupied, the menu action is disabled and
 // neither host nor controller may overwrite an existing pattern.
 KitState full=kit;
 for(std::size_t p=0;p<kPatterns;++p)
   full.grid[p*kRows*kSteps]=static_cast<std::uint8_t>(p+1);
 f.kit(full);
 check(!f.c->canDuplicateCurrentPattern(),"P18 full bank incorrectly duplicable");
 const auto fullBefore=getGrid();
 click(*view,900.,114.);
 click(*view,900.,307.);
 check(getGrid()==fullBefore,"P18 full-bank DUPLIZIEREN overwrote a pattern");
 // An empty source also must never be duplicated into another slot.
 KitState empty=kit;empty.grid.fill(0);
 f.kit(empty);
 check(!f.c->canDuplicateCurrentPattern(),"P18 empty pattern offered duplicate");
 click(*view,900.,114.);
 click(*view,900.,307.);
 check(getGrid()==empty.grid,"P18 empty source duplicated unexpectedly");
 evidence("P18 native DUPLIZIEREN menu, whole-grid copy, Undo, full bank and empty source PASS");
}

void testAbsoluteSongStartRest(){
 constexpr auto flags=ProcessContext::kPlaying|
       ProcessContext::kTempoValid|ProcessContext::kProjectTimeMusicValid;
 Fixture f;KitState kit;kit.visible=2;kit.grid.fill(0);
 kit.songEnabled=1;kit.songStartBar=2;
 check(kit.chain.setCount(3)&&kit.chain.configure(0,0,8)&&
       kit.chain.configure(1,SongChain::kRest,8)&&
       kit.chain.configure(2,1,8),"SONGSTART REST fixture");
 kit.grid[0]=127;kit.grid[(kRows+1)*kSteps]=127;
 kit.names[0]="song-A.wav";kit.wav[0]=constantStereo(.125f,-.25f);
 kit.names[1]="song-B.wav";kit.wav[1]=constantStereo(.375f,-.125f);
 f.kit(kit);prepareAudio(*f.p);
 struct SongAudioBlock {
   std::array<float,256> left{},right{};
   float* channels[2]{left.data(),right.data()};
   AudioBusBuffers out{};ProcessContext clock{};ProcessData data{};
   SongAudioBlock(){
     out.numChannels=2;out.channelBuffers32=channels;
     data.numSamples=256;data.symbolicSampleSize=kSample32;
     data.numOutputs=1;data.outputs=&out;data.processContext=&clock;
   }
 } audio;
 audio.clock.tempo=6000.;
 audio.clock.state=flags;
 auto render=[&](double ppq) {
   audio.clock.projectTimeMusic=ppq;
   check(f.p->process(audio.data)==kResultOk,"SONGSTART processor render");
 };
 // The host starts before BAR 2 and crosses its first boundary mid-buffer:
 // no pre-roll events are permitted, onset must land at sample 96.
 render(3.8);
 for(unsigned i=0;i<96;++i)check(audio.left[i]==0.f,"SONGSTART pre-start trigger");
 check(audio.left[120]==.125f&&audio.right[120]==-.25f,
       "SONGSTART BAR 2 onset missing/wrong channel");
 // Restart directly at BAR 4: offset = 32 steps from BAR 2, and
 // 32 mod 24 = 8 => REST, never chain position 01.
 check(f.p->setProcessing(false)==kResultOk&&
       f.p->setProcessing(true)==kResultOk,"SONGSTART restart lifecycle");
 render(12.0);
 for(const auto value:audio.left)check(value==0.f,
       "SONGSTART BAR 4 incorrectly restarted first pattern");
 // Playback from start, with an existing A tail across REST.
 check(f.p->setProcessing(false)==kResultOk&&
       f.p->setProcessing(true)==kResultOk,"SONGSTART reset before REST");
 for(unsigned block=0;block<9;++block){
   const double ppq=4.0+static_cast<double>(block)*256.0/480.0;
   render(ppq);
   if(block==0)check(audio.left[80]==.125f,"SONGSTART A absent");
   if(block==3)check(audio.left[240]==.125f,
       "REST truncated previously sounding A sample");
   if(block==5)for(const auto value:audio.left)
       check(value==0.f,"REST retriggered drums after A tail");
   if(block==8)check(audio.left[80]==.375f&&audio.right[80]==-.125f,
       "REST lost exact subsequent B onset");
 }
 // Restore actual serialized kit via the normal Processor/Controller path.
 ResizableMemoryIBStream savedSong;KitState persisted;
 check(f.p->getState(&savedSong)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(savedSong.getData()),
                        static_cast<std::size_t>(savedSong.getCursor()),persisted),
       "SONGSTART actual Processor state export");
 check(persisted.songStartBar==2&&persisted.chain.entry(1).pattern==SongChain::kRest,
       "SONGSTART Processor did not save DM17");
 const auto bytes=Fixture::encode(persisted);
 check(f.recall(bytes)==kResultOk&&f.c->songStartBar()==2&&
       f.c->songEntry(1).pattern==SongChain::kRest,
       "SONGSTART Controller DM17 recall mismatch");
 // Explicit active host Cycle test: the loop covers A and REST but never B.
 // The chain remains anchored at absolute BAR 2, and the true audio timeline
 // wraps without truncating/duplicating A while REST remains silent.
 check(f.p->setProcessing(false)==kResultOk&&
       f.p->setProcessing(true)==kResultOk,"SONGSTART Cycle lifecycle");
 constexpr auto cycle=ProcessContext::kCycleActive|ProcessContext::kCycleValid;
 audio.clock.state=flags|cycle;
 audio.clock.cycleStartMusic=4.0;audio.clock.cycleEndMusic=8.0;
 audio.clock.tempo=6000.;
 for(unsigned block=0;block<18;++block){
    const auto position=(block*256u)%1920u;
    render(4.0+static_cast<double>(position)/480.0);
    for(unsigned i=0;i<256;++i){
       const auto cycleSample=(block*256u+i)%1920u;
       const float expectedL=cycleSample<1024u?.125f:0.f;
       const float expectedR=cycleSample<1024u?-.25f:0.f;
       check(audio.left[i]==expectedL&&audio.right[i]==expectedR,
             "SONGSTART/REST host Cycle shifted beat, retriggered pause or clipped tail");
    }
 }
 // Changing host tempo at an absolute bar boundary must not alter the
 // requested entry or place the first sample after that boundary.
 check(f.p->setProcessing(false)==kResultOk&&
       f.p->setProcessing(true)==kResultOk,"SONGSTART tempo lifecycle");
 audio.clock.state=flags;
 audio.clock.tempo=240.;
 render(4.);
 check(audio.left[80]==.125f&&audio.right[80]==-.25f,
       "SONGSTART tempo 240 shifted absolute BAR 2");
 // Real SONG GUI hitboxes, including REST and BAR controls.
 f.open();
 auto* view=IntegrationAccess::view(*f.c,0);
 click(*view,(UiCompactLayout::songEdit.left+UiCompactLayout::songEdit.right)/2.,
       UiCompactLayout::headerButtonTop+13.);
 click(*view,UiSongLayout::startPlusLeft+12.,181.);
 check(f.c->songStartBar()==3,"SONG START + native UI ignored");
 click(*view,UiSongLayout::startMinusLeft+12.,181.);
 check(f.c->songStartBar()==2,"SONG START - native UI ignored");
 click(*view,UiSongLayout::startResetLeft+12.,181.);
 check(f.c->songStartBar()==1,"SONG START RESET native UI ignored");
 click(*view,UiSongLayout::cellX(0)+20.,UiSongLayout::cellY(0)+15.);
 click(*view,UiSongLayout::restLeft+30.,352.);
 check(f.c->songEntry(0).pattern==SongChain::kRest,
       "REST native song editor selection ignored");
 evidence("SONGSTART absolute BAR 2/4, pre-roll boundary, REST audible tail and B continuation, DM17 recall, native GUI PASS");
}

void testV1FourFinalCorrections(){
 Fixture f;
 auto stored=[&]{
   ResizableMemoryIBStream stream;
   KitState kit;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
           static_cast<std::size_t>(stream.getCursor()),kit),
         "V1FIX decode real Processor state");
   return kit;
 };
 // Full factory check, not merely the visible first bar.
 const auto factory=stored();
 for(std::size_t p=0;p<kPatterns;++p)
   for(std::size_t row=0;row<kRows;++row)
     for(std::size_t step=0;step<kSteps;++step){
       const auto expected=p==0&&row==0&&step%4==0?
          static_cast<unsigned>(step%8==0?127:90):0u;
       check(factory.grid[AtomicPatternBank::index(p,row,step)]==expected&&
             f.c->velocity(p,row,step)==expected,
             "V1FIX factory Controller/Processor mismatch incl hidden steps");
     }
 for(std::size_t row=0;row<kRows;++row)
   check(factory.output[row]==0&&f.c->rowOutput(row)==0,
         "V1FIX fresh instance output not MAIN");
 check(f.c->canDuplicateCurrentPattern()&&f.c->duplicateCurrentPattern(),
       "V1FIX fresh DUPLICATE A -> B failed");
 auto duplicate=stored();
 for(std::size_t i=0;i<kRows*kSteps;++i)
   check(duplicate.grid[kRows*kSteps+i]==factory.grid[i],
         "V1FIX factory duplicate omitted a step");
 check(f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk&&
       f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern(),
       "V1FIX DUPLICATE not undoable");
 check(stored().grid==factory.grid&&!f.c->canUndoCurrentPattern(),
       "V1FIX duplicate UNDO left B occupied");

 // Cross-pattern/row clipboard, native ROW context menu, 1-bar hidden notes.
 KitState kit;
 kit.visible=4;kit.grid.fill(0);
 kit.patternLengths[0]=16;kit.patternLengths[1]=16;
 kit.grid[AtomicPatternBank::index(0,1,0)]=113;
 kit.grid[AtomicPatternBank::index(0,1,15)]=64;
 kit.grid[AtomicPatternBank::index(0,1,31)]=27;
 kit.grid[AtomicPatternBank::index(1,2,0)]=55;
 kit.grid[AtomicPatternBank::index(1,3,31)]=81;
 kit.output[1]=3;kit.output[2]=6;
 kit.gain[2]=71;kit.pan[2]=120;kit.pitch[2]=33;
 kit.displayNames[2]="SNARE";kit.names[2]="SNARE.wav";
 f.kit(kit);f.open();
 auto* view=IntegrationAccess::view(*f.c,0);
 check(f.c->setParamNormalized(kPatternParam,0.)==kResultOk,"V1FIX select A");
 check(!f.c->canPasteRow()&&!f.c->pasteRow(2),
       "V1FIX empty row clipboard accepted");
 // The ROW cap is on the lower faceplate, left of VOL. Item 0 is COPY.
 const double srcTop=IntegrationAccess::rowTop(*view,1);
 click(*view,60.,srcTop+88.);
 check(IntegrationAccess::popup(*view)==6&&IntegrationAccess::popupRow(*view)==1,
       "V1FIX ROW cap not clickable");
 const double srcPopup=UiCompactLayout::popupTop(srcTop,view->getViewSize().getHeight(),100.);
 click(*view,100.,srcPopup+12.);
 check(f.c->canPasteRow(),"V1FIX native COPY ROW did not capture");
 check(f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk,"V1FIX select B");
 // Item 1 is PASTE ROW into Pattern B, preserving source pattern A.
 const double dstTop=IntegrationAccess::rowTop(*view,2);
 click(*view,60.,dstTop+88.);
 const double dstPopup=UiCompactLayout::popupTop(dstTop,view->getViewSize().getHeight(),100.);
 click(*view,100.,dstPopup+30.+12.);
 const auto pasted=stored();
 for(std::size_t p=0;p<kPatterns;++p)
   for(std::size_t row=0;row<kRows;++row)
     for(std::size_t step=0;step<kSteps;++step){
       const auto idx=AtomicPatternBank::index(p,row,step);
       const auto expected=p==1&&row==2?
          kit.grid[AtomicPatternBank::index(0,1,step)]:kit.grid[idx];
       check(pasted.grid[idx]==expected,"V1FIX PASTE ROW changed other pattern or row");
     }
 check(pasted.patternLengths==kit.patternLengths&&pasted.output==kit.output&&
       pasted.gain==kit.gain&&pasted.pan==kit.pan&&pasted.pitch==kit.pitch&&
       pasted.names==kit.names&&pasted.displayNames==kit.displayNames&&
       pasted.wav==kit.wav,"V1FIX row paste modified nonsequencer state");
 check(f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern(),
       "V1FIX row PASTE did not create UNDO");
 check(stored().grid==kit.grid&&!f.c->canUndoCurrentPattern(),
       "V1FIX row PASTE undo failed to restore target row only");
 // Existing full-pattern clipboard remains independent.
 check(f.c->copyCurrentPattern()&&
       f.c->setParamNormalized(kPatternParam,2./7.)==kResultOk&&
       f.c->pasteCurrentPattern(),"V1FIX full-pattern clipboard broken");
 // A successful project recall invalidates session clipboards, not saved data.
 f.kit(kit);
 check(!f.c->canPasteRow()&&!f.c->pasteRow(1),
       "V1FIX old row clipboard survived project recall");
 check(stored().output==kit.output,"V1FIX saved output assignments mutated");
 // Filled bank still disallows overwriting occupied destinations.
 KitState all=kit;
 for(std::size_t p=0;p<kPatterns;++p)
   all.grid[AtomicPatternBank::index(p,0,0)]=static_cast<std::uint8_t>(p+5);
 f.kit(all);
 check(f.c->setParamNormalized(kPatternParam,0.)==kResultOk&&
       !f.c->canDuplicateCurrentPattern()&&
       !f.c->duplicateCurrentPattern()&&stored().grid==all.grid,
       "V1FIX DUPLICATE overwrote occupied bank");
 evidence("V1FIX full factory/MAIN; duplicate+UNDO; native 32-step cross-row COPY/PASTE+UNDO; recall and protected occupied slots PASS");
}
void testV1RowMenuClearScope(){
 Fixture f;KitState kit;kit.visible=4;kit.grid.fill(0);
 kit.patternLengths[0]=16;kit.patternLengths[1]=16;
 kit.grid[AtomicPatternBank::index(0,1,0)]=111;
 kit.grid[AtomicPatternBank::index(0,1,15)]=71;
 kit.grid[AtomicPatternBank::index(0,1,31)]=48; // hidden second bar
 kit.grid[AtomicPatternBank::index(0,2,5)]=103;
 kit.grid[AtomicPatternBank::index(1,1,0)]=87;
 kit.names[1]="keep_sample.wav";kit.wav[1]=constantStereo(.3f,-.2f);
 kit.output[1]=4;kit.gain[1]=73;kit.pan[1]=118;kit.pitch[1]=42;
 f.kit(kit);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 auto stored=[&](){
   ResizableMemoryIBStream stream;KitState data;
   check(f.p->getState(&stream)==kResultOk&&
     KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
       static_cast<std::size_t>(stream.getCursor()),data),"ROW context actual processor state");
   return data;
 };
 const auto before=stored();
 check(f.c->setParamNormalized(kPatternParam,0.)==kResultOk,"ROW select pattern A");
 const double y=IntegrationAccess::rowTop(*v,1);
 const auto selected=IntegrationAccess::selectedRow(*v);
 click(*v,60.,y+89.);
 check(IntegrationAccess::popup(*v)==6&&IntegrationAccess::popupRow(*v)==1&&
       IntegrationAccess::selectedRow(*v)==selected,
       "ROW button intercepted by row selector or missing");
 const auto menu=UiCompactLayout::popupTop(y,v->getViewSize().getHeight(),100.);
 click(*v,105.,menu+60.+12.); // item 2 CLEAR ROW
 const auto cleared=stored();
 for(std::size_t p=0;p<kPatterns;++p)
   for(std::size_t row=0;row<kRows;++row)
     for(std::size_t step=0;step<kSteps;++step){
       const auto index=AtomicPatternBank::index(p,row,step);
       const auto expected=p==0&&row==1?0:before.grid[index];
       check(cleared.grid[index]==expected&&f.c->velocity(p,row,step)==expected,
             "ROW CLEAR touched another pattern/row or missed a hidden step");
     }
 check(cleared.wav==before.wav&&cleared.names==before.names&&
       cleared.output==before.output&&cleared.gain==before.gain&&
       cleared.pan==before.pan&&cleared.pitch==before.pitch&&
       cleared.choke==before.choke&&
       cleared.patternLengths==before.patternLengths,
       "ROW CLEAR changed sample, output or sound settings");
 check(f.c->canUndoCurrentPattern()&&f.c->undoCurrentPattern()&&
       stored().grid==before.grid,"ROW CLEAR not fully undoable");
 // FILL has only eight rhythmic commands: former CLEAR/COPY/PASTE hit
 // positions are inert and cannot allocate/update the row clipboard.
 check(!f.c->canPasteRow(),"ROW FILL fixture unexpected clipboard");
 click(*v,425.,y+18.);
 const auto fillTop=UiCompactLayout::popupTop(y,v->getViewSize().getHeight(),210.);
 click(*v,430.,fillTop+9*25.+12.);
 check(!f.c->canPasteRow()&&stored().grid==before.grid,
       "FILL old COPY ROW index still dispatched");
 click(*v,425.,y+18.);
 click(*v,430.,fillTop+12.);
 check(stored().grid!=before.grid&&f.c->canUndoCurrentPattern()&&
       f.c->undoCurrentPattern()&&stored().grid==before.grid,
       "FILL rhythm 1 no longer transactional/undoable");
 evidence("GUI50 ROW COPY/PASTE separate menu, CLEAR all 32 steps and Undo, sample/mix preservation, FILL rhythm-only PASS");
}
void testPatternLengthControls(){
 Fixture f;
 auto kit=namedKit('X');
 // Row five must actually be empty for the existing safe-removal contract.
 kit.displayNames[4].clear();
 kit.names[4].clear();
 f.kit(kit);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 check(f.c->patternLength(0)==32,"UX pattern length initial state");
 click(*v,322.,UiCompactLayout::headerButtonTop+13.);
 check(f.c->patternLength(0)==16,"UX visible 1 TAKT control failed");
 const double blockedX=UiCompactLayout::stepX+20.*UiCompactLayout::stepWidth+8.;
 const auto oldHidden=f.c->velocity(0,0,20);
 click(*v,blockedX,UiCompactLayout::header+88.);
 check(f.c->velocity(0,0,20)==oldHidden,"UX clicked inactive step 21 in 1T");
 click(*v,1248.,UiCompactLayout::header+88.);
 click(*v,blockedX,UiCompactLayout::header+UiCompactLayout::baseRow+12.);
 check(f.c->velocity(0,0,20)==oldHidden,"UX edited inactive velocity in 1T");
 click(*v,322.,UiCompactLayout::headerButtonTop+13.);
 check(f.c->patternLength(0)==16,"UX selected length not idempotent");
 click(*v,414.,UiCompactLayout::headerButtonTop+13.);
 check(f.c->patternLength(0)==32,"UX visible 2 TAKTE control failed");
 click(*v,900.,114.);
 click(*v,900.,150.);
 check(f.c->patternLength(0)==32,"UX copy changed length via duplicate menu");
 const double bottom=UiCompactLayout::header+UiCompactLayout::baseRow*4.+UiCompactLayout::velocityRow;
 click(*v,76.,bottom+17.);
 check(f.c->visibleRows()==5,"UX + ZEILE not clickable");
 click(*v,160.,bottom+17.);
 check(f.c->visibleRows()==4,"UX - ZEILE not clickable");
 evidence("UX direct length and no duplicate pattern menu control PASS");
}
void testControlExtremeDeltas(){
 Fixture f;f.kit(namedKit('D',4));
 // Pitch updates rebuild a prepared sample; the processor needs the host
 // sample rate even for a kit whose audio sample is not yet loaded.
 ProcessSetup setup{};
 setup.sampleRate=48000.;setup.maxSamplesPerBlock=256;
 setup.symbolicSampleSize=kSample32;setup.processMode=kRealtime;
 check(f.p->setupProcessing(setup)==kResultOk,"D11 pitch rate not initialized");
 constexpr int upper=std::numeric_limits<int>::max();
 constexpr int lower=std::numeric_limits<int>::min();
 check(f.c->adjustMasterVolume(upper)&&f.c->masterVolume()==200,"D11 master upper");
 check(f.c->adjustMasterVolume(lower)&&f.c->masterVolume()==0,"D11 master lower");
 check(f.c->adjustHumanize(upper)&&f.c->humanizePercent()==100,"D11 humanize upper");
 check(f.c->adjustHumanize(lower)&&f.c->humanizePercent()==0,"D11 humanize lower");
 check(f.c->adjustSwing(upper)&&f.c->swingPercent()==50,"D11 swing upper");
 check(f.c->adjustSwing(lower)&&f.c->swingPercent()==0,"D11 swing lower");
 check(f.c->adjustRandomVelocity(0,upper)&&f.c->rowRandomVelocity(0)==100,"D11 random upper");
 check(f.c->adjustRandomVelocity(0,lower)&&f.c->rowRandomVelocity(0)==0,"D11 random lower");
 check(f.c->adjustRowPitch(0,upper)&&f.c->rowPitch(0)==24,"D11 pitch upper");
 check(f.c->adjustRowPitch(0,lower)&&f.c->rowPitch(0)==-24,"D11 pitch lower");
 check(f.c->adjustRowMix(0,false,upper)&&f.c->rowGain(0)==200,"D11 volume upper");
 check(f.c->adjustRowMix(0,false,lower)&&f.c->rowGain(0)==0,"D11 volume lower");
 check(f.c->adjustRowMix(0,true,upper)&&f.c->rowPan(0)==100,"D11 pan upper");
 check(f.c->adjustRowMix(0,true,lower)&&f.c->rowPan(0)==-100,"D11 pan lower");
 ResizableMemoryIBStream stream;KitState saved;
 check(f.p->getState(&stream)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                       static_cast<std::size_t>(stream.getCursor()),saved),
       "D11 saved processor decode");
 check(saved.masterVolume==0&&saved.humanize==0&&saved.swing==0,
       "D11 saved global controls mismatch");
 evidence("D11 extreme control deltas, controller endpoints and processor state PASS");
}
void testSongCountExtremeDelta(){
 Fixture f;auto kit=namedKit('C',4);
 check(kit.chain.setCount(3)&&kit.chain.configure(0,5,16),
       "S10 song count fixture");
 f.kit(kit);
 auto checkCount=[&](std::size_t expected){
   check(f.c->songCount()==expected,"S10 controller count mismatch");
   ResizableMemoryIBStream stream;KitState saved;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                     static_cast<std::size_t>(stream.getCursor()),saved),
         "S10 processor state decode");
   check(saved.chain.count()==expected&&saved.chain.entry(0).pattern==5&&
         saved.chain.entry(0).steps==16,
         "S10 processor count or existing song entry mismatch");
 };
 check(f.c->adjustSongCount(std::numeric_limits<int>::max()),
       "S10 INT_MAX delta rejected");
 checkCount(SongChain::kCapacity);
 check(f.c->adjustSongCount(std::numeric_limits<int>::min()),
       "S10 INT_MIN delta rejected");
 checkCount(1);
 check(f.c->adjustSongCount(2),"S10 ordinary +2 rejected");
 checkCount(3);
 check(f.c->adjustSongCount(-1),"S10 ordinary -1 rejected");
 checkCount(2);
 evidence("S10 song count INT_MIN/MAX saturation, normal edits and persisted state PASS");
}
void testSongEntryExtremeDelta(){
 Fixture f;auto kit=namedKit('X',4);
 kit.songEnabled=1;
 check(kit.chain.setCount(3)&&kit.chain.configure(0,7,32)&&
       kit.chain.configure(1,0,8)&&kit.chain.configure(2,3,16),
       "S09 song test fixture");
 f.kit(kit);
 auto checkState=[&](std::uint8_t p,std::uint8_t len) {
   const auto e=f.c->songEntry(0);
   check(e.pattern==p&&e.steps==len,"S09 controller wrong song entry");
   ResizableMemoryIBStream stream;KitState saved;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                 static_cast<std::size_t>(stream.getCursor()),saved),
         "S09 processor state decode");
   const auto persisted=saved.chain.entry(0);
   check(persisted.pattern==p&&persisted.steps==len,
         "S09 processor/controller Song entry mismatch");
 };
 check(f.c->editSongEntry(0,1,1),"S09 ordinary +1 rejected");
 checkState(0,8);
 check(f.c->editSongEntry(0,-1,-1),"S09 ordinary -1 rejected");
 checkState(7,32);
 // All these arguments were previously liable to signed overflow or
 // negative indexing when the native API was invoked directly.
 const auto big=std::numeric_limits<int>::max();
 const auto minimumDelta=std::numeric_limits<int>::min();
 check(f.c->editSongEntry(0,big,big),"S09 INT_MAX delta rejected");
 checkState(6,8); // 7+(INT_MAX mod 8)=6; length 32 -> 8
 check(f.c->editSongEntry(0,minimumDelta,minimumDelta),"S09 INT_MIN delta rejected");
 checkState(6,16); // INT_MIN mod 8=0, mod 3=-2: 8 -> 16
 check(f.c->editSongEntry(0,0,0),"S09 zero delta rejected");
 checkState(6,16);
 check(!f.c->editSongEntry(99,1,1),"S09 invalid song slot accepted");
 checkState(6,16);
 evidence("S09 song editor signed modular delta INT_MIN/MAX, normal edits and recall PASS");
}
void testPagedSongEditor(){
 Fixture f;auto kit=namedKit('X',4);
 check(kit.chain.setCount(16),"UX song fixture capacity");
 f.kit(kit);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 click(*v,1077.,114.);
 check(IntegrationAccess::songPage(*v)==0,"UX song page initial selection");
 // A 16-item chain has no 17th item and no page 2 until explicit ADD.
 click(*v,1170.,181.);
 check(IntegrationAccess::songPage(*v)==0&&f.c->songCount()==16,
       "UX SONG NEXT opened an empty page at 16 items");
 click(*v,UiSongLayout::cellX(0)+20.,UiSongLayout::cellY(0)+15.);
 check(IntegrationAccess::songSlot(*v)==0,"UX existing SONG tile selection");
 click(*v,1060.,352.);
 check(f.c->songCount()==17&&IntegrationAccess::songSlot(*v)==16&&
       IntegrationAccess::songPage(*v)==1,"UX ADD ENTRY must open page 2");
 captureVstgui(*v,"ux-song-page-2",1.0);
 click(*v,UiSongLayout::patternLeft+2.*UiSongLayout::patternPitch+20.,352.);
 check(f.c->songEntry(16).pattern==2,"UX page-2 pattern selection");
 click(*v,707.,352.);
 check(f.c->songEntry(16).steps==8,"UX page-2 half-bar selection");
 const auto snapshot=IntegrationAccess::snapshot(*f.c);
 // The former +ENTRY tile and future tiles MUST NOT append or change selection.
 click(*v,UiSongLayout::cellX(1)+20.,UiSongLayout::cellY(0)+15.);
 check(f.c->songCount()==17&&IntegrationAccess::snapshot(*f.c)==snapshot,
       "UX empty SONG tile appended despite ADD ENTRY-only policy");
 click(*v,167.,222.);
 check(IntegrationAccess::snapshot(*f.c)==snapshot,"UX SONG tile gutter not inert");
 click(*v,(UiSongLayout::previousLeft+UiSongLayout::previousRight)/2.,
        (UiSongLayout::navTop+UiSongLayout::navBottom)/2.);
 check(IntegrationAccess::songPage(*v)==0&&IntegrationAccess::songSlot(*v)==0,
       "UX SONG previous page must select first valid entry");
 click(*v,1170.,222.);
 check(IntegrationAccess::songSlot(*v)==7,"UX selected existing 8th entry");
 click(*v,1060.,352.);
 check(f.c->songCount()==18&&IntegrationAccess::songSlot(*v)==17&&
       IntegrationAccess::songPage(*v)==1,"UX ADD ENTRY from page 1 not paged");
 // REMOVE LAST must target the chain tail, not the selected or clicked tile.
 click(*v,UiSongLayout::cellX(0)+20.,UiSongLayout::cellY(0)+15.);
 check(IntegrationAccess::songSlot(*v)==16,"UX position 17 selection");
 click(*v,1172.,352.);
 check(f.c->songCount()==17&&IntegrationAccess::songSlot(*v)==16&&
       IntegrationAccess::songPage(*v)==1,"UX REMOVE LAST from different selection");
 check(f.c->songEntry(16).pattern==2&&f.c->songEntry(16).steps==8,
       "UX REMOVE LAST corrupted previous SONG entry");
 click(*v,1172.,352.);
 check(f.c->songCount()==16&&IntegrationAccess::songSlot(*v)==15&&
       IntegrationAccess::songPage(*v)==0,"UX REMOVE LAST failed page-boundary clamp");
 click(*v,1170.,181.);
 check(IntegrationAccess::songPage(*v)==0,"UX next empty page reachable after remove");
 // REST and A-H are untouched across add/remove and persisted correctly.
 check(f.c->setSongEntry(0,SongChain::kRest,8),"UX rest setup");
 click(*v,1172.,352.);
 check(f.c->songCount()==15&&f.c->songEntry(0).pattern==SongChain::kRest,
       "UX REMOVE LAST accidentally touched REST");
 auto one=namedKit('R',4);
 check(one.chain.setCount(1),"UX single-entry chain");
 f.kit(one);
 click(*v,1172.,352.);
 check(f.c->songCount()==1,"UX REMOVE LAST broke minimum count 1");
 auto full=namedKit('S',4);
 check(full.chain.setCount(64),"UX full-chain fixture");
 f.kit(full);
 check(f.c->songCount()==64,"UX maximum count fixture not loaded");
 click(*v,1060.,352.);
 check(f.c->songCount()==64,"UX ADD ENTRY exceeded 64");
 click(*v,1172.,352.);
 check(f.c->songCount()==63&&IntegrationAccess::songSlot(*v)<63&&
       IntegrationAccess::songPage(*v)==UiSongLayout::pageOf(IntegrationAccess::songSlot(*v)),
       "UX REMOVE LAST from full chain did not update page/selection");
 auto recalled=namedKit('Y',4);
 check(f.recall(Fixture::encode(recalled))==kResultOk&&
       IntegrationAccess::songPage(*v)==0&&
       IntegrationAccess::songSlot(*v)<f.c->songCount(),
       "UX SONG recall left invalid selected entry or page");
 evidence("UX SONG ADD-only tiles, REMOVE LAST 1/16/17/18/64, selection/page, REST and recall PASS");
}
void testSongFinalGuiAlignment(){
 Fixture f;auto kit=namedKit('X',4);
 kit.songEnabled=1;kit.songStartBar=2;
 check(kit.chain.setCount(3)&&kit.chain.configure(0,0,16)&&
       kit.chain.configure(1,SongChain::kRest,8)&&
       kit.chain.configure(2,2,32),"GUIFINAL chain fixture");
 f.kit(kit);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 click(*v,1077.,114.);
 check(v->getViewSize().getHeight()==UiCompactLayout::songEditorHeight,
       "GUIFINAL song editor unexpectedly resized");
 // Verify gaps and eliminated legacy hitboxes, not just isolated buttons.
 const auto entryBefore=f.c->songEntry(0);
 click(*v,UiSongLayout::patternLeft+UiSongLayout::patternWidth+2.,352.);
 check(f.c->songEntry(0).pattern==entryBefore.pattern,
       "GUIFINAL pattern gap falsely clickable");
 click(*v,UiSongLayout::restLeft+30.,352.);
 check(f.c->songEntry(0).pattern==SongChain::kRest,
       "GUIFINAL adjoining REST choice ignored");
 click(*v,160.,388.);
 check(f.c->songEntry(0).pattern==SongChain::kRest,
       "GUIFINAL removed standalone REST hotspot still active");
 click(*v,UiSongLayout::patternLeft+2.*UiSongLayout::patternPitch+15.,352.);
 check(f.c->songEntry(0).pattern==2,"GUIFINAL pattern C missed after REST");
 click(*v,UiSongLayout::restLeft+30.,352.);
 click(*v,UiSongLayout::startPlusLeft+15.,181.);
 check(f.c->songStartBar()==3,"GUIFINAL header plus missed");
 click(*v,UiSongLayout::startMinusLeft+15.,181.);
 check(f.c->songStartBar()==2,"GUIFINAL header minus missed");
 click(*v,982.,321.); // old + control is gone
 check(f.c->songStartBar()==2,"GUIFINAL removed start hotspot still active");
 for(const auto factor:{1.0,1.3}){
   const std::array<std::uint8_t,5> state{{'D','Z','M','2',
       static_cast<std::uint8_t>(factor==1.0?0:2)}};
   drumator::qa::ByteStream stream(const_cast<std::uint8_t*>(state.data()),state.size());
   check(f.c->setState(&stream)==kResultOk&&v->zoomMatches(factor)&&
         f.c->songEntry(0).pattern==SongChain::kRest&&f.c->songStartBar()==2,
         "GUIFINAL actual 100/130 editor zoom changed Song state");
   captureVstgui(*v,factor==1.0?"song-final-alignment-rest-bar2-100":
                                    "song-final-alignment-rest-bar2-130",factor);
 }
 click(*v,UiSongLayout::startResetLeft+25.,181.);
 check(f.c->songStartBar()==1,"GUIFINAL RESET missed");
 click(*v,UiSongLayout::startResetLeft+25.,181.);
 check(f.c->songStartBar()==1,"GUIFINAL disabled RESET changed state");
 evidence("GUIFINAL aligned SONG START, adjacent REST, exact gaps and native 100/130 zoom PASS");
}
void testSongDesignReview12A(){
 Fixture f;auto kit=namedKit('X',4);
 check(kit.chain.setCount(21),"12A chain capacity");
 for(std::size_t i=0;i<21;++i)
   check(kit.chain.configure(i,static_cast<std::uint8_t>((i*3)%8),
                             static_cast<std::uint8_t>(i%3==0?8:i%3==1?16:32)),
         "12A deliberately varied pattern and length");
 kit.songEnabled=1;
 f.kit(kit);f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 click(*v,1077.,114.); 
 check(v->getViewSize().getHeight()==432.&&f.c->songCount()==21,
       "12A actual SONG panel, populated kit and 21 chain entries");
 for(const auto factor:{1.,1.3}){
   const std::array<std::uint8_t,5> state{{'D','Z','M','2',
       static_cast<std::uint8_t>(factor==1.?0:2)}};
   drumator::qa::ByteStream input(const_cast<std::uint8_t*>(state.data()),state.size());
   check(f.c->setState(&input)==kResultOk&&v->zoomMatches(factor),
         "12A real editor 100/130 zoom missing");
   captureVstgui(*v,factor==1.?"song-12a-varied-chain-100":
                            "song-12a-varied-chain-130",factor);
 }
 click(*v,391.,226.);
 check(IntegrationAccess::songSlot(*v)==2,"12A selection mismatch");
 captureVstgui(*v,"song-12a-selected-pattern-length-130",1.3);
 click(*v,1172.,181.);
 check(IntegrationAccess::songPage(*v)==1,"12A second chain page navigation failed");
 captureVstgui(*v,"song-12a-page2-130",1.3);
 evidence("12A actual native SONG editor 100/130, 21 varied chain slots, selection and page 2 PASS");
}
void testSongCompactViewport(){
 Fixture f;f.kit(namedKit('X',4));f.open();
 auto* v=IntegrationAccess::view(*f.c,0);
 check(v->getViewSize().getHeight()==734.,"UX expected four spacious rows");
 click(*v,1077.,114.); // Song edit (compact)
 check(v->getViewSize().getHeight()==432.,"UX Song editor failed to become compact");
 captureVstgui(*v,"ux-song-compact-432",1.0);
 click(*v,1077.,114.); // return to Step grid
 check(v->getViewSize().getHeight()==734.,"UX Step editor height not restored");
 // Reject the enlargement after Song edit. Users must still be able to
 // reach every instrument, not skip rows in a truncated viewport.
 click(*v,1077.,114.);
 check(v->getViewSize().getHeight()==432.,"UX Song second open");
 f.frames[0].accept=false;
 click(*v,1077.,114.); // host refuses resize back to 682
 check(v->getViewSize().getHeight()==432.&&v->visibleRowsForHit()==1,
       "UX refused host resize did not degrade to one reachable row");
 const auto bottom=UiCompactLayout::header+UiCompactLayout::baseRow;
 click(*v,1190.,bottom+17.); // next page: row 2 within refused 370px client
 check(IntegrationAccess::firstRow(*v)==1&&v->visibleRowsForHit()==1,
       "UX compact fallback skipped rows when paging");
 click(*v,45.,UiCompactLayout::header+17.);
 check(IntegrationAccess::selectedRow(*v)==1,"UX compact fallback row 2 not selectable");
 f.frames[0].accept=true;
 click(*v,1090.,bottom+17.); // back to rows 1..4, now resize accepted
 check(IntegrationAccess::firstRow(*v)==0&&
       v->getViewSize().getHeight()==734.&&v->visibleRowsForHit()==4,
       "UX compact fallback cannot recover after host accepts again");
 evidence("UX 370px Song viewport, return to Steps and refused-resize row navigation PASS");
}
void testControllerMetadataCommit(){
 Fixture f;auto initial=namedKit('X');
 initial.displayNames[1].clear(); // row 2 has no custom label
 f.kit(initial);
 // The controller must not notify a successful processor mutation before
 // allocating its replacement display label (OOM before commit).
 std::string replacement(64,'R');
 const auto before=IntegrationAccess::snapshot(*f.c);
 allocationFail=0;
 bool escaped=false,ok=false;
 try {ok=f.c->renameRow(0,replacement);}
 catch(const std::bad_alloc&) {escaped=true;}
 allocationFail=-1;
 check(!escaped&&!ok&&IntegrationAccess::snapshot(*f.c)==before,
       "N11 Controller rename OOM escaped or mutated local state");
 auto savedKit=[&] {
   ResizableMemoryIBStream stream;KitState state;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                          static_cast<std::size_t>(stream.getCursor()),state),
         "N11 processor state decode");
   return state;
 };
 check(savedKit().displayNames[0]==initial.displayNames[0],
       "N11 Controller rename OOM already changed processor");
 check(f.c->renameRow(0,replacement)&&
       f.c->customName(0)==replacement&&
       savedKit().displayNames[0]==replacement,
       "N11 controller/processor success inconsistent");
 // The actual WAV load must record the filename/path only after a successful
 // processor load. Clear must leave a valid, synchronized empty slot.
 prepareAudio(*f.p);
 const auto candidate=constantStereo(.25f,-.25f);
 const auto path=std::filesystem::current_path()/
       "125A-Drumator-N11-test-sample.wav";
 { std::ofstream out(path,std::ios::binary|std::ios::trunc);
   check(out.good(),"N11 create sample file");
   out.write(reinterpret_cast<const char*>(candidate.data()),
             static_cast<std::streamsize>(candidate.size()));
   check(out.good(),"N11 sample write failed");
 }
 const auto pathText=path.string();
 check(f.c->loadWav(1,pathText)==Controller::LoadStatus::Accepted,
       "N11 processor rejected valid sample file");
 check(IntegrationAccess::paths(*f.c)[1]==pathText&&
       savedKit().names[1]==path.filename().string()&&
       f.c->label(1)==path.filename().string(),
       "N11 loaded sample filename/path/processor state diverged");
 check(f.c->clearSample(1)&&IntegrationAccess::paths(*f.c)[1].empty()&&
       f.c->label(1)=="EMPTY - LOAD WAV"&&
       savedKit().names[1].empty()&&savedKit().wav[1].empty(),
       "N11 clear sample did not atomically reset metadata");
 std::error_code ec;std::filesystem::remove(path,ec);
 check(!ec,"N11 fixture sample cleanup failed");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("N11 Controller metadata allocation, WAV load and clear consistency PASS");
}
void testRealSampleFolderStepping(){
 // N12: exercise genuine controller < > sample browsing, not only the
 // pure adjacentSample filename helper. Invalid neighbors are skipped
 // without changing the loaded sound, kit bytes or visible filename.
 Fixture f;
 auto fixture=namedKit('X');fixture.displayNames[0].clear();
 f.kit(fixture);
 prepareAudio(*f.p);
 const auto folder=std::filesystem::current_path()/"125A-Drumator-N12-SampleBrowse";
 std::filesystem::create_directories(folder);
 const auto A=folder/"A-good.wav";
 const auto badB=folder/"B-broken.wav";
 const auto C=folder/"C-good.wav";
 const auto badD=folder/"D-broken.wav";
 const auto wavA=constantStereo(.25f,-.25f);
 const auto wavC=constantStereo(-.5f,.125f);
 auto writeFile=[&](const std::filesystem::path& p,const std::vector<std::uint8_t>& bytes){
   std::ofstream out(p,std::ios::binary|std::ios::trunc);
   check(out.good(),"N12 cannot create folder fixture");
   out.write(reinterpret_cast<const char*>(bytes.data()),
             static_cast<std::streamsize>(bytes.size()));
   check(out.good(),"N12 cannot write folder fixture");
 };
 writeFile(A,wavA);
 writeFile(badB,std::vector<std::uint8_t>(64,0));
 writeFile(C,wavC);
 writeFile(badD,std::vector<std::uint8_t>(64,0));
 auto snapshot=[&]{
   ResizableMemoryIBStream stream;KitState state;
   check(f.p->getState(&stream)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(stream.getData()),
                          static_cast<std::size_t>(stream.getCursor()),state),
         "N12 processor project snapshot");
   return state;
 };
 const auto aPath=A.string(),cPath=C.string();
 check(f.c->loadWav(0,aPath)==Controller::LoadStatus::Accepted,
       "N12 initial real WAV import");
 check(IntegrationAccess::paths(*f.c)[0]==aPath&&
       snapshot().names[0]=="A-good.wav"&&snapshot().wav[0]==wavA,
       "N12 initial sample/controller/project mismatch");
 check(f.c->stepSample(0,+1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==cPath&&
       snapshot().names[0]=="C-good.wav"&&snapshot().wav[0]==wavC,
       "N12 next did not skip invalid WAV and reach next valid sample");
 check(f.c->stepSample(0,-1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==aPath&&
       snapshot().names[0]=="A-good.wav"&&snapshot().wav[0]==wavA,
       "N12 previous did not skip invalid WAV");
 check(f.c->stepSample(0,-1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==cPath&&
       snapshot().wav[0]==wavC,
       "N12 previous did not wrap after invalid final file");
 check(f.c->stepSample(0,+1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==aPath&&
       snapshot().wav[0]==wavA,
       "N12 next did not wrap after invalid final file");
 // SOR02: restore both VST3 state streams and retain the original folder.
 ResizableMemoryIBStream componentState;
 ResizableMemoryIBStream controllerState;
 check(f.p->getState(&componentState)==kResultOk&&
       f.c->getState(&controllerState)==kResultOk,
       "SOR02 could not capture host project state");
 componentState.seek(0,IBStream::kIBSeekSet,nullptr);
 controllerState.seek(0,IBStream::kIBSeekSet,nullptr);
 check(f.c->setComponentState(&componentState)==kResultOk&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "SOR02 matching kit recall discarded valid browsing origin");
 check(f.c->setState(&controllerState)==kResultOk&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "SOR02 controller state did not restore browsing origin");
 check(f.c->stepSample(0,+1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==cPath,
       "SOR02 next sample failed after two-stream project recall");
 check(f.c->stepSample(0,-1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "SOR02 previous sample failed after project recall");
 // Opposite legal host order: Controller state precedes component state.
 componentState.seek(0,IBStream::kIBSeekSet,nullptr);
 controllerState.seek(0,IBStream::kIBSeekSet,nullptr);
 check(f.c->setState(&controllerState)==kResultOk&&
       f.c->setComponentState(&componentState)==kResultOk&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "SOR02 opposite restore ordering lost matching origin");
 check(f.c->stepSample(0,+1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==cPath,
       "SOR02 next failed when component state restored second");
 check(f.c->stepSample(0,-1)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "SOR02 return to original sample failed");

 const auto previous=snapshot();
 check(f.c->stepSample(0,0)==Controller::LoadStatus::Rejected&&
       f.c->stepSample(1,+1)==Controller::LoadStatus::NotFound&&
       snapshot().wav[0]==previous.wav[0]&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "N12 invalid browsing request changed existing sample");
 std::error_code ec;std::filesystem::remove_all(folder,ec);
 check(!ec,"N12 unable to remove sample browse fixtures");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("N12 real sample folder browsing, invalid-neighbor skip, wrap, rollback PASS");
}
void testHostNameAllocationRollback(){
 Fixture f;auto baseline=namedKit('X');f.kit(baseline);
 // Construct the host message before fault injection: only the processor's
 // string allocation may fail. Names > SSO require dynamic storage.
 HostMessage rename;rename.setMessageID("Drumator.RenameRow");
 auto* attrs=rename.getAttributes();
 std::string longName(64,'N');
 check(attrs&&attrs->setInt("Row",0)==kResultOk&&
       attrs->setBinary("DisplayName",longName.data(),
         static_cast<uint32>(longName.size()))==kResultOk,
       "N10 rename host fixture");
 allocationFail=0;
 bool escaped=false;tresult result=kResultFalse;
 try {result=f.p->notify(&rename);}
 catch(const std::bad_alloc&) {escaped=true;}
 allocationFail=-1;
 check(!escaped&&result==kOutOfMemory,
       "N10 failed name allocation escaped VST3 notify or returned wrong result");
 auto readName=[&](){
   ResizableMemoryIBStream buffer;KitState state;
   check(f.p->getState(&buffer)==kResultOk&&
         KitCodec::decode(static_cast<const std::uint8_t*>(buffer.getData()),
                          static_cast<std::size_t>(buffer.getCursor()),state),
         "N10 save real processor name");
   return state.displayNames[0];
 };
 check(readName()==baseline.displayNames[0],
       "N10 rejected RenameRow damaged previous project label");
 check(f.p->notify(&rename)==kResultOk&&readName()==longName,
       "N10 valid RenameRow failed after allocation recovery");
 evidence("N10 processor rename fault injection and unchanged-state rollback PASS");
}
void testNativeName(){
 for(auto key:{VK_RETURN,VK_ESCAPE,0}){
   Fixture f;f.kit(namedKit('X',16));f.open();f.open();
   auto old=nameEditor(f,0,10);SetWindowTextW(old,L"OLD PENDING NAME");
   const auto token=f.c->editGeneration();
   auto bad=Fixture::encode(namedKit('Y'));bad.back()^=1;
   check(f.recall(bad)!=kResultOk&&IsWindow(old)&&f.c->editGeneration()==token,"Z03 rejected recall closed editor");
   check(f.recall(Fixture::encode(namedKit('Y',4)))==kResultOk&&!IsWindow(old),"Z03 successful recall retained old HWND");
   auto* v=IntegrationAccess::view(*f.c,0);const auto name=f.c->customName(0);
   const auto committed=IntegrationAccess::snapshot(*f.c);
   auto* edit=IntegrationAccess::edit(*v);check(edit!=nullptr,"Z03 CTextEdit unexpectedly deleted");
   v->valueChanged(edit);v->onTextEditPlatformControlLostFocus(edit);
   check(IntegrationAccess::snapshot(*f.c)==committed,"Z03 delayed callback renamed new kit");
   auto fresh=nameEditor(f,1,0);SetWindowTextW(fresh,L"FRESH NAME");
   if(key)SendMessageW(fresh,WM_KEYDOWN,static_cast<WPARAM>(key),0);
   else SetFocus(f.frames[1].hwnd);
   pump();
   check(!IsWindow(fresh),"Z03 native editor survived key/focus finish");
   check(f.c->customName(0)==(key==VK_ESCAPE?name:std::string("FRESH NAME")),"Z03 Enter/Escape/blur name contract");
   auto closing=nameEditor(f,1,0);f.closeLast();check(!IsWindow(closing),"Z03 remove kept HWND");
 }
 evidence("Z03 PASS: real Win32 EDIT lifetime, rejected/accepted 16-to-4 recall, late live-listener callbacks, Enter/Escape/blur, two views and close");
}
#include "FunctionalAuditCases.inl"
#include "SecondAuditCases.inl"
void testTransportSequenceTorture(){
 Fixture f;KitState kit;kit.visible=1;kit.grid.fill(0);
 kit.names[0]="transport.wav";kit.wav[0]=constantStereo(.25f,-.5f);
 f.kit(kit);prepareAudio(*f.p);
 AuditAudioBlock block;
 constexpr auto playing=ProcessContext::kPlaying|ProcessContext::kTempoValid|
                        ProcessContext::kProjectTimeMusicValid;
 EventList notes;Event hit{};hit.type=Event::kNoteOnEvent;
 hit.noteOn.pitch=36;hit.noteOn.velocity=1.f;hit.noteOn.noteId=-1;
 check(notes.addEvent(hit)==kResultOk,"TORTURE MIDI trigger fixture");
 unsigned stale=0,wrongSilence=0,missingHits=0;
 for(unsigned cycle=0;cycle<96;++cycle){
   // Empty pattern: every sound MUST originate from this explicit MIDI hit.
   // This detects a missed hit separately from a stale voice or double hit.
   block.data.numSamples=256;
   block.clock.state=playing;
   block.clock.projectTimeMusic=(cycle%2?4.:8.);
   block.data.inputEvents=&notes;
   check(f.p->process(block.data)==kResultOk,"TORTURE MIDI trigger block");
   // Verify sample-exact onset and prefix, not only an interior sample.
   // A delayed MIDI event or missing initial PCM must fail mutation QA.
   for(std::size_t sample=0;sample<128;++sample)
     if(std::abs(block.left[sample]-.25f)>1e-6f||
        std::abs(block.right[sample]+.5f)>1e-6f)++missingHits;
   block.data.inputEvents=nullptr;
   block.data.numSamples=(cycle%3==0?0:256);
   block.clock.state=0;
   check(f.p->process(block.data)==kResultOk,"TORTURE stopped callback");
   if(IntegrationAccess::activeAudioVoices(*f.p)!=0)++stale;
   block.data.numSamples=256;block.clock.state=playing;
   block.clock.projectTimeMusic=(cycle%4==0?4.:8.);
   check(f.p->process(block.data)==kResultOk,"TORTURE restart block");
   if(block.left[80]!=0.f||block.right[80]!=0.f)++wrongSilence;
   // Moving the host clock while playing must never resurrect old voices.
   block.clock.projectTimeMusic=(cycle%2?8.:4.);
   check(f.p->process(block.data)==kResultOk,"TORTURE locate/loop block");
   if(block.left[80]!=0.f||block.right[80]!=0.f)++wrongSilence;
 }
 qa::workflowNotice("TORTURE 96 MIDI/stop/restart/loop sequences: stale="+
   std::to_string(stale)+" missing-or-doubled-hits="+std::to_string(missingHits)+
   " unexpected-PCM="+std::to_string(wrongSilence));
 f.p->setProcessing(false);f.p->setActive(false);
 check(stale==0&&missingHits==0&&wrongSilence==0,
       "TORTURE missed/doubled trigger, stale voice or unexpected output");
}
void testPacedSampleSwapContinuity(){
 using Clock=std::chrono::steady_clock;
 Fixture active,baseline;
 KitState kit;kit.grid.fill(0);kit.visible=2;
 kit.names[1]="reference.wav";kit.wav[1]=constantStereo(.25f,-.5f);
 active.kit(kit);baseline.kit(kit);
 prepareAudio(*active.p);prepareAudio(*baseline.p);
 const auto folder=std::filesystem::current_path()/"125A-paced-import-QA";
 std::filesystem::create_directories(folder);
 const auto fileA=folder/"swapA.wav",fileB=folder/"swapB.wav";
 for(const auto& [path,left,right]:std::array<std::tuple<std::filesystem::path,float,float>,2>{
       std::tuple{fileA,.125f,-.25f},std::tuple{fileB,-.375f,.125f}}){
   const auto wav=constantStereo(left,right);
   std::ofstream output(path,std::ios::binary|std::ios::trunc);
   output.write(reinterpret_cast<const char*>(wav.data()),
                static_cast<std::streamsize>(wav.size()));
   check(output.good(),"PACING fixture write");
 }
 std::atomic<unsigned> blocks{0},badPcm{0},errors{0},misses{0};
 std::atomic<double> maximumMs{0.};
 // Audio actor owns writes; read only after join. Fixed memory, no
 // allocations in the callback or measured pacing loop.
 std::array<double,500> callbackMs{};
 std::jthread actor([&](std::stop_token stop){
   AuditAudioBlock tested,reference;
   EventList events;Event note{};note.type=Event::kNoteOnEvent;
   note.noteOn.pitch=37;note.noteOn.velocity=1.f;note.noteOn.noteId=-1;
   if(events.addEvent(note)!=kResultOk){++errors;return;}
   tested.data.inputEvents=&events;reference.data.inputEvents=&events;
   auto target=Clock::now();
   for(unsigned n=0;n<500&&!stop.stop_requested();++n){
     const auto begin=Clock::now();
     const auto ok=active.p->process(tested.data);
     const auto end=Clock::now();
     const double ms=std::chrono::duration<double,std::milli>(end-begin).count();
     callbackMs[n]=ms;
     if(ms>256./48000.*1000.)++misses;
     auto peak=maximumMs.load();
     while(ms>peak&&!maximumMs.compare_exchange_weak(peak,ms)){}
     if(ok!=kResultOk||baseline.p->process(reference.data)!=kResultOk)++errors;
     for(std::size_t i=0;i<256;++i)
       if(tested.left[i]!=reference.left[i]||tested.right[i]!=reference.right[i])
         ++badPcm;
     ++blocks;
     target+=std::chrono::duration_cast<Clock::duration>(
                  std::chrono::duration<double>(256./48000.));
     std::this_thread::sleep_until(target);
   }
 });
 while(blocks.load()<4&&errors.load()==0)std::this_thread::yield();
 for(unsigned i=0;i<12;++i){
   const auto path=(i&1)?fileA:fileB;
   if(active.c->loadWav(0,pathToUtf8(path))!=Controller::LoadStatus::Accepted)
     ++errors;
 }
 actor.join();
 // Assert that accepted controller imports actually reach the processor kit.
 // Unchanged-row PCM alone cannot distinguish an ignored import from success.
 ResizableMemoryIBStream importedState;
 KitState importedKit;
 check(active.p->getState(&importedState)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(importedState.getData()),
                        static_cast<std::size_t>(importedState.getCursor()),importedKit),
       "PACING imported kit state decode");
 check(importedKit.wav[0]==constantStereo(.125f,-.25f),
       "PACING accepted sample import did not update processor asset");
 // Compute quantiles off the actor thread. An otherwise green PCM
 // comparison must never hide measured audio callback overruns.
 auto sortedCallbackMs=callbackMs;
 std::sort(sortedCallbackMs.begin(),sortedCallbackMs.end());
 const double p95=sortedCallbackMs[474],p99=sortedCallbackMs[494];
 qa::workflowNotice("PACING 48k/256 blocks="+std::to_string(blocks.load())+
   " unchanged-row PCM mismatches="+std::to_string(badPcm.load())+
   " deadline-misses="+std::to_string(misses.load())+
   " p95-ms="+std::to_string(p95)+" p99-ms="+std::to_string(p99)+
   " max-callback-ms="+std::to_string(maximumMs.load())+
   "; paced shared Windows runner, NOT Studio One/ASIO certification");
 std::error_code ec;std::filesystem::remove_all(folder,ec);
 active.p->setProcessing(false);active.p->setActive(false);
 baseline.p->setProcessing(false);baseline.p->setActive(false);
 check(blocks.load()==500&&badPcm.load()==0&&errors.load()==0,
       "PACING actual-controller sample swaps disrupted unchanged audio");
 check(misses.load()==0,
       "PACING missed an audio deadline despite a previously green PCM result");
}
void testComponentOnlySameBasenameRejectsStaleOrigin(){
 Fixture f;
 KitState initial;initial.grid.fill(0);initial.visible=1;
 f.kit(initial);prepareAudio(*f.p);
 const auto directory=std::filesystem::current_path()/"125A-v1a04-source";
 std::filesystem::create_directories(directory);
 const auto source=directory/"kick.wav";
 {
   const auto bytes=constantStereo(.25f,-.5f);
   std::ofstream out(source,std::ios::binary);
   out.write(reinterpret_cast<const char*>(bytes.data()),
             static_cast<std::streamsize>(bytes.size()));
   check(out.good(),"V1A04 write source WAV");
 }
 check(f.c->loadWav(0,pathToUtf8(source))==Controller::LoadStatus::Accepted,
       "V1A04 load source via real controller");
 check(!IntegrationAccess::paths(*f.c)[0].empty(),
       "V1A04 expected original navigation path");
 KitState other;other.grid.fill(0);other.visible=1;
 other.names[0]="kick.wav";other.wav[0]=constantStereo(-.125f,.375f);
 const auto state=Fixture::encode(other);
 check(f.recall(state)==kResultOk,"V1A04 component-only different kit recall");
 check(IntegrationAccess::paths(*f.c)[0].empty(),
       "V1A04 component-only recall retained stale same-basename origin");
 std::error_code ec;std::filesystem::remove_all(directory,ec);
}
void testPeerVelocityCancelPreservesCommittedStep(){
 Fixture f;KitState kit;kit.grid.fill(0);kit.grid[0]=60;f.kit(kit);
 f.open();f.open();
 auto* first=IntegrationAccess::view(*f.c,0);
 auto* peer=IntegrationAccess::view(*f.c,1);
 click(*first,1248.,UiCompactLayout::header+88.);
 click(*first,630.,UiCompactLayout::header+UiCompactLayout::baseRow+12.);
 check(std::get<0>(IntegrationAccess::viewSnapshot(*first)),
       "V1A03 first view gesture absent");
 click(*peer,630.,UiCompactLayout::header+UiCompactLayout::baseRow+88.);
 check(f.c->velocity(0,1,0)==100,
       "V1A03 peer step was not committed");
 // The peer edit commits the first gesture, so its last velocity
 // must survive a subsequently delivered stale Cancel unchanged.
 const auto committedOwnVelocity=f.c->velocity(0,0,0);
 first->onMouseCancel();
 check(f.c->velocity(0,1,0)==100,
       "V1A03 late Cancel erased second view step");
 check(f.c->velocity(0,0,0)==committedOwnVelocity,
       "V1A03 stale Cancel changed previously committed velocity");
}
void testZeroFrameStopRetiresVoice(){
 Fixture f;KitState kit;kit.grid.fill(0);kit.visible=1;
 kit.names[0]="zero-stop.wav";kit.wav[0]=constantStereo(.25f,-.5f);
 f.kit(kit);prepareAudio(*f.p);
 AuditAudioBlock audio;
 constexpr auto playing=ProcessContext::kPlaying|ProcessContext::kTempoValid|
                        ProcessContext::kProjectTimeMusicValid;
 audio.clock.state=playing;audio.clock.projectTimeMusic=4.;
 EventList notes;Event hit{};hit.type=Event::kNoteOnEvent;hit.noteOn.pitch=36;
 hit.noteOn.velocity=1.f;hit.noteOn.noteId=-1;
 check(notes.addEvent(hit)==kResultOk,"SOR03Z trigger fixture");
 audio.data.inputEvents=&notes;
 check(f.p->process(audio.data)==kResultOk&&IntegrationAccess::activeAudioVoices(*f.p)==1,
       "SOR03Z initial voice missing");
 audio.data.inputEvents=nullptr;audio.data.numSamples=0;
 audio.clock.state=0;audio.clock.projectTimeMusic=4.+256./24000.;
 check(f.p->process(audio.data)==kResultOk&&IntegrationAccess::activeAudioVoices(*f.p)==0,
       "SOR03Z zero-frame stopped callback failed to retire voice");
 audio.data.numSamples=256;audio.clock.state=playing;
 check(f.p->process(audio.data)==kResultOk&&audio.left[80]==0.f&&audio.right[80]==0.f,
       "SOR03Z continuous PPQ restart resumed old voice");
 f.p->setProcessing(false);f.p->setActive(false);
}
void testTransportJumpRetiresOldVoices(){
 Fixture f;
 KitState kit;kit.visible=1;kit.grid.fill(0);kit.grid[0]=100;
 kit.names[0]="restart.wav";kit.wav[0]=constantStereo(.25f,.25f);
 f.kit(kit);prepareAudio(*f.p);
 AuditAudioBlock block;
 block.clock.state=ProcessContext::kPlaying|
       ProcessContext::kTempoValid|ProcessContext::kProjectTimeMusicValid;
 block.clock.projectTimeMusic=4.0;
 check(f.p->process(block.data)==kResultOk,"SOR03 first transport block");
 check(IntegrationAccess::activeAudioVoices(*f.p)==1,
       "SOR03 first voice absent");
 // Simulate a host which delivers no stopped transport process callback.
 // It locates to bar three, preserving playing=true on both observed blocks.
 block.clock.projectTimeMusic=8.0;
 check(f.p->process(block.data)==kResultOk,"SOR03 host jump process");
 check(IntegrationAccess::activeAudioVoices(*f.p)==1,
       "SOR03 stale previous-run voice survived host locate");
 // Render the full callback, not only a voice counter. A loop discontinuity
 // must produce a valid finite new hit; stale voices must not double the PCM.
 check(std::isfinite(block.left[80])&&std::isfinite(block.right[80])&&
       std::abs(block.left[80])<=.25f&&std::abs(block.right[80])<=.25f,
       "SOR03 loop jump produced doubled or nonfinite sample audio");
 f.p->setProcessing(false);f.p->setActive(false);
 evidence("SOR03 native processor timeline jump retires old voice without stopped callback PASS");
}
#include "ReauditCases.inl"
#include "FinalAudit20261023.inl"
#include "V101FullGuiAudit.inl"

struct A01SerializedConnection final:Steinberg::Vst::IConnectionPoint {
 IConnectionPoint* target;
 bool holdReplies=false;
 std::size_t copied=0;
 std::vector<Steinberg::Vst::HostMessage*> delayed;
 explicit A01SerializedConnection(IConnectionPoint* p):target(p){}
 ~A01SerializedConnection() {for(auto* m:delayed)m->release();}
 tresult PLUGIN_API queryInterface(const TUID id,void** out) override {
   if(!out)return kInvalidArgument;
   if(FUnknownPrivate::iidEqual(id,IConnectionPoint::iid)||
      FUnknownPrivate::iidEqual(id,FUnknown::iid)){
     *out=static_cast<IConnectionPoint*>(this);addRef();return kResultOk;
   }*out=nullptr;return kNoInterface;
 }
 uint32 PLUGIN_API addRef() override{return 1;}
 uint32 PLUGIN_API release() override{return 1;}
 tresult PLUGIN_API connect(IConnectionPoint*) override {return kResultOk;}
 tresult PLUGIN_API disconnect(IConnectionPoint*) override {return kResultOk;}
 tresult PLUGIN_API notify(IMessage* original) override {
   if(!original||!original->getMessageID())return kInvalidArgument;
   const std::string id=original->getMessageID();
   const bool copiedType=id=="Drumator.QueryPattern"||
     id=="Drumator.PatternReply"||id=="Drumator.SampleEditorQuery"||
     id=="Drumator.SampleEditorReply";
   if(!copiedType)return target->notify(original);
   HostMessage* clone=nullptr;
   try{
     clone=new HostMessage();
     clone->setMessageID(id.c_str());
     auto* input=original->getAttributes();
     auto* output=clone->getAttributes();
     check(input&&output,"A01 proxy source attributes");
     for(const auto* field:{"RequestId","Row","Status","Selected","Revision",
                             "Start","End","FadeIn","FadeOut","GainBits","KnownRevision"}){
       Steinberg::int64 value=0;
       if(input->getInt(field,value)==kResultOk)
         check(output->setInt(field,value)==kResultOk,"A01 proxy int serialization");
     }
     const void* sample=nullptr;Steinberg::uint32 length=0;
     if(input->getBinary("Wave",sample,length)==kResultOk)
       check(output->setBinary("Wave",sample,length)==kResultOk,
             "A01 proxy WAV serialization");
     ++copied;
     const bool reply=id=="Drumator.PatternReply"||id=="Drumator.SampleEditorReply";
     if(reply&&holdReplies){delayed.push_back(clone);return kResultOk;}
     const auto result=target->notify(clone);
     clone->release();return result;
   }catch(...){if(clone)clone->release();return kResultFalse;}
 }
 void flush() {
   auto pending=std::move(delayed);
   delayed.clear();
   for(auto* msg:pending){
     const auto result=target->notify(msg);
     msg->release();
     check(result==kResultOk,"A01 copied delayed reply rejected");
   }
 }
};
void testA01SerializedAsyncIPC() {
 // The intermediate host copies messages. Mutations of the original request
 // can never reach the caller; replies are separate and may arrive later.
 {
   Fixture f;KitState kit;kit.grid.fill(0);
   kit.names[0]="proxy.wav";kit.wav[0]=constantStereo(.25f,-.5f);
   f.kit(kit);f.open();
   check(f.c->disconnect(f.p)==kResultOk&&f.p->disconnect(f.c)==kResultOk,
         "A01 disconnect direct host");
   A01SerializedConnection toProcessor(f.p),toController(f.c);
   toController.holdReplies=true;
   check(f.c->connect(&toProcessor)==kResultOk&&f.p->connect(&toController)==kResultOk,
         "A01 connect serializing host");
   auto* view=IntegrationAccess::view(*f.c,0);
   click(*view,240.,UiCompactLayout::header+20.);
   check(!IntegrationAccess::sampleEditorOpen(*view)&&
         f.c->sampleEditorRequestPending(0,view)&&toProcessor.copied>0&&toController.copied>0,
         "A01 editor assumed synchronous mutated request");
   toController.flush();
   check(toProcessor.copied==1,"A01 delayed reply triggered recursive extra query");
   check(IntegrationAccess::sampleEditorOpen(*view)&&
         IntegrationAccess::editorStereo(*view)&&
         std::abs(IntegrationAccess::editorLeft(*view)[0].minimum-.25f)<.001f&&
         std::abs(IntegrationAccess::editorRight(*view)[0].maximum+.5f)<.001f,
         "A01 separate delayed Wave reply did not open real stereo editor");
   check(f.c->disconnect(&toProcessor)==kResultOk&&
         f.p->disconnect(&toController)==kResultOk,
         "A01 proxy disconnect");
   check(f.c->connect(f.p)==kResultOk&&f.p->connect(f.c)==kResultOk,
         "A01 restore original connection");
 }
 {
   Fixture f;KitState kit;kit.grid.fill(0);
   kit.names[0]="stale.wav";kit.wav[0]=constantStereo(.25f,-.5f);
   f.kit(kit);f.open();
   check(f.c->disconnect(f.p)==kResultOk&&f.p->disconnect(f.c)==kResultOk,
         "A01 stale Wave disconnect");
   A01SerializedConnection toProcessor(f.p),toController(f.c);
   toController.holdReplies=true;
   check(f.c->connect(&toProcessor)==kResultOk&&f.p->connect(&toController)==kResultOk,
         "A01 stale Wave proxy");
   auto* view=IntegrationAccess::view(*f.c,0);
   click(*view,240.,UiCompactLayout::header+20.);
   check(f.c->sampleEditorRequestPending(0,view),"A01 Wave response not delayed");
   check(f.recall(Fixture::encode(kit))==kResultOk,"A01 recall during pending Wave");
   toController.flush();
   check(!f.c->sampleEditorRequestPending(0,view)&&
         !IntegrationAccess::sampleEditorOpen(*view),
         "A01 old Wave reply reopened discarded project editor");
   check(f.c->disconnect(&toProcessor)==kResultOk&&
         f.p->disconnect(&toController)==kResultOk,"A01 stale Wave proxy disconnect");
   check(f.c->connect(f.p)==kResultOk&&f.p->connect(f.c)==kResultOk,
         "A01 stale Wave restore direct");
 }
 {
   Fixture f;KitState kit;kit.grid.fill(0);kit.selected=0;f.kit(kit);
   check(f.c->disconnect(f.p)==kResultOk&&f.p->disconnect(f.c)==kResultOk,
         "A01 pattern fixture disconnect");
   A01SerializedConnection toProcessor(f.p),toController(f.c);
   toController.holdReplies=true;
   check(f.c->connect(&toProcessor)==kResultOk&&f.p->connect(&toController)==kResultOk,
         "A01 pattern proxy connect");
   IntegrationAccess::setProcessorPattern(*f.p,1);
   check(f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk,
         "A01 host pattern fixture");
   const auto state=Fixture::encode(kit);
   check(f.recall(state)==kResultOk&&f.c->getParamNormalized(kPatternParam)<.001,
         "A01 pattern recall must not wait for reply");
   toController.flush();
   check(std::abs(f.c->getParamNormalized(kPatternParam)-1./7.)<.0001,
         "A01 host pattern not reconciled on delayed independent reply");
   check(f.c->disconnect(&toProcessor)==kResultOk&&
         f.p->disconnect(&toController)==kResultOk,"A01 pattern proxy disconnect");
   check(f.c->connect(f.p)==kResultOk&&f.p->connect(f.c)==kResultOk,
         "A01 pattern restore direct connection");
 }
 {
   // An old reply must lose to subsequent host automation, even if it was
   // serialized correctly and delivered after a successful Component recall.
   Fixture f;KitState kit;kit.grid.fill(0);kit.selected=0;f.kit(kit);
   check(f.c->disconnect(f.p)==kResultOk&&f.p->disconnect(f.c)==kResultOk,"A01 stale disconnect");
   A01SerializedConnection toProcessor(f.p),toController(f.c);
   toController.holdReplies=true;
   check(f.c->connect(&toProcessor)==kResultOk&&f.p->connect(&toController)==kResultOk,
         "A01 stale copied IPC");
   IntegrationAccess::setProcessorPattern(*f.p,1);
   check(f.c->setParamNormalized(kPatternParam,1./7.)==kResultOk,"A01 stale request host");
   check(f.recall(Fixture::encode(kit))==kResultOk,"A01 stale recall");
   check(f.c->setParamNormalized(kPatternParam,2./7.)==kResultOk,"A01 later host wins");
   toController.flush();
   check(std::abs(f.c->getParamNormalized(kPatternParam)-2./7.)<.0001,
         "A01 stale delayed pattern response overwrote newer automation");
   check(f.c->disconnect(&toProcessor)==kResultOk&&f.p->disconnect(&toController)==kResultOk,
         "A01 stale proxy disconnect");
   check(f.c->connect(f.p)==kResultOk&&f.p->connect(f.c)==kResultOk,
         "A01 stale restore connection");
 }

 // Cross-view, cross-row, asynchronous viewport and lifetime regression.
 {
   Fixture f;KitState kit;kit.grid.fill(0);
   kit.names[0]="viewA.wav";kit.wav[0]=constantStereo(.25f,-.5f);
   kit.names[1]="viewB.wav";kit.wav[1]=constantStereo(.4f,-.3f);
   f.kit(kit);f.open();f.open();
   check(f.c->disconnect(f.p)==kResultOk&&f.p->disconnect(f.c)==kResultOk,
         "A01 two-view disconnect");
   A01SerializedConnection toProcessor(f.p),toController(f.c);
   toController.holdReplies=true;
   check(f.c->connect(&toProcessor)==kResultOk&&f.p->connect(&toController)==kResultOk,
         "A01 two-view connect");
   auto* a=IntegrationAccess::view(*f.c,0);
   auto* b=IntegrationAccess::view(*f.c,1);
   check(!IntegrationAccess::openSampleEditorForCapture(*a,0)&&
         !IntegrationAccess::openSampleEditorForCapture(*b,1),
         "A01 different views must wait for distinct replies");
   check(f.c->sampleEditorRequestPending(0,a)&&f.c->sampleEditorRequestPending(1,b)&&
         IntegrationAccess::outstandingWaves(*f.c)==2&&
         IntegrationAccess::requestId(*f.c,a)!=IntegrationAccess::requestId(*f.c,b),
         "A01 cross-view requests conflated");
   toController.flush();
   check(IntegrationAccess::sampleEditorOpen(*a)&&IntegrationAccess::sampleEditorOpen(*b)&&
         IntegrationAccess::editorStereo(*a)&&IntegrationAccess::editorStereo(*b)&&
         std::abs(IntegrationAccess::editorLeft(*a)[0].minimum-.25f)<.001f&&
         std::abs(IntegrationAccess::editorLeft(*b)[0].minimum-.4f)<.001f,
         "A01 delayed cross-row independent WAV responses failed");
   check(IntegrationAccess::outstandingWaves(*f.c)==0,
         "A01 completed replies retain binary messages");
   const auto full=IntegrationAccess::editorViewport(*a);
   check(IntegrationAccess::zoomSample(*a,true),"A01 async zoom not accepted");
   check(IntegrationAccess::editorViewport(*a)==full&&f.c->sampleEditorRequestPending(0,a),
         "A01 delayed zoom applied before its reply");
   toController.flush();
   const auto zoomed=IntegrationAccess::editorViewport(*a);
   check(zoomed.second-zoomed.first==(full.second-full.first)/2,
         "A01 delayed zoom did not update viewport");
   check(IntegrationAccess::scrollSample(*a,930.),"A01 delayed scroll not accepted");
   check(IntegrationAccess::editorViewport(*a)==zoomed,"A01 scroll applied before reply");
   toController.flush();
   const auto scrolled=IntegrationAccess::editorViewport(*a);
   check(scrolled.first>zoomed.first,"A01 delayed scroll did not move viewport");
   // Edge autoscroll at the left boundary must defer marker application until
   // the matching waveform viewport is ready, not apply stale coordinates.
   const auto oldEdit=IntegrationAccess::sampleEditorDraft(*a);
   IntegrationAccess::moveEditorMarker(*a,280.,false);
   check(f.c->sampleEditorRequestPending(0,a)&&
         IntegrationAccess::sampleEditorDraft(*a).start==oldEdit.start,
         "A01 marker edge scroll was committed against stale viewport");
   toController.flush();
   check(IntegrationAccess::editorViewport(*a).first<scrolled.first&&
         IntegrationAccess::sampleEditorDraft(*a).start>oldEdit.start,
         "A01 deferred marker edge scroll not completed");
   // Invalid reply has matching correlation but missing required metadata.
   check(IntegrationAccess::zoomSample(*a,false),"A01 malformed reply setup");
   const auto pendingId=IntegrationAccess::requestId(*f.c,a);
   check(pendingId>0,"A01 missing pending viewport request ID");
   auto* invalid=new HostMessage();invalid->setMessageID("Drumator.SampleEditorReply");
   auto* attrs=invalid->getAttributes();
   check(attrs->setInt("RequestId",static_cast<Steinberg::int64>(pendingId))==kResultOk&&
         attrs->setInt("Row",0)==kResultOk&&
         attrs->setInt("Status",1)==kResultOk,"A01 malformed reply setup attributes");
   check(f.c->notify(invalid)==kInvalidArgument&&f.c->sampleEditorRequestPending(0,a),
         "A01 malformed reply incorrectly completed pending action");
   invalid->release();
   toController.flush();
   check(!f.c->sampleEditorRequestPending(0,a),"A01 valid reply after malformed not accepted");
   // Destroy one native View while its own full PCM reply is outstanding.
   check(IntegrationAccess::zoomSample(*b,true),"A01 close-pending setup");
   check(f.c->sampleEditorRequestPending(1,b),"A01 pending close not registered");
   f.closeLast();
   check(IntegrationAccess::outstandingWaves(*f.c)==0,
         "A01 closing view retained pending reply or WAV");
   toController.flush(); // stale copied response may not dereference destroyed View
   check(IntegrationAccess::sampleEditorOpen(*a),"A01 surviving View was affected by close");
   // Successful project recall cancels pending viewport and prevents stale UI.
   check(IntegrationAccess::zoomSample(*a,true),"A01 recall-pending setup");
   check(f.recall(Fixture::encode(kit))==kResultOk,"A01 concurrent recall");
   toController.flush();
   check(!IntegrationAccess::sampleEditorOpen(*a)&&
         IntegrationAccess::outstandingWaves(*f.c)==0,
         "A01 delayed reply resurrected closed editor after recall");
   check(f.c->disconnect(&toProcessor)==kResultOk&&
         f.p->disconnect(&toController)==kResultOk,"A01 complete proxy teardown");
   check(f.c->connect(f.p)==kResultOk&&f.p->connect(f.c)==kResultOk,
         "A01 restore direct connection");
 }
 evidence("A01 delayed zoom/scroll/marker, independent two Views, malformed reply, close and recall PASS");
 evidence("A01 copied IConnectionPoint request/reply, delayed editor and pattern reconciliation PASS");
}
void testA03ProcessorMidiOverflow() {
 Fixture f;KitState kit;kit.grid.fill(0);f.kit(kit);
 prepareAudio(*f.p);
 AuditAudioBlock block;
 EventList events(2300);
 Event event{};
 event.type=Event::kNoteOnEvent;event.noteOn.pitch=36;event.noteOn.velocity=1.f;
 event.sampleOffset=0;
 for(int i=0;i<2048;++i)check(events.addEvent(event)==kResultOk,"A03 fill MIDI fixture");
 block.data.inputEvents=&events;
 check(f.p->process(block.data)==kResultOk&&
       IntegrationAccess::overflowEvents(*f.p)==0,
       "A03 exactly 2048 events wrongly dropped");
 events.clear();
 for(int i=0;i<2200;++i)check(events.addEvent(event)==kResultOk,"A03 overflow MIDI fixture");
 check(f.p->process(block.data)==kResultOk&&
       IntegrationAccess::overflowEvents(*f.p)==152,
       "A03 2200 events must deterministically report 152 overflow hits");
 f.c->requestMeterSnapshot();
 check(f.c->meterState().triggerDrops==152,"A03 overflow not observable through host-safe meter IPC");
 events.clear();
 event.noteOn.pitch=60;
 for(int i=0;i<2200;++i)check(events.addEvent(event)==kResultOk,"A03 invalid pitch fixture");
 check(f.p->process(block.data)==kResultOk&&
       IntegrationAccess::overflowEvents(*f.p)==152,
       "A03 invalid pitch counted as overflow");

 // Host MIDI work budget: only 4096 inspected even with a much larger list.
 EventList huge(16000);
 event.noteOn.pitch=36;event.noteOn.velocity=1.f;
 for(int i=0;i<16000;++i)check(huge.addEvent(event)==kResultOk,"A03 huge MIDI fixture");
 block.data.inputEvents=&huge;
 const auto beforeInspected=IntegrationAccess::hostInspected(*f.p);
 const auto beforeValid=IntegrationAccess::hostValid(*f.p);
 const auto beforeSkipped=IntegrationAccess::hostUninspected(*f.p);
 const auto beforeDrops=IntegrationAccess::overflowEvents(*f.p);
 const auto start=std::chrono::steady_clock::now();
 allocationFail=0;
 const auto result=f.p->process(block.data);
 allocationFail=-1;
 const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(
     std::chrono::steady_clock::now()-start).count();
 check(result==kResultOk&&
       IntegrationAccess::hostInspected(*f.p)-beforeInspected==4096&&
       IntegrationAccess::hostValid(*f.p)-beforeValid==4096&&
       IntegrationAccess::hostUninspected(*f.p)-beforeSkipped==11904&&
       IntegrationAccess::overflowEvents(*f.p)-beforeDrops==2048,
       "A03 16000 host events did not respect 4096 inspection / 2048 capacity");
 f.c->requestMeterSnapshot();
 check(f.c->meterState().hostInspected==IntegrationAccess::hostInspected(*f.p)&&
       f.c->meterState().hostValidNotes==IntegrationAccess::hostValid(*f.p)&&
       f.c->meterState().hostUninspected==IntegrationAccess::hostUninspected(*f.p),
       "A03 separate host budget diagnostics lost in meter IPC");
 huge.clear();event.noteOn.pitch=60;
 for(int i=0;i<16000;++i)check(huge.addEvent(event)==kResultOk,"A03 invalid huge MIDI fixture");
 const auto invalidBase=IntegrationAccess::hostValid(*f.p);
 const auto invalidDrops=IntegrationAccess::overflowEvents(*f.p);
 const auto invalidSkipped=IntegrationAccess::hostUninspected(*f.p);
 check(f.p->process(block.data)==kResultOk&&
       IntegrationAccess::hostValid(*f.p)==invalidBase&&
       IntegrationAccess::overflowEvents(*f.p)==invalidDrops&&
       IntegrationAccess::hostUninspected(*f.p)-invalidSkipped==11904,
       "A03 invalid notes misclassified as valid or rejected triggers");
 huge.clear();event.noteOn.pitch=36;
 for(int i=0;i<4096;++i)check(huge.addEvent(event)==kResultOk,"A03 preview mixed fixture");
 IntegrationAccess::pendingPreview(*f.p,0);
 const auto previewDrops=IntegrationAccess::overflowEvents(*f.p);
 check(f.p->process(block.data)==kResultOk&&
       IntegrationAccess::overflowEvents(*f.p)-previewDrops==2049,
       "A03 GUI preview must count one genuine capacity rejection");
 evidence("A03 bounded 4096-event inspection of 16000-host-event block; duration "+
          std::to_string(elapsed)+" us offline; valid/dropped/skipped distinct PASS");
 evidence("A03 real processor 2048/2200 MIDI density, deterministic overflow and invalid-event exclusion PASS");
}
}
int main(int argc,char** argv){
 std::cout<<std::unitbuf;
 if(argc!=2&&argc!=3)return 2;
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
 CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
 moduleHandle=GetModuleHandleW(nullptr);VSTGUI::init(static_cast<HINSTANCE>(moduleHandle));
 auto* platform=VSTGUI::getPlatformFactory().asWin32Factory();
 if(!platform)return 3;platform->setResourceBasePath(VSTGUI::UTF8String(argv[1]));
 entered=CreateEventW(nullptr,TRUE,FALSE,nullptr);resume=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 if(argc==3){
   if(std::strcmp(argv[2],"--v101-preview-render")==0){
     int result=0;
     try {
       captureV101Preview("v101-layout-native-prototype-100",1.0);
       captureV101Preview("v101-layout-native-prototype-130",1.3);
       captureV101Preview("v101-layout-native-zoom-menu-100",1.0,true);
       captureV101Preview("v101-controller-velocity-lane-100",1.0,false,V101CaptureMode::Velocity);
       captureV101Preview("v101-controller-settings-lane-100",1.0,false,V101CaptureMode::Settings);
     }catch(const std::exception& ex){
       std::cerr<<"V1.0.1 native preview render FAIL: "<<ex.what()<<'\n';
       result=3;
     }
     CloseHandle(entered);CloseHandle(resume);
     VSTGUI::exit();CoUninitialize();return result;
   }
   if(std::strcmp(argv[2],"--packaged-logo-render")==0){
     int result=0;
     try {
       Fixture f;f.open();
       auto* view=IntegrationAccess::view(*f.c,0);
       check(view&&IntegrationAccess::bundledBrandingLoaded(*view),
             "packaged PNGs were not decoded into native VSTGUI bitmaps");
       captureVstgui(*view,"packaged-vst3-resources-silver-100",1.0);
       evidence("Packaged VST3 Resources: exact metallic logo and faceplate decoded; native Windows VSTGUI offscreen pixels show silver logo PASS");
     }catch(const std::exception& ex){
       std::cerr<<"Packaged resource native render FAIL: "<<ex.what()<<'\n';
       result=3;
     }
     CloseHandle(entered);CloseHandle(resume);
     VSTGUI::exit();CoUninitialize();return result;
   }
   const bool warm=std::strcmp(argv[2],"--reaudit-song-warm")==0;
   if(!warm&&std::strcmp(argv[2],"--reaudit-song-cold")!=0)return 2;
   int childResult=0;
   try{testReauditSongCold(warm);}catch(const std::exception& ex){std::cerr<<ex.what()<<'\n';childResult=3;}
   CloseHandle(entered);CloseHandle(resume);VSTGUI::exit();CoUninitialize();return childResult;
 }

 int failures=0;
 std::string auditDiagnostics;drumator::qa::collectedNotices=&auditDiagnostics;
 std::string qaCaseSummary="Executed native integration cases:\n";
 auto run=[&](const std::string& name,const auto& test){
   try{test();const auto line=name+" PASS";evidence(line);qaCaseSummary+=line+'\n';}
   catch(const std::exception& ex){allocationFail=-1;releaseProbe();++failures;
     const auto line=name+" FAIL: "+ex.what();evidence(line);qaCaseSummary+=line+'\n';}
 };
 run("V1FIX factory, row COPY/PASTE, DUPLICATE/UNDO, routing",testV1FourFinalCorrections);
 run("GUI50 dedicated ROW menu/clear/undo and FILL-only",testV1RowMenuClearScope);
 run("SONGSTART absolute timeline REST, tail, state and GUI",testAbsoluteSongStartRest);
 run("RE01 original deterministic VFA02",testReauditPatternLength);
 run("RE02 original fresh DZM3 matrix",testFinalAuditDzm3);
 run("RE03 paired duration fallback in Pattern and Song",testReauditDurationFallback);
 run("RE04 peer FILL survives late Cancel",testReauditPeerFill);
 run("RE05 component-first recall does not arm next kit",testReauditOriginOrder);
 run("RE06 reentrant setDirty captures consistent streams",testReauditDirtySave);
 run("RE07 host pattern feedback before component recall",testReauditHostMirror);
 run("RE08 independent complete PCM timeline",testReauditPcmTimeline);
 run("RE09 64 voices live swap locate Stop",testReauditLoadedSwap);
 run("RE10 actual Processor deadline matrix",testReauditDeadlineMatrix);
 run("A26 combined 16 active buses, 64 voices, 12 imports, Cycle Stop and PCM",testA26CombinedLiveMultibusCycleImport);
 run("RE11 cold Song overlap cannot divide by zero",[&]{testReauditSongColdChild(argv[1]);});
 run("RM01 zero Stop repair mutation",[]{expectMutationKilled(qa::AuditMutation::suppressZeroStop,"zero Stop",testZeroFrameStopRetiresVoice);});
 run("RM02 paired duration repair mutation",[]{expectMutationKilled(qa::AuditMutation::staleDuration,"paired duration",testReauditPatternLength);});
 run("RM03 peer owner repair mutation",[]{expectMutationKilled(qa::AuditMutation::skipPeerCommit,"peer commit",testPeerVelocityCancelPreservesCommittedStep);});
 run("RM04 asset identity repair mutation",[]{expectMutationKilled(qa::AuditMutation::basenameOnly,"asset identity",testComponentOnlySameBasenameRejectsStaleOrigin);});
 run("RM05 TORTURE missing MIDI mutation",[]{expectMutationKilled(qa::AuditMutation::dropMidi,"TORTURE missing MIDI",testTransportSequenceTorture);});
 run("RM06 TORTURE duplicate MIDI mutation",[]{expectMutationKilled(qa::AuditMutation::doubleMidi,"TORTURE doubled MIDI",testTransportSequenceTorture);});
 run("RM07 TORTURE one-sample delay mutation",[]{expectMutationKilled(qa::AuditMutation::delayMidiOne,"TORTURE one-sample delay",testTransportSequenceTorture);});
 run("RM08 TORTURE missing prefix mutation",[]{expectMutationKilled(qa::AuditMutation::zeroPrefix,"TORTURE zero first32",testTransportSequenceTorture);});
 run("RM09 PACING accepted import no-op mutation",[]{expectMutationKilled(qa::AuditMutation::skipImport,"PACING import no-op",testPacedSampleSwapContinuity);});
 run("RM10 PACING active dropout mutation",[]{expectMutationKilled(qa::AuditMutation::muteImportedProcessor,"PACING active-only dropout",testPacedSampleSwapContinuity);});
 run("RM11 independent PCM detects offset and gaps",testReauditPcmNegatives);
 run("RM12 PACING deadline diagnostic injection",testReauditDeadlineDiagnostic);
 run("RM13 loaded swap detects ignored replacement",[]{expectMutationKilled(qa::AuditMutation::skipImport,"64 voice audible replacement",testReauditLoadedSwap);});
 run("WE01 actual WAV preview and project recall",testWavEditorPreviewAndRecall);
 run("T03 controlled actual bank",testPatternOverlap);
 run("T04 controlled real restore gate",testKitGate);
 for(bool zero:{true,false})for(bool busy:{false,true})for(const auto& points:std::vector<std::vector<unsigned>>{{5},{2},{5,2}}){
   const std::string name="O01 "+std::string(zero?"zero":"audio")+(busy?" busy ":" snapshot ")+
     (points.size()>1?"ABA":points[0]==2?"equal":"different");
   run(name,[&]{testArbitration(zero,busy,points);});
 }
 run("Z01 full Controller allocation injection",testRecallOOM);
 run("Z01 legacy Controller",testLegacyReject);
 run("V18 multi-view native lifecycle",testMultiViewLifecycleNotifications);
 run("Z02 native attached views",testZoom);
 run("VSTGUI precision controls",testPrecisionPanel);
 run("V101 actual Controller to native visual scene",testV101ControllerScene);
 run("V101 native interactive controller, lane and zoom actions",testV101Interaction);
 run("V101 real VST3 editor attach, mouse dispatch, dialogs and zoom",testV101LiveEditor);
 run("V101 total menu matrix and per-key geometry",testV101MenuMatrixAndGeometry);
 run("VSTGUI native pixel render set",testVisualRender);
 run("GUI49 musical four-sample native 130pct capture",testGui49MusicalFourSampleCapture);
 run("GUI 11I centered PATTERN and meter header geometry",test11IHeaderPatternCentered);
 run("M11I actual MAIN and stem meter, GUI capture and clip reset",test11IMeterStereoAndStems);
 run("S13 IPC meter with absent registry and two native editors",test13MeterTransportAndMultiEditor);
 run("M11I normal 16bit mono FLAC import",test11IMonoFlacImport);
 run("M11I six rotary Min Center Max and double-click defaults",test11IRotaryMinCenterMax);
 run("VSTGUI velocity design capture 100/130",testVelocityDesignCaptures);
 run("VSTGUI wave editor design capture 100/130",testWaveEditorDesignCaptures);
 run("W11D native mouse-driven wave editor controls",testWaveEditorRealMouseControls);
 run("W11F real WAV/FLAC imports and status",testWaveEditor11FFlacAndStatus);
 run("W11F independent stereo fade PCM",testWaveEditor11FStereoFadePcm);
 run("W11F mouse-scroll source PCM numeric oracle",testWaveEditor11FScrollNumeric);
 run("VSTGUI long WAV display full/zoom at 100/130",testWaveEditorLongCaptures);
  run("GUI physical step state/raster toggle",testGuiPhysicalStepsRaster);
 run("UX direct pattern length",testPatternLengthControls);
 run("P18 native pattern duplicate menu",testPatternDuplicateMenu);
 run("P19 1T/2T copy-paste-duplicate duration",testPatternDurationClipboard);
 run("T28 rate-switch transport clock reset",testRateChangeTransportReset);
 run("SOR03 host timeline jump voice cleanup",testTransportJumpRetiresOldVoices);
 run("SOR03Z zero-frame stop retires old voices",testZeroFrameStopRetiresVoice);
 run("V1A03 peer edit survives velocity cancel",testPeerVelocityCancelPreservesCommittedStep);
 run("V1A04 component-only same-name origin safety",testComponentOnlySameBasenameRejectsStaleOrigin);
 run("PACING long running sample swap audio continuity",testPacedSampleSwapContinuity);
 run("TORTURE 96 transport stop and locate sequences",testTransportSequenceTorture);
 run("RANDOM one-shot existing notes",testRandomVelocityOneShot);
  run("SOR01 new Step Velocity 100",testNewStepDefault100);
 run("VELO 100 reset existing notes",testVelocityResetPattern);
 run("V16 1T hidden velocity preservation",testVelocityInactiveBarProtection);
 run("V17 1T row FILL/CLEAR hidden-step protection",testFillActivePatternLength);
 run("UX Velocity empty-step editing",testVelocityLaneExistingStepsOnly);
 run("UX paged Song board",testPagedSongEditor);
 run("S09 Song entry signed extreme edit deltas",testSongEntryExtremeDelta);
 run("S10 Song count signed extreme edit deltas",testSongCountExtremeDelta);
 run("D11 control signed extreme deltas",testControlExtremeDeltas);
 run("UX compact Song viewport",testSongCompactViewport);
 run("Step 12A native SONG design review at 100/130",testSongDesignReview12A);
 run("GUIFINAL SONG START/REST layout, 100/130 genuine pixels",testSongFinalGuiAlignment);
 run("A01 independent copied and delayed Processor/Controller IPC",testA01SerializedAsyncIPC);
 run("A03 actual processor 2200-event bounded overflow",testA03ProcessorMidiOverflow);
 run("N10 processor rename OOM",testHostNameAllocationRollback);
 run("N11 controller metadata transactional commit",testControllerMetadataCommit);
 run("N12 real WAV sample folder previous/next",testRealSampleFolderStepping);
 run("Z03 Win32 name editor",testNativeName);
 run("FA02 MIDI/host arbitration during actual kit restore",testAuditMidiRecall);
 run("FA02 zero-frame valid-offset arbitration",testAuditZeroFlushOffsets);
 run("FA03 new-kit Song fallback during publication",testAuditSongGeneration);
 run("FA04 sample path failures and real View drop callbacks",testAuditSamplePathFailures);
 run("FA05 modal menu gutters",testAuditMenuGutters);
 run("FA06 minimum-height host refusal menus",testAuditMinimumHeightMenus);
 run("SA01 two-view native invalidation after row and global edits",testSecondAuditViewInvalidation);
 run("SA02 actual sequencer/MIDI choke partition",testSecondAuditTriggerPartition);
 run("FA23-01 edited Clear remains saveable",testFa23ClearEditedState);
 run("FA23-02 preview restarts without layering",testFa23PreviewRetrigger);
 run("FA23-03 preview publication boundary",testFa23PreviewPublication);
 run("FA23-04 WAV modal blocks drop",testFa23ModalDrop);
 run("FA23-05 stale draft cannot edit new sample",testFa23StaleEditor);
 run("FA23-06 multiple restore provenance cycles",testFa23OriginCycles);
 run("FA23-07 Undo duration during live gesture",testFa23UndoDuringStroke);
 run("FA23-08 interpolation respects trim bounds",testFa23TrimInterpolation);
 run("FA23-09 audition deactivation",testFa23AuditionDeactivate);
 run("FA23-10 waveform query OOM",testFa23QueryOOM);
 run("FR01 stop ordering after consumed start",testFr01StopAfterStartConsumed);
 run("RV01 stop then new preview ordering",testRv01RestartAfterStopDuringAudioStart);
 run("RV01 asset+command publication ordering",testRv01AssetCommandPublication);
 run("RV01 preview retirement across outputless callback",testRv01OutputlessPreviewRetirement);
 run("A04 preview lifecycle across 48k / 44.1k / 48k",testA04PreviewRateLifecycle);
 run("A05 dense Song automation one schedule",testA05DenseSongAutomationDoesNotReschedule);
 run("A06 committed WAV remains accepted after repaint OOM",testA06CommittedImportSurvivesRepaintFailure);
 run("A07 failed peer view refresh preserves accepted edit and dirty request",testA07NotificationFaultIsolation);
 run("A08 native WAV editor revision cache and invalidation",testA08WaveEditorRevisionCache);
 run("RM16 waveform cache disabled is detected",[]{expectMutationKilled(qa::AuditMutation::disableWaveCache,"full WAV on every viewport query",testA08WaveEditorRevisionCache);});
 run("A09 native variable stereo waveform exact reference",testA09WaveformExtremaAgainstNativeReference);
 run("A10 context gap audio and step continuity",testA10MissingContextAudioContinuity);
 run("A11 verified Cycle tail vs Locate",testA11CycleWrapPreservesSampleTails);
 run("A12 nonaligned Cycle wrap and negative controls",testA12CycleWrapMidBlockAndNegativeControls);
 run("A13 internal Step at mid-block Cycle",testA13PureSequencerCyclePcm);
 run("A14 pattern and Song at intra-block Cycle seam",testA14PatternAndSongAcrossCycleSeam);
 run("A15 Cycle random velocity block-invariance",testA15CycleRandomVelocityReproducible);
 run("A16 multiple Cycles in one host callback",testA16MultiCyclePerCallback);
 run("A17 optional Cycle context gap full stereo PCM",testA17MissingCycleClockKeepsExactPcm);
 run("A18 variable sample rate and host frames Cycle PCM",testA18CycleBlockPartitionAcrossSampleRates);
 run("A19 changing tempo within active Cycle PCM",testA19TempoChangeWithinHostCycle);
 run("A20 partial clock explicitly disables Cycle",testA20PartialClockExplicitCycleDisable);
 run("A21 valid tempo with temporarily absent PPQ",testA21PartialTempoWithMissingPpq);
 run("A22 bounded Song snapshot per host callback",testA22SongSnapshotBoundedByHostCallbacks);
 run("A23 valid host PPQ during missing tempo and locate",testA23AuthoritativePpqSurvivesMissingTempo);
 run("A24 component-first DZM3 origin permissions are one-shot",testA24ComponentFirstDzm3OriginConsumed);
 run("A25 old unpaired Component then new DZM3 keeps provenance",testA25UnpairedComponentNewDzm3Origin);
 run("RM28 dropping valid partial PPQ is detected",[]{expectMutationKilled(qa::AuditMutation::ignoreValidPartialPpq,"host PPQ lost with invalid tempo",testA23AuthoritativePpqSurvivesMissingTempo);});
 run("RM29 stale component-first DZM3 permission detected",[]{expectMutationKilled(qa::AuditMutation::keepComponentFirstOriginArmed,"component-first DZM3 origin leak",testA24ComponentFirstDzm3OriginConsumed);});
 run("RM27 repeated per-Cycle Song snapshot detected",[]{expectMutationKilled(qa::AuditMutation::repeatSongSnapshots,"Song re-snapshot on every Cycle",testA22SongSnapshotBoundedByHostCallbacks);});
 run("RM25 stale Cycle after explicit partial disable killed",[]{expectMutationKilled(qa::AuditMutation::keepCycleOnPartialDisable,"partial ProcessContext Cycle-off ignored",testA20PartialClockExplicitCycleDisable);});
 run("RM26 valid partial tempo ignored killed",[]{expectMutationKilled(qa::AuditMutation::ignorePartialTempo,"partially valid host BPM ignored",testA21PartialTempoWithMissingPpq);});
 run("RM23 variable block no Cycle split is detected",[]{expectMutationKilled(qa::AuditMutation::skipCycleSplit,"variable callback Cycle split",testA18CycleBlockPartitionAcrossSampleRates);});
 run("RM24 stale Cycle tempo mutation is detected",[]{expectMutationKilled(qa::AuditMutation::freezeCycleTempo,"wrong tempo at Cycle seam",testA19TempoChangeWithinHostCycle);});
 run("RM22 missing Cycle clock cache is killed",[]{expectMutationKilled(qa::AuditMutation::suppressCycleClockCache,"missing Cycle clock caused skipped sample triggers",testA17MissingCycleClockKeepsExactPcm);});
 run("RM19 old unsplit Cycle scheduler is killed",[]{expectMutationKilled(qa::AuditMutation::skipCycleSplit,"unsplit host Cycle lost Step",testA13PureSequencerCyclePcm);});
 run("RM20 old linear Velocity PPQ is killed",[]{expectMutationKilled(qa::AuditMutation::skipCycleVelocityMap,"randomized loop velocity changed with block partition",testA15CycleRandomVelocityReproducible);});
 run("RM21 old single-wrap voice reset is killed",[]{expectMutationKilled(qa::AuditMutation::skipCycleModulo,"multiple Cycle wraps retire valid tails",testA16MultiCyclePerCallback);});
 run("RM18 missing-context old-cutoff regression is killed",[]{expectMutationKilled(qa::AuditMutation::suppressMissingContextCarry,"missing clock truncates hits",testA10MissingContextAudioContinuity);});
 run("RM17 direct-scan mutation is detected",[]{expectMutationKilled(qa::AuditMutation::bypassWaveRangeIndex,"linear scan instead of indexed bins",testA09WaveformExtremaAgainstNativeReference);});
 run("RV02 allocation-free STOP and rejected-delivery retry",testRv02AllocationFreeStopAndRetry);
 run("RV02 transient STOP rejection during native editor close",testRv02NativeEditorCloseTransientReject);
 run("RV02 rejected STOP leaves Apply uncommitted",testRv02ApplyRejectKeepsEdit);
 run("A03 permanent STOP refusal and recovery",testA03PersistentStopRejectionRecovery);
 run("R05 sample menu allocation failures",testR05SampleMenuAllocationFailures);
 run("FR02 controller query/edit OOM boundaries",testFr02ControllerMessageOOM);
 run("RV03 full and legacy recall query allocation failures",testRv03RecallQueryOOM);
 run("FR03 legacy sample-present synchronization",testFr03LegacyRecallSamplePresence);
 run("FR08 controller path length consistency",testFr08PathSerializationBoundary);
 run("FR04 waveform border real mouse input",testFr04RealWaveformEdgeMouse);
 run("FR05 one-frame Fade Out native pixel contract",testFr05ShortFadePixelsMatchAudioContract);
 run("FR06 modal error message native pixels",testFr06ModalErrorPixels);
 run("FR10 global knob right-edge mouse input",testFr10GlobalKnobEdgeHit);
 run("RV05 all global knob rendered centers",testRv05GlobalKnobPaintCenters);
 run("RM14 historical Fade renderer regression killed",[]{
   expectMutationKilled(qa::AuditMutation::oldFadeRenderer,"RV04 historical Fade/Gain formula",
                        testFr05ShortFadePixelsMatchAudioContract);
 });
 run("RM15 historical knob draw offsets killed",[]{
   expectMutationKilled(qa::AuditMutation::oldKnobDrawCenters,"RV05 original 80/178/1178 draw centers",
                        testRv05GlobalKnobPaintCenters);
 });
 evidence("Integration total failures: "+std::to_string(failures)+"; Studio One NOT RUN");
 qaCaseSummary+="Total failures: "+std::to_string(failures)+"; Studio One NOT RUN";
 drumator::qa::collectedNotices=nullptr;
 drumator::qa::workflowNotice(qaCaseSummary+"\nDetailed audit diagnostics:\n"+auditDiagnostics);
 CloseHandle(entered);CloseHandle(resume);VSTGUI::exit();CoUninitialize();return failures?1:0;
}
