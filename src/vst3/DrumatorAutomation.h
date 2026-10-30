#pragma once
// Host automation contract for Drumator V1.0.1.
// Pattern A-H keeps its historical ID 1000 and threshold mapping.
// New IDs are fixed and never depend on viewport/pattern or row visibility.
#include "PluginIDs.h"
#include "core/SequencerCore.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace drumator {
constexpr Steinberg::Vst::ParamID kRowVolumeBase=2000;
constexpr Steinberg::Vst::ParamID kRowPanBase=2100;
constexpr Steinberg::Vst::ParamID kRowMuteBase=2200;
constexpr Steinberg::Vst::ParamID kMasterVolumeParam=2300;
constexpr Steinberg::Vst::ParamID kRowPitchBase=2400;

enum class AutomationKind : std::uint8_t { None, Volume, Pan, Mute, Pitch, Master };
struct AutomationAddress {
 AutomationKind kind=AutomationKind::None;
 std::size_t row=kRows;
};
constexpr AutomationAddress automationAddress(Steinberg::Vst::ParamID id) noexcept {
 if(id>=kRowVolumeBase&&id<kRowVolumeBase+kRows)
   return {AutomationKind::Volume,id-kRowVolumeBase};
 if(id>=kRowPanBase&&id<kRowPanBase+kRows)
   return {AutomationKind::Pan,id-kRowPanBase};
 if(id>=kRowMuteBase&&id<kRowMuteBase+kRows)
   return {AutomationKind::Mute,id-kRowMuteBase};
 if(id>=kRowPitchBase&&id<kRowPitchBase+kRows)
   return {AutomationKind::Pitch,id-kRowPitchBase};
 if(id==kMasterVolumeParam)return {AutomationKind::Master,kRows};
 return {};
}
constexpr std::size_t kAutomatedMixLanes=4*kRows+1;
constexpr std::size_t automationLane(AutomationAddress a) noexcept {
 switch(a.kind){
  case AutomationKind::Volume:return a.row;
  case AutomationKind::Pan:return kRows+a.row;
  case AutomationKind::Mute:return 2*kRows+a.row;
  case AutomationKind::Pitch:return 3*kRows+a.row;
  case AutomationKind::Master:return 4*kRows;
  default:return kAutomatedMixLanes;
 }
}
inline unsigned automationPercent(Steinberg::Vst::ParamValue normalized,
                                  unsigned maximum=200) noexcept {
 if(!std::isfinite(normalized))return 0;
 return static_cast<unsigned>(std::lround(
    std::clamp(normalized,0.0,1.0)*maximum));
}
inline Steinberg::Vst::ParamValue automationNormalized(unsigned plain,
                                                       unsigned maximum=200) noexcept {
 return static_cast<double>(std::min(plain,maximum))/
        static_cast<double>(maximum);
}
} // namespace drumator
