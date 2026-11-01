#include "core/BeatCreatorTransaction.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>
using namespace drumator;
static void check(bool v,const char* why){if(!v)throw std::runtime_error(why);}
int main(){
 try{
  const auto preview=BeatCreator::preview({BeatStyle::IndustrialNDH,55,16},123);
  check(preview.valid,"invalid generated preview");
  BeatTriple before{};
  before[0][0]=101;before[1][1*kSteps+2]=66;
  before[2][10*kSteps+20]=73; // protected row and hidden bar
  before[0][0*kSteps+27]=57;  // protected hidden bar in 1T
  BeatLengths lengths{32,16,32};
  const auto occupied=BeatCreator::occupiedMask(before);
  check(occupied==7,"existing pattern occupied flag wrong");
  const auto refused=BeatCreatorTransaction::prepare(preview,before,lengths,false);
  check(!refused.valid&&refused.occupied==7,"overwrite without consent allowed");
  const auto granted=BeatCreatorTransaction::prepare(preview,before,lengths,true);
  check(granted.valid&&granted.before==before,"approved transaction invalid");
  check(granted.oldLengths==lengths&&granted.newLengths==BeatLengths{16,16,16},
       "pattern lengths incorrect");
  for(std::size_t p=0;p<3;++p){
   for(std::size_t row=0;row<8;++row)
    for(std::size_t step=0;step<16;++step)
     check(granted.after[p][row*kSteps+step]==
       (p==0?preview.a:p==1?preview.b:preview.c)[row*kSteps+step],
       "preview-to-physical-row mapping wrong");
   for(std::size_t row=0;row<kRows;++row)
    for(std::size_t step=0;step<kSteps;++step)
     if(row>=8||step>=16)
      check(granted.after[p][row*kSteps+step]==before[p][row*kSteps+step],
            "creator overwrote unrelated row or hidden step");
  }
  check(before[0][27]==57&&before[2][10*kSteps+20]==73,
        "read-only preview modified user snapshot");
  BeatCreatorPreview malformed=preview;
  malformed.a[8*kSteps]=100;
  check(!BeatCreatorTransaction::prepare(malformed,before,lengths,true).valid,
        "malformed preview with user row accepted");
  malformed=preview;malformed.b[20]=96;
  check(!BeatCreatorTransaction::prepare(malformed,before,lengths,true).valid,
        "malformed hidden-bar preview accepted");
  malformed=preview;malformed.c[0]=255;
  check(!BeatCreatorTransaction::prepare(malformed,before,lengths,true).valid,
        "out of range velocity accepted");
  auto invalidLengths=lengths;invalidLengths[2]=0;
  check(!BeatCreatorTransaction::prepare(preview,before,invalidLengths,true).valid,
        "bad project length accepted");
  const auto empty=BeatCreatorTransaction::prepare(preview,BeatTriple{},BeatLengths{32,32,32},false);
  check(empty.valid&&empty.occupied==0,"empty-project apply should be possible");
  std::cout<<"Beat Creator staging/consent/preservation tests PASS\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BeatCreatorTransaction: "<<e.what()<<"\n";return 1;}
}
