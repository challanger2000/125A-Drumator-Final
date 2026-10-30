#pragma once
// Drumator: host-independent, deterministic Standard MIDI File export.
// Uses the very same StepScheduler and TriggerBatch variation as live playback.
#include "SequencerCore.h"
#include "SongChain.h"
#include "TriggerBatch.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace drumator {
struct MidiExportConfig final {
  std::array<std::uint8_t,kCells> grid{};
  std::array<std::uint8_t,kPatterns> patternLengths{};
  std::array<std::uint8_t,kRows> randomVelocity{};
  SongChain song{};
  std::uint8_t selectedPattern{0};
  std::uint8_t swingPercent{0};
  std::uint8_t humanizePercent{0};
  bool exportSong{false};
  double tempoBpm{120.0}; // DAW tempo received through the existing meter link
};

class MidiExport final {
  struct Note {std::uint32_t start{0};std::uint8_t velocity{0};};
  static constexpr std::uint32_t kPPQ=960;
  static constexpr std::uint32_t kTicksPerStep=kPPQ/4;
  static constexpr std::uint32_t kNoteLength=kTicksPerStep/4;
  static constexpr std::uint32_t kSampleRate=48000;
  static constexpr std::uint32_t kBpm=120;
  static constexpr std::uint32_t kSamplesPerStep=kSampleRate*15/kBpm;

