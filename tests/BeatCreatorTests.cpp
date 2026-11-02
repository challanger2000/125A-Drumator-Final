// BC40: source-only musical QA (no audio, sample loading, DAW or SDK dependency).
#include "core/BeatCreator.h"
#include "core/BeatCreatorTransaction.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

using namespace drumator;
static void ck(bool pass,const char* name){if(!pass)throw std::runtime_error(name);}
static std::uint8_t v(const PatternSteps& p,BeatRole r,std::size_t s){
 return p[static_cast<std::size_t>(r)*kSteps+s];
}
static unsigned count(const PatternSteps& p,std::size_t until){
 unsigned n=0;
 for(std::size_t r=0;r<8;++r)
  for(std::size_t s=0;s<until;++s)n+=p[r*kSteps+s]!=0u;
 return n;
}
static std::array<std::uint16_t,8> structure(const PatternSteps& p,std::size_t start=0u){
 std::array<std::uint16_t,8> masks{};
 for(std::size_t r=0;r<8;++r)
  for(std::size_t s=0;s<16u;++s)
   if(p[r*kSteps+start+s])masks[r]|=static_cast<std::uint16_t>(1u<<s);
 return masks;
}
int main(){
 try{
  ck(BeatCreator::styleNames.size()==10u,"not exactly ten approved electronic styles");
  ck(!BeatCreator::valid({BeatStyle::Count,35,16}),"invalid style accepted");
  ck(!BeatCreator::valid({BeatStyle::House,101,16}),"invalid complexity accepted");
  ck(!BeatCreator::valid({BeatStyle::House,35,24}),"invalid length accepted");
  ck(!BeatCreator::preview({BeatStyle::Count,35,16},5).valid,"invalid preview accepted");
  std::size_t audited=0;
  for(unsigned style=0;style<10u;++style){
   const auto s=static_cast<BeatStyle>(style);
   // Exactly eight different authored archetypes, judged by note placement,
   // not merely random velocity values, at the 35% factory complexity.
   std::array<std::array<std::uint16_t,8>,8> footprints{};
   for(unsigned seed=0;seed<8u;++seed){
    const auto p=BeatCreator::preview({s,35,16},seed);
    ck(p.valid,"base preview invalid");
    footprints[seed]=structure(p.a);
    for(unsigned older=0;older<seed;++older)
     ck(footprints[seed]!=footprints[older],"duplicate authored genre groove");
   }
   for(const std::uint8_t length:{std::uint8_t{16},std::uint8_t{32}}){
    for(const std::uint8_t complexity:{std::uint8_t{0},std::uint8_t{35},
                                        std::uint8_t{75},std::uint8_t{100}}){
     const BeatCreatorSettings settings{s,complexity,length};
     for(unsigned seed=0;seed<64u;++seed){
      const auto p=BeatCreator::preview(settings,seed);
      const auto repeated=BeatCreator::preview(settings,seed);
      ck(p.valid&&p.steps==length,"preview length invalid");
      ck(p.a==repeated.a&&p.b==repeated.b&&p.c==repeated.c,
         "non-deterministic seeded output");
      ck(p.a!=p.b&&p.a!=p.c,"A/B/C not musically distinct");
      for(const auto* pattern:{&p.a,&p.b,&p.c}){
       for(const auto vel:*pattern)ck(vel<=127u,"velocity over MIDI range");
       for(std::size_t row=8;row<kRows;++row)
        for(std::size_t pos=0;pos<kSteps;++pos)
         ck((*pattern)[row*kSteps+pos]==0u,"modified reserved user slots 9-16");
       for(std::size_t pos=length;pos<kSteps;++pos)
        for(std::size_t row=0;row<8;++row)
         ck((*pattern)[row*kSteps+pos]==0u,"modified disabled second bar");
       for(std::size_t pos=0;pos<length;++pos)
        ck(!(v(*pattern,BeatRole::CHH,pos)&&v(*pattern,BeatRole::OHH,pos)),
           "closed/open hi-hat note collision");
      }
      for(std::size_t bar=0;bar<length;bar+=16u){
       ck(v(p.a,BeatRole::BD,bar)>0u,"downbeat missing");
       ck(v(p.a,BeatRole::Snare,bar+12u)>0u,"backbeat missing");
       ck(v(p.b,BeatRole::Snare,bar+12u)>0u,"variation removed backbeat");
       ck(v(p.c,BeatRole::Snare,bar+12u)>0u,"break removed backbeat");
      }
      for(std::size_t row=0;row<kRows;++row)
       for(std::size_t pos=0;pos<length-8u;++pos)
        ck(p.a[row*kSteps+pos]==p.c[row*kSteps+pos],
           "break rewrote first part of rhythm");
      ck(count(p.a,length)>4u,"silent or unusable base rhythm");
      ++audited;
     }
    }
   }
   // Every two-bar phrase must develop, not repeat, bar one.
   for(unsigned seed=0;seed<32u;++seed) {
    const auto p=BeatCreator::preview({s,35,32},seed);
    ck(structure(p.a,0)!=structure(p.a,16),"two-bar pattern lacks development");
   }
   const auto minimal=BeatCreator::preview({s,0,16},0);
   const auto normal=BeatCreator::preview({s,35,16},0);
   ck(count(minimal.a,16)<=count(normal.a,16),"zero complexity increased density");
  }
  // All ten archetypes need a distinct default rhythm; sound presets are
  // irrelevant because their actual samples are deliberately unspecified.
  for(unsigned s=0;s<10u;++s)
   for(unsigned t=s+1u;t<10u;++t){
    const auto one=BeatCreator::preview({static_cast<BeatStyle>(s),35,16},0);
    const auto two=BeatCreator::preview({static_cast<BeatStyle>(t),35,16},0);
    ck(structure(one.a)!=structure(two.a),"two genres share identical factory groove");
   }
  // Neither PREVIEW nor APPLY may overwrite samples or user slots 9-16.
  PatternSteps original{};original[0]=100;original[8*kSteps+13]=92;
  original[9*kSteps+23]=83;original[0*kSteps+29]=97;
  const BeatTriple existing{original,original,original};
  const BeatLengths oldLengths{32,32,32};
  ck(BeatCreator::occupiedMask(existing)==7u,"overwritten pattern not detected");
  const auto preview=BeatCreator::preview({BeatStyle::ItaloDisco,35,16},17);
  const auto refused=BeatCreatorTransaction::prepare(preview,existing,oldLengths,false);
  ck(!refused.valid&&existing[0]==original,"overwrite without confirmation");
  const auto applied=BeatCreatorTransaction::prepare(preview,existing,oldLengths,true);
  ck(applied.valid&&applied.occupied==7u,"confirmed apply rejected");
  for(std::size_t pat=0;pat<3u;++pat){
   ck(applied.after[pat][8*kSteps+13]==92&&
      applied.after[pat][9*kSteps+23]==83&&
      applied.after[pat][0*kSteps+29]==97,
      "APPLY overwrote unassigned slots or hidden bar");
   ck(applied.newLengths[pat]==16u,"APPLY did not update visible pattern length");
  }
  std::printf("BC40 PASS: 10 electronic styles, 80 authored archetypes, %zu deterministic A/B/C QA cases, isolation and overwrite checks\\n",audited);
  return 0;
 }catch(const std::exception& e){
  std::fprintf(stderr,"BC40 FAIL: %s\\n",e.what());
  return 1;
 }
}
