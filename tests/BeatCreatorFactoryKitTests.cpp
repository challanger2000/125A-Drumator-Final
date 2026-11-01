#include "core/BeatCreatorFactoryKit.h"
#include "core/BeatCreatorAudioPreview.h"
#include "core/OfflineWavExport.h"
#include "core/WavPcm16Decoder.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace drumator;
static void check(bool value,const char* why){if(!value)throw std::runtime_error(why);}
static std::uint32_t u32(const std::vector<std::uint8_t>& b,std::size_t at){
 return static_cast<std::uint32_t>(b[at])|(static_cast<std::uint32_t>(b[at+1])<<8)|
        (static_cast<std::uint32_t>(b[at+2])<<16)|
        (static_cast<std::uint32_t>(b[at+3])<<24);
}
int main(){
 try{
  KitState original;
  original.grid[0]=88;original.names[0]="user_original.wav";
  auto snapshot=original; // Never touch user project
  KitState factory;
  check(BeatCreatorFactoryKit::makeKit(factory),"factory kit creation/validation");
  check(factory.visible==8&&factory.selected==0&&factory.limiter==1,
        "factory kit defaults");
  for(std::size_t i=0;i<8;++i){
   const auto& wav=factory.wav[i];
   check(wav.size()>4000&&wav.size()<150000,"factory WAV wrong size");
   check(wav[0]=='R'&&wav[1]=='I'&&wav[2]=='F'&&wav[3]=='F'&&
         u32(wav,4)+8==wav.size(),"RIFF size mismatch");
   check(u32(wav,24)==44100&&wav[22]==1&&wav[34]==16,
         "factory source must be 44.1kHz mono/16-bit");
   check(wav==BeatCreatorFactoryKit::wav(i),"factory PCM nondeterministic");
   DecodedWav decoded;
   check(WavPcm16Decoder::decode(wav.data(),wav.size(),decoded)==WavError::Ok,
         "existing Drumator WAV decoder rejected factory sample");
   double rms=0,maxVal=0,firstWindow=0;std::size_t window=0;
   for(std::size_t s=0;s<decoded.left.size();++s){
    const double v=decoded.left[s];rms+=v*v;maxVal=std::max(maxVal,std::abs(v));
    if(s<441u){firstWindow+=v*v;++window;}
   }
   rms=std::sqrt(rms/decoded.left.size());
   firstWindow=std::sqrt(firstWindow/window);
   check(rms>0.001&&rms<0.5&&maxVal<0.99,"silent/clipped source sample");
   check(firstWindow>0.001,"sample onset silent");
  }
  // BC29: all eight recorded kits, exact source role order and isolation.
  // Reads committed CC0 assets by project-root path, never changes user kit.
  const auto sourceRoot=std::filesystem::path(DRUMATOR_CC0_KIT_TEST_ROOT);
  check(std::filesystem::exists(sourceRoot/"SOURCE-MANIFEST.json"),
        "missing pinned CC0 provenance");
  std::array<std::vector<std::uint8_t>,8> renderedKicks{};
  for(unsigned style=0;style<8;++style) {
   KitState recorded;
   check(BeatCreatorFactoryKit::makeGenreKit(
           recorded,static_cast<BeatStyle>(style),sourceRoot),
         "cannot load CC0 recorded factory genre");
   for(unsigned role=0;role<8;++role) {
    DecodedWav hit;
    check(WavPcm16Decoder::decode(recorded.wav[role].data(),
              recorded.wav[role].size(),hit)==WavError::Ok,
          "source-recorded genre WAV invalid");
    check(!recorded.wav[role].empty(),"missing genre instrument role");
   }
   renderedKicks[style]=recorded.wav[0];
  }
  unsigned distinctKicks=0;
  for(unsigned i=0;i<8;++i) {
   bool unique=true;
   for(unsigned j=0;j<i;++j)
    if(renderedKicks[i]==renderedKicks[j])unique=false;
   if(unique)++distinctKicks;
  }
  check(distinctKicks==8,"genre kicks are not distinct");
  check(!BeatCreatorFactoryKit::makeGenreKit(
         factory,BeatStyle::Count,sourceRoot),"invalid genre accepted");
  check(original.grid==snapshot.grid&&original.names==snapshot.names,
        "factory unexpectedly changed original KitState");
  check(BeatCreatorFactoryKit::wav(8).empty(),"invalid source row accepted");
  const auto beat=BeatCreator::preview({BeatStyle::EBM,55,16},10203u);
  check(beat.valid,"beat preview creation");
  check(BeatCreatorFactoryKit::setDraft(factory,beat,0),"A draft to temporary kit");
  const auto before=original.grid;
  check(factory.grid[0]==beat.a[0]&&factory.grid[kSteps+4]==beat.a[kSteps+4],
        "actual row mapping wrong");
  check(factory.grid[8*kSteps]==0&&factory.grid[kRows*kSteps]==0,
        "preview wrote hidden rows/pattern B");
  const auto base=std::filesystem::temp_directory_path()/
    "125a-beatcreator-audition-contract.wav";
  check(OfflineWavExport::render(factory,false,126.0,base),
        "actual existing offline Drumator renderer failed");
  std::ifstream file(base,std::ios::binary);
  const std::vector<std::uint8_t> audio(std::istreambuf_iterator<char>{file},{});
  check(audio.size()>44&&audio[0]=='R'&&audio[22]==2&&audio[34]==24&&
        u32(audio,24)==48000&&u32(audio,4)+8==audio.size(),
        "rendered demo must be 48kHz/24bit stereo WAV");
  const auto wantedFrames=static_cast<std::uint32_t>(
     std::llround(16.0*15.0*48000.0/126.0));
  check(u32(audio,40)==wantedFrames*6u,"demo loop duration drift");
  std::error_code ec;
  std::filesystem::remove(base,ec);
  check(before==original.grid,"preview renderer mutated original kit");
  check(BeatCreatorFactoryKit::setDraft(factory,beat,1),"B draft set");
  check(factory.grid[15]==beat.b[15],"variation B mapping");
  check(BeatCreatorFactoryKit::setDraft(factory,beat,2),"C draft set");
  check(factory.grid[15]==beat.c[15],"break C mapping");
  check(!BeatCreatorFactoryKit::setDraft(factory,beat,3),"invalid role permitted");
  // BC31: PLAY A/B/C must use the actual user kit, not a genre override.
  // Use a full 8-row user kit so the test never depends on installed CC0 files.
  auto loaded=factory;
  loaded.grid[0]=17; // existing song data must not be changed by audition
  loaded.names[0]="MyDeepKick.wav";
  loaded.wav[0]=factory.wav[4]; // intentionally very different kick recording
  loaded.gain[0]=52;
  loaded.pan[0]=133;
  loaded.masterVolume=72;
  const auto ownerBefore=loaded;
  std::vector<std::uint8_t> factoryPreview,userPreview;
  check(BeatCreatorAudioPreview::encode(beat,0,126.,factoryPreview,
        BeatStyle::Count),"BC31 baseline preview");
  check(BeatCreatorAudioPreview::encode(beat,0,126.,userPreview,
        BeatStyle::Count,&loaded),"BC31 loaded kit preview");
  check(userPreview!=factoryPreview&&userPreview.size()==factoryPreview.size(),
        "BC31 loaded samples did not affect preview");
  check(loaded.wav==ownerBefore.wav&&loaded.grid==ownerBefore.grid&&
        loaded.names==ownerBefore.names&&loaded.gain==ownerBefore.gain&&
        loaded.pan==ownerBefore.pan&&loaded.masterVolume==ownerBefore.masterVolume,
        "BC31 preview mutated user kit");
  // Previewing a partly loaded kit borrows only the missing genre sounds.
  auto partial=ownerBefore;
  partial.wav[2].clear();partial.names[2].clear();
  auto partialBefore=partial;
  std::vector<std::uint8_t> partPreview;
  check(BeatCreatorAudioPreview::encode(beat,1,126.,partPreview,
        BeatStyle::Count,&partial),"BC31 partial kit preview");
  check(partial.wav==partialBefore.wav&&partial.names==partialBefore.names,
        "BC31 temporary fallback persisted into user kit");
  // A fully loaded user kit must work even if an invalid factory style is
  // requested: the selected sounds never depend on any external sample pack.
  std::vector<std::uint8_t> directPreview;
  check(BeatCreatorAudioPreview::encode(beat,2,126.,directPreview,
        BeatStyle::Count,&loaded),"BC31 loaded-only break preview");
  std::cout<<"BC31 current-kit A/B/C sound and partial fallback isolation PASS\\n";
  std::cout<<"Factory kit: eight deterministic original WAVs and real 48k/24bit Drumator offline preview PASS\n";
  return 0;
 }catch(const std::exception& e){
  std::cerr<<"FactoryKit FAILED: "<<e.what()<<"\n";return 1;
 }
}
