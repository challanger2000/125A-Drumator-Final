#include "core/UiV101Layout.h"
#include "vst3/UiV101Popup.h"
#include "vst3/UiV101WaveLayout.h"
#include <iostream>
using L=drumator::UiV101Layout;
#define CHECK(x) do{if(!(x)){std::cerr<<__LINE__<<" " #x "\n";return 1;}}while(false)
int main(){
  // Existing host zoom choices, identical cap geometry, and modal hit map.
  for(std::size_t step=0;step<L::steps;step+=4){
    const auto number=L::stepNumberRect(step);
    const auto painted=L::stepRect(step);
    CHECK(number.x1-number.x0==17.&&number.y0==140.&&number.y1==157.);
    CHECK((number.x0+number.x1)*.5==(painted.x0+painted.x1)*.5+1.);
    if(step)CHECK(number.x0-L::stepNumberRect(step-4).x0==4.*L::stepPitch);
  }
  CHECK(L::footerPanelLeft==22.&&L::footerPanelRight==1258.);
  CHECK(L::footerPanelLeft>L::footerScrewLeftX+L::footerScrewRadius&&
        L::footerPanelRight<L::footerScrewRightX-L::footerScrewRadius);
  CHECK(L::zoomPresets.size()==4);
  CHECK(L::zoomPresets[0]==1.&&L::zoomPresets[1]==1.2);
  CHECK(L::zoomPresets[2]==1.3&&L::zoomPresets[3]==1.5);
  // SONG and SONG EDIT are the only buttons in this group.
  static_assert(L::headerGroupCount==2);
  CHECK(L::patternMenu.x1-L::patternMenu.x0==83.);
  CHECK(L::barsButton.x1-L::barsButton.x0==83.);
  CHECK(L::headerGroup(0).x1-L::headerGroup(0).x0==83.);
  CHECK(L::headerGroup(1).x1-L::headerGroup(1).x0==83.);
  CHECK(L::barsButton.x0-L::patternMenu.x1==8.);
  CHECK(L::headerGroup(0).x0-L::barsButton.x1==8.);
  CHECK(L::headerGroup(1).x0-L::headerGroup(0).x1==8.);
  CHECK(!L::patternMenu.hit(451.,115.));
  CHECK(!L::barsButton.hit(542.,115.));
  CHECK(!L::headerGroup(0).hit(634.,115.));
  CHECK(L::patternMenu.hit(440.,115.));
  static_assert(L::headerGroup(0).y1-L::headerGroup(0).y0==L::headerButtonHeight);
  static_assert(L::headerGroup(1).y1-L::headerGroup(1).y0==L::headerButtonHeight);
  static_assert(L::headerButtonY==99.&&L::headerButtonHeight==32.);
  static_assert(L::barsButton.y0==L::headerGroup(0).y0);
  static_assert(L::barsButton.y1==L::headerGroup(1).y1);
  static_assert(L::patternMenu.y0==L::headerButtonY);
  static_assert(L::patternMenu.y1==L::barsButton.y1);
  for(std::size_t key=0;key<8;++key){
    const auto r=L::patternButton(key);
    CHECK(r.y0==L::headerButtonY&&r.y1==L::barsButton.y1);
    CHECK(r.y0+8.<r.y1-8.);
  }
  CHECK(L::headerGroup(0).x1+8.==L::headerGroup(1).x0);
  CHECK(L::headerKnobCenters.size()==3);
  CHECK(L::headerKnob(0).x0>L::headerGroup(1).x1+20.);
  for(std::size_t i=0;i<L::headerKnobCenters.size();++i){
    const auto area=L::headerKnob(i);
    CHECK(area.x1-area.x0==36.);
    CHECK(area.y1-area.y0==36.);
    CHECK(area.hit(L::headerKnobCenters[i],L::headerKnobY));
    if(i)CHECK(area.x0>L::headerKnob(i-1).x1+60.);
  }
  CHECK(L::headerKnob(2).x1<1132.);
  // All eight V1.0.1 Wave keys share the exact same paint/hit geometry.
  // The old modal remains independently checked by the 1.0.0 tests.
  using W=drumator::UiV101WaveLayout;
  CHECK(W::toolbar.size()==8);
  for(std::size_t i=0;i<W::toolbar.size();++i){
    const auto r=W::toolbar[i];
    CHECK(r.width()==86.&&r.bottom-r.top==32.);
    CHECK(W::buttonAt((r.left+r.right)/2.,(r.top+r.bottom)/2.)==
          static_cast<int>(i));
    if(i)CHECK(r.left-W::toolbar[i-1].right==8.);
    if(i)CHECK(W::buttonAt(r.left-4.,473.)==-1);
  }
  for(std::size_t i=0;i<5;++i){
    const auto r=W::readout(i);
    CHECK(r.width()==140.&&r.bottom-r.top==46.);
    if(i)CHECK(r.left-W::readout(i-1).right==13.);
  }
  // ZOOM now shares the exact 83x32 vintage face with all large keys.
  CHECK(L::zoomTrigger.x1-L::zoomTrigger.x0==L::headerActionWidth);
  CHECK(L::zoomTrigger.y1-L::zoomTrigger.y0==L::headerButtonHeight);
  CHECK(L::zoomHit(1145.,25.,false)==-1);
  CHECK(L::zoomHit(1166.,14.,false)==-2); // newly added top-left edge
  CHECK(L::zoomHit(1164.,14.,false)==-1); // outside bezel
  CHECK(L::zoomHit(1200.,25.,false)==-2);
  CHECK(L::zoomHit(1200.,25.,true)==-2);
  for(std::size_t i=0;i<L::zoomPresets.size();++i){
    const auto r=L::zoomOption(i);
    CHECK(r.x1-r.x0==86.&&r.y1-r.y0==24.);
    CHECK(L::zoomHit(r.x0+20.,r.y0+12.,true)==static_cast<int>(i));
    CHECK(L::zoomHit(r.x0+20.,r.y0+12.,false)==-1);
  }
  CHECK(L::zoomHit(1100.,165.,true)==-1);
  L::Expansions e{};
  const double y=L::header;
  CHECK(L::footerAdd.x1-L::footerAdd.x0==90.);
  CHECK(L::footerRemove.x1-L::footerRemove.x0==90.);
  CHECK(L::footerAdd.y0==L::footerPrev.y0&&
        L::footerRemove.y1==L::footerNext.y1);
  CHECK(L::footerRemove.x1<L::footerPrev.x0);
  CHECK(L::editorHeight(0,10,e)==686.);
  CHECK(L::editorHeight(0,10,e)*1.5==1029.);
  const auto action=[&](double x,double yy){return L::hit(x,yy,0,10,e).action;};
  CHECK(action(25,y+20)==L::Action::Mute);
  CHECK(action(55,y+20)==L::Action::Solo);
  CHECK(action(90,y+20)==L::Action::Fill);
  CHECK(action(140,y+20)==L::Action::Previous);
  CHECK(action(200,y+20)==L::Action::Load);
  CHECK(action(267,y+20)==L::Action::SampleMenu);
  CHECK(action(290,y+20)==L::Action::Next);
  CHECK(action(320,y+20)==L::Action::Preview);
  CHECK(action(380,y+20)==L::Action::Edit);
  CHECK(action(1062,y+20)==L::Action::Volume);
  CHECK(action(1122,y+20)==L::Action::Pan);
  CHECK(action(1182,y+20)==L::Action::Pitch);
  CHECK(action(1230,y+12)==L::Action::Velocity);
  CHECK(action(1230,y+32)==L::Action::Settings);
  CHECK(action(419.9,y+20)==L::Action::None);
  CHECK(action(420,y+20)==L::Action::Step);
  CHECK(L::hit(420,y+20,0,10,e).step==0);
  CHECK(L::hit(420+31*19,y+20,0,10,e).step==31);
  CHECK(action(1027.9,y+20)==L::Action::None);
  CHECK(action(1028,y+20)==L::Action::None);
  CHECK(L::hit(420,y+9*L::rowHeight+20,0,10,e).row==9);
  CHECK(action(420,y+10*L::rowHeight+20)==L::Action::None);
  CHECK(L::hit(420,y+20,10,6,e).row==10);
  CHECK(L::toggle(L::Expansion::None,L::Expansion::Velocity)==L::Expansion::Velocity);
  CHECK(L::toggle(L::Expansion::Velocity,L::Expansion::Velocity)==L::Expansion::None);
  CHECK(L::toggle(L::Expansion::Velocity,L::Expansion::Settings)==L::Expansion::Settings);
  e[1]=L::Expansion::Velocity;
  CHECK(L::rowTop(0,2,e)==y+L::rowHeight*2+L::expansionHeight);
  CHECK(L::hit(500,y+L::rowHeight*2+10,0,10,e).action==L::Action::VelocityLane);
  e[1]=L::Expansion::Settings;
  CHECK(L::hit(500,y+L::rowHeight*2+10,0,10,e).action==L::Action::SettingsLane);
  CHECK(L::hit(420,L::rowTop(0,2,e)+20,0,10,e).row==2);
  // Every step has an isolated 17 px clickable cap and a 2 px non-clickable gutter.
  for(std::size_t i=0;i<32;++i){
    const double x=L::stepX+L::stepPitch*i;
    CHECK(L::rowHit(0,x+8.,20.,L::Expansion::None).step==static_cast<int>(i));
    CHECK(L::rowHit(0,x+18.,20.,L::Expansion::None).action==L::Action::None);
  }
  using P=drumator::UiV101Popup;
  for(const auto kind:{P::Kind::Pattern,P::Kind::Fill,P::Kind::Output,
                       P::Kind::Choke,P::Kind::Sample}){
    const auto geometry=P::geometry(kind,L::header+5.*L::rowHeight,686.);
    CHECK(geometry.y>=L::header);
    CHECK(geometry.y+geometry.rows*geometry.cellHeight<=686.);
    for(int i=0;i<P::count(kind);++i){
      const auto button=P::cell(geometry,i);
      CHECK(P::hit(kind,geometry,(button.x0+button.x1)/2.,
                   (button.y0+button.y1)/2.)==i);
    }
  }
  std::cout<<"V1.0.1 independent GUI hit-map PASS\n";
  return 0;
}
