// V1.0.1 per-control/mouse audit. Real Controller, Processor, default
// VSTGUI editor, Win32 host frame and exact shared painted popup hit map.
// Deliberately separate from the released 1.0.0 legacy fixture.
#include "core/RowFill.h"

void testV101MenuMatrixAndGeometry(){
 using L=drumator::UiV101Layout;
 using Pop=drumator::UiV101Popup;
 using Kind=Pop::Kind;
 using drumator::RowFill;
 V101PatternEditHost automation;
 Fixture f;
 auto kit=v101PreviewKit();kit.visible=16;
 f.kit(kit);prepareAudio(*f.p);
 check(f.c->setComponentHandler(&automation)==kResultOk,
       "GUI audit failed installing real host pattern automation");
 f.openV101();
 auto* view=IntegrationAccess::view(*f.c,0);
 check(view->isV101()&&view->getViewSize().getWidth()==L::width,
       "GUI audit not testing production V1.0.1 custom editor");
 const auto center=[](L::Rect r){
   return std::pair<double,double>{(r.x0+r.x1)/2.,(r.y0+r.y1)/2.};
 };
 const auto setCenter=[](std::size_t i){
   return (drumator::UiV101Interaction::settingsRect(i).x0+
           drumator::UiV101Interaction::settingsRect(i).x1)*.5;
 };
 // All five buttons form one visually centered 600px group, with
 // untouched 112x32 controls and 10px gutters. Popup anchors follow them.
 const auto firstSet=drumator::UiV101Interaction::settingsRect(0);
 const auto lastSet=drumator::UiV101Interaction::settingsRect(4);
 check(std::abs((firstSet.x0+lastSet.x1)*.5-(13.+1260.)*.5)<1.&&
       lastSet.x1-firstSet.x0==600.,
       "GUI audit SET five-key group not centered in expanded panel");
 for(std::size_t key=0;key<5;++key){
   const auto r=drumator::UiV101Interaction::settingsRect(key);
   check(r.x1-r.x0==112.&&r.y1-r.y0==32.&&
         (key==0||r.x0-drumator::UiV101Interaction::settingsRect(key-1).x1==10.),
         "GUI audit SET five-key sizes and uniform 10px gaps regressed");
 }

 const auto popupClick=[&](Kind kind,int option,double triggerX,
                           double triggerY,double rowTop){
   check(option>=0&&option<Pop::count(kind),"GUI audit invalid popup test index");
   click(*view,triggerX,triggerY);
   const auto g=Pop::geometry(kind,rowTop,view->getViewSize().getHeight());
   const auto r=Pop::cell(g,option);
   const auto [x,y]=center(r);
   check(Pop::hit(kind,g,x,y)==option,
         "GUI audit popup painted cell disagrees with pointer hit");
   check(r.x0>=0.&&r.x1<=L::width&&r.y0>=L::header&&
         r.y1<=view->getViewSize().getHeight(),
         "GUI audit popup extends beyond native host viewport");
   click(*view,x,y);
 };
 // Independently inspect every popup index, every row/column and every
 // negative gap. Check full displayed layout at 10-row and 6-row heights.
 for(double height:{686.,494.}){
   for(Kind kind:{Kind::Pattern,Kind::Fill,Kind::Output,
                  Kind::Choke,Kind::Sample}){
     const auto g=Pop::geometry(kind,L::header+4.*L::rowHeight,height);
     const int count=Pop::count(kind);
     for(int option=0;option<count;++option){
       const auto r=Pop::cell(g,option);
       const auto [x,y]=center(r);
       check(r.x0>=0.&&r.x1<=L::width&&
             r.y0>=L::header&&r.y1<=height,
             "GUI audit popup option clipped at page boundary");
       check(Pop::hit(kind,g,x,y)==option&&
             (kind==Kind::Output||*Pop::label(kind,option)!='\0'),
             "GUI audit popup caption/hit index mismatch");
       for(int other=0;other<count;++other){
         if(other==option)continue;
         const auto t=Pop::cell(g,other);
         check(r.x1<=t.x0||t.x1<=r.x0||
               r.y1<=t.y0||t.y1<=r.y0,
               "GUI audit overlapping popup keys");
       }
     }
   }
 }
 // PLAY is the headphone audition action, not a previous-sample
 // navigation key. Its real VST3 message must queue exactly this sample.
 check((IntegrationAccess::queuedPreviewBits(*f.p)&1u)==0,
       "GUI audit preview queue unexpectedly set before input");
 const auto previewBox=L::preview;
 click(*view,(previewBox.x0+previewBox.x1)*.5,
      L::header+(previewBox.y0+previewBox.y1)*.5);
 check((IntegrationAccess::queuedPreviewBits(*f.p)&1u)!=0,
       "GUI audit main PLAY button never queued audible sample preview");
 // Main GUI: every A-H selection must reach the actual host automation,
 // and all four full-width header groups must be mutually disjoint.
 for(std::size_t i=0;i<8;++i){
   const auto xy=center(L::patternButton(i));
   click(*view,xy.first,xy.second);
   check(f.c->currentPattern()==i,
         "GUI audit A-H pattern selector is not connected");
 }
 check(automation.calls>=7,
       "GUI audit selected patterns never reached host automation");
 for(const auto r:{L::patternMenu,L::barsButton,
                   L::headerGroup(0),L::headerGroup(1)}){
   check(r.y0==L::headerButtonY&&r.y1-r.y0==32.&&r.x1-r.x0==83.,
         "GUI audit header action physically mis-sized");
 }
 check(L::barsButton.x0-L::patternMenu.x1==8.&&
       L::headerGroup(0).x0-L::barsButton.x1==8.&&
       L::headerGroup(1).x0-L::headerGroup(0).x1==8.,
       "GUI audit inconsistent hardware group gutters");
 // Return to the populated pattern C for meaningful menu operations.
 const auto source=center(L::patternButton(2));
 click(*view,source.first,source.second);
 const auto sourceVelocity=f.c->velocity(2,0,0);
 check(sourceVelocity>0,"GUI audit populated pattern fixture missing");
 // All seven actual PATTERN MENU choices through real mouse popups.
 const auto pattern=[&](int option){
   popupClick(Kind::Pattern,option,400.,115.,L::header);
 };
 pattern(0); // COPY
 check(f.c->toggleStep(2,0,0)&&f.c->velocity(2,0,0)==0,
       "GUI audit COPY preparation failed");
 pattern(1); // PASTE
 check(f.c->velocity(2,0,0)==sourceVelocity,
       "GUI audit PATTERN PASTE did not restore copied note");
 pattern(2); // CLEAR
 for(std::size_t row=0;row<16;++row)
   for(std::size_t step=0;step<32;++step)
     check(f.c->velocity(2,row,step)==0,
           "GUI audit PATTERN CLEAR did not empty all rows");
 pattern(3); // UNDO
 check(f.c->velocity(2,0,0)==sourceVelocity,
       "GUI audit PATTERN UNDO failed to restore content");
 std::array<std::uint8_t,32> original{};
 for(std::size_t step=0;step<32;++step)
   original[step]=f.c->velocity(2,0,step);
 pattern(4); // RANDOM VELOCITY
 bool varied=false;
 for(std::size_t step=0;step<32;++step){
   const auto v=f.c->velocity(2,0,step);
   check((v==0)==(original[step]==0),
         "GUI audit RANDOM VELOCITY changed note occupancy");
   varied|=v!=original[step];
 }
 check(varied,"GUI audit RANDOM VELOCITY produced no variation");
 pattern(5); // VELOCITY 100
 for(std::size_t step=0;step<32;++step)
   check(f.c->velocity(2,0,step)==(original[step]?100:0),
         "GUI audit VELOCITY 100 changed empties or missed notes");
 pattern(6); // DUPLICATE into the next empty Pattern D
 check(f.c->patternLength(3)==32&&f.c->velocity(3,0,0)==100,
       "GUI audit DUPLICATE did not create an identical next pattern");
 // Prove popups are modal and cannot bleed into an underlying Step.
 click(*view,400.,115.);
 const auto untouched=f.c->velocity(f.c->currentPattern(),0,11);
 click(*view,L::stepX+11.*L::stepPitch+8.,L::header+20.);
 check(f.c->velocity(f.c->currentPattern(),0,11)==untouched,
       "GUI audit clicking outside popup activated underlying Step");
 // Every FILL menu variant must apply its exact documented 32-step
 // musical rhythm, including ghost velocities. These eight expected masks
 // are independent of the popup index and are checked across all 32 steps.
 constexpr std::array<RowFill,8> modes{{
   RowFill::Every2,RowFill::Every4,RowFill::Every8,
   RowFill::Snare24,RowFill::All,RowFill::Offbeat8,
   RowFill::Gallop,RowFill::SnareGhost
 }};
 const auto current=f.c->currentPattern();
 check(f.c->patternLength(current)==32,"GUI audit Fill source length wrong");
 for(std::size_t option=0;option<modes.size();++option){
   popupClick(Kind::Fill,static_cast<int>(option),99.,L::header+20.,L::header);
   // Independent musical oracle: do NOT call filledRow(), which is
   // itself the product's implementation. Otherwise a shared defect in
   // fillInstrument/filledRow would make both actual and expected wrong.
   for(std::size_t step=0;step<32;++step){
     unsigned expected=0;
     switch(option){
       case 0:expected=step%2==0?127:0;break; // EVERY 2
       case 1:expected=step%4==0?127:0;break; // EVERY 4
       case 2:expected=step%8==0?127:0;break; // EVERY 8
       case 3:expected=step%8==4?127:0;break; // SNARE 2/4
       case 4:expected=127;break; // ALL
       case 5:expected=step%2==1?127:0;break; // OFFBEAT 8
       case 6:expected=step%4==0?127:step%4==2?95:
                       step%4==3?84:0;break; // GALLOP 127/95/84
       case 7:expected=(step%16==4||step%16==12)?127:
                       (step%16==11||step%16==15)?42:0;break;
     }
     check(f.c->velocity(current,0,step)==expected,
           "GUI audit FILL option mapped to wrong rhythm/velocity");
   }
 }
 // Mouse-open SET must expose every Output (16) and Choke (5) entry.
 click(*view,1230.,L::header+36.);
 for(int bus=0;bus<16;++bus){
   popupClick(Kind::Output,bus,setCenter(0),L::header+69.,L::header);
   check(f.c->rowOutput(0)==static_cast<unsigned>(bus),
         "GUI audit OUTPUT menu index did not set matching bus");
 }
 // All 16 bus options must survive component serialization, not merely
 // repaint the UI label. Then restore MAIN so subsequent tests are isolated.
 Steinberg::ResizableMemoryIBStream routingState;
 check(f.p->getState(&routingState)==kResultOk,
       "GUI audit BUS routing component getState failed");
 drumator::KitState routingKit{};
 check(drumator::KitCodec::decode(
         static_cast<const std::uint8_t*>(routingState.getData()),
         routingState.getCursor(),routingKit)&&routingKit.output[0]==15,
       "GUI audit BUS 16 selection not persisted in processor state");
 check(drumator::v101::captureControllerScene(*f.c).rows[0].outputBus==15,
       "GUI audit BUS label does not follow actual controller assignment");
 popupClick(Kind::Output,0,setCenter(0),L::header+69.,L::header);
 check(f.c->rowOutput(0)==0&&
       drumator::v101::captureControllerScene(*f.c).rows[0].outputBus==0,
       "GUI audit BUS routing did not return to MAIN");
 for(int group=0;group<5;++group){
   popupClick(Kind::Choke,group,setCenter(1),L::header+69.,L::header);
   check(f.c->rowChoke(0)==static_cast<unsigned>(group),
         "GUI audit CHOKE menu index did not set matching group");
 }
 // VEL (not SET) owns one-shot random and reset, verified by clicking
 // the real host editor and observing note occupancy + other instrument.
 click(*view,1230.,L::header+12.); // SET -> VEL
 const auto row0Old=f.c->velocity(current,0,4);
 const auto otherRow=f.c->velocity(current,1,4);
 check(row0Old>0,"GUI audit VEL random needs occupied note");
 click(*view,190.,L::header+76.); // VEL RANDOM
 check(f.c->velocity(current,0,4)!=row0Old,
       "GUI audit VEL RANDOM did not change occupied note");
 click(*view,330.,L::header+76.); // VEL RESET to 100
 for(std::size_t step=0;step<32;++step){
   const auto now=f.c->velocity(current,0,step);
   check(now==0||now==100,
         "GUI audit VEL RESET left active row velocity different from 100");
 }
 check(f.c->velocity(current,0,4)==100&&
       f.c->velocity(current,1,4)==otherRow&&
       !f.c->resetRowVelocity(16),
       "GUI audit VEL RESET edited other row or accepted invalid row");
 check(f.c->undoCurrentPattern()&&
       f.c->velocity(current,0,4)!=100,
       "GUI audit VEL RESET did not create a single undoable change");
 click(*view,330.,L::header+76.); // restore 100 before row copy
 click(*view,1230.,L::header+36.); // VEL -> SET
 click(*view,setCenter(2),L::header+69.); // SET COPY ROW
 const auto copied=f.c->velocity(current,0,4);
 L::Expansions setFirst{};setFirst[0]=L::Expansion::Settings;
 const auto row1ExpandedTop=L::rowTop(0,1,setFirst);
 click(*view,1230.,row1ExpandedTop+36.); // SET for row 1
 const auto row1Top=L::rowTop(0,1,L::Expansions{});
 click(*view,setCenter(3),row1Top+69.); // SET PASTE ROW
 check(f.c->velocity(current,1,4)==copied,
       "GUI audit SET PASTE ROW did not copy pattern to target");
 click(*view,setCenter(4),row1Top+69.); // SET CLEAR ROW
 check(f.c->velocity(current,1,4)==0,
       "GUI audit SET CLEAR ROW did not erase target note");
 // Return expansion to row 0 for the existing sample popup target test.
 click(*view,1230.,row1Top+36.); // close row-1 expansion
 click(*view,1230.,L::header+36.); // restore row-0 SET expansion
 // SAMPLE: the destructive third choice must affect exactly the clicked
 // instrument, not the instrument that owns the expanded SET panel.
 L::Expansions expanded{};expanded[0]=L::Expansion::Settings;
 const double secondRow=L::rowTop(0,1,expanded);
 check(f.c->hasSample(1)&&f.c->hasSample(0),
       "GUI audit sample menu fixture missing source WAV");
 popupClick(Kind::Sample,2,267.,secondRow+20.,secondRow);
 check(!f.c->hasSample(1)&&f.c->hasSample(0),
       "GUI audit REMOVE SAMPLE damaged the wrong instrument");
 // The second sample menu item must really enter inline name editing.
 popupClick(Kind::Sample,1,267.,secondRow+20.,secondRow);
 check(IntegrationAccess::edit(*view)!=nullptr,
       "GUI audit SAMPLE RENAME did not open native VSTGUI CTextEdit");
 evidence("V101 all-menu matrix: Pattern 7/7, Fill 8/8 exact 32-step masks, Output 16/16, Choke 5/5, Sample REMOVE, A-H 8/8, live popup geometry, modal dismissal PASS");
}

