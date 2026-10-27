#include "core/UiV101Layout.h"
#include <iostream>
using L=drumator::UiV101Layout;
#define CHECK(x) do{if(!(x)){std::cerr<<__LINE__<<" " #x "\n";return 1;}}while(false)
int main(){
  // Existing host zoom choices, identical cap geometry, and modal hit map.
  CHECK(L::zoomPresets.size()==4);
  CHECK(L::zoomPresets[0]==1.&&L::zoomPresets[1]==1.2);
  CHECK(L::zoomPresets[2]==1.3&&L::zoomPresets[3]==1.5);
  // Label-specific fixed sizes; uniform 34px cap height and 9px spacing.
  constexpr std::array<double,5> capWidths{{84.,74.,96.,112.,102.}};
  for(std::size_t i=0;i<L::headerGroupCount;++i){
    const auto r=L::headerGroup(i);
    CHECK(r.x1-r.x0==capWidths[i]&&r.y1-r.y0==34.);
    CHECK(r.hit(r.x0+4.,r.y0+4.));
    if(i)CHECK(L::headerGroup(i-1).x1+9.==r.x0);
  }
  CHECK(L::headerGroup(0).x0>514.);
  CHECK(L::headerGroup(4).x1<1132.);
  CHECK(L::zoomHit(1200.,25.,false)==-2);
  CHECK(L::zoomHit(1200.,25.,true)==-2);
  for(std::size_t i=0;i<L::zoomPresets.size();++i){
    const auto r=L::zoomOption(i);
    CHECK(r.x1-r.x0==114.&&r.y1-r.y0==24.);
    CHECK(L::zoomHit(r.x0+20.,r.y0+12.,true)==static_cast<int>(i));
    CHECK(L::zoomHit(r.x0+20.,r.y0+12.,false)==-1);
  }
  CHECK(L::zoomHit(1100.,165.,true)==-1);
  L::Expansions e{};
  const double y=L::header;
  CHECK(L::editorHeight(0,10,e)==686.);
  CHECK(L::editorHeight(0,10,e)*1.5==1029.);
  const auto action=[&](double x,double yy){return L::hit(x,yy,0,10,e).action;};
  CHECK(action(25,y+20)==L::Action::Mute);
  CHECK(action(55,y+20)==L::Action::Solo);
  CHECK(action(90,y+20)==L::Action::Fill);
  CHECK(action(140,y+20)==L::Action::Previous);
  CHECK(action(200,y+20)==L::Action::Load);
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
  std::cout<<"V1.0.1 independent GUI hit-map PASS\n";
  return 0;
}
