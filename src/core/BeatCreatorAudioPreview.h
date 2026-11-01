#pragma once
// Ephemeral A/B/C audition. No project, sample or pattern mutation.
// Drumator's OfflineWavExport/StepScheduler/SampleVoiceEngine supply the sound.
// A PCM16 container permits playback by the existing real-time audition voice.
#include "BeatCreatorFactoryKit.h"
#include "OfflineWavExport.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

namespace drumator {
class BeatCreatorAudioPreview final {
public:
 static bool encode(const BeatCreatorPreview& draft,std::size_t choice,double bpm,
                    std::vector<std::uint8_t>& wav16,
                    BeatStyle style=BeatStyle::Count) noexcept {
  if(!draft.valid||choice>=3||(draft.steps!=16&&draft.steps!=32)||
     !std::isfinite(bpm)||bpm<20.||bpm>300.)return false;
  try{
   KitState isolated;
   if(!(style==BeatStyle::Count?
        BeatCreatorFactoryKit::makeKit(isolated):
        BeatCreatorFactoryKit::makeGenreKit(isolated,style))||
      !BeatCreatorFactoryKit::setDraft(isolated,draft,choice))return false;
   std::vector<std::uint8_t> wav24;
   if(!OfflineWavExport::renderMemory(isolated,false,bpm,wav24)||
      wav24.size()<44u||(wav24.size()-44u)%6u!=0u)return false;
   const auto frames=(wav24.size()-44u)/6u;
   if(!frames||frames>48000u*12u)return false;
   std::vector<std::uint8_t> out(44u+frames*4u,0);
   const auto put=[&](std::size_t at,std::uint32_t v,unsigned count){
    for(unsigned i=0;i<count;++i)
     out[at+i]=static_cast<std::uint8_t>(v>>(8u*i));
   };
   std::memcpy(out.data(),"RIFF",4);
   put(4,static_cast<std::uint32_t>(out.size()-8u),4);
   std::memcpy(out.data()+8,"WAVEfmt ",8);
   put(16,16,4);put(20,1,2);put(22,2,2);
   put(24,48000,4);put(28,48000u*4u,4);
   put(32,4,2);put(34,16,2);
   std::memcpy(out.data()+36,"data",4);
   put(40,static_cast<std::uint32_t>(frames*4u),4);
   for(std::size_t i=0;i<frames*2u;++i){
    out[44u+i*2u]=wav24[44u+i*3u+1u];
    out[44u+i*2u+1u]=wav24[44u+i*3u+2u];
   }
   wav16=std::move(out);
   return true;
  }catch(...){return false;}
 }
};
} // namespace drumator
