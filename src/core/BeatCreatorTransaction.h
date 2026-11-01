// 125A Drumator 1.5: prepare A/B/C without touching a host, kit or GUI.
// Single control-thread transaction. The Processor performs the final
// compare-and-swap under its writer lock for all three patterns.
#pragma once
#include "BeatCreator.h"
#include <array>
#include <cstddef>
#include <cstdint>
namespace drumator {
using BeatTriple = std::array<PatternSteps,3>;
using BeatLengths = std::array<std::uint8_t,3>;
static_assert(sizeof(BeatTriple)==3*kRows*kSteps);
static_assert(sizeof(BeatLengths)==3);
struct BeatCreatorTransaction final {
 BeatTriple before{},after{};
 BeatLengths oldLengths{},newLengths{};
 std::uint8_t occupied=0;
 bool valid=false;
 static constexpr bool lengthsValid(const BeatLengths& ls) noexcept {
  for(auto l:ls)if(l!=16&&l!=32)return false;
  return true;
 }
 static constexpr bool candidateValid(const PatternSteps& p,
                                      std::size_t length) noexcept {
  if(length!=16&&length!=32)return false;
  for(std::size_t r=0;r<kRows;++r)
   for(std::size_t s=0;s<kSteps;++s){
    const auto value=p[r*kSteps+s];
    if(value>127||(r>=8||s>=length)&&value!=0)return false;
   }
  return true;
 }
 static BeatCreatorTransaction prepare(const BeatCreatorPreview& preview,
                                      const BeatTriple& existing,
                                      const BeatLengths& lengths,
                                      bool allowOccupied) noexcept {
  BeatCreatorTransaction result{};
  if(!preview.valid||(preview.steps!=16&&preview.steps!=32)||
     !lengthsValid(lengths)||
     !candidateValid(preview.a,preview.steps)||
     !candidateValid(preview.b,preview.steps)||
     !candidateValid(preview.c,preview.steps))return result;
  result.occupied=BeatCreator::occupiedMask(existing);
  if(result.occupied!=0&&!allowOccupied)return result;
  result.before=existing;
  result.after=existing; // preserve user rows 9..16 and hidden second-bar data
  result.oldLengths=lengths;
  result.newLengths.fill(preview.steps);
  const BeatTriple created{preview.a,preview.b,preview.c};
  for(std::size_t pat=0;pat<3;++pat)
   for(std::size_t row=0;row<8;++row)
    for(std::size_t step=0;step<preview.steps;++step)
     result.after[pat][row*kSteps+step]=created[pat][row*kSteps+step];
  result.valid=true;
  return result;
 }
};
} // namespace drumator
