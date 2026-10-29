#pragma once
// Low-overhead, stereo-linked lookahead peak limiter. Fixed 64-sample latency
// on ALL states (including OFF) so VST3 PDC stays constant while toggling.
// All state is preallocated. No file I/O, locks or heap work in process().
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
namespace drumator {
class StereoPeakLimiter final {
public:
 static constexpr std::size_t kLookahead=64;
 static constexpr float kCeiling=0.9660508789898133f; // -0.30 dBFS, linear
 bool prepare(double sampleRate) noexcept {
  if(!std::isfinite(sampleRate)||sampleRate<=0.||sampleRate>10000000.)return false;
  // 64-frame bypass crossfade and 80 ms release. The hold makes
  // hard transient peaks safe at the output without clipping the waveform.
  fadeStep_=1.f/static_cast<float>(kLookahead);
  release_=static_cast<float>(1.0/(0.08*sampleRate));
  reset();return true;
 }
 void reset()noexcept{
  left_.fill(0.f);right_.fill(0.f);
  pos_=0;gain_=1.f;mix_=0.f;hold_=0;
 }
 void process(float& l,float& r,bool enabled) noexcept {
  const float inputL=std::isfinite(l)?l:0.f;
  const float inputR=std::isfinite(r)?r:0.f;
  const float peak=std::max(std::abs(inputL),std::abs(inputR));
  const float demand=peak>kCeiling?kCeiling/peak:1.f;
  // Instantaneous lookahead detector: attenuation begins 64 samples before
  // the corresponding transient emerges from the delay buffer.
  if(demand<1.f){gain_=std::min(gain_,demand);hold_=kLookahead+1;}
  else if(hold_>0)--hold_;
  else gain_=std::min(1.f,gain_+release_);
  const float priorL=left_[pos_],priorR=right_[pos_];
  left_[pos_]=inputL;right_[pos_]=inputR;
  pos_=(pos_+1)%kLookahead;
  const float desiredMix=enabled?1.f:0.f;
  mix_+=std::clamp(desiredMix-mix_,-fadeStep_,fadeStep_);
  const float applied=1.f+mix_*(gain_-1.f);
  l=priorL*applied;r=priorR*applied;
 }
private:
 std::array<float,kLookahead> left_{},right_{};
 std::size_t pos_{0},hold_{0};
 float gain_{1.f},mix_{0.f},fadeStep_{0.001f},release_{0.0002f};
};
}
