#include "core/VelocityStrokeShape.h"
#include <array>
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

  // BC46: original V1.5 modifier controls, 16/32 bar limits and empty cells.
  ck(velocityStrokeGesture(false,false,false).mode==VelocityStrokeMode::Paint,
     "plain V1.5 click must paint");
  const auto shift=velocityStrokeGesture(false,true,false);
  ck(shift.mode==VelocityStrokeMode::Line&&!shift.minimum,
     "Shift+drag must draw a straight line");
  const auto v15Curve=velocityStrokeGesture(false,true,true);
  ck(v15Curve.mode==VelocityStrokeMode::Curve&&!v15Curve.minimum,
     "Shift+Alt must draw a curve");
  const auto v15Accent=velocityStrokeGesture(false,false,true);
  ck(v15Accent.mode==VelocityStrokeMode::Accent&&!v15Accent.minimum,
     "Alt must accent");
  S::Values complete{}; complete.fill(64);
  ck(shape.begin(complete,32,0,100,VelocityStrokeMode::Paint),"single click start");
  const auto single=shape.values();
  for(std::size_t i=0;i<32;++i)
   ck(single[i]==(i==0?100:64),"single click touched unrelated step");
  ck(shape.begin(complete,32,0,1,shift.mode),"32-step Shift ramp start");
  auto full=shape.advance(31,127);
  ck(full[0]==1&&full[31]==127&&full[15]==62&&full[16]==66,
     "32-step Shift ramp is not exact");
  for(std::size_t i=1;i<32;++i)
   ck(full[i]>=full[i-1],"Shift ramp not monotonic");
  ck(shape.begin(complete,16,0,1,shift.mode),"16-step Shift ramp start");
  const auto shortRamp=shape.advance(15,127);
  ck(shortRamp[0]==1&&shortRamp[15]==127&&shortRamp[16]==64&&
     shortRamp[31]==64,"16-step ramp touched inactive second bar");
  S::Values holes=complete;holes[5]=0;holes[23]=0;
  ck(shape.begin(holes,32,0,1,shift.mode),"gapped Shift ramp start");
  const auto missing=shape.advance(31,127);
  ck(missing[5]==0&&missing[23]==0&&missing[0]==1&&missing[31]==127,
     "shift ramp created an empty step");
  ck(!shape.begin(holes,32,5,50,shift.mode),"empty note is an illegal anchor");
  ck(shape.begin(holes,32,0,1,v15Curve.mode),"curve start");
  const auto curved=shape.advance(31,127);
  ck(curved[5]==0&&curved[23]==0&&curved[8]<full[8]&&
     curved[24]>full[24],"Shift+Alt ease-in/ease-out or blanks wrong");
  ck(shape.begin(holes,32,1,50,v15Accent.mode),"accent start");
  const auto accented=shape.advance(31,5);
  ck(accented[1]>=120&&accented[31]>=120&&
     accented[5]==0&&accented[23]==0,"Alt accent not 120+ or touched empty");
  ck(shape.begin(holes,32,0,1,VelocityStrokeMode::Paint),"ordinary drag start");
  const auto brushed=shape.advance(31,127);
  ck(brushed[5]==0&&brushed[23]==0&&brushed[2]>1&&brushed[30]<127,
     "sparse host drag interpolation broken");
  ck(shape.begin(complete,16,15,127,shift.mode),"reverse line start");
  auto reverse=shape.advance(0,1);
  ck(reverse[0]==1&&reverse[15]==127&&reverse[16]==64,
     "right-to-left ramp broken");
  reverse=shape.advance(10,75);
  ck(reverse[0]==64&&reverse[10]==75&&reverse[11]==64,
     "retract failed to restore old values");
  std::cout<<"BC46 V1.5 click/paint/Shift line/curve/Alt/16-32/empty plus BC45 scale PASS\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BC30 FAIL: "<<e.what()<<"\n";return 1;}
}
