#pragma once
// The Beat Creator uses ONLY the currently loaded sample set.
// No hidden factory drum synthesis or sample injection.
#include "core/BeatCreator.h"
#include "core/KitState.h"
#include <algorithm>
#include <cstddef>
namespace drumator {
class BeatCreatorDraftRenderer final {
public:
 static bool setDraft(KitState& temp,const BeatCreatorPreview& draft,
                      std::size_t choice) noexcept {
  if(!draft.valid||(draft.steps!=16&&draft.steps!=32)||choice>=3)return false;
  const PatternSteps* pattern=choice==0?&draft.a:choice==1?&draft.b:&draft.c;
  if(emptyPattern(*pattern))return false;
  temp.grid.fill(0);
  std::copy(pattern->begin(),pattern->end(),temp.grid.begin());
  temp.patternLengths[0]=draft.steps;
  temp.selected=0;
  return true;
 }
};
}
