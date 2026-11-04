#include "core/OfflineWavExport.h"
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>
#include <cmath>
#include <cstring>
#include <algorithm>
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
std::vector<std::uint8_t> tone(std::size_t frames,float amplitude,
                               std::uint32_t rate=48000){
 std::vector<std::uint8_t> b(44+frames*2);
 const char* h="RIFF";for(int i=0;i<4;++i)b[static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(h[i]);
 u32(b,4,static_cast<std::uint32_t>(b.size()-8));
 for(int i=0;i<8;++i)b[8u+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>("WAVEfmt "[i]);
 u32(b,16,16);u16(b,20,1);u16(b,22,1);u32(b,24,rate);u32(b,28,rate*2u);
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
// Signed-polarity stereo sine enables independent PCM, frequency and
// channel-orientation checks; constant samples cannot establish correct pitch.
std::vector<std::uint8_t> sineStereo(std::uint32_t rate,std::size_t frames,
                                     double hz=1000.){
 std::vector<std::uint8_t> b(44+frames*4u,0);
 std::memcpy(b.data(),"RIFF",4);u32(b,4,static_cast<std::uint32_t>(b.size()-8));
 std::memcpy(b.data()+8,"WAVEfmt ",8);
 u32(b,16,16);u16(b,20,1);u16(b,22,2);
 u32(b,24,rate);u32(b,28,rate*4u);u16(b,32,4);u16(b,34,16);
 std::memcpy(b.data()+36,"data",4);
 u32(b,40,static_cast<std::uint32_t>(frames*4u));
 for(std::size_t i=0;i<frames;++i){
   const auto phase=2.*3.141592653589793*hz*static_cast<double>(i)/rate;
   const auto value=std::sin(phase);
   const auto left=static_cast<std::int16_t>(std::lround(value*24000.));
   const auto right=static_cast<std::int16_t>(std::lround(value*-12000.));
   for(int ch=0;ch<2;++ch){
     const auto sample=ch==0?left:right;
     const auto at=44+4u*i+2u*static_cast<std::size_t>(ch);
     b[at]=static_cast<std::uint8_t>(sample);
     b[at+1]=static_cast<std::uint8_t>(sample>>8);
   }
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

void putPcm24(std::vector<std::uint8_t>& b,std::size_t pos,float f){
 const auto x=static_cast<std::int32_t>(std::lround(
     std::clamp(f,-.999999f,.999999f)*8388607.f));
 for(int c=0;c<3;++c)b[pos+static_cast<std::size_t>(c)]=
     static_cast<std::uint8_t>(x>>(8*c));
}
struct SineOracle {
 bool stereo{false},frequency{false},duration{false};
};
// Independent measurable facts: channel assignment, interpolated periods,
// sustained PCM just before the exact source endpoint, and silence after it.
// A file that merely contains the right RIFF header cannot satisfy these.
SineOracle inspectSine(const std::vector<std::uint8_t>& b,
                       std::uint32_t sourceRate,int semitones){
 SineOracle result{};
 if(b.size()!=44u+6u*96000u)return result;
 const double ratio=static_cast<double>(sourceRate)/48000.*
                    std::exp2(static_cast<double>(semitones)/12.);
 const auto expectedEnd=static_cast<std::size_t>(65536./ratio);
 const auto analysisEnd=std::min<std::size_t>(10000,expectedEnd>150?expectedEnd-100:0);
 const auto analysisStart=std::min<std::size_t>(200,analysisEnd/10);
 if(analysisEnd<=analysisStart+10)return result;
 double leftE=0.,rightE=0.,assignmentError=0.;
 double firstCross=-1.,lastCross=-1.;
 unsigned positiveCrossings=0;
 float previous=pcm24(b,44u+analysisStart*6u);
 for(std::size_t frame=analysisStart+1;frame<analysisEnd;++frame){
  const auto l=pcm24(b,44u+frame*6u);
  const auto r=pcm24(b,44u+frame*6u+3u);
  leftE+=static_cast<double>(l)*l;
  rightE+=static_cast<double>(r)*r;
  const auto residual=static_cast<double>(r)+.5*l;
  assignmentError+=residual*residual;
  if(previous<0.f&&l>=0.f&&l>previous){
   const double point=static_cast<double>(frame-1)+
       static_cast<double>(-previous)/(l-previous);
   if(firstCross<0.)firstCross=point;
   lastCross=point;++positiveCrossings;
  }
  previous=l;
 }
 // Opposite polarity alone is insufficient: -0.41*L was previously
 // accepted as -0.5*L (about 1.72dB right-channel loss).
 result.stereo=leftE>1.&&rightE>.2&&assignmentError/leftE<.0004;
 const double expectedHz=1000.*std::exp2(static_cast<double>(semitones)/12.);
 const double actualHz=positiveCrossings>=3&&lastCross>firstCross?
      (positiveCrossings-1)*48000./(lastCross-firstCross):0.;
 result.frequency=std::abs(actualHz-expectedHz)<=std::max(5.,expectedHz*.015);
 auto meanPower=[&](std::size_t from,std::size_t to){
  double sum=0.;
  for(std::size_t j=from;j<to;++j){
   const double l=pcm24(b,44u+j*6u);
   sum+=l*l;
  }
  return sum/static_cast<double>(to-from);
 };
 if(expectedEnd+500u<96000u&&expectedEnd>800u){
  // Sustained power must reach the LAST tenth of the predicted playback;
  // a voice cut at 1600/2048 frames may not pass on an earlier window.
  bool active=true;
  for(const double point:{.40,.70,.85,.95}){
   const auto end=static_cast<std::size_t>(expectedEnd*point);
   if(end<128) {active=false;break;}
   active=active&&meanPower(end-100u,end)>.001;
  }
  const auto after=meanPower(expectedEnd+200u,expectedEnd+450u);
  result.duration=active&&after<.000001;
 }else if(expectedEnd>=96000u){
  // Source exceeds fixed two-second export window. Check nearly the
  // entire exported duration, not a single 50k-frame midpoint.
  result.duration=meanPower(50000u,51000u)>.001&&
                  meanPower(75000u,76000u)>.001&&
                  meanPower(93000u,94000u)>.001&&
                  meanPower(95000u,95500u)>.001;
 }
 return result;
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
 check(bytes.size()==44u+6u*16u*6000u,
       "120 BPM 1 bar exactly 2.000 seconds");
 if(bytes.size()>=50){
  check(get32(bytes,4)+8==bytes.size(),"RIFF size exact");
  check(get32(bytes,40)+44==bytes.size(),"PCM data chunk size exact");
  check(bytes[34]==24&&bytes[22]==2&&get32(bytes,24)==48000,"48kHz / 24bit stereo");
  check(std::abs(pcm24(bytes,44)-0.6f)<0.003f,"PCM audio at start");
  check(std::abs(pcm24(bytes,47)-0.6f)<0.003f,"stereo follows mono source");
 }
 // Measure real PCM transients against Studio One's 120 BPM beat grid,
 // not only RIFF file length (a fast sequence could have a correct header).
 kit.grid.fill(0);
 for(const std::size_t step:{std::size_t{0},std::size_t{4},
                             std::size_t{8},std::size_t{12}})
   kit.grid[step]=127;
 kit.wav[0]=tone(256,0.6f);
 check(OfflineWavExport::render(kit,false,120.,path),
       "render four quarter-note beats at 120 BPM");
 bytes=read(path);
 if(bytes.size()==44u+6u*16u*6000u){
   for(const std::size_t beat:{std::size_t{0},std::size_t{1},
                               std::size_t{2},std::size_t{3}}){
     const auto onset=beat*24000u;
     check(std::abs(pcm24(bytes,44u+onset*6u)-0.6f)<0.003f,
           "120 BPM beat onset must hit exact 0.500s grid");
     if(beat>0)
       check(std::abs(pcm24(bytes,44u+(onset-1u)*6u))<0.0001f,
             "120 BPM beat must not begin early");
     check(std::abs(pcm24(bytes,44u+(onset+200u)*6u)-0.6f)<0.003f,
           "beat PCM playback duration must not be accelerated");
     check(std::abs(pcm24(bytes,44u+(onset+300u)*6u))<0.0001f,
           "one-shot must end before next quarter note");
   }
 }else check(false,"120 BPM four-beat RIFF must be exactly 2.000 seconds");
 // SW03: compare real exported PCM. 120BPM/48kHz eighth at step 2
 // must move 12000 -> 14000 frames at Swing 50%; step 4 stays at 24000.
 kit.grid.fill(0);
 kit.grid[2]=127;kit.grid[4]=127;
 kit.wav[0]=tone(256,0.6f);
 kit.humanize=0;kit.swing=0;
 check(OfflineWavExport::render(kit,false,120.,path),
       "SW03 straight eighth WAV export");
 const auto straightPcm=read(path);
 kit.swing=50;
 check(OfflineWavExport::render(kit,false,120.,path),
       "SW03 swung eighth WAV export");
 const auto swingPcm=read(path);
 if(straightPcm.size()==44u+6u*96000u&&swingPcm.size()==straightPcm.size()){
  const auto at=[](const std::vector<std::uint8_t>& b,std::size_t frame){
   return pcm24(b,44u+6u*frame);
  };
  check(std::abs(at(straightPcm,12000u)-.6f)<.003f&&
        std::abs(at(straightPcm,11999u))<.0001f,
        "SW03 straight eighth onset wrong");
  check(std::abs(at(swingPcm,12000u))<.0001f&&
        std::abs(at(swingPcm,13999u))<.0001f&&
        std::abs(at(swingPcm,14000u)-.6f)<.003f,
        "SW03 swung eighth onset not at 14000");
  check(std::abs(at(straightPcm,24000u)-.6f)<.003f&&
        std::abs(at(swingPcm,24000u)-.6f)<.003f,
        "SW03 quarter-note kick shifted by Swing");
 }else check(false,"SW03 WAV export size mismatch");
 kit.swing=0;
 kit.grid.fill(0);
 kit.grid[15]=127;
 kit.wav[0]=tone(10000,0.6f);
 check(OfflineWavExport::render(kit,false,120.,path),"render last-step tail");
 bytes=read(path);
 check(bytes.size()==44u+6u*16u*6000u,
       "last-step one-shot may not extend a 120 BPM bar");
 if(bytes.size()==44u+6u*16u*6000u){
   check(std::abs(pcm24(bytes,44u+6u*15u*6000u)-0.6f)<0.003f,
         "last-step onset at exactly 1.875 seconds");
   check(std::abs(pcm24(bytes,44u+6u*(15u*6000u-1u)))<0.0001f,
         "no early onset on last step");
   check(std::abs(pcm24(bytes,bytes.size()-6u))<0.003f,
         "end fade avoids abrupt sample discontinuity");
 }
 kit.patternLengths[0]=32;
 kit.grid[15]=0;kit.grid[31]=127;
 check(OfflineWavExport::render(kit,false,120.,path),"render 2-bar pattern");
 bytes=read(path);
 check(bytes.size()==44u+6u*32u*6000u,"120 BPM 2 bars 4 seconds");
 kit.patternLengths[0]=16;kit.grid[31]=0;kit.grid[15]=127;
 check(kit.chain.setCount(2),"two song entries");
 check(kit.chain.configure(0,0,16)&&kit.chain.configure(1,0,16),
       "two-bar song configuration");
 check(OfflineWavExport::render(kit,true,120.,path),"render 2-bar song");
 bytes=read(path);
 check(bytes.size()==44u+6u*32u*6000u,"song WAV exact two-bar grid");
 check(OfflineWavExport::render(kit,false,100.,path),"100 BPM pattern");
 bytes=read(path);
 check(bytes.size()==44u+6u*16u*7200u,"100 BPM exact 2.400 seconds");
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
 // NA05: real stereo sine, not only a file-size-only DC fixture.
 // Verify all five rates and the full +/-24 semitone range at 48 kHz output.
 for(const auto sourceRate:{44100u,48000u,96000u,192000u,384000u}){
  for(const int semitones:{-24,0,24}){
   KitState highRateKit{};
   highRateKit.visible=1;highRateKit.patternLengths.fill(16);
   highRateKit.grid.fill(0);highRateKit.grid[0]=127;
   highRateKit.names[0]="high-rate-stereo.wav";
   highRateKit.wav[0]=sineStereo(sourceRate,65536);
   highRateKit.pitch[0]=static_cast<std::uint8_t>(semitones+24);
   std::unique_ptr<PreparedSample> liveAsset;
   check(PreparedSample::fromWav(highRateKit.wav[0].data(),
             highRateKit.wav[0].size(),48000,static_cast<double>(semitones),
             1.f,0.f,0,false,liveAsset,SampleEdit{},true)==PrepareStatus::Ok&&liveAsset,
         "NA05 realtime sample-rate/pitch preparation");
   check(OfflineWavExport::render(highRateKit,false,120.,path),
         "NA05 offline sample-rate/pitch must match LIVE acceptance");
   const auto result=read(path);
   check(result.size()==44u+6u*16u*6000u,
         "NA05 exact WAV frame count at 120 BPM");
   if(result.size()!=44u+6u*16u*6000u)continue;
   const auto oracle=inspectSine(result,sourceRate,semitones);
   check(oracle.stereo,"NA05 true left/right assignment and gain ratio");
   check(oracle.frequency,"NA05 interpolated sine-period pitch");
   check(oracle.duration,"NA05 audible end-of-sample and silent post-tail");
   // Falsify the oracle with three independently controlled corruptions.
   if(sourceRate==48000u&&semitones==-24){
     auto swapped=result;
     for(std::size_t frame=0;frame<96000u;++frame)
       for(int i=0;i<3;++i)
         std::swap(swapped[44u+6u*frame+static_cast<std::size_t>(i)],
                   swapped[44u+6u*frame+3u+static_cast<std::size_t>(i)]);
     check(!inspectSine(swapped,sourceRate,semitones).stereo,
           "NA05 negative control must reject swapped stereo channels");
     auto wrongFrequency=result;
     for(std::size_t frame=0;frame<96000u;++frame){
       const float v=static_cast<float>(.3*std::sin(
                       2.*3.141592653589793*160.*frame/48000.));
       putPcm24(wrongFrequency,44u+6u*frame,v);
       putPcm24(wrongFrequency,44u+6u*frame+3u,-.5f*v);
     }
     check(!inspectSine(wrongFrequency,sourceRate,semitones).frequency,
           "NA05 negative control must reject 160Hz in place of 250Hz");
     auto lateTruncate=result;
     for(std::size_t frame=51001;frame<96000u;++frame)
       for(unsigned byte=0;byte<6;++byte)
         lateTruncate[44u+6u*frame+byte]=0;
     check(!inspectSine(lateTruncate,sourceRate,semitones).duration,
           "NA05 negative control must reject long source truncated at 51001");
   }
   if(sourceRate==384000u&&semitones==24){
     auto earlyEnd=result;
     for(std::size_t frame=1301u;frame<96000u;++frame)
       for(unsigned byte=0;byte<6;++byte)
         earlyEnd[44u+6u*frame+byte]=0;
     check(!inspectSine(earlyEnd,sourceRate,semitones).duration,
           "NA05 negative control must reject early sample truncation");
     auto lateEnd=result;
     for(std::size_t frame=1600u;frame<96000u;++frame)
       for(unsigned byte=0;byte<6;++byte)
         lateEnd[44u+6u*frame+byte]=0;
     check(!inspectSine(lateEnd,sourceRate,semitones).duration,
           "NA05 negative control must reject 1600/2048 partial tail");
     auto wrongGain=result;
     for(std::size_t frame=0;frame<96000u;++frame){
       const auto l=pcm24(wrongGain,44u+6u*frame);
       putPcm24(wrongGain,44u+6u*frame+3u,-.41f*l);
     }
     check(!inspectSine(wrongGain,sourceRate,semitones).stereo,
           "NA05 negative control must reject -0.41 vs -0.5 channel gain");
   }
  }
 }
 const auto previous=read(path);
 check(!OfflineWavExport::render(kit,false,0.,path),"bad tempo rejected");
 check(read(path)==previous,"failed export preserves existing destination");
 std::error_code ec;
 check(!std::filesystem::exists(path.string()+".125a-export-part",ec),"no partial file");
 std::filesystem::remove(path,ec);
 std::printf("Offline WAV tests: %s\n",failures==0?"PASS":"FAIL");
 return failures?1:0;
}