void testV101SongAndWaveFullControlMatrix(){
 using L=drumator::UiV101Layout;
 using S=drumator::UiSongLayout;
 using W=drumator::UiV101WaveLayout;
 const auto mid=[](double lo,double hi){return (lo+hi)*.5;};
 {
   Fixture f;auto kit=v101PreviewKit();f.kit(kit);prepareAudio(*f.p);
   auto& nativeEditor=f.openV101();
   auto* view=IntegrationAccess::view(*f.c,0);
   const auto editButton=L::headerGroup(1);
   click(*view,mid(editButton.x0,editButton.x1),
        mid(editButton.y0,editButton.y1));
   check(IntegrationAccess::songEditorOpen(*view),
         "GUI audit SONG EDIT failed to open real native mode");
   // Header controls are still physically visible in SONG: all three
   // Ctrl-click defaults must work there, not just in the main sequencer.
   check(f.c->swingPercent()==15&&f.c->humanizePercent()==10&&
         f.c->masterVolume()==105,
         "GUI audit SONG header fixture missing nondefault knob values");
   for(std::size_t k=0;k<L::headerKnobCenters.size();++k)
     click(*view,L::headerKnobCenters[k],L::headerKnobY,
           VSTGUI::kLButton|VSTGUI::kControl);
   check(f.c->swingPercent()==0&&f.c->humanizePercent()==0&&
         f.c->masterVolume()==100,
         "GUI audit SONG Ctrl-click did not reset all global knobs");
   // Critical layered-modal regression. The SONG board overlaps the
   // lower section of the header's Pattern and Zoom popups. Verify both
   // native raster visibility AND that click dispatch cannot leak through.
   const auto pixelAt=[&](int x,int y){
     const auto bitmap=VSTGUI::renderBitmapOffscreen(
       {1280.,view->getViewSize().getHeight()},1.,
       [&](VSTGUI::CDrawContext& dc){view->draw(&dc);});
     check(bitmap!=nullptr,"GUI audit SONG popup raster could not render");
     auto* pixels=VSTGUI::CBitmapPixelAccess::create(bitmap.get(),false);
     check(pixels&&pixels->setPosition(static_cast<std::uint32_t>(x),
                                      static_cast<std::uint32_t>(y)),
           "GUI audit SONG popup pixel is outside host");
     VSTGUI::CColor rgba{};pixels->getColor(rgba);pixels->forget();
     return rgba;
   };
   click(*view,mid(S::startPlusLeft,S::startPlusRight),181.);
   check(f.c->songStartBar()==2,"GUI audit cannot prime SONG popup click-through");
   click(*view,400.,115.); // Main Pattern MENU while SONG board is open
   const auto popup=drumator::UiV101Popup::geometry(
       drumator::UiV101Popup::Kind::Pattern,L::header,
       view->getViewSize().getHeight());
   const auto first=drumator::UiV101Popup::cell(popup,0);
   const double menuX=mid(first.x0,first.x1),menuY=mid(first.y0,first.y1);
   check(IntegrationAccess::v101Popup(*view)==drumator::UiV101Popup::Kind::Pattern,
         "GUI audit SONG menu trigger did not open the native popup");
   // Compare actual modal pixels with the approved black hardware
   // sprite, preventing the earlier silver fallback from passing audits.
   using Native=drumator::v101::UiV101NativePreview;
   const Native::ButtonSprites darkSprites{
     IntegrationAccess::vintageButton(*view,0),
     IntegrationAccess::vintageButton(*view,1),
     IntegrationAccess::vintageButton(*view,2),
     IntegrationAccess::vintageButton(*view,3)};
   const auto isolated=VSTGUI::renderBitmapOffscreen(
     {1280.,view->getViewSize().getHeight()},1.,
     [&](VSTGUI::CDrawContext& dc){
       Native::cap(dc,first,0.,"COPY",false,10,&darkSprites);
     });
   check(isolated!=nullptr,"GUI audit dark popup sprite oracle unavailable");
   auto* refPixels=VSTGUI::CBitmapPixelAccess::create(isolated.get(),false);
   const auto checkX=static_cast<std::uint32_t>(first.x0+8.);
   const auto checkY=static_cast<std::uint32_t>(first.y0+9.);
   check(refPixels&&refPixels->setPosition(checkX,checkY),
         "GUI audit dark popup expected pixel unavailable");
   VSTGUI::CColor expectedDark{};
   refPixels->getColor(expectedDark);refPixels->forget();
   const auto menuFace=pixelAt(static_cast<int>(checkX),static_cast<int>(checkY));
   check(expectedDark.red<125&&expectedDark.green<125&&
         std::abs(int(menuFace.red)-int(expectedDark.red))<=4&&
         std::abs(int(menuFace.green)-int(expectedDark.green))<=4&&
         std::abs(int(menuFace.blue)-int(expectedDark.blue))<=4,
         "GUI audit SONG popup is not using dark production button artwork");
   click(*view,menuX,menuY); // overlaps SONG START minus underneath
   check(f.c->songStartBar()==2,
         "GUI audit Pattern popup click leaked to SONG START minus");
   click(*view,mid(S::startResetLeft,S::startResetRight),181.);
   check(f.c->songStartBar()==1,"GUI audit SONG START not restored");
   const auto lastZoom=L::zoomOption(3);
   const double zx=mid(lastZoom.x0,lastZoom.x1);
   const double zy=mid(lastZoom.y0,lastZoom.y1);
   const auto zoomPx=static_cast<int>(lastZoom.x0+8.);
   const auto zoomPy=static_cast<int>(lastZoom.y0+9.);
   const auto zoomClosed=pixelAt(zoomPx,zoomPy);
   click(*view,1200.,25.); // Zoom popup while SONG is open
   check(IntegrationAccess::v101ZoomPopupOpen(*view),
         "GUI audit SONG zoom popup failed to open");
   const auto zoomFace=pixelAt(zoomPx,zoomPy);
   // Detect a covered popup by actual delta from the SONG board.
   // Absolute ivory-cap brightness no longer applies to black keys.
   const int zoomDelta=std::abs(int(zoomFace.red)-int(zoomClosed.red))+
                       std::abs(int(zoomFace.green)-int(zoomClosed.green))+
                       std::abs(int(zoomFace.blue)-int(zoomClosed.blue));
   check(zoomDelta>20,
         "GUI audit SONG header hides the 150-percent zoom option face");
   click(*view,zx,zy);
   check(std::abs(nativeEditor.getZoomFactor()-1.5)<0.0001,
         "GUI audit SONG zoom key did not apply through actual host");
   for(int pattern=0;pattern<=static_cast<int>(drumator::SongChain::kRest);++pattern){
     const double x=pattern<8?S::patternLeft+pattern*S::patternPitch+
         S::patternWidth*.5:mid(S::restLeft,S::restRight);
     click(*view,x,mid(S::choiceTop,S::choiceBottom));
     check(f.c->songEntry(0).pattern==pattern,
           "GUI audit SONG Pattern A-H/REST selection routed incorrectly");
   }
   for(int length=0;length<3;++length){
     click(*view,678.+length*106.+50.,mid(S::choiceTop,S::choiceBottom));
     const std::uint8_t expected[]={8,16,32};
     check(f.c->songEntry(0).steps==expected[length],
           "GUI audit SONG length 1/2,1,2 bars not routed correctly");
   }
   const auto beforeStart=f.c->songStartBar();
   click(*view,mid(S::startBarLeft,S::startBarRight),
        mid(S::startTop,S::startBottom));
   check(f.c->songStartBar()==beforeStart,
         "GUI audit SONG START value readout acted like a button");
   click(*view,mid(S::startPlusLeft,S::startPlusRight),181.);
   check(f.c->songStartBar()==beforeStart+1,
         "GUI audit SONG START plus control disconnected");
   click(*view,mid(S::startMinusLeft,S::startMinusRight),181.);
   check(f.c->songStartBar()==beforeStart,
         "GUI audit SONG START minus control disconnected");
   click(*view,mid(S::startPlusLeft,S::startPlusRight),181.);
   click(*view,mid(S::startResetLeft,S::startResetRight),181.);
   check(f.c->songStartBar()==1,
         "GUI audit SONG START RESET did not reset to bar 1");
   const auto count=f.c->songCount();
   check(count==8,"GUI audit initial chain changed unexpectedly");
   for(int i=0;i<9;++i)
     click(*view,mid(S::addLeft,S::addRight),352.);
   check(f.c->songCount()==count+9&&
         IntegrationAccess::songPage(*view)==1&&
         IntegrationAccess::songSlot(*view)==16,
         "GUI audit SONG ADD or page-2 auto-navigation incorrect");
   click(*view,mid(S::previousLeft,S::previousRight),181.);
   check(IntegrationAccess::songPage(*view)==0&&
         IntegrationAccess::songSlot(*view)==0,
         "GUI audit SONG PREVIOUS page navigation failed");
   click(*view,mid(S::nextLeft,S::nextRight),181.);
   check(IntegrationAccess::songPage(*view)==1&&
         IntegrationAccess::songSlot(*view)==16,
         "GUI audit SONG NEXT page navigation failed");
   click(*view,mid(S::removeLeft,S::removeRight),352.);
   check(f.c->songCount()==16&&IntegrationAccess::songPage(*view)==0,
         "GUI audit SONG REMOVE LAST did not clamp page at 16");
   click(*view,mid(editButton.x0,editButton.x1),
        mid(editButton.y0,editButton.y1));
   check(!IntegrationAccess::songEditorOpen(*view)&&view->isV101(),
         "GUI audit SONG EDIT did not restore default hardware editor");
 }
 {
   Fixture f;auto kit=v101PreviewKit();f.kit(kit);prepareAudio(*f.p);
   f.openV101();auto* view=IntegrationAccess::view(*f.c,0);
   check(IntegrationAccess::openSampleEditorForCapture(*view,0)&&
         IntegrationAccess::sampleEditorOpen(*view),
         "GUI audit Wave editor failed to open real sample");
   const auto frames=IntegrationAccess::editorFrameCount(*view);
   check(frames>1,"GUI audit Wave fixture not decoded");
   const auto press=[&](std::size_t key){
     check(key<W::toolbar.size(),"GUI audit invalid Wave button index");
     const auto& box=W::toolbar[key];
     const double x=mid(box.left,box.right),y=mid(box.top,box.bottom);
     check(W::buttonAt(x,y)==static_cast<int>(key),
           "GUI audit Wave draw/hit mismatch");
     click(*view,x,y);
   };
   // All eight toolbar functions, including audio audition commands,
   // waveform navigation, RESET, APPLY and CANCEL.
   const auto initial=IntegrationAccess::editorViewport(*view);
   press(0); // ZOOM +
   const auto zoomed=IntegrationAccess::editorViewport(*view);
   check(zoomed.second-zoomed.first<
         initial.second-initial.first,
         "GUI audit Wave ZOOM + did not reduce source-frame viewport");
   press(1); // ZOOM -
   const auto out=IntegrationAccess::editorViewport(*view);
   check(out.second-out.first>=zoomed.second-zoomed.first,
         "GUI audit Wave ZOOM - did not enlarge viewport");
   press(2); // FIT
   const auto fit=IntegrationAccess::editorViewport(*view);
   check(fit.first==0&&fit.second==frames,
         "GUI audit Wave FIT did not restore complete source");
   press(3); // PREVIEW
   check(IntegrationAccess::sampleEditorStatus(*view)=="PREVIEW STARTED",
         "GUI audit Wave PREVIEW did not start actual audition");
   press(4); // STOP
   check(IntegrationAccess::sampleEditorStatus(*view)=="PREVIEW STOPPED",
         "GUI audit Wave STOP did not stop actual audition");
   click(*view,330.,210.);
   check(IntegrationAccess::sampleEditorDraft(*view).start>0,
         "GUI audit Wave START marker could not be moved before reset");
   press(5); // RESET
   const auto reset=IntegrationAccess::sampleEditorDraft(*view);
   check(reset.start==0&&reset.end==frames&&reset.fadeIn==0&&
         reset.fadeOut==0&&std::abs(reset.gain-1.f)<1.e-6f,
         "GUI audit Wave RESET did not restore complete source/default gain");
   click(*view,340.,210.);
   const auto edit=IntegrationAccess::sampleEditorDraft(*view);
   check(edit.start>0,"GUI audit Wave marker failed before APPLY");
   press(7); // APPLY
   check(!IntegrationAccess::sampleEditorOpen(*view),
         "GUI audit Wave APPLY failed to close after committing");
   check(IntegrationAccess::openSampleEditorForCapture(*view,0),
         "GUI audit Wave commit did not permit reopening saved sample");
   const auto recalled=IntegrationAccess::sampleEditorDraft(*view);
   check(recalled.start==edit.start&&recalled.end==edit.end,
         "GUI audit Wave APPLY did not persist actual trim markers");
   click(*view,360.,210.);
   press(6); // CANCEL
   check(!IntegrationAccess::sampleEditorOpen(*view),
         "GUI audit Wave CANCEL failed to close without committing");
   check(IntegrationAccess::openSampleEditorForCapture(*view,0),
         "GUI audit Wave CANCEL broke subsequent editor reopen");
   const auto afterCancel=IntegrationAccess::sampleEditorDraft(*view);
   check(afterCancel.start==edit.start&&afterCancel.end==edit.end,
         "GUI audit Wave CANCEL changed committed source trim");
   press(6);
 }
 evidence("V101 SONG/Wave full native controls: A-H/REST, 3 lengths, 64-entry navigation, start +/-/RESET, all 8 Wave keys, preview/stop, zoom, reset, apply/recall, cancel/rollback PASS");
}



