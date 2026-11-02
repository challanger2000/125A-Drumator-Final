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
  std::cout<<"BC30 velocity stroke paint, gap fill, line, curve, retract and accent PASS\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BC30 FAIL: "<<e.what()<<"\n";return 1;}
}
