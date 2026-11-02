#pragma once
// Independent file/PCM oracle: no StepScheduler in the expectation path.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>
namespace export_timing {
inline void require(bool ok,const char* what){if(!ok)throw std::runtime_error(what);}
inline std::uint32_t le32(const std::vector<std::uint8_t>& b,std::size_t p){
 require(p+4<=b.size(),"truncated LE32");
 return std::uint32_t(b[p])|(std::uint32_t(b[p+1])<<8)|
        (std::uint32_t(b[p+2])<<16)|(std::uint32_t(b[p+3])<<24);
}
inline std::vector<std::uint8_t> read(const std::filesystem::path& p){
 std::ifstream f(p,std::ios::binary);
 require(f.good(),"export file missing");
 return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
struct Wav {
 std::vector<std::uint8_t> bytes;
 std::size_t data=0,frames=0;
 explicit Wav(const std::filesystem::path& p):bytes(read(p)){
  require(bytes.size()>=44&&std::memcmp(bytes.data(),"RIFF",4)==0&&
    std::memcmp(bytes.data()+8,"WAVE",4)==0&&
    le32(bytes,4)+8u==bytes.size(),"invalid WAV RIFF size/signature");
  bool format=false;
  for(std::size_t at=12;at+8<=bytes.size();){
   const auto size=static_cast<std::size_t>(le32(bytes,at+4));
   require(size<=bytes.size()-at-8,"truncated RIFF chunk");
   if(std::memcmp(bytes.data()+at,"fmt ",4)==0){
    require(size>=16&&bytes[at+8]==1&&bytes[at+9]==0&&
      bytes[at+10]==2&&bytes[at+11]==0&&le32(bytes,at+12)==48000&&
      le32(bytes,at+16)==288000&&bytes[at+20]==6&&bytes[at+21]==0&&
      bytes[at+22]==24&&bytes[at+23]==0,"WAV is not 48k/24-bit/stereo PCM");
    format=true;
   }else if(std::memcmp(bytes.data()+at,"data",4)==0){
    require(data==0&&size%6==0,"invalid duplicate/nonintegral PCM chunk");
    data=at+8;frames=size/6;
   }
   at+=8+size+(size&1u);
  }
  require(format&&data!=0&&frames>0,"missing WAV fmt/data");
 }
 float sample(std::size_t f,unsigned channel=0)const{
  require(f<frames&&channel<2,"PCM oracle index out of range");
  const auto p=data+f*6+channel*3;
  std::int32_t v=bytes[p]|(std::int32_t(bytes[p+1])<<8)|
      (std::int32_t(bytes[p+2])<<16);
  if(v&0x800000)v-=0x1000000;
  return static_cast<float>(v/8388608.);
 }
};
inline std::vector<std::uint8_t> impulse(unsigned row=0){
 std::vector<std::uint8_t> b(52,0);
 auto put=[&](std::size_t at,std::uint32_t v,unsigned n){
  for(unsigned i=0;i<n;++i)b[at+i]=static_cast<std::uint8_t>(v>>(8*i));
 };
 std::memcpy(b.data(),"RIFF",4);put(4,44,4);
 std::memcpy(b.data()+8,"WAVEfmt ",8);put(16,16,4);put(20,3,2);
 put(22,2,2);put(24,48000,4);put(28,384000,4);put(32,8,2);put(34,32,2);
 std::memcpy(b.data()+36,"data",4);put(40,8,4);
 const float l=.01f*static_cast<float>(row+1),r=-l*.5f;
 std::memcpy(b.data()+44,&l,4);std::memcpy(b.data()+48,&r,4);
 return b;
}
inline std::size_t frames(unsigned steps,double bpm){
 return static_cast<std::size_t>(std::llround(steps*15.*48000./bpm));
}
inline std::size_t onset(unsigned step,double bpm){
 return static_cast<std::size_t>(std::ceil(step*15.*48000./bpm-1e-7));
}
// Return actual note onsets and conductor BPM; parse tracks independently.
struct Midi {
 struct Note{std::uint32_t tick;unsigned row,velocity;};
 std::vector<Note> notes;
 std::uint32_t micros=0;
 explicit Midi(const std::vector<std::uint8_t>& b){
  require(b.size()>=14&&std::memcmp(b.data(),"MThd",4)==0&&
    b[8]==0&&b[9]==1&&b[10]==0&&b[11]==17&&b[12]==3&&b[13]==192,
    "invalid type-1 / 17-track / 960-PPQ MIDI");
  const auto big=[&](std::size_t p){require(p+4<=b.size(),"truncated BE32");
   return (std::uint32_t(b[p])<<24)|(std::uint32_t(b[p+1])<<16)|
      (std::uint32_t(b[p+2])<<8)|b[p+3];};
  std::size_t p=14;
  for(unsigned track=0;track<17;++track){
   require(p+8<=b.size()&&std::memcmp(b.data()+p,"MTrk",4)==0,"missing MIDI track");
   const auto end=p+8+big(p+4);p+=8;require(end<=b.size(),"truncated MIDI track");
   auto vlq=[&](){std::uint32_t v=0;
    for(unsigned n=0;n<4;++n){require(p<end,"truncated VLQ");const auto x=b[p++];
     v=(v<<7)|(x&127u);if(!(x&128u))return v;}
    throw std::runtime_error("oversized VLQ");};
   std::uint32_t tick=0;bool ended=false;
   while(p<end){tick+=vlq();require(p<end,"missing MIDI status");const auto status=b[p++];
    if(status==255){require(p<end,"missing meta type");const auto type=b[p++];const auto len=vlq();
     require(len<=end-p,"truncated MIDI meta");
     if(type==81){require(len==3,"invalid tempo meta");micros=(std::uint32_t(b[p])<<16)|(std::uint32_t(b[p+1])<<8)|b[p+2];}
     if(type==47){require(len==0,"invalid EOT");ended=true;}
     p+=len;
    }else{require((status==0x99||status==0x89)&&p+2<=end,"unexpected MIDI event");
     const unsigned pitch=b[p++],velocity=b[p++];require(pitch>=36&&pitch<52,"invalid row note");
     if(status==0x99)notes.push_back({tick,pitch-36,velocity});}
   }
   require(ended,"missing MIDI EOT");
  }
  require(p==b.size()&&micros>0,"invalid MIDI end/tempo");
 }
};
}
