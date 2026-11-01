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
  std::cout<<"Beat Creator preview: styles=8, lengths=16/32, 2560 seed/style/complexity cases, A/B/C, safely isolated\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BEAT CREATOR FAIL: "<<e.what()<<"\n";return 1;}
}
