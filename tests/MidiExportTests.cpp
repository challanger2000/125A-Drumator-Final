#include "core/MidiExport.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <vector>
using namespace drumator;
namespace {
int errors=0;
void check(bool ok,const char* what){if(!ok){std::fprintf(stderr,"FAIL: %s\n",what);++errors;}}
std::uint32_t u32(const std::vector<std::uint8_t>& b,std::size_t p){
 return (std::uint32_t(b[p])<<24)|(std::uint32_t(b[p+1])<<16)|
        (std::uint32_t(b[p+2])<<8)|b[p+3];
}
std::uint32_t delta(const std::vector<std::uint8_t>& b,std::size_t& p,std::size_t end){
 std::uint32_t x=0;
 for(int i=0;i<4&&p<end;++i){auto v=b[p++];x=(x<<7)|(v&0x7f);if(!(v&128))return x;}
 ++errors;return 0;
}
struct Hit{std::uint32_t tick;std::uint8_t pitch,vel;};
std::vector<Hit> parse(const std::vector<std::uint8_t>& b,std::uint32_t expectedTracks){
 std::vector<Hit> result;
 if(b.size()<14){++errors;return result;}
 check(b[0]=='M'&&b[1]=='T'&&b[2]=='h'&&b[3]=='d',"SMF header");
 check(u32(b,4)==6,"six byte header");
 check(b[8]==0&&b[9]==1&&b[10]==0&&b[11]==expectedTracks,"SMF type 1 track count");
 check(b[12]==3&&b[13]==0xc0,"960 PPQ");
 std::size_t p=14;
 for(unsigned t=0;t<expectedTracks;++t){
  if(p+8>b.size()){++errors;break;}
  check(b[p]=='M'&&b[p+1]=='T'&&b[p+2]=='r'&&b[p+3]=='k',"MTrk signature");
  const auto size=u32(b,p+4);p+=8;
  const auto end=p+size;
  if(end>b.size()){++errors;break;}
  std::uint32_t tick=0;
  bool endOfTrack=false;
  while(p<end){
   tick+=delta(b,p,end);
   if(p>=end){++errors;break;}
   const auto status=b[p++];
   if(status==0xff){
    if(p>=end){++errors;break;}
    const auto kind=b[p++];const auto length=delta(b,p,end);
    if(p+length>end){++errors;break;}
    if(kind==0x2f){check(length==0,"EOT size");endOfTrack=true;}
    p+=length;
   }else if(status==0x99||status==0x89){
    if(p+2>end){++errors;break;}
    const auto pitch=b[p++],vel=b[p++];
    if(status==0x99)result.push_back({tick,pitch,vel});
   }else{++errors;break;}
  }
  check(endOfTrack,"EOT present");
  check(p==end,"track lengths exact");
 }
 check(p==b.size(),"no trailing bytes");
 std::sort(result.begin(),result.end(),[](const Hit& a,const Hit& b){
  return a.tick<b.tick;});
 return result;
}
}
int main(){
 MidiExportConfig c{};
 c.patternLengths.fill(32);
 c.grid[0]=100; // pattern A row 1 step 0
 c.grid[4]=127; // step 4
 c.grid[(1*kRows+2)*kSteps+2]=75; // pattern B row 3 step 2
 std::vector<std::uint8_t> b;
 for(std::uint8_t p=0;p<8;++p){
  c.selectedPattern=p;
  c.exportSong=false;
  check(MidiExport::encode(c,b),"all patterns export");
  const auto events=parse(b,17);
  const auto expect=p==0?2u:p==1?1u:0u;
  check(events.size()==expect,"pattern note count");
  if(p==0){
   check(events[0].tick==0&&events[0].pitch==36&&events[0].vel==100,"step 1 note");
   check(events[1].tick==960&&events[1].pitch==36&&events[1].vel==127,"step 5 note");
  }
 }
 c.selectedPattern=0;
 c.patternLengths[0]=16;
 check(MidiExport::encode(c,b),"one bar pattern");
 check(parse(b,17).size()==2,"one-bar count");
 c.patternLengths[0]=32;
 c.exportSong=true;
 SongChain song;
 check(song.setCount(4),"song count");
 check(song.configure(0,0,8),"song A 8");
 check(song.configure(1,SongChain::kRest,16),"song REST16");
 check(song.configure(2,1,32),"song B32");
 check(song.configure(3,0,8),"song A repeat8");
 c.song=song;
 check(MidiExport::encode(c,b),"song export");
 const auto notes=parse(b,17);
 check(notes.size()==5,"song repeated + rest has five hits");
 if(notes.size()==5){
  check(notes[0].tick==0&&notes[1].tick==960,"first A section");
  check(notes[2].tick==26*240&&notes[2].pitch==38,"song B offset");
  check(notes[3].tick==56*240&&notes[4].tick==60*240,"final A repeat");
 }
 std::vector<std::uint8_t> baseline=b,again;
 c.swingPercent=50;c.humanizePercent=100;c.randomVelocity[0]=75;
 check(MidiExport::encode(c,b),"with swing and humanize");
 check(MidiExport::encode(c,again)&&b==again,"deterministic export");
 check(parse(b,17).size()==5,"humanized number of notes unchanged");
 c.patternLengths[1]=17;
 baseline=b;
 check(!MidiExport::encode(c,b)&&b==baseline,"invalid input atomic rollback");
 std::printf("MIDI export tests: %s\n",errors==0?"PASS":"FAIL");
 return errors?1:0;
}
