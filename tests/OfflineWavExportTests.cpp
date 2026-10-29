#include "core/OfflineWavExport.h"
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>
#include <cmath>
using namespace drumator;
namespace {
void u16(std::vector<std::uint8_t>& b,std::size_t pos,std::uint16_t v){
 b[pos]=static_cast<std::uint8_t>(v);b[pos+1]=static_cast<std::uint8_t>(v>>8);
}
void u32(std::vector<std::uint8_t>& b,std::size_t pos,std::uint32_t v){
 for(int i=0;i<4;++i)b[pos+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(v>>(8*i));
}
std::uint32_t get32(const std::vector<std::uint8_t>& b,std::size_t p){
 return static_cast<std::uint32_t>(b[p])|(std::uint32_t(b[p+1])<<8)|
       (std::uint32_t(b[p+2])<<16)|(std::uint32_t(b[p+3])<<24);
}
std::vector<std::uint8_t> tone(std::size_t frames,float amplitude){
 std::vector<std::uint8_t> b(44+frames*2);
 const char* h="RIFF";for(int i=0;i<4;++i)b[static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(h[i]);
 u32(b,4,static_cast<std::uint32_t>(b.size()-8));
 for(int i=0;i<8;++i)b[8u+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>("WAVEfmt "[i]);
 u32(b,16,16);u16(b,20,1);u16(b,22,1);u32(b,24,48000);u32(b,28,96000);
 u16(b,32,2);u16(b,34,16);
 for(int i=0;i<4;++i)b[36u+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>("data"[i]);
 u32(b,40,static_cast<std::uint32_t>(frames*2));
 const auto value=static_cast<std::int16_t>(std::lround(amplitude*32767.f));
 for(std::size_t i=0;i<frames;++i){
   b[44+i*2]=static_cast<std::uint8_t>(value);
   b[44+i*2+1]=static_cast<std::uint8_t>(value>>8);
 }
 return b;
}
std::vector<std::uint8_t> read(const std::filesystem::path& p){
 std::ifstream file(p,std::ios::binary);
 return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(file),{});
}
float pcm24(const std::vector<std::uint8_t>& b,std::size_t pos){
 auto n=static_cast<std::int32_t>(b[pos])|
        (static_cast<std::int32_t>(b[pos+1])<<8)|
        (static_cast<std::int32_t>(b[pos+2])<<16);
 if(n&0x800000)n|=static_cast<std::int32_t>(0xff000000);
 return static_cast<float>(n/8388608.0);
}
}
int main(){
 int failures=0;
 auto check=[&](bool value,const char* name){if(!value){++failures;std::printf("FAIL: %s\n",name);}};
 const auto path=std::filesystem::temp_directory_path()/"125a-drumator-offline-test.wav";
 KitState kit;
 kit.patternLengths.fill(16);
 kit.grid[0]=127;
 kit.names[0]="kick.wav";
 kit.wav[0]=tone(256,0.6f);
 check(KitCodec::valid(kit,true),"valid kit");
 check(OfflineWavExport::render(kit,false,120.,path),"render pattern");
 auto bytes=read(path);
 check(bytes.size()>=44+6*16*6000,"nominal duration");
 if(bytes.size()>=50){
  check(get32(bytes,4)+8==bytes.size(),"RIFF size exact");
  check(get32(bytes,40)+44==bytes.size(),"PCM data chunk size exact");
  check(bytes[34]==24&&bytes[22]==2&&get32(bytes,24)==48000,"48kHz / 24bit stereo");
  check(std::abs(pcm24(bytes,44)-0.6f)<0.003f,"PCM audio at start");
  check(std::abs(pcm24(bytes,47)-0.6f)<0.003f,"stereo follows mono source");
 }
 kit.grid[0]=0;kit.grid[15]=127;
 kit.wav[0]=tone(10000,0.6f);
 check(OfflineWavExport::render(kit,false,120.,path),"render last-step tail");
 bytes=read(path);
 check(bytes.size()>44u+6u*16u*6000u,"tail not cut at final bar");
 kit.grid[15]=0;kit.grid[0]=127;
 kit.wav[0]=tone(128,0.8f);
 kit.wav[1]=tone(128,0.8f);kit.names[1]="snare.wav";
 kit.grid[32]=127;kit.limiter=1;
 check(OfflineWavExport::render(kit,false,120.,path),"render limiter ON");
 bytes=read(path);
 if(bytes.size()>=44+128*6){
   float peak=0.f;
   for(std::size_t i=44;i<44+128*6;i+=3)peak=std::max(peak,std::abs(pcm24(bytes,i)));
   check(peak<=StereoPeakLimiter::kCeiling+0.003f,"export obeys peak limiter ceiling");
 }
 const auto previous=bytes;
 check(!OfflineWavExport::render(kit,false,0.,path),"bad tempo rejected");
 check(read(path)==previous,"failed export preserves existing destination");
 std::error_code ec;
 check(!std::filesystem::exists(path.string()+".125a-export-part",ec),"no partial file");
 std::filesystem::remove(path,ec);
 std::printf("Offline WAV tests: %s\n",failures==0?"PASS":"FAIL");
 return failures?1:0;
}
