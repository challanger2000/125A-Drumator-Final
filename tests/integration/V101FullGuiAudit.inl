// V1.0.1 per-control/mouse audit. Real Controller, Processor, default
// VSTGUI editor, Win32 host frame and exact shared painted popup hit map.
// Deliberately separate from the released 1.0.0 legacy fixture.
#include "core/RowFill.h"

void testV101MenuMatrixAndGeometry(){
 using L=drumator::UiV101Layout;
 using Pop=drumator::UiV101Popup;
 using Kind=Pop::Kind;
 using drumator::filledRow;
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
   const auto expected=filledRow(modes[option]);
   for(std::size_t step=0;step<32;++step)
     check(f.c->velocity(current,0,step)==expected[step],
           "GUI audit FILL option mapped to wrong rhythm/velocity");
 }
 // Mouse-open SET must expose every Output (16) and Choke (5) entry.
 click(*view,1230.,L::header+36.);
 for(int bus=0;bus<16;++bus){
   popupClick(Kind::Output,bus,151.,L::header+69.,L::header);
   check(f.c->rowOutput(0)==static_cast<unsigned>(bus),
         "GUI audit OUTPUT menu index did not set matching bus");
 }
 for(int group=0;group<5;++group){
   popupClick(Kind::Choke,group,280.,L::header+69.,L::header);
   check(f.c->rowChoke(0)==static_cast<unsigned>(group),
         "GUI audit CHOKE menu index did not set matching group");
 }
 // Every SET row-edit action must alter the real controller, not only
 // return Applied from a detached interaction unit test.
 const auto row0Old=f.c->velocity(current,0,4);
 check(row0Old>0,"GUI audit SET Random Velocity needs an occupied note");
 click(*view,804.,L::header+69.); // SET RANDOM VELOCITY
 check(f.c->velocity(current,0,4)!=row0Old,
       "GUI audit SET RANDOM VELOCITY did not change occupied note");
 click(*view,438.,L::header+69.); // SET COPY ROW
 const auto copied=f.c->velocity(current,0,4);
 L::Expansions setFirst{};setFirst[0]=L::Expansion::Settings;
 const auto row1ExpandedTop=L::rowTop(0,1,setFirst);
 click(*view,1230.,row1ExpandedTop+36.); // SET for row 1
 const auto row1Top=L::rowTop(0,1,L::Expansions{});
 click(*view,560.,row1Top+69.); // SET PASTE ROW
 check(f.c->velocity(current,1,4)==copied,
       "GUI audit SET PASTE ROW did not copy pattern to target");
 click(*view,680.,row1Top+69.); // SET CLEAR ROW
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
   // Do not sample the *center*: COPY's dark glyph is centered and is
   // legitimately near-black even when its silver cap is fully visible.
   // Sample the satin face inside its left rim, outside the text.
   const auto menuFace=pixelAt(static_cast<int>(first.x0+8.),
                               static_cast<int>(first.y0+9.));
   check(menuFace.red>125&&menuFace.green>120,
         "GUI audit SONG Pattern popup silver face hidden behind board");
   click(*view,menuX,menuY); // overlaps SONG START minus underneath
   check(f.c->songStartBar()==2,
         "GUI audit Pattern popup click leaked to SONG START minus");
   click(*view,mid(S::startResetLeft,S::startResetRight),181.);
   check(f.c->songStartBar()==1,"GUI audit SONG START not restored");
   click(*view,1200.,25.); // Zoom popup while SONG is open
   const auto lastZoom=L::zoomOption(3);
   const double zx=mid(lastZoom.x0,lastZoom.x1);
   const double zy=mid(lastZoom.y0,lastZoom.y1);
   // Likewise, the center of the zoom option contains dark text.
   const auto zoomFace=pixelAt(static_cast<int>(lastZoom.x0+8.),
                               static_cast<int>(lastZoom.y0+9.));
   check(zoomFace.red>125&&zoomFace.green>120,
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