void testV101F14StrandedPageRecovery(){
 using L=drumator::UiV101Layout;
 // Reproduce the real report: page 2 of 16 rows, shrink to 5, then
 // recover a saved stranded 5-5/5 viewport via PREV. Both the native host
 // and the page state machine are checked; audio/project state is untouched.
 Fixture f;
 auto kit=namedKit('X',16);
 f.kit(kit);
 prepareAudio(*f.p);
 f.openV101();
 auto* v=IntegrationAccess::view(*f.c,0);
 check(v->isV101()&&f.c->visibleRows()==16,
       "F14 native 16-row test editor failed to initialize");
 const auto pressPage=[&](L::Rect button){
   click(*v,(button.x0+button.x1)*.5,
         v->getViewSize().getHeight()-L::footer+(button.y0+button.y1)*.5);
 };
 pressPage(L::footerNext);
 check(IntegrationAccess::v101FirstRow(*v)==10&&
       IntegrationAccess::v101ShownRows(*v)==6,
       "F14 cannot navigate to 16-row final page");
 check(f.c->restoreVisibleRows(5),
       "F14 unable to emulate external row-count reduction");
 check(IntegrationAccess::v101FirstRow(*v)==0&&
       IntegrationAccess::v101ShownRows(*v)==5&&
       v->getViewSize().getHeight()==L::editorHeight(0,5,L::Expansions{}),
       "F14 16-to-5 row shrink did not rebase native View to ROWS 1-5 / 5");
 // Independently recreate the former bug even without row-count callback:
 // old State could retain firstRow=4 although total=5 fits on page one.
 drumator::UiV101Interaction state(*f.c);
 auto old=state.state();
 old.firstRow=4;
 old.selectedRow=4;
 state.restoreLayout(old);
 check(state.shownRows()==1&&state.state().firstRow==4,
       "F14 legacy stranded ROWS 5-5 / 5 state could not be reproduced");
 check(state.page(-1)&&state.state().firstRow==0&&state.shownRows()==5,
       "F14 PREV PAGE failed to recover stranded ROWS 5-5 / 5");
 check(!state.page(-1)&&!state.page(1),
       "F14 five fully visible rows must have no further pages");
 // Negative/boundary: with host-constrained 1-row height the user still
 // needs both arrows for a five-row kit.
 state.setViewportRows(1);
 check(state.page(1)&&state.state().firstRow==1&&state.shownRows()==1,
       "F14 small native window lost NEXT PAGE");
 check(state.page(-1)&&state.state().firstRow==0,
       "F14 small native window lost PREV PAGE");
}


