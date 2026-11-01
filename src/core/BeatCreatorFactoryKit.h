// 125A Drumator 1.5 - original, synthesized demonstration drum voices.
// No 3rd party PCM and no alteration of any user's samples or kits.
// Control thread / offline only. These deliberately simple sounds are
// intended to judge generated GROOVES, not as a premium drum library.
#pragma once
#include "BeatCreator.h"
#include "KitState.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace drumator {
class BeatCreatorFactoryKit final {
 static constexpr double pi=3.14159265358979323846;
 static constexpr double twoPi=2.0*pi;
 static constexpr std::uint32_t rate=44100u;
 struct Random {
  std::uint32_t x;
  explicit Random(std::uint32_t seed) noexcept:x(seed) {}
  double sample() noexcept {
   x^=x<<13;x^=x>>17;x^=x<<5;
   return static_cast<double>(x&0xffffu)/32767.5-1.0;
  }
 };
 static void put16(std::vector<std::uint8_t>& x,std::size_t o,std::uint16_t v) {
  x[o]=static_cast<std::uint8_t>(v);
  x[o+1]=static_cast<std::uint8_t>(v>>8);
 }
 static void put32(std::vector<std::uint8_t>& x,std::size_t o,std::uint32_t v) {
  for(unsigned k=0;k<4;++k)x[o+k]=static_cast<std::uint8_t>(v>>(8*k));
 }
 static double seconds(std::size_t role) noexcept {
  constexpr std::array<double,8> lengths{
   0.52,0.34,0.12,0.52,0.46,0.32,1.28,0.39
  };
  return lengths[role];
 }
 public:
 static constexpr std::array<const char*,8> filenames{{
  "01_BD.wav","02_Snare.wav","03_CHH.wav","04_OHH.wav",
  "05_Low_Tom.wav","06_High_Tom.wav","07_Crash.wav","08_Clap.wav"
 }};
 static constexpr std::array<const char*,8> labels{{
  "BD","Snare","CHH","OHH","Low Tom","High Tom","Crash","Clap"
 }};
 static constexpr std::uint32_t sampleRate=rate;
 // Build one standard 44.1kHz mono PCM16 WAVE in memory. No file I/O.
 static std::vector<std::uint8_t> wav(std::size_t role) {
  if(role>=filenames.size())return {};
  const auto frames=static_cast<std::size_t>(
      std::lround(seconds(role)*static_cast<double>(rate)));
  if(frames==0||frames>70000u)return {};
  std::vector<std::uint8_t> out(44u+frames*2u);
  std::memcpy(out.data(),"RIFF",4);
  put32(out,4,static_cast<std::uint32_t>(out.size()-8u));
  std::memcpy(out.data()+8,"WAVEfmt ",8);
  put32(out,16,16);put16(out,20,1);put16(out,22,1);
  put32(out,24,rate);put32(out,28,rate*2u);
  put16(out,32,2);put16(out,34,16);
  std::memcpy(out.data()+36,"data",4);
  put32(out,40,static_cast<std::uint32_t>(frames*2u));
  Random noise(0x125AD00Du+static_cast<std::uint32_t>(role)*0x09E3779Bu);
  double low=0.0,phase=0.0;
  for(std::size_t i=0;i<frames;++i){
   const double t=static_cast<double>(i)/rate;
   const double n=noise.sample();
   low+=0.105*(n-low); // filtered body removed from sizzle components
   const double bright=n-low;
   double sound=0.0;
   switch(role){
    case 0: { // Kick: fast down-swept oscillator plus click
     const double hz=45.0+165.0*std::exp(-t*38.0);
     phase+=twoPi*hz/rate;
     sound=0.87*std::sin(phase)*std::exp(-t*12.5)+
           0.23*bright*std::exp(-t*260.0);
     break;
    }
    case 1: { // Snare: noisy high-pass snap with tuned short shell
     phase+=twoPi*(178.0+18.0*std::exp(-t*28.0))/rate;
     sound=0.72*bright*std::exp(-t*20.0)+
           0.28*std::sin(phase)*std::exp(-t*15.0);
     break;
    }
    case 2: { // Closed hat: short metallic high-passed wash
     sound=(0.67*bright+0.23*std::sin(twoPi*6350.0*t)+
            0.10*std::sin(twoPi*8810.0*t))*std::exp(-t*49.0);
     break;
    }
    case 3: { // Open hat: related timbre, longer but decaying tail
     sound=(0.78*bright+0.13*std::sin(twoPi*6391.0*t)+
            0.09*std::sin(twoPi*8791.0*t))*std::exp(-t*8.5);
     break;
    }
    case 4: // Low tom
    case 5: { // High tom: distinct pitched decays with a slight click
     const double bottom=role==4?89.0:154.0;
     const double sweep=role==4?65.0:105.0;
     phase+=twoPi*(bottom+sweep*std::exp(-t*19.0))/rate;
     sound=(0.85*std::sin(phase)+0.11*std::sin(phase*1.8))*
            std::exp(-t*(role==4?9.8:13.6))+
            0.05*bright*std::exp(-t*155.0);
     break;
    }
    case 6: { // Crash: long noisy metallic texture
     const double onset=1.0-std::exp(-t*650.0);
     sound=(0.65*bright+0.17*std::sin(twoPi*3781.0*t)+
            0.11*std::sin(twoPi*5437.0*t)+
            0.07*std::sin(twoPi*7979.0*t))*onset*
            (0.75*std::exp(-t*3.15)+0.25*std::exp(-t*9.0));
     break;
    }
    case 7: { // Handclap: four fluttered noise bursts and a short tail
     const auto burst=[t](double delay)->double{
      const double local=t-delay;
      return local>=0.0?std::exp(-local*200.0):0.0;
     };
     const double envelope=0.54*burst(0.0)+0.64*burst(0.012)+
           0.75*burst(0.025)+0.55*burst(0.038)+
           (t>=0.038?0.31*std::exp(-(t-0.038)*18.0):0.0);
     sound=bright*envelope;
     break;
    }
    default:break;
   }
   // Smooth the edges, keep headroom and avoid hard-truncated tails.
   const double fadeIn=std::min(1.0,t*1250.0);
   const double remaining=static_cast<double>(frames-1u-i)/rate;
   const double fadeOut=std::min(1.0,remaining*600.0);
   const double safe=std::clamp(sound*fadeIn*fadeOut*0.82,-0.97,0.97);
   const auto pcm=static_cast<std::int16_t>(std::lround(safe*32767.0));
   const auto bits=static_cast<std::uint16_t>(pcm);
   put16(out,44u+i*2u,bits);
  }
  return out;
 }
 // Builds an entirely independent temporary kit; no user-kit reference.
 // It is NOT loaded into the active Drumator by this operation.
 static bool makeKit(KitState& result) {
  try {
   KitState candidate;
   candidate.visible=8;
   candidate.selected=0;
   candidate.limiter=1;
   candidate.masterVolume=85;
   candidate.patternLengths.fill(16);
   constexpr std::array<std::uint8_t,8> gain{{90,83,64,57,73,73,49,68}};
   constexpr std::array<std::uint8_t,8> pan{{100,100,113,87,89,111,100,100}};
   for(std::size_t row=0;row<8;++row){
    candidate.names[row]=filenames[row];
    candidate.displayNames[row]=labels[row];
    candidate.wav[row]=wav(row);
    candidate.gain[row]=gain[row];
    candidate.pan[row]=pan[row];
    if(candidate.wav[row].empty())return false;
   }
   if(!KitCodec::valid(candidate,true))return false;
   result=std::move(candidate);
   return true;
  }catch(...){return false;}
 }
 // Copy only selected draft, always into an isolated kit's Pattern A.
 static bool setDraft(KitState& temporary,const BeatCreatorPreview& draft,
                      std::size_t candidate) noexcept {
  if(!draft.valid||(draft.steps!=16&&draft.steps!=32)||candidate>=3)
   return false;
  const PatternSteps* choice=candidate==0?&draft.a:candidate==1?&draft.b:&draft.c;
  if(!BeatCreator::occupiedMask({*choice,{},{}})&&emptyPattern(*choice))return false;
  temporary.grid.fill(0);
  std::copy(choice->begin(),choice->end(),temporary.grid.begin());
  temporary.patternLengths[0]=draft.steps;
  temporary.selected=0;
  return true;
 }
};
} // namespace drumator
