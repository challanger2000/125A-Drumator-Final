#include "core/BeatCreatorAudioPreview.h"
#include "core/BeatCreatorDraftRenderer.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace drumator;
static void ck(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
static std::vector<std::uint8_t> testWav(){
 const std::uint32_t count=4410;
 std::vector<std::uint8_t> w(44u+count*2u);
 auto put=[&](std::size_t at,std::uint32_t v,int count){
  for(int i=0;i<count;++i)w[at+i]=static_cast<std::uint8_t>(v>>(8*i));
 };
 std::copy_n("RIFF",4,w.begin());put(4,static_cast<std::uint32_t>(w.size()-8),4);
 std::copy_n("WAVEfmt ",8,w.begin()+8);
 put(16,16,4);put(20,1,2);put(22,1,2);
 put(24,44100,4);put(28,88200,4);put(32,2,2);put(34,16,2);
 std::copy_n("data",4,w.begin()+36);put(40,count*2,4);
 for(std::uint32_t i=0;i<count;++i){
  const auto bits=static_cast<std::uint16_t>(i%200<100?9000:-9000);
  w[44+2*i]=static_cast<std::uint8_t>(bits);
  w[45+2*i]=static_cast<std::uint8_t>(bits>>8);
 }
 return w;
}
int main(){
 try{
  const auto beats=BeatCreator::preview({BeatStyle::EBM,55,16},112233u);
  ck(beats.valid,"generate rhythm");
  KitState empty;
  const auto before=empty.grid;
  for(std::size_t i=0;i<3;++i){
   KitState staged=empty;
   ck(BeatCreatorDraftRenderer::setDraft(staged,beats,i),"stage A B C");
   std::vector<std::uint8_t> blank;
   ck(BeatCreatorAudioPreview::encode(beats,i,124.,blank,empty),
      "empty kit preview");
   ck(blank.size()>44&&std::all_of(blank.begin()+44,blank.end(),
      [](std::uint8_t b){return b==0;}),
      "EMPTY DRUMSET SHOULD BE SILENT");
  }
  ck(empty.grid==before,"preview modified project patterns");
  KitState loaded;
  loaded.wav[0]=testWav();loaded.names[0]="UserKick.wav";
  std::vector<std::uint8_t> audio;
  ck(BeatCreatorAudioPreview::encode(beats,0,124.,audio,loaded),
     "loaded drumset preview");
  ck(std::any_of(audio.begin()+44,audio.end(),
     [](std::uint8_t b){return b!=0;}),"loaded kit produced silence");
  loaded.wav[0].clear();loaded.names[0].clear();
  std::vector<std::uint8_t> blank;
  ck(BeatCreatorAudioPreview::encode(beats,0,124.,blank,loaded),
     "empty row preview");
  ck(std::all_of(blank.begin()+44,blank.end(),
     [](std::uint8_t b){return b==0;}),
     "unloaded sample row played a factory fallback");
  std::cout<<"BC36 audio preview obeys loaded kit and is silent for empty rows PASS\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BC36 FAIL: "<<e.what()<<"\n";return 1;}
}
