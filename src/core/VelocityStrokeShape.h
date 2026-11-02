#pragma once
// Host-independent 32-step velocity gesture planning; never creates notes.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
namespace drumator {
enum class VelocityStrokeMode : std::uint8_t { Paint, Line, Curve, Accent };
// V1.5 controls: normal drag Paint, Shift Line, Shift+Alt Curve, Alt Accent.
// BC45 additionally lets Ctrl+drag act as a Line alias for existing users.
// Ctrl+Alt retains minimum-velocity painting; blank notes stay inactive.
struct VelocityStrokeGesture final {
 VelocityStrokeMode mode;
 bool minimum;
};
constexpr VelocityStrokeGesture velocityStrokeGesture(bool control,bool shift,bool alt) noexcept {
 if(control&&alt&&!shift)return {VelocityStrokeMode::Paint,true};
 if(shift&&alt)return {VelocityStrokeMode::Curve,false};
 if(control||shift)return {VelocityStrokeMode::Line,false};
 if(alt)return {VelocityStrokeMode::Accent,false};
 return {VelocityStrokeMode::Paint,false};
}
// Native Drumator velocity well is painted from y+53 to y+92 and the
// active bars finish at y+89. Click values and visual bar heights must
// use one exact logical scale, not the former unrelated 48px input range.
struct VelocityLaneScale final {
 static constexpr double top=53.,bottom=89.;
 static std::uint8_t valueAt(double relativeY) noexcept {
  const auto level=std::clamp((bottom-relativeY)/(bottom-top),0.,1.);
  return static_cast<std::uint8_t>(std::clamp(std::lround(1.+126.*level),1L,127L));
 }
 static constexpr double heightFor(std::uint8_t value) noexcept {
  return value==0?0.:1.+34.*static_cast<double>(value-1)/126.;
 }
};
class VelocityStrokeShape final {
public:
 using Values=std::array<std::uint8_t,32>;
 bool begin(const Values& before,std::size_t length,std::size_t step,
            std::uint8_t value,VelocityStrokeMode mode) noexcept {
  if((length!=16&&length!=32)||step>=length||!before[step])return false;
  source_=desired_=before;
  length_=length;anchor_=last_=step;
  anchorValue_=lastValue_=std::clamp<int>(value,1,127);
  mode_=mode;active_=true;
  advance(step,value);
  return true;
 }
 const Values& advance(std::size_t step,std::uint8_t value) noexcept {
  if(!active_||step>=length_)return desired_;
  value=static_cast<std::uint8_t>(std::clamp<int>(value,1,127));
  if(mode_==VelocityStrokeMode::Line||mode_==VelocityStrokeMode::Curve) {
   desired_=source_; // retracting a drawn line restores prior bar values
   const auto lo=std::min(step,anchor_),hi=std::max(step,anchor_);
   for(std::size_t i=lo;i<=hi;++i){
    if(!source_[i])continue;
    const double t=step==anchor_?1.:
      static_cast<double>(static_cast<int>(i)-static_cast<int>(anchor_))/
      static_cast<double>(static_cast<int>(step)-static_cast<int>(anchor_));
    const double shaped=mode_==VelocityStrokeMode::Curve?t*t*(3.-2.*t):t;
    desired_[i]=static_cast<std::uint8_t>(std::clamp(
      static_cast<int>(std::lround(anchorValue_+
      (static_cast<double>(value)-anchorValue_)*shaped)),1,127));
   }
  }else{
   // Interpolate across steps skipped by sparse host mouse-move events.
   const auto lo=std::min(step,last_),hi=std::max(step,last_);
   for(std::size_t i=lo;i<=hi;++i){
    if(!source_[i])continue;
    if(mode_==VelocityStrokeMode::Accent)
      desired_[i]=std::max<std::uint8_t>(source_[i],120);
    else{
      const double t=step==last_?1.:
        static_cast<double>(static_cast<int>(i)-static_cast<int>(last_))/
        static_cast<double>(static_cast<int>(step)-static_cast<int>(last_));
      desired_[i]=static_cast<std::uint8_t>(std::clamp(
        static_cast<int>(std::lround(lastValue_+
          (static_cast<double>(value)-lastValue_)*t)),1,127));
    }
   }
  }
  last_=step;lastValue_=value;
  return desired_;
 }
 const Values& values()const noexcept{return desired_;}
 bool active()const noexcept{return active_;}
 void clear()noexcept{active_=false;}
private:
 Values source_{},desired_{};
 std::size_t length_{0},anchor_{0},last_{0};
 int anchorValue_{100},lastValue_{100};
 VelocityStrokeMode mode_{VelocityStrokeMode::Paint};
 bool active_{false};
};
}