  static void be16(std::vector<std::uint8_t>& b,std::uint32_t x){
    b.push_back(static_cast<std::uint8_t>((x>>8)&255));
    b.push_back(static_cast<std::uint8_t>(x&255));
  }
  static void be32(std::vector<std::uint8_t>& b,std::uint32_t x){
    b.push_back(static_cast<std::uint8_t>((x>>24)&255));
    b.push_back(static_cast<std::uint8_t>((x>>16)&255));
    b.push_back(static_cast<std::uint8_t>((x>>8)&255));
    b.push_back(static_cast<std::uint8_t>(x&255));
  }
  static void vlq(std::vector<std::uint8_t>& b,std::uint32_t x){
    std::uint8_t temp[5]{};
    std::size_t n=0;
    temp[n++]=static_cast<std::uint8_t>(x&0x7f);
    while((x>>=7)!=0)temp[n++]=static_cast<std::uint8_t>((x&0x7f)|0x80);
    while(n)b.push_back(temp[--n]);
  }
  static void track(std::vector<std::uint8_t>& out,
                    const std::vector<std::uint8_t>& events){
    out.insert(out.end(),{'M','T','r','k'});
    be32(out,static_cast<std::uint32_t>(events.size()));
    out.insert(out.end(),events.begin(),events.end());
  }
  struct PatternRead final {
    const MidiExportConfig& c;
    std::uint32_t totalSteps;
    std::uint8_t selected() const noexcept {return c.selectedPattern;}
    SongChain::Position mapStep(std::int64_t absolute,std::int64_t) const noexcept {
      if(absolute<0||static_cast<std::uint64_t>(absolute)>=totalSteps)
        return {SongChain::kRest,0,0};
      if(c.exportSong){
        auto at=c.song.at(absolute,0);
        if(at.pattern<kPatterns)
          at.step=static_cast<std::uint8_t>(at.step%c.patternLengths[at.pattern]);
        return at;
      }
      const auto length=c.patternLengths[c.selectedPattern];
      return {c.selectedPattern,static_cast<std::uint8_t>(absolute%length),0};
    }
    std::uint8_t get(std::size_t p,std::size_t r,std::size_t s) const noexcept {
      return p<kPatterns&&r<kRows&&s<kSteps?c.grid[(p*kRows+r)*kSteps+s]:0;
    }
  };
public:
  // A MIDI file represents the programmed sequence, including muted rows.
  // Row mutes/solos affect audio playback, not destructive musical export.
  // 960 PPQ, type 1, tempo 120 BPM as self-contained fallback. Import into
  // the Studio One tempo grid to follow its project tempo.
  static bool encode(const MidiExportConfig& c,std::vector<std::uint8_t>& dest){
    if(c.selectedPattern>=kPatterns||c.swingPercent>50||c.humanizePercent>100||
       !std::isfinite(c.tempoBpm)||c.tempoBpm<20.||c.tempoBpm>300.)
      return false;
    for(const auto n:c.patternLengths)if(n!=16&&n!=32)return false;
    for(const auto v:c.grid)if(v>127)return false;
    for(const auto v:c.randomVelocity)if(v>100)return false;
    const std::uint32_t totalSteps=static_cast<std::uint32_t>(
      c.exportSong?c.song.totalSteps():c.patternLengths[c.selectedPattern]);
    if(totalSteps==0||totalSteps>SongChain::kCapacity*32)return false;
    if(c.exportSong)for(std::size_t i=0;i<c.song.count();++i){
      const auto entry=c.song.entry(i);
      if(entry.pattern>SongChain::kRest||!SongChain::validLength(entry.steps))return false;
    }
    PatternRead read{c,totalSteps};
    std::array<std::vector<Note>,kRows> lanes{};
    // Use complete musical windows and an extra step for delayed final hits.
    // Adjacent half-open windows avoid duplicate notes at block boundaries.
    for(std::uint32_t first=0;first<totalSteps+1;first+=32){
      const auto steps=std::min(std::uint32_t{32},totalSteps+1-first);
      const std::uint32_t frames=steps*kSamplesPerStep;
      const Clock clock{static_cast<double>(first)/4.0,static_cast<double>(kBpm),
                         static_cast<double>(kSampleRate),
                         static_cast<std::int32_t>(frames),true,
                         static_cast<double>(c.swingPercent)/100.0,
                         static_cast<double>(c.humanizePercent)/100.0,0};
      TriggerBatch batch;
      const auto scheduled=StepScheduler::run(read,clock,
                   [&](const Trigger& hit){(void)batch.appendStep(hit);});
      if(scheduled.status!=ScheduleStatus::Ok||batch.droppedCapacity()!=0)return false;
      batch.varyVelocity(c.randomVelocity,clock.ppq,clock.bpm,clock.rate);
      batch.sort();
      for(std::size_t i=0;i<batch.size();++i){
        const auto& hit=batch.data()[i];
        const auto absoluteFrame=static_cast<std::uint64_t>(first)*kSamplesPerStep+
                                  hit.sampleOffset;
        // 25 samples/tick at the fixed internal 120 BPM / 48kHz grid.
        const auto tick=static_cast<std::uint32_t>((absoluteFrame+12)/25);
        lanes[hit.row].push_back({tick,hit.velocity});
      }
    }
    std::vector<std::uint8_t> out;
    out.insert(out.end(),{'M','T','h','d'});
    be32(out,6);be16(out,1);be16(out,kRows+1);be16(out,kPPQ);
    std::vector<std::uint8_t> conductor;
    // 4/4 time signature, 120 BPM, end of track at the pattern/song boundary.
    conductor.insert(conductor.end(),{0,0xff,0x58,4,4,2,24,8,
                                      0,0xff,0x51,3});
    const auto microseconds=static_cast<std::uint32_t>(
        std::lround(60000000.0/c.tempoBpm));
    conductor.push_back(static_cast<std::uint8_t>(microseconds>>16));
    conductor.push_back(static_cast<std::uint8_t>(microseconds>>8));
    conductor.push_back(static_cast<std::uint8_t>(microseconds));
    vlq(conductor,totalSteps*kTicksPerStep);
    conductor.insert(conductor.end(),{0xff,0x2f,0});
    track(out,conductor);
    for(std::size_t row=0;row<kRows;++row){
      auto& notes=lanes[row];
      std::sort(notes.begin(),notes.end(),[](const Note& a,const Note& b){
        return a.start<b.start;
      });
      std::vector<std::uint8_t> events;
      // All 16 instruments use the dedicated MIDI drum channel (channel 10),
      // organized as 16 distinct tracks with stable row-to-note mapping.
      events.insert(events.end(),{0,0xff,3,7,'D','r','u','m',' ','0','0'});
      events[events.size()-2]=static_cast<std::uint8_t>('0'+(row+1)/10);
      events[events.size()-1]=static_cast<std::uint8_t>('0'+(row+1)%10);
      std::uint32_t lastTick=0;
      for(const auto& note:notes){
        if(note.start<lastTick)return false;
        vlq(events,note.start-lastTick);
        events.push_back(0x99);events.push_back(static_cast<std::uint8_t>(36+row));
        events.push_back(note.velocity);
        vlq(events,kNoteLength);
        events.push_back(0x89);events.push_back(static_cast<std::uint8_t>(36+row));
        events.push_back(0);
        lastTick=note.start+kNoteLength;
      }
      const auto endTick=std::max(totalSteps*kTicksPerStep,lastTick);
      vlq(events,endTick-lastTick);
      events.insert(events.end(),{0xff,0x2f,0});
      track(out,events);
    }
    dest.swap(out); // transactional: invalid input never changes output
    return true;
  }
};
} // namespace drumator
