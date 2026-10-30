#pragma once
// Stereo-linked, zero-latency sample-peak output limiter.
// OFF is bit-transparent and sample-aligned with pre-limiter Drumator;
// no new plugin delay compensation or transport relocation is necessary.
// Fixed state only; no locks, allocations or host callbacks on the audio thread.
#include <algorithm>
#include <cmath>
#include <cstddef>
namespace drumator {
class StereoPeakLimiter final {
public:
 // Retain the symbolic constant for existing host/offline integration points.
 // A zero-latency limiter must not delay, truncate or reroute bypass audio.
 static constexpr std::size_t kLookahead=0;
 static constexpr float kCeiling=0.9660508789898133f; // -0.30 dBFS
 bool prepare(double sampleRate) noexcept {
  if(!std::isfinite(sampleRate)||sampleRate<=0.||sampleRate>10000000.)return false;
  // Immediate peak-safe attack and 80 ms smooth gain-release.
  releasePole_=static_cast<float>(std::exp(-1.0/(0.080*sampleRate)));
  reset();return true;
 }
 void reset() noexcept {gain_=1.f;}
 void process(float& l,float& r,bool enabled) noexcept {
  if(!enabled) {
   // Absolutely no processing in bypass: preserve even tiny values/bit patterns.
   gain_=1.f;
   return;
  }
  if(!std::isfinite(l)||!std::isfinite(r)) {
   l=0.f;r=0.f;gain_=1.f;return;
  }
  const float peak=std::max(std::abs(l),std::abs(r));
  const float required=peak>kCeiling?kCeiling/peak:1.f;
  if(required<gain_)gain_=required; // instantaneous linked downward gain
  else gain_=std::min(required,1.f-(1.f-gain_)*releasePole_);
  l*=gain_;
  r*=gain_;
 }
private:
 float gain_{1.f},releasePole_{0.9998f};
};
}
