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
 // 16-step genre motifs. Each style has three deliberately different
 // arrangements; seeds select musical templates, not arbitrary step noise.
 // Bit positions are the 1/16-note cells inside one bar.
 static constexpr std::uint16_t beat(unsigned n) noexcept {
  return static_cast<std::uint16_t>(1u<<n);
 }
 struct Groove {
  std::uint16_t kick,hat,clap,open;
 };
 static constexpr Groove motif(BeatStyle style,unsigned which) noexcept {
  constexpr auto q=beat(0)|beat(4)|beat(8)|beat(12);
  constexpr auto e=beat(0)|beat(2)|beat(4)|beat(6)|
                   beat(8)|beat(10)|beat(12)|beat(14);
  constexpr auto off=beat(2)|beat(6)|beat(10)|beat(14);
  constexpr auto back=beat(4)|beat(12);
  switch(style) {
   case BeatStyle::IndustrialNDH:
    switch(which) {
     case 0:return {static_cast<std::uint16_t>(q|beat(7)),e,0,beat(14)};
     case 1:return {static_cast<std::uint16_t>(q|beat(11)),e,0,beat(14)};
     default:return {static_cast<std::uint16_t>(q|beat(3)|beat(10)),
                     static_cast<std::uint16_t>(e|beat(15)),0,beat(14)};
    }
   case BeatStyle::EBM:
    switch(which) {
     case 0:return {q,off,0,beat(14)};
     case 1:return {q,static_cast<std::uint16_t>(off|beat(7)|beat(15)),0,beat(14)};
     default:return {static_cast<std::uint16_t>(q|beat(10)),
                     static_cast<std::uint16_t>(off|beat(3)|beat(11)),0,beat(14)};
    }
   case BeatStyle::Techno:
    switch(which) {
     case 0:return {q,off,back,beat(14)};
     case 1:return {q,static_cast<std::uint16_t>(off|beat(7)),back,beat(14)};
     default:return {q,static_cast<std::uint16_t>(off|beat(3)|beat(11)),
                     static_cast<std::uint16_t>(back|beat(15)),beat(14)};
    }
   case BeatStyle::House:
    switch(which) {
     case 0:return {q,e,back,static_cast<std::uint16_t>(beat(6)|beat(14))};
     case 1:return {q,static_cast<std::uint16_t>(off|beat(7)|beat(15)),
                     back,static_cast<std::uint16_t>(beat(2)|beat(10))};
     default:return {q,static_cast<std::uint16_t>(e|beat(11)),
                     static_cast<std::uint16_t>(back|beat(15)),
                     static_cast<std::uint16_t>(beat(6)|beat(14))};
    }
   case BeatStyle::Rock:
    switch(which) {
     case 0:return {static_cast<std::uint16_t>(beat(0)|beat(8)|beat(10)),e,0,0};
     case 1:return {static_cast<std::uint16_t>(beat(0)|beat(7)|beat(8)|beat(10)),e,0,0};
     default:return {static_cast<std::uint16_t>(beat(0)|beat(6)|beat(8)|beat(10)|beat(14)),
                     static_cast<std::uint16_t>(e|beat(15)),0,0};
    }
   case BeatStyle::Metal:
    switch(which) {
     case 0:return {static_cast<std::uint16_t>(beat(0)|beat(2)|beat(6)|beat(8)|beat(10)|beat(14)),e,0,0};
     case 1:return {static_cast<std::uint16_t>(beat(0)|beat(2)|beat(3)|beat(6)|beat(8)|beat(10)|beat(14)),e,0,0};
     default:return {static_cast<std::uint16_t>(beat(0)|beat(2)|beat(6)|beat(7)|beat(8)|beat(10)|beat(11)|beat(14)),
                     static_cast<std::uint16_t>(e|beat(15)),0,0};
    }
   case BeatStyle::HipHop:
    switch(which) {
     case 0:return {static_cast<std::uint16_t>(beat(0)|beat(7)|beat(10)),e,back,0};
     case 1:return {static_cast<std::uint16_t>(beat(0)|beat(6)|beat(10)),
                     static_cast<std::uint16_t>(e|beat(11)),back,0};
     default:return {static_cast<std::uint16_t>(beat(0)|beat(3)|beat(8)|beat(10)),
                     static_cast<std::uint16_t>(e|beat(7)|beat(15)),back,0};
    }
   case BeatStyle::DrumAndBass:
    switch(which) {
     case 0:return {static_cast<std::uint16_t>(beat(0)|beat(10)),e,0,0};
     case 1:return {static_cast<std::uint16_t>(beat(0)|beat(7)|beat(10)),
                     static_cast<std::uint16_t>(e|beat(15)),0,0};
     default:return {static_cast<std::uint16_t>(beat(0)|beat(6)|beat(10)|beat(15)),
                     static_cast<std::uint16_t>(e|beat(11)),0,0};
    }
   case BeatStyle::Count:break;
  }
  return {};
 }
 static PatternSteps generate(const BeatCreatorSettings& s,std::uint32_t seed) noexcept {
  PatternSteps p{};
  if(!valid(s))return p;
  Rng structure(seed^0x474E5241u),dynamics(seed^0x56454C31u);
  const unsigned busy=static_cast<unsigned>(s.complexity);
  const unsigned style=static_cast<unsigned>(s.style);
  // Consecutive GENERATE clicks must not choose the same arrangement:
  // cycle the 3 core motifs while independently changing the ornamental
  // variation every third click (up to 9 distinct step structures).
  const unsigned first=busy<15u?0u:seed%3u;
  const unsigned ornament=busy<25u?0u:(seed/3u)%3u;
  // A two-bar phrase must develop the initial motif, not mechanically
  // repeat it. At Complexity 0, a deliberately minimal loop is fine.
  const unsigned second=busy<15u?0u:
     (first+1u+(structure.next()%2u))%3u;
  for(std::size_t bar=0;bar<s.steps/16u;++bar) {
   const auto g=motif(static_cast<BeatStyle>(style),bar?second:first);
   const std::size_t offset=bar*16u;
   for(unsigned loc=0;loc<16u;++loc) {
    const auto st=offset+loc;
    const auto flag=beat(loc);
    // Even at 0% retain each style's skeleton, not the full metal
    // double-kick barrage. Electronic four-on-floor stays intact.
    const bool foundation=loc==0u||loc==8u||loc==10u||
                          (electronic(s.style)&&loc%4u==0u);
    if((g.kick&flag)&&(busy>=20u||foundation))
     hit(p,BeatRole::BD,st,vel(dynamics,loc%4u==0u?115:95));
    // Preserve a reliable backbeat in all eight existing genre workflows.
    if(loc==4u||loc==12u)
     hit(p,BeatRole::Snare,st,vel(dynamics,loc==12u?116:110));
    // A two-step DnB pattern needs displaced, quiet ghost snares.
    // Add one already at the normal 35%, never replacing the backbeat.
    if(s.style==BeatStyle::DrumAndBass&&busy>=30u&&
       (loc==7u||(busy>=55u&&loc==15u)))
     hit(p,BeatRole::Snare,st,vel(dynamics,loc==7u?44:40));
    if(g.clap&flag)
     hit(p,BeatRole::Clap,st,vel(dynamics,87));
    if(g.hat&flag) {
     const bool open=(g.open&flag)&&busy>=35u&&electronic(s.style);
     hit(p,open?BeatRole::OHH:BeatRole::CHH,st,
         vel(dynamics,loc%4u==0u?89:77));
    }
    // Phrase-level 16th-note pickup, not unbounded random noise. It adds
    // quiet motion while leaving all foundation beats and snare intact.
    if(ornament&&loc==(ornament==1u?5u:13u))
     hit(p,BeatRole::CHH,st,vel(dynamics,53));
    // Keep complexity incremental and style-aware. Extra notes are placed
    // on musically useful pickups, not distributed across random cells.
    if(busy>=50u&&(loc==3u||loc==11u||loc==15u)&&
       structure.chance((busy-35u)*6u)) {
     if(s.style==BeatStyle::Metal||s.style==BeatStyle::IndustrialNDH||
        s.style==BeatStyle::HipHop||s.style==BeatStyle::DrumAndBass)
      hit(p,BeatRole::BD,st,vel(dynamics,74));
    }
    if(busy>=55u&&loc%2u==1u&&
       structure.chance((busy-40u)*6u) &&
       !(g.open&flag))
      hit(p,BeatRole::CHH,st,vel(dynamics,54));
    if(busy>=70u&&(loc==3u||loc==11u)&&
       structure.chance((busy-60u)*8u))
      hit(p,BeatRole::Snare,st,vel(dynamics,41)); // controlled ghost
   }
  }
  // Use the crash as an accent, not as an unavoidable sound on EVERY loop.
  if(busy>=40u&&(s.style==BeatStyle::Rock||
                 s.style==BeatStyle::Metal||
                 s.style==BeatStyle::IndustrialNDH||
                 s.style==BeatStyle::DrumAndBass))
   hit(p,BeatRole::Crash,0,vel(dynamics,105));
  return p;
 }
 static PatternSteps variate(const PatternSteps& a,
                             const BeatCreatorSettings& s,
                             std::uint32_t seed) noexcept {
  PatternSteps b=a;
  if(!valid(s))return b;
  Rng rng(seed^0x56415231u);
  const std::size_t last=static_cast<std::size_t>(s.steps)-1u;
  // A variation changes a small phrase ending while retaining the entire
  // core rhythmic identity. The target voice is genre-specific; unlike
  // the old implementation it does not always toggle the last-step kick.
  BeatRole role=BeatRole::BD;
  switch(s.style) {
   case BeatStyle::IndustrialNDH:role=BeatRole::BD;break;
   case BeatStyle::EBM:role=BeatRole::CHH;break;
   case BeatStyle::Techno:role=BeatRole::Clap;break;
   case BeatStyle::House:role=BeatRole::Clap;break;
   case BeatStyle::Rock:role=BeatRole::BD;break;
   case BeatStyle::Metal:role=BeatRole::BD;break;
   case BeatStyle::HipHop:role=BeatRole::CHH;break;
   case BeatStyle::DrumAndBass:role=BeatRole::BD;break;
   case BeatStyle::Count:return b;
  }
  const std::size_t target=role==BeatRole::CHH?
     last-2u*(rng.next()%2u):last-(rng.next()%3u); // hats: 13 or 15
  auto& note=b[idx(role,target)];
  note=note?0u:vel(rng,role==BeatRole::CHH?63:role==BeatRole::Clap?86:89);
  // A restrained second ornament at higher complexities makes the phrase
  // less mechanical without replacing the main motif or backbeat.
  if(s.complexity>=55u){
   const auto extra=last-2u; // step 13: avoid the open hat at 14
   if(extra!=target&&rng.chance(450u))
    hit(b,BeatRole::CHH,extra,vel(rng,56));
  }
  return b;
 }
 static PatternSteps makeBreak(const PatternSteps& a,
                               const BeatCreatorSettings& s,
                               std::uint32_t seed) noexcept {
  PatternSteps c=a;
  if(!valid(s))return c;
  Rng rng(seed^0x4252454Bu);
  const std::size_t start=static_cast<std::size_t>(s.steps)-8u;
  const unsigned ending=rng.next()%3u;
  const unsigned busy=static_cast<unsigned>(s.complexity);
  // BC39: An effective turnaround develops over the LAST HALF BAR.
  // Retain all prior material and the beat-four snare at start+4; do
  // not simply pile four extra notes onto the original final beat.
  const auto erase=[&](BeatRole role,std::size_t pos) noexcept {
   c[idx(role,start+pos)]=0u;
  };
  const auto add=[&](BeatRole role,std::size_t pos,int velocity) noexcept {
   hit(c,role,start+pos,vel(rng,velocity));
  };
  // Give the transition an audible gap and avoid a long cymbal washing
  // over the next downbeat. The first half of the measure is untouched.
  for(std::size_t pos=4u;pos<8u;++pos){
   erase(BeatRole::CHH,pos);
   erase(BeatRole::OHH,pos);
  }
  switch(s.style){
   case BeatStyle::Rock:
   case BeatStyle::Metal: {
    // Descending tom accents rather than 16th-note random snare hits.
    // Remove kicks competing with the fill while keeping the backbeat.
    for(const auto pos:{2u,3u,6u,7u})erase(BeatRole::BD,pos);
    if(ending==0u){
     add(BeatRole::HighTom,2u,88);
     add(BeatRole::LowTom,6u,95);
     if(busy>=25u)add(BeatRole::LowTom,7u,107);
    }else if(ending==1u){
     add(BeatRole::HighTom,3u,84);
     if(busy>=25u)add(BeatRole::Snare,5u,78);
     add(BeatRole::LowTom,7u,110);
    }else{
     add(BeatRole::HighTom,2u,88);
     if(busy>=40u)add(BeatRole::HighTom,5u,93);
     add(BeatRole::LowTom,7u,111);
    }
    if(s.style==BeatStyle::Metal&&busy>=55u){
     add(BeatRole::BD,5u,93);
     if(ending==2u)add(BeatRole::BD,6u,89);
    }
    break;
   }
   case BeatStyle::DrumAndBass: {
    // Deliberate displaced ghost snare and a closing syncopation.
    // A breakbeat pickup leads to the next measure, not a tom roll.
    if(ending==0u){
     add(BeatRole::Snare,2u,55);
     add(BeatRole::Snare,6u,90);
     add(BeatRole::BD,7u,104);
    }else if(ending==1u){
     add(BeatRole::Snare,3u,53);
     add(BeatRole::BD,5u,87);
     add(BeatRole::Snare,7u,104);
    }else{
     add(BeatRole::Snare,2u,54);
     add(BeatRole::BD,5u,88);
     add(BeatRole::LowTom,7u,98);
    }
    if(busy>=60u)add(BeatRole::Snare,6u,65);
    break;
   }
   case BeatStyle::HipHop: {
    // Restrained: one subtle early pickup, and a single ending.
    // Keep space between notes; no automatic four-snare barrage.
    add(BeatRole::CHH,3u,48);
    if(ending==0u)add(BeatRole::Clap,7u,84);
    else if(ending==1u)add(BeatRole::Snare,7u,78);
    else add(BeatRole::BD,7u,90);
    if(busy>=65u&&ending==2u)add(BeatRole::Snare,6u,58);
    break;
   }
   case BeatStyle::IndustrialNDH:
   case BeatStyle::EBM: {
    // Mechanical early pickup, then a short, decisive final accent.
    add(BeatRole::Snare,2u,74);
    if(ending==0u){
     if(busy>=25u)add(BeatRole::Snare,6u,92);
     add(BeatRole::Clap,7u,106);
    }else if(ending==1u){
     if(busy>=25u)add(BeatRole::LowTom,5u,87);
     add(BeatRole::LowTom,7u,108);
    }else{
     if(busy>=25u)add(BeatRole::Clap,5u,86);
     add(BeatRole::BD,7u,106);
    }
    if(busy>=70u)add(BeatRole::Snare,6u,80);
    break;
   }
   case BeatStyle::Techno:
   case BeatStyle::House: {
    // Build and release across half a bar. The four-on-floor skeleton
    // stays in place, but the last few hats leave room for the pickup.
    add(BeatRole::Snare,2u,72);
    if(ending==0u){
     if(busy>=25u)add(BeatRole::Snare,5u,80);
     add(BeatRole::Clap,7u,102);
    }else if(ending==1u){
     add(BeatRole::Clap,3u,83);
     add(BeatRole::Snare,7u,105);
    }else{
     erase(BeatRole::CHH,3u);
     add(BeatRole::OHH,3u,88);
     add(BeatRole::Clap,6u,102);
     // Leave the final 16th open: a breath before the next downbeat.
     erase(BeatRole::BD,7u);
     erase(BeatRole::Snare,7u);
     erase(BeatRole::Clap,7u);
    }
    if(busy>=65u&&ending!=2u){
     add(BeatRole::Snare,6u,76);
    }
    break;
   }
   case BeatStyle::Count:break;
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
