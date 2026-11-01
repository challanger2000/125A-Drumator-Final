// 125A Drumator 1.5: genre-aware Beat Creator (control thread only).
// Adapted concepts: 125A Midiator Final, src/DrumBrain.cpp / DrumBrain.h.
// No MIDI output, no sample replacement, no GUI or controller mutation here.
#pragma once
#include "PatternOperations.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace drumator {
enum class BeatStyle : std::uint8_t {
 IndustrialNDH, EBM, Techno, House, Rock, Metal, HipHop, DrumAndBass, Count
};
enum class BeatRole : std::size_t {
 BD=0, Snare=1, CHH=2, OHH=3, LowTom=4, HighTom=5, Crash=6, Clap=7
};
struct BeatCreatorSettings {
 BeatStyle style{BeatStyle::IndustrialNDH};
 std::uint8_t complexity{35}; // 0=minimal backbone, 100=busy; no duplicate Humanize knob
 std::uint8_t steps{16};      // 1 or 2 bars
};
struct BeatCreatorPreview {
 PatternSteps a{},b{},c{}; // in-memory only until explicit host/GUI APPLY
 std::uint8_t steps{0};
 bool valid{false};
};
class BeatCreator final {
 struct Rng {
  std::uint32_t state;
  explicit Rng(std::uint32_t seed) noexcept : state(seed?seed:0x125AD00Du) {}
  std::uint32_t next() noexcept {
   auto x=state;x^=x<<13;x^=x>>17;x^=x<<5;state=x;return x;
  }
  bool chance(unsigned permille) noexcept {
   return next()%1000u<std::min(permille,1000u);
  }
  int spread(int magnitude) noexcept {
   if(magnitude<=0)return 0;
   return static_cast<int>(next()%static_cast<unsigned>(2*magnitude+1))-magnitude;
  }
 };
 static constexpr std::size_t idx(BeatRole role,std::size_t step) noexcept {
  return static_cast<std::size_t>(role)*kSteps+step;
 }
 static void hit(PatternSteps& p,BeatRole role,std::size_t step,int velocity) noexcept {
  if(step>=kSteps)return;
  const auto pos=idx(role,step);
  const auto next=static_cast<std::uint8_t>(std::clamp(velocity,1,127));
  p[pos]=std::max(p[pos],next);
 }
 static std::uint8_t vel(Rng& rng,int base) noexcept {
  return static_cast<std::uint8_t>(std::clamp(base+rng.spread(5),1,127));
 }
 static bool electronic(BeatStyle s) noexcept {
  return s==BeatStyle::IndustrialNDH||s==BeatStyle::EBM||
         s==BeatStyle::Techno||s==BeatStyle::House;
 }
 public:
 static constexpr bool valid(const BeatCreatorSettings& s) noexcept {
  return s.steps==16 || s.steps==32 ?
         static_cast<unsigned>(s.style)<static_cast<unsigned>(BeatStyle::Count)&&
         s.complexity<=100 : false;
 }
 static constexpr std::array<const char*,8> styleNames{{
  "INDUSTRIAL / NDH","EBM","TECHNO","HOUSE","ROCK","METAL","HIP-HOP","DRUM & BASS"
 }};
 static PatternSteps generate(const BeatCreatorSettings& s,std::uint32_t seed) noexcept {
  PatternSteps p{};
  if(!valid(s))return p;
  Rng structure(seed^0x474E5241u), dynamics(seed^0x56454C31u);
  const unsigned busy=static_cast<unsigned>(s.complexity);
  for(std::size_t st=0;st<s.steps;++st) {
   const auto loc=st%16u;
   const bool quarter=loc%4u==0u,eighth=loc%2u==0u;
   const bool snare=loc==4u||loc==12u;
   bool kick=false,hat=false,clap=false,open=false;
   switch(s.style) {
    case BeatStyle::IndustrialNDH:
     kick=quarter;hat=eighth;break;
    case BeatStyle::EBM:
     kick=quarter;hat=loc%4u==2u;clap=snare&&busy>=55;break;
    case BeatStyle::Techno:
     kick=quarter;hat=loc%4u==2u;clap=snare;break;
    case BeatStyle::House:
     kick=quarter;hat=eighth;clap=snare;break;
    case BeatStyle::Rock:
     kick=loc==0u||loc==8u||loc==10u;hat=eighth;break;
    case BeatStyle::Metal:
     kick=loc==0u||loc==2u||loc==6u||loc==8u||loc==10u||loc==14u;
     hat=eighth;break;
    case BeatStyle::HipHop:
     kick=loc==0u||loc==7u||loc==10u;hat=eighth;
     clap=snare&&busy>=40;break;
    case BeatStyle::DrumAndBass:
     kick=loc==0u||loc==10u;hat=eighth;break;
    case BeatStyle::Count:break;
   }
   // Structural backbeats and downbeats remain stable, even at 100%.
   if(kick)hit(p,BeatRole::BD,st,vel(dynamics,loc%4u==0u?115:100));
   if(snare)hit(p,BeatRole::Snare,st,vel(dynamics,loc==12u?116:110));
   if(clap)hit(p,BeatRole::Clap,st,vel(dynamics,88));
   // Hat layers are mutually exclusive; don't choke the same step twice.
   if(hat){
    open=electronic(s.style)&&loc==14u&&busy>=35;
    hit(p,open?BeatRole::OHH:BeatRole::CHH,st,
        vel(dynamics,loc%4u==0u?88:78));
   }
   // Complexity enriches structure in a genre-aware, bounded manner.
   if(busy>15&&
      (loc==3u||loc==7u||loc==11u||loc==15u)&&
      structure.chance((busy-15u)*4u)) {
    if(s.style==BeatStyle::Metal||s.style==BeatStyle::IndustrialNDH||
       s.style==BeatStyle::DrumAndBass||s.style==BeatStyle::HipHop)
     hit(p,BeatRole::BD,st,vel(dynamics,75));
   }
   if(busy>30&&loc%2u==1u&&
      structure.chance((busy-30u)*6u))
    hit(p,BeatRole::CHH,st,vel(dynamics,54));
   if(busy>55&&(loc==3u||loc==11u)&&
      structure.chance((busy-55u)*9u))
    hit(p,BeatRole::Snare,st,vel(dynamics,43)); // ghost
   // A two-bar phrase may have a subtle pickup in the second bar.
   if(s.steps==32&&st==31u&&busy>=60)
    hit(p,BeatRole::BD,st,vel(dynamics,82));
  }
  // Crash punctuation is meaningful but not forced on every bar.
  hit(p,BeatRole::Crash,0,vel(dynamics,110));
  return p;
 }
 static PatternSteps variate(const PatternSteps& a,
                             const BeatCreatorSettings& s,
                             std::uint32_t seed) noexcept {
  PatternSteps b=a;
  if(!valid(s))return b;
  Rng rng(seed^0x56415231u);
  const auto busy=static_cast<unsigned>(s.complexity);
  for(std::size_t bar=0;bar<s.steps/16u;++bar){
   const std::size_t first=16u*bar;
   // Recognizable motif: keep all quarter-note kick/snare anchors.
   if(busy>=20&&rng.chance(350u+busy*3u))
    hit(b,BeatRole::BD,first+11u,82);
   if(busy>=40&&rng.chance(200u+busy*4u))
    hit(b,BeatRole::CHH,first+13u,57);
   if(busy>=60&&electronic(s.style))
    hit(b,BeatRole::Clap,first+12u,91);
  }
  // Guaranteed audible structural difference, never change beats 1/2/3/4.
  const auto tail=static_cast<std::size_t>(s.steps)-1u;
  const auto pos=idx(BeatRole::BD,tail);
  b[pos]=b[pos]?0u:static_cast<std::uint8_t>(80u);
  return b;
 }
 static PatternSteps makeBreak(const PatternSteps& a,
                               const BeatCreatorSettings& s,
                               std::uint32_t seed) noexcept {
  PatternSteps c=a;
  if(!valid(s))return c;
  Rng dynamics(seed^0x4252454Bu);
  const std::size_t last=static_cast<std::size_t>(s.steps)-4u;
  const unsigned busy=static_cast<unsigned>(s.complexity);
  // Only replace the end of the last bar: retain the groove's identity.
  for(std::size_t step=last;step<s.steps;++step) {
   c[idx(BeatRole::CHH,step)]=0;
   c[idx(BeatRole::OHH,step)]=0;
  }
  const bool acoustic=s.style==BeatStyle::Rock||s.style==BeatStyle::Metal||
                      s.style==BeatStyle::DrumAndBass;
  if(acoustic) {
   hit(c,BeatRole::LowTom,last,vel(dynamics,88));
   if(busy>=25)hit(c,BeatRole::LowTom,last+1u,vel(dynamics,86));
   hit(c,BeatRole::HighTom,last+2u,vel(dynamics,98));
   hit(c,BeatRole::HighTom,last+3u,vel(dynamics,112));
  } else {
   hit(c,BeatRole::Snare,last,vel(dynamics,95));
   if(busy>=25)hit(c,BeatRole::Snare,last+1u,vel(dynamics,87));
   hit(c,BeatRole::Snare,last+2u,vel(dynamics,103));
   hit(c,BeatRole::Snare,last+3u,vel(dynamics,115));
   hit(c,BeatRole::HighTom,last+3u,vel(dynamics,99));
  }
  return c;
 }
 static BeatCreatorPreview preview(const BeatCreatorSettings& s,
                                   std::uint32_t seed) noexcept {
  BeatCreatorPreview result{};
  if(!valid(s))return result;
  result.steps=s.steps;result.valid=true;
  result.a=generate(s,seed);
  result.b=variate(result.a,s,seed^0xA17Bu);
  result.c=makeBreak(result.a,s,seed^0xC17Bu);
  return result;
 }
 // Flags A=1, B=2, C=4. Check ALL 32 steps, including a hidden second bar.
 // Caller must explicitly approve replacement when any bit is set.
 static std::uint8_t occupiedMask(const std::array<PatternSteps,3>& existing) noexcept {
  std::uint8_t mask=0;
  for(std::size_t i=0;i<existing.size();++i)
   if(!emptyPattern(existing[i]))mask=static_cast<std::uint8_t>(mask|(1u<<i));
  return mask;
 }
};
} // namespace drumator