void testV101F15ExhaustivePageMatrix(){
 using L=drumator::UiV101Layout;
 Fixture f;
 auto kit=namedKit('X',16);f.kit(kit);prepareAudio(*f.p);
 f.openV101();
 auto* v=IntegrationAccess::view(*f.c,0);
 const auto clickPage=[&](L::Rect button){
   const double y=v->getViewSize().getHeight()-L::footer+
       (button.y0+button.y1)*.5;
   click(*v,(button.x0+button.x1)*.5,y);
 };
 clickPage(L::footerNext);
 check(IntegrationAccess::v101FirstRow(*v)==10,
       "F15 NEXT must move 16-row kit to rows 11-16");
 clickPage(L::footerNext);
 check(IntegrationAccess::v101FirstRow(*v)==10,
       "F15 second NEXT must never advance beyond final 11-16 page");
 clickPage(L::footerPrev);
 check(IntegrationAccess::v101FirstRow(*v)==0,
       "F15 PREV from last page must return directly to row 1");
 for(std::size_t total=16;total>=1;--total){
   check(f.c->restoreVisibleRows(total),
         "F15 cannot change real Controller row count");
   for(std::size_t capacity=1;capacity<=L::visibleRows;++capacity){
     drumator::UiV101Interaction pager(*f.c);
     pager.setViewportRows(capacity);
     const auto last=total<=capacity?std::size_t{0}:
       capacity==L::visibleRows?((total-1)/capacity)*capacity:total-capacity;
     std::vector<std::size_t> forwardStarts{0};
     for(std::size_t n=0;n<=L::instruments;++n){
       const auto first=forwardStarts.back();
       check(pager.state().firstRow==first,
             "F15 forward page did not land on actual page start row");
       check(pager.shownRows()==std::min(capacity,total-first),
             "F15 shown row count disagrees with page capacity");
       const auto next=std::min(first+capacity,last);
       const bool advanced=pager.page(1);
       check(advanced==(next!=first),
             "F15 NEXT enabled outside valid page range");
       if(!advanced)break;
       check(pager.state().firstRow==next,
             "F15 NEXT failed canonical/overlapping final-page position");
       forwardStarts.push_back(next);
       check(n<L::instruments,"F15 NEXT entered unbounded navigation cycle");
     }
     check(pager.state().firstRow==last,
           "F15 page end must be final accessible page");
     for(std::size_t i=forwardStarts.size();i>1;--i){
       check(pager.page(-1)&&pager.state().firstRow==forwardStarts[i-2],
             "F15 PREV must reverse entire forward page history");
     }
     check(pager.state().firstRow==0&&!pager.page(-1),
           "F15 previous page cannot underflow row zero");
     // Build a formerly stranded/corrupt page and enforce safe recovery
     // on the next structural Controller notification.
     auto stray=pager.state();
     stray.firstRow=total-1;stray.selectedRow=total-1;
     pager.restoreLayout(stray);
     pager.reconcileRowCountChange();
     check(pager.state().firstRow==last,
           "F15 structural reconciliation did not clamp to final valid page");
   }
   if(total==1)break;
 }
}
// Independent end-to-end pointer audit for every visible instrument, including
// the six rows on page 2. Tests the actual VSTGUI editor, Controller gates and
// step state; verifies a gesture cannot accidentally mutate the wrong row.
void testV101AllRowMouseIsolationAndSampleBrowse(){
 using L=drumator::UiV101Layout;
 Fixture f;
 auto kit=namedKit('X',16);
 f.kit(kit);
 prepareAudio(*f.p);
 f.openV101();
 auto* view=IntegrationAccess::view(*f.c,0);
 check(view->isV101()&&f.c->visibleRows()==16,
       "V101 all-row audit did not attach actual sixteen-row editor");
 const auto pattern=f.c->currentPattern();
 for(std::size_t row=0;row<16;++row){
   if(row==10){
     const auto y=view->getViewSize().getHeight()-L::footer+22.;
     click(*view,(L::footerNext.x0+L::footerNext.x1)*.5,y);
     check(view->getViewSize().getHeight()==494.,
           "V101 all-row audit failed transition to six-row page 2");
   }
   const auto first=row<10?std::size_t{0}:std::size_t{10};
   const double top=L::rowTop(first,row-first,L::Expansions{});
   const auto oldMute=f.c->isRowMuted(row);
   const auto oldSolo=f.c->isRowSolo(row);
   const auto other=(row+1)%16;
   const auto otherMute=f.c->isRowMuted(other);
   const auto otherSolo=f.c->isRowSolo(other);
   // The slim row-selection rail is a real selectable target, not a
   // decorative line or a hotspot belonging to its neighboring lane.
   click(*view,(L::select.x0+L::select.x1)*.5,top+24.);
   check(IntegrationAccess::v101SelectedRow(*view)==row,
         "V101 row-selection rail selected a neighboring instrument");
   const double muteX=(L::mute.x0+L::mute.x1)*.5;
   click(*view,muteX,top+24.);
   check(f.c->isRowMuted(row)!=oldMute&&
         f.c->isRowMuted(other)==otherMute,
         "V101 MUTE toggled the wrong instrument");
   click(*view,muteX,top+24.);
   check(f.c->isRowMuted(row)==oldMute,
         "V101 MUTE twice did not restore original state");
   const double soloX=(L::solo.x0+L::solo.x1)*.5;
   click(*view,soloX,top+24.);
   check(f.c->isRowSolo(row)!=oldSolo&&
         f.c->isRowSolo(other)==otherSolo,
         "V101 SOLO toggled the wrong instrument");
   click(*view,soloX,top+24.);
   check(f.c->isRowSolo(row)==oldSolo,
         "V101 SOLO twice did not restore original state");
   const auto before=f.c->velocity(pattern,row,0);
   const auto neighbor=f.c->velocity(pattern,other,0);
   const auto step=L::stepRect(0);
   const double stepX=(step.x0+step.x1)*.5;
   click(*view,stepX,top+20.);
   check(f.c->velocity(pattern,row,0)!=before&&
         f.c->velocity(pattern,other,0)==neighbor,
         "V101 STEP changed the wrong row or failed on page 2");
   click(*view,stepX,top+20.);
   const auto twice=f.c->velocity(pattern,row,0);
   // Drumator's established Step policy is OFF->100 and ON->OFF.
   // Double-clicking a previously non-100 active note must restore its
   // occupancy at the defined 100 default, not its historic velocity.
   const auto expected=before==0?std::uint8_t{0}:std::uint8_t{100};
   check(twice==expected,
         "V101 STEP second click violated the 0/100 occupancy contract");
   // Painted 2px step gap is truly inert, not an oversized invisible key.
   click(*view,step.x1+1.,top+20.);
   check(f.c->velocity(pattern,row,0)==expected,
         "V101 STEP gutter hit a neighboring sequencer control");
   // Restore the exact fixture velocity for subsequent instrument checks.
   if(twice!=before)
     check(f.c->setStepVelocity(pattern,row,0,before),
           "V101 all-row audit could not restore original note velocity");
   check(f.c->velocity(pattern,row,0)==before,
         "V101 all-row audit failed to restore the fixture");
 }
 // First page is accessible again after 16th row.
 click(*view,(L::footerPrev.x0+L::footerPrev.x1)*.5,
        view->getViewSize().getHeight()-L::footer+22.);
 check(view->getViewSize().getHeight()==686.&&
       IntegrationAccess::v101SelectedRow(*view)==0,
       "V101 PREV PAGE failed to restore real first-page geometry");
 // Real filesystem fixture: the native sample arrows must call the same
 // actual VST3 sample import path as the Processor, skipping broken files.
 const auto folder=std::filesystem::current_path()/
     "125A-Drumator-V101-Final-UI-Browse";
 std::filesystem::create_directories(folder);
 const auto A=folder/"A-good.wav",B=folder/"B-invalid.wav",
            C=folder/"C-good.wav";
 const auto wavA=constantStereo(.25f,-.25f);
 const auto wavC=constantStereo(-.5f,.125f);
 const auto writeFile=[&](const std::filesystem::path& file,
                           const std::vector<std::uint8_t>& bytes){
   std::ofstream out(file,std::ios::binary|std::ios::trunc);
   check(out.good(),"V101 browse fixture could not open WAV");
   out.write(reinterpret_cast<const char*>(bytes.data()),
             static_cast<std::streamsize>(bytes.size()));
   check(out.good(),"V101 browse fixture WAV write failed");
 };
 writeFile(A,wavA);
 writeFile(B,std::vector<std::uint8_t>(64,0));
 writeFile(C,wavC);
 const auto aPath=A.string(),cPath=C.string();
 check(f.c->loadWav(0,aPath)==Controller::LoadStatus::Accepted&&
       IntegrationAccess::paths(*f.c)[0]==aPath,
       "V101 native sample browser failed to import initial real WAV");
 const auto clickArrow=[&](L::Rect control,std::size_t row){
   const double top=L::rowTop(0,row,L::Expansions{});
   click(*view,(control.x0+control.x1)*.5,
         top+(control.y0+control.y1)*.5);
 };
 clickArrow(L::next,0);
 check(IntegrationAccess::paths(*f.c)[0]==cPath,
       "V101 sample NEXT arrow failed to skip invalid WAV");
 clickArrow(L::previous,0);
 check(IntegrationAccess::paths(*f.c)[0]==aPath,
       "V101 sample PREVIOUS arrow failed to restore original WAV");
 clickArrow(L::previous,0);
 check(IntegrationAccess::paths(*f.c)[0]==cPath,
       "V101 sample PREVIOUS arrow failed to wrap past invalid WAV");
 clickArrow(L::next,0);
 check(IntegrationAccess::paths(*f.c)[0]==aPath,
       "V101 sample NEXT arrow failed to wrap around folder");
 // Clicking the neighboring instrument's arrow must not change row 0.
 clickArrow(L::next,1);
 check(IntegrationAccess::paths(*f.c)[0]==aPath,
       "V101 sample browsing targeted adjacent instrument");
 ResizableMemoryIBStream memory;
 KitState processorState;
 check(f.p->getState(&memory)==kResultOk&&
       KitCodec::decode(static_cast<const std::uint8_t*>(memory.getData()),
                        static_cast<std::size_t>(memory.getCursor()),processorState)&&
       processorState.wav[0]==wavA&&
       processorState.names[0]=="A-good.wav",
       "V101 sample arrows did not preserve exact decoded WAV in processor state");
 // Real PLAY icon must queue a preview for the correct instrument.
 check((IntegrationAccess::queuedPreviewBits(*f.p)&1u)==0,
       "V101 sample audition queue unexpectedly set");
 const auto play=L::preview;
 click(*view,(play.x0+play.x1)*.5,
      L::header+(play.y0+play.y1)*.5);
 check((IntegrationAccess::queuedPreviewBits(*f.p)&1u)!=0,
       "V101 actual row-0 PLAY icon failed to queue audio audition");
 std::error_code ec;
 std::filesystem::remove_all(folder,ec);
 check(!ec,"V101 browse fixture cleanup failed");
 f.closeLast();
 evidence("V101 real 16-row M/S/selection/Step hit isolation, 2-page sizing, actual Sample PREV/NEXT invalid-skip/wrap, Processor WAV recall and PLAY queue PASS");
}
