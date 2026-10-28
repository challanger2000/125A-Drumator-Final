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
 // SAMPLE: the destructive third choice must affect exactly the clicked
 // instrument, not the instrument that owns the expanded SET panel.
 L::Expansions expanded{};expanded[0]=L::Expansion::Settings;
 const double secondRow=L::rowTop(0,1,expanded);
 check(f.c->hasSample(1)&&f.c->hasSample(0),
       "GUI audit sample menu fixture missing source WAV");
 popupClick(Kind::Sample,2,267.,secondRow+20.,secondRow);
 check(!f.c->hasSample(1)&&f.c->hasSample(0),
       "GUI audit REMOVE SAMPLE damaged the wrong instrument");
 evidence("V101 all-menu matrix: Pattern 7/7, Fill 8/8 exact 32-step masks, Output 16/16, Choke 5/5, Sample REMOVE, A-H 8/8, live popup geometry, modal dismissal PASS");
}
