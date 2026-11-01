// 125A Drumator v1.5: offline listening proof, not a VST3 GUI feature.
// All WAV loops pass through Drumator's existing OfflineWavExport (actual
// StepScheduler, PreparedSample, SampleVoiceEngine and peak limiter).
#include "core/BeatCreatorFactoryKit.h"
#include "core/OfflineWavExport.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace drumator;
int main(int argc,char** argv){
 try {
  if(argc!=2){std::cerr<<"Usage: beat_creator_preview_pack <output-directory>\n";return 2;}
  const std::filesystem::path root(argv[1]);
  const auto voices=root/"Factory Kit";
  const auto loops=root/"Grooves - A B C";
  std::filesystem::create_directories(voices);
  std::filesystem::create_directories(loops);
  KitState kit;
  if(!BeatCreatorFactoryKit::makeKit(kit)){
   std::cerr<<"original factory-kit synthesis failed\n";return 3;
  }
  for(std::size_t r=0;r<8;++r){
   std::ofstream out(voices/BeatCreatorFactoryKit::filenames[r],std::ios::binary);
   const auto& data=kit.wav[r];
   out.write(reinterpret_cast<const char*>(data.data()),
             static_cast<std::streamsize>(data.size()));
   if(!out){std::cerr<<"Cannot write source WAV "<<r<<"\n";return 4;}
  }
  struct Demo { const char* name; double bpm; };
  constexpr std::array<Demo,8> demos{{
   {"01_Industrial_NDH",124.0},{"02_EBM",126.0},
   {"03_Techno",132.0},{"04_House",124.0},
   {"05_Rock",110.0},{"06_Metal",160.0},
   {"07_Hip_Hop",92.0},{"08_Drum_and_Bass",174.0}
  }};
  for(std::size_t style=0;style<demos.size();++style){
   const BeatCreatorSettings settings{static_cast<BeatStyle>(style),55,16};
   const auto preview=BeatCreator::preview(settings,0x125A2026u+
      static_cast<std::uint32_t>(style)*97u);
   if(!preview.valid)return 5;
   for(std::size_t version=0;version<3;++version){
    if(!BeatCreatorFactoryKit::setDraft(kit,preview,version))return 6;
    const std::string filename=std::string(demos[style].name)+
        (version==0?"_A_Generate.wav":version==1?
         "_B_Variate.wav":"_C_Break.wav");
    if(!OfflineWavExport::render(kit,false,demos[style].bpm,loops/filename)){
     std::cerr<<"Cannot render "<<filename<<"\n";return 7;
    }
    std::cout<<filename<<" "<<demos[style].bpm<<" BPM\n";
   }
  }
  std::ofstream info(root/"START-HERE.txt");
  info<<"125A Drumator 1.5 BEAT CREATOR - DEVELOPMENT LISTENING PACK\n"
      <<"Not a released plugin or premium sample library.\n\n"
      <<"Factory Kit: eight ORIGINAL synthesized 44.1kHz/16bit mono WAVs.\n"
      <<"Grooves: all eight styles, each A/Generate, B/Variate, C/Break.\n"
      <<"Groove previews use the existing Drumator audio renderer at 48kHz/24bit stereo.\n"
      <<"Use a WAV player or import the individual loops into Studio One.\n"
      <<"The groove tempo is in the filename list written by the preview generator logs.\n"
      <<"Nothing here loads into or replaces any user's Drumator samples or project.\n"
      <<"GUI and live VST3 PREVIEW are NOT implemented yet.\n";
  if(!info)return 8;
  std::cout<<"Beat Creator source kit (8 WAVs) and A/B/C style previews (24 WAVs) generated\n";
  return 0;
 }catch(const std::exception& e){
  std::cerr<<"Audition packaging failed: "<<e.what()<<"\n";return 9;
 }
}
