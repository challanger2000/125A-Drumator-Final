#include "core/StereoPeakLimiter.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>
using namespace drumator;
int main(){
 int bad=0;
 auto check=[&](bool ok,const char* what){if(!ok){++bad;std::printf("FAIL: %s\n",what);}};
 StereoPeakLimiter s;
 check(s.prepare(48000),"prepare");
 std::vector<float> input(12000);
 for(std::size_t i=0;i<input.size();++i)input[i]=
    i==8000?2.f:(i>=3500&&i<4500?1.8f:0.1f);
 for(std::size_t i=0;i<input.size();++i){
  float l=input[i],r=-input[i]*.75f;
  s.process(l,r,true);
  if(i>300&&i<input.size()-100){
   check(std::max(std::abs(l),std::abs(r))<=StereoPeakLimiter::kCeiling+1e-5f,
         "ON peak ceiling after ramp settled");
  }
  if(i>300&&input[i-StereoPeakLimiter::kLookahead]>0.001f)
    check(std::abs(l+r/.75f)<1e-4f,"linked stereo balance");
 }
 s.reset();
 for(std::size_t i=0;i<300;++i){
  float l=i==0?.5f:.1f,r=-l;
  s.process(l,r,false);
  if(i>=StereoPeakLimiter::kLookahead){
   check(std::abs(l-(i==StereoPeakLimiter::kLookahead?.5f:.1f))<1e-5f,
        "OFF neutral level at fixed latency");
  }
 }
 check(!s.prepare(0)&&!s.prepare(1e9),"invalid rate rejected");
 std::printf("StereoPeakLimiter: %s\n",bad==0?"PASS":"FAIL");
 return bad?1:0;
}
