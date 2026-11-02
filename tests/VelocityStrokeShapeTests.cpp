#include "core/VelocityStrokeShape.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>
using namespace drumator;
static void ck(bool x,const char* why){if(!x)throw std::runtime_error(why);}
int main(){
 try{
  using S=VelocityStrokeShape;
  S::Values before{};
  for(std::size_t i=0;i<32;++i)before[i]=static_cast<std::uint8_t>(i%3==0?0:65);
  before[1]=25;before[9]=117;
  S shape;
  ck(!shape.begin(before,24,1,32,VelocityStrokeMode::Paint),"bad length");
  ck(!shape.begin(before,16,0,32,VelocityStrokeMode::Paint),"blank anchor");
  ck(shape.begin(before,16,1,32,VelocityStrokeMode::Paint),"begin paint");
  auto v=shape.advance(9,120);
  ck(v[1]==32&&v[9]==120&&v[2]>32&&v[8]<120,"paint gap interpolation");
  for(std::size_t i=0;i<32;++i)
   if(before[i]==0||i>=16)ck(v[i]==before[i],"paint touched blank or hidden");
  ck(shape.begin(before,16,1,20,VelocityStrokeMode::Line),"begin line");
  v=shape.advance(9,120);
  ck(v[1]==20&&v[9]==120&&v[5]==70,"linear ramp");
  v=shape.advance(4,100);
  ck(v[1]==20&&v[4]==100&&v[5]==before[5]&&v[9]==before[9],"retract");
  ck(shape.begin(before,16,1,20,VelocityStrokeMode::Curve),"begin curve");
  v=shape.advance(9,120);
  ck(v[3]==0&&v[2]<35&&v[8]>105,"ease-in-out");
  ck(shape.begin(before,16,1,60,VelocityStrokeMode::Accent),"begin accent");
  v=shape.advance(12,20);
  ck(v[1]==120&&v[9]==120&&v[3]==0&&v[12]==0&&v[13]==before[13],
     "accent sound/blank/out-of-range");
  shape.clear();
  ck(!shape.active()&&shape.advance(2,45)==v,"inactive modified");

  // BC45 Studio One regression: Ctrl+left drag creates a linear 1..127
  // ramp over all present notes. The visible lower/upper edges must be
  // reachable without dragging outside the drawn velocity well.
  const auto ctrl=velocityStrokeGesture(true,false,false);
  ck(ctrl.mode==VelocityStrokeMode::Line&&!ctrl.minimum,
     "Ctrl gesture does not select a line");
  ck(velocityStrokeGesture(false,true,false).mode==VelocityStrokeMode::Line,
     "legacy Shift line changed");
  ck(velocityStrokeGesture(false,true,true).mode==VelocityStrokeMode::Curve,
     "Shift+Alt curve changed");
  ck(velocityStrokeGesture(false,false,true).mode==VelocityStrokeMode::Accent,
     "Alt accent changed");
  const auto minimum=velocityStrokeGesture(true,false,true);
  ck(minimum.mode==VelocityStrokeMode::Paint&&minimum.minimum,
     "Ctrl+Alt minimum brush changed");
  ck(VelocityLaneScale::valueAt(89.)==1&&
     VelocityLaneScale::valueAt(53.)==127&&
     VelocityLaneScale::valueAt(71.)==64,
     "painted lane does not cover the MIDI 1..127 range");
  ck(VelocityLaneScale::heightFor(1)==1.&&
     VelocityLaneScale::heightFor(127)==35.&&
     VelocityLaneScale::heightFor(0)==0.,
     "rendered bar height mismatches the input range");
  S::Values populated{};
  populated.fill(90);
  ck(shape.begin(populated,16,0,VelocityLaneScale::valueAt(89.),ctrl.mode),
     "Ctrl ramp cannot start at lowest visible edge");
  const auto ramp=shape.advance(15,VelocityLaneScale::valueAt(53.));
  ck(ramp[0]==1&&ramp[15]==127&&ramp[7]<ramp[8]&&
     ramp[8]>55&&ramp[8]<75,"Ctrl ramp does not span 1..127");
  for(std::size_t i=1;i<16;++i)
   ck(ramp[i]>=ramp[i-1],"Ctrl ramp is not monotonic");
  for(std::size_t i=16;i<32;++i)
   ck(ramp[i]==populated[i],"Ctrl ramp touched inactive bar");
  std::cout<<"BC45 Ctrl line 1..127 input/visual geometry, modifiers and BC30 strokes PASS\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BC30 FAIL: "<<e.what()<<"\n";return 1;}
}
