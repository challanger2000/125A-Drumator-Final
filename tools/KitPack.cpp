// 125A Drumator .125akit authoring helper. Not linked into the VST3.
// Invocation: drumator_kit_pack INPUT_WAV_FOLDER OUTPUT.125akit
// File names: 01_Kick.wav ... 16_AnyInstrument.wav
#include "core/KitPresetCatalog.h"
#include "core/Utf8Path.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace drumator;
int main(int argc,char** argv){
 if(argc!=3){
  std::cerr<<"Usage: drumator_kit_pack WAV_FOLDER OUTPUT.125akit\n"
              "Number filenames 01_ through 16_. Missing rows remain SILENT.\n";
  return 2;
 }
 try{
  const auto folder=pathFromUtf8(argv[1]),output=pathFromUtf8(argv[2]);
  if(!std::filesystem::is_directory(folder)||output.extension()!=".125akit"){
   std::cerr<<"Folder missing or output is not .125akit\n";return 3;
  }
  KitState kit;kit.visible=8;kit.grid.fill(0);kit.patternLengths.fill(16);
  std::array<bool,kRows> loaded{};
  std::size_t total=0;
  for(const auto& entry:std::filesystem::directory_iterator(folder)){
   if(!entry.is_regular_file())continue;
   const auto name=pathToUtf8(entry.path().filename());
   if(name.size()<7||!std::isdigit(static_cast<unsigned char>(name[0]))||
      !std::isdigit(static_cast<unsigned char>(name[1]))||name[2]!='_')
     continue;
   auto suffix=pathToUtf8(entry.path().extension());
   std::transform(suffix.begin(),suffix.end(),suffix.begin(),
       [](unsigned char c){return static_cast<char>(std::tolower(c));});
   if(suffix!=".wav")continue;
   const auto number=(name[0]-'0')*10+(name[1]-'0');
   if(number<1||number>static_cast<int>(kRows)){std::cerr<<"Invalid row: "<<name<<"\n";return 4;}
   const auto row=static_cast<std::size_t>(number-1);
   if(loaded[row]){std::cerr<<"Duplicate row: "<<number<<"\n";return 5;}
   const auto size=entry.file_size();
   if(size<44||size>kMaxSingleWav||size>kMaxKitWavs-total){
     std::cerr<<"WAV too large/small: "<<name<<"\n";return 6;
   }
   std::vector<std::uint8_t> wav(static_cast<std::size_t>(size));
   std::ifstream input(entry.path(),std::ios::binary);
   if(!input||!input.read(reinterpret_cast<char*>(wav.data()),
                          static_cast<std::streamsize>(size))){
      std::cerr<<"WAV read failed: "<<name<<"\n";return 7;
   }
   DecodedWav decode;
   if(WavPcm16Decoder::decode(wav.data(),wav.size(),decode)!=WavError::Ok){
      std::cerr<<"Unsupported WAV: "<<name<<"\n";return 8;
   }
   kit.names[row]=name;
   auto label=name.substr(3,name.size()-3-4);
   if(label.size()>kMaxRowLabel)label.resize(kMaxRowLabel);
   if(!validRowLabel(label)){std::cerr<<"Invalid UTF-8 label: "<<name<<"\n";return 9;}
   kit.displayNames[row]=label;
   kit.wav[row]=std::move(wav);
   total+=static_cast<std::size_t>(size);
   loaded[row]=true;
  }
  const auto n=std::count(loaded.begin(),loaded.end(),true);
  if(!n||!KitCodec::valid(kit,true)||!KitPresetCatalog::save(output,kit)){
   std::cerr<<"No valid drumkit written\n";return 10;
  }
  std::cout<<"PASS: "<<n<<" real recordings packaged in "<<pathToUtf8(output)
           <<" (missing instruments silent)\n";
  return 0;
 }catch(const std::exception& e){
  std::cerr<<"Kit generation failed: "<<e.what()<<"\n";return 11;
 }
}
