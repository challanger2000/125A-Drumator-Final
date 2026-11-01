#include "core/BeatCreator.h"
#include "core/PatternOperations.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
using namespace drumator;
static void check(bool v,const char* why){if(!v)throw std::runtime_error(why);}
static std::uint8_t get(const PatternSteps& p,BeatRole role,std::size_t step){
 return p[static_cast<std::size_t>(role)*kSteps+step];
}
static int count(const PatternSteps& p,BeatRole role,std::size_t n){
 int found=0;for(std::size_t i=0;i<n;++i)found+=get(p,role,i)>0;return found;
}
// Musical structure signatures intentionally ignore velocities. Different
// random gains must NEVER be counted as an alternative groove.
static std::array<std::uint16_t,8> footprint(const PatternSteps& p,
                                              std::size_t first) {
 std::array<std::uint16_t,8> mask{};
 for(std::size_t role=0;role<8;++role)
  for(std::size_t step=0;step<16;++step)
   if(p[role*kSteps+first+step])
    mask[role]=static_cast<std::uint16_t>(mask[role]|(1u<<step));
 return mask;
}
int main(){
 try{
  check(BeatCreator::styleNames.size()==8,"expected eight meaningful styles");
  check(!BeatCreator::valid({BeatStyle::IndustrialNDH,35,0}),"zero length accepted");
  check(!BeatCreator::valid({BeatStyle::IndustrialNDH,35,24}),"bad length accepted");
  check(!BeatCreator::valid({BeatStyle::IndustrialNDH,101,16}),"bad complexity accepted");
  check(!BeatCreator::valid({BeatStyle::Count,35,16}),"bad style accepted");
  check(!BeatCreator::preview({BeatStyle::Count,35,16},1).valid,"bad preview accepted");
  for(unsigned kind=0;kind<8;++kind){
   for(const std::uint8_t length:{std::uint8_t{16},std::uint8_t{32}}){
    for(const std::uint8_t complexity:{std::uint8_t{0},std::uint8_t{35},
                                        std::uint8_t{75},std::uint8_t{100}}){
     BeatCreatorSettings s{static_cast<BeatStyle>(kind),complexity,length};
     for(unsigned seed=0;seed<40;++seed){
      const auto p=BeatCreator::preview(s,seed);
      check(p.valid&&p.steps==length,"valid preview rejected");
      const auto other=BeatCreator::preview(s,seed);
      check(p.a==other.a&&p.b==other.b&&p.c==other.c,
            "nondeterministic generation");
      check(p.a!=p.b,"variation must change the groove");
      check(p.a!=p.c,"break must change the groove");
      for(const auto* pat:{&p.a,&p.b,&p.c}) {
       for(auto v:*pat)check(v<=127,"invalid velocity");
       for(std::size_t r=8;r<kRows;++r)
        for(std::size_t step=0;step<kSteps;++step)
         check((*pat)[r*kSteps+step]==0,"creator touched user rows 9-16");
       for(std::size_t step=length;step<kSteps;++step)
        for(std::size_t r=0;r<8;++r)
         check((*pat)[r*kSteps+step]==0,"creator touched hidden second bar");
      }
      for(std::size_t step=0;step<length;step+=16){
       check(get(p.a,BeatRole::Snare,step+4)>0&&
             get(p.a,BeatRole::Snare,step+12)>0,"missing backbeat");
       check(get(p.b,BeatRole::Snare,step+4)>0&&
             get(p.b,BeatRole::Snare,step+12)>0,"variation lost backbeat");
       check(get(p.c,BeatRole::Snare,step+4)>0&&
             get(p.c,BeatRole::Snare,step+12)>0,"break lost backbeat");
       for(auto local:{0u,4u,8u,12u})
        check(get(p.a,BeatRole::BD,step+local)==get(p.b,BeatRole::BD,step+local),
              "variation changed kick anchors");
      }
      for(std::size_t step=0;step<length;++step)
       check(!(get(p.a,BeatRole::CHH,step)&&get(p.a,BeatRole::OHH,step)),
             "open+closed hats collide in main beat");
      const std::size_t breakStart=static_cast<std::size_t>(length)-4;
      for(std::size_t row=0;row<kRows;++row)
       for(std::size_t step=0;step<breakStart;++step)
        check(p.a[row*kSteps+step]==p.c[row*kSteps+step],
              "break modified original groove before transition");
      check(count(p.a,BeatRole::BD,length)>0&&
            count(p.a,BeatRole::Snare,length)>=2,"silence/weak main groove");
     }
    }
   }
  }
  // BC26: check MUSICAL VARIETY at the actual default 35% Complexity,
  // not just random velocity changes. All 8 genres need multiple distinctly
  // placed note arrangements, a second-bar development and varied endings.
  for(unsigned kind=0;kind<8;++kind) {
   const auto style=static_cast<BeatStyle>(kind);
   for(const std::uint8_t density:{std::uint8_t{35},std::uint8_t{75}}) {
    const BeatCreatorSettings shortSettings{style,density,16};
    std::array<std::array<std::uint16_t,8>,64> structures{};
    std::array<std::array<std::uint16_t,8>,64> endings{};
    unsigned sameAsPrevious=0u;
    for(unsigned seed=0;seed<structures.size();++seed) {
     const auto p=BeatCreator::preview(shortSettings,seed+0xA570u);
     structures[seed]=footprint(p.a,0);
     endings[seed]=footprint(p.c,0);
     if(seed&&structures[seed]==structures[seed-1u])++sameAsPrevious;
     for(std::size_t step=0;step<16;++step) {
      for(const auto* candidate:{&p.a,&p.b,&p.c})
       check(!(get(*candidate,BeatRole::CHH,step)&&
               get(*candidate,BeatRole::OHH,step)),
             "BC26 two simultaneous open and closed hi-hats");
     }
    }
    unsigned different=0u,differentEndings=0u;
    for(unsigned i=0;i<structures.size();++i){
     bool unique=true,uniqueBreak=true;
     for(unsigned j=0;j<i;++j){
      if(structures[i]==structures[j])unique=false;
      if(endings[i]==endings[j])uniqueBreak=false;
     }
     different+=unique?1u:0u;
     differentEndings+=uniqueBreak?1u:0u;
    }
    check(different>=6u,"BC26 genre lacks six distinct step arrangements");
    check(differentEndings>=3u,"BC26 repeated identical breaks");
    // The explicit no-repeat promise applies at the default 35%.
    // At 75%, intentionally added ghost notes may sometimes converge
    // two different motifs; keep a useful upper bound instead.
    check(density==35u?sameAsPrevious==0u:sameAsPrevious<40u,
          "BC26 repeated structures too frequently");
    unsigned differentiatedBars=0u;
    for(unsigned seed=0;seed<64u;++seed){
     const BeatCreatorSettings longSettings{style,density,32};
     const auto p=BeatCreator::preview(longSettings,seed+0xB570u);
     if(footprint(p.a,0)!=footprint(p.a,16))++differentiatedBars;
    }
    check(density==35u?differentiatedBars==64u:differentiatedBars>=48u,
          "BC26 two-bar phrases repeat excessively");
   }
  }
  // Isolation: generating a preview cannot modify an existing project.
  PatternSteps occupied{};occupied[0]=127;occupied[kSteps+31]=92;
  const std::array<PatternSteps,3> existing{occupied,{},occupied};
  const auto before=existing;
  check(BeatCreator::occupiedMask(existing)==5u,"occupied A/C not detected");
  auto candidate=BeatCreator::preview({},0x3456u);
  check(candidate.valid,"default preview invalid");
  check(existing==before,"preview modified user project");
  check(BeatCreator::occupiedMask(std::array<PatternSteps,3>{})==0u,"empty kit mask");
  PatternSteps onlyHidden{};onlyHidden[16]=96;
  check(BeatCreator::occupiedMask(std::array<PatternSteps,3>{PatternSteps{},onlyHidden,PatternSteps{}})==2u,
        "hidden second-bar value not protected");
  std::cout<<"Beat Creator preview: styles=8, lengths=16/32, 2560 legacy cases + BC26 style variety/two-bar phrases/break variants, A/B/C isolated\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BEAT CREATOR FAIL: "<<e.what()<<"\n";return 1;}
}
