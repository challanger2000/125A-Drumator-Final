#pragma once
// Standalone control-thread renderer using Drumator's ACTUAL StepScheduler,
// TriggerBatch, sample preparation and SampleVoiceEngine. No second synthesizer.
// No DAW transport required. 48 kHz, 24-bit stereo PCM RIFF/WAVE.
// Files are bar-accurate loop clips; final sample tails never extend their duration.
#include "KitState.h"
#include "MidiExport.h"
#include "SampleExchange.h"
#include "StereoPeakLimiter.h"
#include "ExportFile.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <limits>
#include <memory>
#include <vector>

namespace drumator {
class OfflineWavExport final {
 static constexpr std::uint32_t kRate=48000;
 static constexpr std::uint32_t kBlock=512;
 static constexpr std::uint32_t kChannels=2;
 static constexpr std::uint32_t kBytesPerSample=3;
 static constexpr std::uint64_t kMaxData=0xffffffffULL-36ULL;

 static void put16(std::array<std::uint8_t,44>& h,int n,std::uint16_t v){
   h[static_cast<std::size_t>(n)]=static_cast<std::uint8_t>(v);
   h[static_cast<std::size_t>(n+1)]=static_cast<std::uint8_t>(v>>8);
 }
 static void put32(std::array<std::uint8_t,44>& h,int n,std::uint32_t v){
   for(int j=0;j<4;++j)h[static_cast<std::size_t>(n+j)]=static_cast<std::uint8_t>(v>>(8*j));
 }
 static std::array<std::uint8_t,44> header(std::uint32_t dataSize){
   std::array<std::uint8_t,44> h{};
   std::memcpy(h.data(),"RIFF",4);put32(h,4,dataSize+36u);
   std::memcpy(h.data()+8,"WAVEfmt ",8);
   put32(h,16,16);put16(h,20,1);put16(h,22,kChannels);
   put32(h,24,kRate);put32(h,28,kRate*kChannels*kBytesPerSample);
   put16(h,32,kChannels*kBytesPerSample);put16(h,34,24);
   std::memcpy(h.data()+36,"data",4);put32(h,40,dataSize);
   return h;
 }
 static void pack(float v,std::uint8_t* at) noexcept {
   const double clamped=std::clamp(static_cast<double>(std::isfinite(v)?v:0.f),
                                   -1.0,1.0-1.0/8388608.0);
   const auto pcm=static_cast<std::int32_t>(std::llround(clamped*8388608.));
   at[0]=static_cast<std::uint8_t>(pcm);
   at[1]=static_cast<std::uint8_t>(pcm>>8);
   at[2]=static_cast<std::uint8_t>(pcm>>16);
 }
 static bool impl(const KitState& kit,bool exportSong,double bpm,
                  const std::filesystem::path& destination) {
   if(!std::isfinite(bpm)||bpm<20.||bpm>300.||
      !KitCodec::valid(kit,true))return false;
   MidiExportConfig sequence{};
   sequence.grid=kit.grid;
   sequence.patternLengths=kit.patternLengths;
   sequence.randomVelocity=kit.randomVelocity;
   sequence.selectedPattern=kit.selected;
   sequence.song=kit.chain;
   sequence.exportSong=exportSong;
   sequence.swingPercent=kit.swing;
   sequence.humanizePercent=kit.humanize;
   const auto totalSteps=static_cast<std::uint64_t>(
       exportSong?kit.chain.totalSteps():kit.patternLengths[kit.selected]);
   if(totalSteps==0||totalSteps>SongChain::kCapacity*32)return false;
   const double samplesPerStep=15.0*kRate/bpm;
   // 120 BPM: one 16-step bar is exactly 96000 frames = 2 seconds.
   // Fractional tempos are rounded to the nearest PCM sample.
   const auto nominal=static_cast<std::uint64_t>(
      std::llround(static_cast<double>(totalSteps)*samplesPerStep));
   if(nominal==0||nominal>kMaxData/(kChannels*kBytesPerSample))return false;
   SampleVoiceEngine voices;
   if(!voices.setOutputRate(kRate))return false;
   std::array<std::unique_ptr<PreparedSample>,kRows> prepared{};
   for(std::size_t row=0;row<kRows;++row){
     if(kit.wav[row].empty())continue;
     const bool muted=(kit.muted&(std::uint16_t{1}<<row))!=0||
                      (kit.solo!=0&&(kit.solo&(std::uint16_t{1}<<row))==0);
     const auto status=PreparedSample::fromWav(
       kit.wav[row].data(),kit.wav[row].size(),kRate,
       static_cast<double>(static_cast<int>(kit.pitch[row])-24),
       static_cast<float>(kit.gain[row])/100.f,
       (static_cast<float>(kit.pan[row])-100.f)/100.f,
       kit.choke[row],muted,prepared[row],kit.sampleEdits[row]);
     if(status!=PrepareStatus::Ok||!prepared[row])return false;
     auto sound=prepared[row]->rowSound();
     // Export sums all instrument buses into the stereo master, regardless
     // of which DAW stem each instrument normally routes to.
     sound.outputBus=0;
     if(!voices.setRow(row,sound)||!voices.setRowOutput(row,0))return false;
     if(sound.kernel->ratio()<=0.)return false;
   }

   StereoPeakLimiter limiter;
   if(!limiter.prepare(kRate))return false;
   ExportFile writer(destination);
   if(!writer.open())return false;
   const auto blank=header(0);
   if(!writer.write(blank.data(),blank.size()))return false;
   const double master=static_cast<double>(kit.masterVolume)/100.0;
   const bool limitOn=kit.limiter!=0;
   std::array<float,kBlock> left{},right{};
   std::array<std::uint8_t,kBlock*kChannels*kBytesPerSample> pcm{};
   struct SequenceRead {
      const MidiExportConfig& c;
      std::uint64_t total;
      std::uint8_t selected()const noexcept{return c.selectedPattern;}
      SongChain::Position mapStep(std::int64_t absolute,std::int64_t)const noexcept{
       if(absolute<0||static_cast<std::uint64_t>(absolute)>=total)
         return {SongChain::kRest,0,0};
       if(c.exportSong){
        auto pos=c.song.at(absolute,0);
        if(pos.pattern<kPatterns)
          pos.step=static_cast<std::uint8_t>(pos.step%c.patternLengths[pos.pattern]);
        return pos;
       }
       const auto length=c.patternLengths[c.selectedPattern];
       return {c.selectedPattern,static_cast<std::uint8_t>(absolute%length),0};
      }
      std::uint8_t get(std::size_t p,std::size_t r,std::size_t step)const noexcept{
       return p<kPatterns&&r<kRows&&step<kSteps?
             c.grid[(p*kRows+r)*kSteps+step]:0;
      }
   } read{sequence,totalSteps};
   std::uint64_t inputFrames=0,outputFrames=0;
   // The former voice-tail drain extended clips past the musical grid,
   // making a 120-BPM pattern appear out of sync when looped in a DAW.
   constexpr std::uint64_t kFadeFrames=240; // 5 ms at 48 kHz
   while(inputFrames<nominal){
      const auto frames=static_cast<std::uint32_t>(
          std::min<std::uint64_t>(kBlock,nominal-inputFrames));
      const double ppq=static_cast<double>(inputFrames)*bpm/(60.0*kRate);
      const Clock clock{ppq,bpm,static_cast<double>(kRate),
                        static_cast<std::int32_t>(frames),true,
                        kit.swing/100.0,kit.humanize/100.0,0};
      TriggerBatch hits;
      const auto scheduled=StepScheduler::run(read,clock,
                          [&](const Trigger& hit){(void)hits.appendStep(hit);});
      if(scheduled.status!=ScheduleStatus::Ok||hits.droppedCapacity()!=0)return false;
      hits.applyMuteSolo(kit.muted,kit.solo);
      hits.varyVelocity(kit.randomVelocity,clock.ppq,clock.bpm,clock.rate);
      hits.sort();
      const auto rendered=voices.render(hits.data(),hits.size(),left.data(),right.data(),frames);
      if(rendered.status!=RenderStatus::Ok||rendered.rejectedCapacity!=0)return false;
      std::size_t encoded=0;
      for(std::uint32_t i=0;i<frames;++i){
        float l=static_cast<float>(left[i]*master),r=static_cast<float>(right[i]*master);
        limiter.process(l,r,limitOn);
        // Smoothly truncate any last-step one-shot without moving its onset.
        const auto remaining=nominal-(inputFrames+i);
        if(remaining<=kFadeFrames){
          const auto fade=static_cast<float>(remaining)/kFadeFrames;
          l*=fade;r*=fade;
        }
        pack(l,pcm.data()+encoded*6);
        pack(r,pcm.data()+encoded*6+3);
        ++encoded;
      }
      if(encoded&&!writer.write(pcm.data(),encoded*6))return false;
      inputFrames+=frames;outputFrames+=encoded;
      if(outputFrames*6>kMaxData)return false;
   }
   if(outputFrames!=nominal||outputFrames*6>kMaxData)return false;
   const auto h=header(static_cast<std::uint32_t>(outputFrames*6));
   if(!writer.seek(0)||!writer.write(h.data(),h.size())||!writer.finish())return false;
   return true;
 }
public:
 static bool render(const KitState& kit,bool exportSong,double bpm,
                    const std::filesystem::path& destination)noexcept{
   try{return impl(kit,exportSong,bpm,destination);}
   catch(...){return false;}
 }
};
} // namespace drumator
