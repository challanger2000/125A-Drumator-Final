#include "ExportTimingOracle.h"
#include "core/BeatCreator.h"
#include "core/OfflineWavExport.h"
#include <cstdio>
#include <string>
using namespace drumator;
int main(){
 using namespace export_timing;
 const auto path=std::filesystem::temp_directory_path()/"125a-export-tempo-matrix.wav";
 unsigned cases=0;
 try {
  for(unsigned style=0;style<10;++style)for(unsigned steps:{16u,32u}){
   BeatCreatorSettings settings{static_cast<BeatStyle>(style),35,static_cast<std::uint8_t>(steps)};
   const auto generated=BeatCreator::preview(settings,42);
   require(generated.valid,"Beat Creator fixture invalid");
   KitState kit;kit.visible=8;kit.patternLengths.fill(static_cast<std::uint8_t>(steps));
   std::copy(generated.a.begin(),generated.a.end(),kit.grid.begin());
   std::copy(generated.b.begin(),generated.b.end(),kit.grid.begin()+kRows*kSteps);
   for(unsigned row=0;row<8;++row){kit.names[row]="impulse.wav";kit.wav[row]=impulse(row);}
   require(kit.chain.setCount(2)&&kit.chain.configure(0,0,static_cast<std::uint8_t>(steps))&&
      kit.chain.configure(1,1,static_cast<std::uint8_t>(steps)),"Song fixture invalid");
   for(double bpm:{90.,100.,120.,128.,140.})for(bool song:{false,true}){
    require(OfflineWavExport::render(kit,song,bpm,path),"WAV render rejected");
    const Wav wav(path);const unsigned total=steps*(song?2u:1u);
    require(wav.frames==frames(total,bpm),"musical PCM duration mismatch");
    std::vector<float> expected(wav.frames,0.f);
    std::vector<Midi::Note> notes;
    for(unsigned step=0;step<total;++step){
     const unsigned pattern=step/steps,local=step%steps;
     for(unsigned row=0;row<8;++row){
      const auto v=kit.grid[(pattern*kRows+row)*kSteps+local];if(!v)continue;
      expected[onset(step,bpm)]+=static_cast<float>(.01f*static_cast<float>(row+1)*static_cast<double>(v)/127.);
      notes.push_back({step*240u,row,v});
     }
    }
    unsigned audible=0;
    for(std::size_t frame=0;frame<wav.frames;++frame){
     require(std::abs(wav.sample(frame)-expected[frame])<2e-6f&&
       std::abs(wav.sample(frame,1)+expected[frame]*.5f)<2e-6f,
       "PCM transient missing, duplicated, shifted or silent interval corrupted");
     if(expected[frame]!=0.f)++audible;
    }
    require(audible>0,"silent positive control");
    MidiExportConfig midi; midi.grid=kit.grid;midi.patternLengths=kit.patternLengths;
    midi.song=kit.chain;midi.exportSong=song;midi.tempoBpm=bpm;
    std::vector<std::uint8_t> bytes;require(MidiExport::encode(midi,bytes),"MIDI encode rejected");
    Midi parsed(bytes);
    require(parsed.micros==static_cast<std::uint32_t>(std::lround(60000000./bpm)),"wrong MIDI tempo metadata");
    require(parsed.notes.size()==notes.size(),"MIDI missing/extra notes");
    for(const auto& n:notes){
     const auto match=std::count_if(parsed.notes.begin(),parsed.notes.end(),[&](const auto& actual){
      return actual.tick==n.tick&&actual.row==n.row&&actual.velocity==n.velocity;});
     require(match==1,"wrong MIDI ticks/row/velocity");
    }
    // A plausible half-tempo file must be rejected by this independent oracle.
    require(frames(total,bpm*.5)!=wav.frames,"half-tempo negative control is ineffective");
    ++cases;
   }
  }
  std::error_code ec;std::filesystem::remove(path,ec);
  std::printf("EXPORT TIMING PASS: %u actual WAV+MIDI cases, 10 styles, 16/32 steps, Pattern/Song, 90/100/120/128/140 BPM; every stereo PCM frame checked\n",cases);
  return 0;
 }catch(const std::exception& e){std::fprintf(stderr,"EXPORT TIMING FAIL after %u cases: %s\n",cases,e.what());return 1;}
}
