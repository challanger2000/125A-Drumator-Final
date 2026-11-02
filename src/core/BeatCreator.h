// 125A Drumator Beat Creator V2 – electronic-only rhythm engine.
// Deliberately independent of sample loading, kits, VST3 clocks and audio export.
// Notes represent eight conventional slot roles, never particular WAV files.
#pragma once
#include "PatternOperations.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace drumator {
enum class BeatStyle : std::uint8_t {
 Synthpop80s, ItaloDisco, Synthwave, EBM, Eurodance90s, House, Techno, Trance, HipHop, DrumAndBass, Count
};
enum class BeatRole : std::size_t {
 BD=0, Snare=1, CHH=2, OHH=3, LowTom=4, HighTom=5, Crash=6, Clap=7
};
struct BeatCreatorSettings {
 BeatStyle style{BeatStyle::Synthpop80s};
 std::uint8_t complexity{35};
 std::uint8_t steps{16}; // one/two bars, 16th-note grid
};
struct BeatCreatorPreview {
 PatternSteps a{},b{},c{};
 std::uint8_t steps{0};
 bool valid{false};
};
class BeatCreator final {
 static constexpr std::uint16_t bits(const char* p) noexcept {
  std::uint16_t mask=0;unsigned n=0;bool reading=false;
  for(;*p;++p){
   if(*p>='0'&&*p<='9'){n=n*10u+static_cast<unsigned>(*p-'0');reading=true;}
   else if(reading){if(n<16u)mask|=static_cast<std::uint16_t>(1u<<n);n=0;reading=false;}
  }
  if(reading&&n<16u)mask|=static_cast<std::uint16_t>(1u<<n);
  return mask;
 }
 struct Motif {std::uint16_t kick,snare,hat,open,clap;};
 static inline const std::array<std::array<Motif,8>,10> motifs_{{
  {{ // 80s SYNTHPOP: eight authored rhythmic archetypes
   Motif{bits("0 8"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 1
   Motif{bits("0 6 8"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 2
   Motif{bits("0 8 11"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 3
   Motif{bits("0 7 8 14"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 4
   Motif{bits("0 8 10"), bits("4 12"), bits("0 2 4 6 8 10 12"), bits("14"), bits("")}, // 5
   Motif{bits("0 8"), bits("4 12"), bits("0 4 8 12"), bits("6 14"), bits("")}, // 6
   Motif{bits("0 3 8 11"), bits("4 12"), bits("0 2 6 8 10 14"), bits(""), bits("12")}, // 7
   Motif{bits("0 8 15"), bits("4 12"), bits("0 2 4 8 10 12 14"), bits("6"), bits("")}, // 8
  }},
  {{ // ITALO DISCO: eight authored rhythmic archetypes
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 3 4 7 8 11 12 15"), bits("6 14"), bits("4 12")}, // 1
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 3 4 7 8 11 12 15"), bits("2 6 10 14"), bits("4 12")}, // 2
   Motif{bits("0 4 8 12 15"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("6 14"), bits("4 12")}, // 3
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 8 10 12"), bits("6 14"), bits("4 12 15")}, // 4
   Motif{bits("0 4 8 12 14"), bits("4 12"), bits("0 2 4 6 8 10 12"), bits("14"), bits("4 12")}, // 5
   Motif{bits("0 4 7 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("4 12")}, // 6
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("6 14"), bits("12")}, // 7
   Motif{bits("0 4 8 11 12"), bits("4 12"), bits("0 3 4 7 8 11 12 15"), bits("2 6 10 14"), bits("4 12")}, // 8
  }},
  {{ // SYNTHWAVE: eight authored rhythmic archetypes
   Motif{bits("0 8"), bits("4 12"), bits("0 4 8 12"), bits("14"), bits("")}, // 1
   Motif{bits("0 8 10"), bits("4 12"), bits("0 2 4 8 10 12 14"), bits(""), bits("")}, // 2
   Motif{bits("0 7 8"), bits("4 12"), bits("0 4 8 12"), bits("6 14"), bits("")}, // 3
   Motif{bits("0 8 11"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 4
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 4 8 12"), bits("14"), bits("")}, // 5
   Motif{bits("0 8 14"), bits("4 12"), bits("0 2 4 6 8 10 12"), bits("14"), bits("")}, // 6
   Motif{bits("0 8"), bits("4 12"), bits("0 4 6 8 12 14"), bits(""), bits("12")}, // 7
   Motif{bits("0 3 8"), bits("4 12"), bits("0 4 8 10 12 14"), bits("6"), bits("")}, // 8
  }},
  {{ // EBM: eight authored rhythmic archetypes
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 1
   Motif{bits("0 4 7 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 2
   Motif{bits("0 3 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 3
   Motif{bits("0 4 8 11 12"), bits("4 12"), bits("0 3 4 7 8 11 12 15"), bits(""), bits("")}, // 4
   Motif{bits("0 4 8 10 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 5
   Motif{bits("0 4 8 12 15"), bits("4 12"), bits("0 4 6 8 10 12 14"), bits(""), bits("")}, // 6
   Motif{bits("0 4 6 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 7
   Motif{bits("0 4 8 12 14"), bits("4 12"), bits("0 2 4 7 8 10 12 15"), bits(""), bits("")}, // 8
  }},
  {{ // 90s EURODANCE: eight authored rhythmic archetypes
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("4 12")}, // 1
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 3 4 6 8 10 11 12 14 15"), bits(""), bits("4 12")}, // 2
   Motif{bits("0 4 8 12 15"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("4 12")}, // 3
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 7 8 10 12 14 15"), bits(""), bits("4 12")}, // 4
   Motif{bits("0 4 8 12"), bits("4 12 15"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("4 12")}, // 5
   Motif{bits("0 4 8 12 14"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("4 12")}, // 6
   Motif{bits("0 4 8 11 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("4 12")}, // 7
   Motif{bits("0 4 7 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("14"), bits("4 12")}, // 8
  }},
  {{ // HOUSE: eight authored rhythmic archetypes
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 4 8 12"), bits("2 6 10 14"), bits("4 12")}, // 1
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 3 4 7 8 11 12 15"), bits("2 6 10 14"), bits("4 12")}, // 2
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 4 7 8 12 15"), bits("2 6 10 14"), bits("4 12")}, // 3
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 4 8 11 12 15"), bits("2 6 10 14"), bits("4 12")}, // 4
   Motif{bits("0 4 8 12 15"), bits("4 12"), bits("0 4 8 12"), bits("2 6 10 14"), bits("4 12")}, // 5
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 3 4 8 11 12"), bits("2 6 10 14"), bits("4 12")}, // 6
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 4 7 8 10 12 15"), bits("2 6 14"), bits("4 12")}, // 7
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 4 8 12 15"), bits("2 6 10 14"), bits("4 12 15")}, // 8
  }},
  {{ // TECHNO: eight authored rhythmic archetypes
   Motif{bits("0 4 8 12"), bits("12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 1
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 3 4 7 8 11 12 15"), bits(""), bits("")}, // 2
   Motif{bits("0 4 8 12"), bits("12"), bits("0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"), bits(""), bits("")}, // 3
   Motif{bits("0 4 8 12 15"), bits("12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 4
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 3 4 6 7 8 10 11 12 14 15"), bits(""), bits("")}, // 5
   Motif{bits("0 4 8 12"), bits("12"), bits("0 4 6 8 10 12 14"), bits("2"), bits("")}, // 6
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("14"), bits("")}, // 7
   Motif{bits("0 4 8 11 12"), bits("12"), bits("0 3 4 7 8 11 12 15"), bits(""), bits("")}, // 8
  }},
  {{ // TRANCE: eight authored rhythmic archetypes
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("6 14"), bits("")}, // 1
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 3 4 6 8 10 11 12 14"), bits("6 14"), bits("")}, // 2
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("2 10 14"), bits("")}, // 3
   Motif{bits("0 4 8 12 15"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("6 14"), bits("")}, // 4
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 3 4 6 7 8 10 11 12 14 15"), bits("6 14"), bits("")}, // 5
   Motif{bits("0 4 8 12"), bits("4 12 15"), bits("0 2 4 6 8 10 12 14"), bits("6 14"), bits("")}, // 6
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits("6 10 14"), bits("")}, // 7
   Motif{bits("0 4 8 12"), bits("4 12"), bits("0 2 4 5 6 8 10 12 13 14"), bits("6 14"), bits("")}, // 8
  }},
  {{ // HIP-HOP: eight authored rhythmic archetypes
   Motif{bits("0 7 10"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 1
   Motif{bits("0 6 10"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 2
   Motif{bits("0 3 8 10"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 3
   Motif{bits("0 8 11 15"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 4
   Motif{bits("0 7 10 14"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 5
   Motif{bits("0 6 8 11"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 6
   Motif{bits("0 3 10 15"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 7
   Motif{bits("0 8 10 14"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 8
  }},
  {{ // DRUM & BASS: eight authored rhythmic archetypes
   Motif{bits("0 10"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 1
   Motif{bits("0 7 10"), bits("4 12"), bits("0 2 4 6 8 10 12 14 15"), bits(""), bits("")}, // 2
   Motif{bits("0 6 10 15"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 3
   Motif{bits("0 8 10"), bits("4 12"), bits("0 2 3 4 6 8 10 11 12 14"), bits(""), bits("")}, // 4
   Motif{bits("0 3 10"), bits("4 12"), bits("0 2 4 6 8 10 12 14 15"), bits(""), bits("")}, // 5
   Motif{bits("0 6 10 14"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 6
   Motif{bits("0 10 15"), bits("4 12"), bits("0 2 4 6 8 10 12 13 14"), bits(""), bits("")}, // 7
   Motif{bits("0 7 11"), bits("4 12"), bits("0 2 4 6 8 10 12 14"), bits(""), bits("")}, // 8
  }}
 }};
 struct Rng {
  std::uint32_t state;
  explicit Rng(std::uint32_t seed) noexcept:state(seed?seed:0x125AD00Du){}
  unsigned next() noexcept {
   auto v=state;v^=v<<13;v^=v>>17;v^=v<<5;state=v;return v;
  }
  int deviation(int maximum) noexcept {
   return static_cast<int>(next()%static_cast<unsigned>(2*maximum+1))-maximum;
  }
 };
 static constexpr std::size_t at(BeatRole r,std::size_t s) noexcept {
  return static_cast<std::size_t>(r)*kSteps+s;
 }
 static void set(PatternSteps& p,BeatRole role,std::size_t s,int velocity) noexcept {
  if(s>=kSteps)return;
  p[at(role,s)]=static_cast<std::uint8_t>(std::clamp(velocity,1,127));
 }
 static int voiceVelocity(Rng& r,BeatRole role,unsigned local) noexcept {
  int base=90;
  switch(role){
   case BeatRole::BD:base=local%4u==0u?113:93;break;
   case BeatRole::Snare:base=local==4u||local==12u?112:64;break;
   case BeatRole::CHH:base=local%4u==0u?80:67;break;
   case BeatRole::OHH:base=88;break;
   case BeatRole::Clap:base=99;break;
   case BeatRole::Crash:base=106;break;
   case BeatRole::LowTom:base=86;break;
   case BeatRole::HighTom:base=80;break;
  }
  return std::clamp(base+r.deviation(4),1,127);
 }
 static bool fourOnFloor(BeatStyle s) noexcept {
  return s==BeatStyle::ItaloDisco||s==BeatStyle::EBM||
         s==BeatStyle::Eurodance90s||s==BeatStyle::House||
         s==BeatStyle::Techno||s==BeatStyle::Trance;
 }
 static bool roleAt(const PatternSteps& p,BeatRole role,std::size_t step) noexcept {
  return p[at(role,step)]!=0;
 }
public:
 static constexpr std::array<const char*,10> styleNames{{
  "80s SYNTHPOP", "ITALO DISCO", "SYNTHWAVE", "EBM", "90s EURODANCE", "HOUSE", "TECHNO", "TRANCE", "HIP-HOP", "DRUM & BASS"
 }};
 static constexpr bool valid(const BeatCreatorSettings& s) noexcept {
  return static_cast<unsigned>(s.style)<10u&&s.complexity<=100u&&
         (s.steps==16u||s.steps==32u);
 }
 static PatternSteps generate(const BeatCreatorSettings& settings,std::uint32_t seed) noexcept {
  PatternSteps result{};
  if(!valid(settings))return result;
  Rng rng(seed^0x57a3bf21u);
  const auto complexity=static_cast<unsigned>(settings.complexity);
  const auto style=static_cast<unsigned>(settings.style);
  const unsigned first=seed%8u; // eight nonrepeating authored motifs on successive GENERATE clicks
  for(std::size_t bar=0;bar<settings.steps/16u;++bar) {
   const auto choice=bar? (first+1u+(seed/8u)%3u)%8u:first;
   const Motif motif=motifs_[style][choice];
   for(unsigned local=0;local<16u;++local) {
    const auto flag=static_cast<std::uint16_t>(1u<<local);
    const std::size_t step=bar*16u+local;
    // 0% Complexity: skeleton intact; extra kick syncopations and busy hats are suppressed.
    if(motif.kick&flag) {
     const bool core=(fourOnFloor(settings.style)&&local%4u==0u)||
                     local==0u||local==8u||local==10u;
     if(complexity>=15u||core)
      set(result,BeatRole::BD,step,voiceVelocity(rng,BeatRole::BD,local));
    }
    if(motif.snare&flag) {
     if(complexity>=25u||local==4u||local==12u)
      set(result,BeatRole::Snare,step,voiceVelocity(rng,BeatRole::Snare,local));
    }
    if((motif.clap&flag)&&complexity>=20u)
     set(result,BeatRole::Clap,step,voiceVelocity(rng,BeatRole::Clap,local));
    if((motif.open&flag)&&complexity>=30u)
     set(result,BeatRole::OHH,step,voiceVelocity(rng,BeatRole::OHH,local));
    else if((motif.hat&flag)&&(complexity>=25u||local%4u==0u))
     set(result,BeatRole::CHH,step,voiceVelocity(rng,BeatRole::CHH,local));
    if(complexity>=65u&&local==15u&&
       (settings.style==BeatStyle::HipHop||settings.style==BeatStyle::DrumAndBass))
     set(result,BeatRole::Snare,step,44+rng.deviation(3));
    if(complexity>=80u&&(local==3u||local==11u)&&
       (settings.style==BeatStyle::Eurodance90s||settings.style==BeatStyle::Trance))
     set(result,BeatRole::Snare,step,50+rng.deviation(4));
    if(complexity>=85u&&(local==5u||local==13u)&&
       !roleAt(result,BeatRole::OHH,step))
     set(result,BeatRole::CHH,step,53+rng.deviation(3));
   }
  }
  return result;
 }
 // BC44: B must be an audible, style-specific groove variation,
 // not a single final-step toggle. Keep the established quarter-note kicks
 // and main 2/4 backbeats while distributing embellishments across each bar.
 static PatternSteps variate(const PatternSteps& source,const BeatCreatorSettings& settings,
                             std::uint32_t seed) noexcept {
  PatternSteps b=source;
  if(!valid(settings))return b;
  Rng rng(seed^0x42716aa1u);
  struct Accent {BeatRole role; unsigned step;};
  // The first three edits occur at the factory 35% Complexity. Offbeat
  // drums, early hats and a different secondary voice ensure audible
  // variation well before the end of each bar; higher values add details.
  static constexpr std::array<std::array<Accent,5>,10> plans{{
   {{{BeatRole::BD,7},{BeatRole::CHH,3},{BeatRole::HighTom,10},
     {BeatRole::CHH,13},{BeatRole::Clap,15}}}, // 80s SYNTHPOP
   {{{BeatRole::CHH,1},{BeatRole::BD,15},{BeatRole::Clap,10},
     {BeatRole::CHH,13},{BeatRole::LowTom,11}}}, // ITALO DISCO
   {{{BeatRole::BD,7},{BeatRole::CHH,3},{BeatRole::LowTom,10},
     {BeatRole::CHH,13},{BeatRole::HighTom,15}}}, // SYNTHWAVE
   {{{BeatRole::BD,11},{BeatRole::CHH,3},{BeatRole::Snare,10},
     {BeatRole::CHH,13},{BeatRole::LowTom,15}}}, // EBM
   {{{BeatRole::BD,15},{BeatRole::CHH,3},{BeatRole::Clap,10},
     {BeatRole::CHH,13},{BeatRole::Snare,11}}}, // 90s EURODANCE
   {{{BeatRole::CHH,3},{BeatRole::BD,15},{BeatRole::Snare,10},
     {BeatRole::CHH,13},{BeatRole::Clap,11}}}, // HOUSE
   {{{BeatRole::CHH,3},{BeatRole::BD,15},{BeatRole::Snare,10},
     {BeatRole::CHH,13},{BeatRole::LowTom,11}}}, // TECHNO
   {{{BeatRole::CHH,3},{BeatRole::BD,15},{BeatRole::Snare,10},
     {BeatRole::CHH,13},{BeatRole::Clap,11}}}, // TRANCE
   {{{BeatRole::BD,6},{BeatRole::CHH,3},{BeatRole::Snare,11},
     {BeatRole::CHH,13},{BeatRole::LowTom,15}}}, // HIP-HOP
   {{{BeatRole::BD,7},{BeatRole::CHH,3},{BeatRole::Snare,11},
     {BeatRole::CHH,13},{BeatRole::HighTom,15}}}  // DRUM & BASS
  }};
  const auto& accents=plans[static_cast<unsigned>(settings.style)];
  for(std::size_t bar=0;bar<settings.steps;bar+=16u){
   for(std::size_t i=0;i<accents.size();++i){
    if((i==1&&settings.complexity<20u)||
       (i==2&&settings.complexity<30u)||
       (i==3&&settings.complexity<65u)||
       (i==4&&settings.complexity<85u))continue;
    const auto role=accents[i].role;
    const auto step=bar+accents[i].step;
    // Never let the two hi-hat lanes fire together.
    if(role==BeatRole::CHH&&roleAt(b,BeatRole::OHH,step)){
     b[at(BeatRole::OHH,step)]=0;
    }
    auto& note=b[at(role,step)];
    const int strength=role==BeatRole::BD?94:
                       role==BeatRole::Snare?64:
                       role==BeatRole::CHH?69:
                       role==BeatRole::Clap?77:82;
    note=note?0u:static_cast<std::uint8_t>(strength+rng.deviation(4));
   }
  }
  return b;
 }
 static PatternSteps makeBreak(const PatternSteps& a,const BeatCreatorSettings& settings,
                               std::uint32_t seed) noexcept {
  PatternSteps c=a;
  if(!valid(settings))return c;
  Rng r(seed^0x71ba02dfu);
  const std::size_t start=static_cast<std::size_t>(settings.steps)-8u;
  const unsigned ending=seed%3u;
  const unsigned complexity=settings.complexity;
  const auto put=[&](BeatRole role,unsigned pos,int v){
   const auto step=start+pos;
   // A note may not be simultaneously an open and a closed hi-hat.
   if(role==BeatRole::OHH)c[at(BeatRole::CHH,step)]=0;
   if(role==BeatRole::CHH)c[at(BeatRole::OHH,step)]=0;
   set(c,role,step,v+r.deviation(3));
  };
  const auto erase=[&](BeatRole role,unsigned pos){c[at(role,start+pos)]=0;};
  // A / B / C differ only in the final half-bar: a musically meaningful turnaround.
  for(unsigned pos=4;pos<8;++pos){erase(BeatRole::CHH,pos);erase(BeatRole::OHH,pos);}
  switch(settings.style){
   case BeatStyle::Synthpop80s:
     put(BeatRole::HighTom,2,80);
     if(complexity>=25u)put(BeatRole::LowTom,5,89);
     if(ending==0u)put(BeatRole::LowTom,7,100);
     else if(ending==1u)put(BeatRole::Clap,7,92);
     else put(BeatRole::Snare,7,91);
     break;
   case BeatStyle::ItaloDisco:
     put(BeatRole::CHH,3,70);
     if(complexity>=25u)put(BeatRole::Clap,5,84);
     if(ending==0u)put(BeatRole::Clap,7,103);
     else if(ending==1u)put(BeatRole::Snare,7,99);
     else put(BeatRole::OHH,7,96);
     break;
   case BeatStyle::Synthwave:
     put(BeatRole::HighTom,2,80);
     if(complexity>=35u)put(BeatRole::LowTom,5,94);
     if(ending==0u)put(BeatRole::LowTom,7,107);
     else if(ending==1u)put(BeatRole::Snare,7,100);
     else {erase(BeatRole::BD,7);put(BeatRole::Crash,7,94);}
     break;
   case BeatStyle::EBM:
     put(BeatRole::BD,3,91);
     if(complexity>=35u)put(BeatRole::Snare,5,76);
     if(ending==0u)put(BeatRole::Clap,7,105);
     else if(ending==1u)put(BeatRole::BD,7,108);
     else put(BeatRole::Snare,7,106);
     break;
   case BeatStyle::Eurodance90s:
     put(BeatRole::Clap,3,79);
     if(complexity>=25u)put(BeatRole::Snare,5,82);
     if(ending==0u){put(BeatRole::Snare,6,87);put(BeatRole::Snare,7,110);}
     else if(ending==1u){put(BeatRole::Clap,6,90);put(BeatRole::Snare,7,110);}
     else {put(BeatRole::Snare,7,109);put(BeatRole::Crash,7,95);}
     break;
   case BeatStyle::House:
     put(BeatRole::CHH,3,64);
     if(complexity>=35u)put(BeatRole::Clap,5,83);
     if(ending==0u)put(BeatRole::Clap,7,101);
     else if(ending==1u)put(BeatRole::Snare,7,95);
     else {erase(BeatRole::BD,7);put(BeatRole::OHH,7,89);}
     break;
   case BeatStyle::Techno:
     put(BeatRole::CHH,3,78);
     if(complexity>=35u)put(BeatRole::CHH,5,87);
     if(ending==0u)put(BeatRole::Snare,7,99);
     else if(ending==1u)put(BeatRole::CHH,7,103);
     else put(BeatRole::BD,7,99);
     break;
   case BeatStyle::Trance:
     put(BeatRole::Snare,3,70);
     if(complexity>=25u)put(BeatRole::Snare,5,82);
     if(ending==0u){put(BeatRole::Snare,6,94);put(BeatRole::Snare,7,109);}
     else if(ending==1u){put(BeatRole::Clap,6,88);put(BeatRole::Snare,7,105);}
     else {put(BeatRole::Snare,7,107);put(BeatRole::Crash,7,102);}
     break;
   case BeatStyle::HipHop:
     put(BeatRole::Snare,2,46);
     if(complexity>=35u)put(BeatRole::CHH,3,52);
     if(ending==0u)put(BeatRole::BD,7,95);
     else if(ending==1u)put(BeatRole::Snare,7,77);
     else put(BeatRole::Clap,7,82);
     break;
   case BeatStyle::DrumAndBass:
     put(BeatRole::Snare,2,54);
     if(complexity>=35u)put(BeatRole::Snare,5,67);
     if(ending==0u){put(BeatRole::Snare,6,87);put(BeatRole::BD,7,99);}
     else if(ending==1u){put(BeatRole::BD,5,88);put(BeatRole::Snare,7,108);}
     else {put(BeatRole::Snare,6,78);put(BeatRole::LowTom,7,97);}
     break;
   case BeatStyle::Count:break;
  }
  return c;
 }
 static BeatCreatorPreview preview(const BeatCreatorSettings& s,std::uint32_t seed) noexcept {
  BeatCreatorPreview p{};
  if(!valid(s))return p;
  p.steps=s.steps;p.valid=true;
  p.a=generate(s,seed);
  p.b=variate(p.a,s,seed^0xA17Bu);
  p.c=makeBreak(p.a,s,seed^0xC17Bu);
  return p;
 }
 static std::uint8_t occupiedMask(const std::array<PatternSteps,3>& existing) noexcept {
  std::uint8_t mask=0;
  for(unsigned i=0;i<3u;++i)if(!emptyPattern(existing[i]))mask|=static_cast<std::uint8_t>(1u<<i);
  return mask;
 }
};
} // namespace drumator
