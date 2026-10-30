// 125A Drumator — transient, realtime-safe sample edit publication.
// Only the UI/control actor publishes; the audio callback observes at most
// two bounded attempts, never locks, allocates, or touches persisted kit data.
#pragma once
#include "SampleEdit.h"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cmath>
namespace drumator {
class LiveEditMailbox final {
public:
 struct Snapshot {bool active=false;SampleEdit edit{};std::uint64_t generation=0;};
 void publish(bool active,const SampleEdit& edit) noexcept {
   sequence_.fetch_add(1,std::memory_order_acq_rel); // odd
   edges_.store((std::uint64_t{edit.start}<<32)|edit.end,std::memory_order_relaxed);
   fades_.store((std::uint64_t{edit.fadeIn}<<32)|edit.fadeOut,std::memory_order_relaxed);
   std::uint32_t gain=0;std::memcpy(&gain,&edit.gain,sizeof(gain));
   gain_.store(gain,std::memory_order_relaxed);
   active_.store(active?1:0,std::memory_order_relaxed);
   sequence_.fetch_add(1,std::memory_order_release); // even
 }
 bool snapshot(Snapshot& out) const noexcept {
   for(int tries=0;tries<2;++tries){
     const auto before=sequence_.load(std::memory_order_acquire);
     if(before&1u)continue;
     const auto edges=edges_.load(std::memory_order_relaxed);
     const auto fades=fades_.load(std::memory_order_relaxed);
     const auto gain=gain_.load(std::memory_order_relaxed);
     const auto active=active_.load(std::memory_order_relaxed);
     if(before!=sequence_.load(std::memory_order_acquire))continue;
     SampleEdit edit{};edit.start=static_cast<std::uint32_t>(edges>>32);
     edit.end=static_cast<std::uint32_t>(edges);
     edit.fadeIn=static_cast<std::uint32_t>(fades>>32);
     edit.fadeOut=static_cast<std::uint32_t>(fades);
     std::memcpy(&edit.gain,&gain,sizeof(gain));
     out={active!=0,edit,before};return true;
   }
   return false;
 }
private:
 std::atomic<std::uint64_t> sequence_{0},edges_{0},fades_{0};
 std::atomic<std::uint32_t> gain_{0};
 std::atomic<std::uint8_t> active_{0};
};
// An orphaned GUI session cannot hold a temporary sound indefinitely,
 // even if the host permanently refuses the EditorLiveEnd message.
 // Leases advance only on the real audio callback; no wall-clock or mutex
 // access is performed from the realtime thread.
 class LiveEditLease final {
 public:
  static constexpr double kSeconds=4.0;
  bool useDraft(const LiveEditMailbox::Snapshot* snap,
                std::uint32_t frames,double sampleRate) noexcept {
   if(snap&&snap->generation!=generation_){
    generation_=snap->generation;
    edit_=snap->edit;
    active_=snap->active;
    ageFrames_=0;
   }else{
    ageFrames_+=frames; // within bounded process() block sizes
   }
   const double ceiling=sampleRate*kSeconds;
   return active_&&sampleRate>0.0&&std::isfinite(ceiling)&&
          static_cast<double>(ageFrames_)<ceiling;
  }
  const SampleEdit& edit()const noexcept{return edit_;}
 private:
  std::uint64_t generation_{0},ageFrames_{0};
  bool active_{false};
  SampleEdit edit_{};
 };
} // namespace drumator
