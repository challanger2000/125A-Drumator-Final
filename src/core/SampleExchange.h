// 125A Drumator - immutable prepared assets and bounded SPSC sample transfer.
// Producer: one control/loader thread. Consumer: one audio thread.
// No ownership deletion, file reads, decoding, locks, or allocation on audio.
#pragma once

#include "SampleVoiceEngine.h"
#include "WavPcm16Decoder.h"
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <map>
#include <mutex>
#include <cstring>
#include <new>
#include <utility>

namespace drumator {

enum class PrepareStatus {
    Ok, InvalidSettings, InvalidWav, InsufficientMemory, InvalidKernel
};
// Control-thread-only cache; identical WAV sample-rate pairs share the same
// immutable 49-filter bank. Weak entries cannot pin retired assets forever.
// All access/allocations occur before SampleExchange::publish, NEVER in process.
inline std::shared_ptr<const PitchKernelBank> acquirePitchKernelBank(
    std::uint32_t sourceRate,double outputRate) {
    using Key=std::pair<std::uint32_t,std::uint64_t>;
    static std::mutex guard;
    static std::map<Key,std::weak_ptr<const PitchKernelBank>> cache;
    std::uint64_t bits=0;
    static_assert(sizeof(bits)==sizeof(outputRate),"unexpected double width");
    std::memcpy(&bits,&outputRate,sizeof(bits));
    const Key key{sourceRate,bits};
    std::lock_guard<std::mutex> lock(guard);
    if(const auto it=cache.find(key);it!=cache.end())
        if(auto existing=it->second.lock())return existing;
    auto bank=std::make_shared<PitchKernelBank>();
    if(!bank->prepare(sourceRate,outputRate))return {};
    for(auto it=cache.begin();it!=cache.end();)
        if(it->second.expired())it=cache.erase(it);
        else ++it;
    cache[key]=bank;
    return bank;
}

// One prepared asset per published row version. Only the loader constructs
// or destroys it. Samples and kernel are immutable after publication.
class PreparedSample final {
public:
    PreparedSample(const PreparedSample&)=delete;
    PreparedSample& operator=(const PreparedSample&)=delete;
    ~PreparedSample()=default; // destroyed only by the non-audio owner

    static PrepareStatus fromWav(const std::uint8_t* bytes, std::size_t size,
                                 double outputRate, double pitchSemitones,
                                 float gain, float pan, std::uint8_t chokeGroup,
                                 bool mute, std::unique_ptr<PreparedSample>& out,
                                 SampleEdit edit={},
                                 bool enableRealtimePitch=false) noexcept {
        if(!std::isfinite(outputRate)||outputRate<=0.0 ||
           !std::isfinite(pitchSemitones)||pitchSemitones< -48.0||
           pitchSemitones>48.0||!std::isfinite(gain)||gain<0.f||
           !std::isfinite(pan)||pan< -1.f||pan>1.f)
            return PrepareStatus::InvalidSettings;
        try {
            auto candidate=std::unique_ptr<PreparedSample>(new PreparedSample);
            if(WavPcm16Decoder::decode(bytes,size,candidate->decoded_)!=WavError::Ok)
                return PrepareStatus::InvalidWav;
            if(candidate->decoded_.left.empty())return PrepareStatus::InvalidWav;
            const auto frames=static_cast<std::uint32_t>(candidate->decoded_.left.size());
            if(edit.end==0)edit=SampleEdit::reset(frames);
            if(!edit.valid(frames))return PrepareStatus::InvalidSettings;
            candidate->edit_=edit;
            if(enableRealtimePitch){
                // The source/output rate pair determines all sinc kernels.
                // A newly selected sample never reaches the audio actor until
                // all 49 semitone filters are ready and immutable.
                candidate->pitchBank_=acquirePitchKernelBank(
                    candidate->decoded_.sampleRate,outputRate);
                if(!candidate->pitchBank_)return PrepareStatus::InvalidKernel;
                const int index=static_cast<int>(std::lround(pitchSemitones))+24;
                if(index<0||index>=static_cast<int>(PitchKernelBank::kCount))
                    return PrepareStatus::InvalidSettings;
                candidate->pitchIndex_=static_cast<std::uint8_t>(index);
            }
            const double ratio=static_cast<double>(candidate->decoded_.sampleRate)/
                               outputRate*std::exp2(pitchSemitones/12.0);
            // Realtime bank covers all 49 semitones, even when a valid
            // high-rate WAV exceeds the legacy static-kernel ratio limit.
            // The single static kernel is unused in realtime-bank mode.
            if(!std::isfinite(ratio)||ratio<=0.0||
               (!enableRealtimePitch &&
                (ratio>16.0||!candidate->kernel_.prepare(ratio))))
                return PrepareStatus::InvalidKernel;
            candidate->outputRate_=outputRate;
            candidate->gain_=gain;
            candidate->pan_=pan;
            candidate->choke_=chokeGroup;
            candidate->mute_=mute;
            out=std::move(candidate); // previous out retained upon failure
            return PrepareStatus::Ok;
        } catch(const std::exception&) {
            return PrepareStatus::InsufficientMemory;
        }
    }

    // Non-audio factory marker for a sample slot intentionally empty on recall.
    // Published like an ordinary asset to clear a previously loaded row.
    static std::unique_ptr<PreparedSample> silence(double outputRate) {
        if(!std::isfinite(outputRate)||outputRate<=0.0)return {};
        auto value=std::unique_ptr<PreparedSample>(new PreparedSample);
        value->outputRate_=outputRate;
        return value;
    }

    RowSound rowSound() const noexcept {
        RowSound r;
        r.sample={decoded_.left.data(),
                  decoded_.channels==2?decoded_.right.data():nullptr,
                  decoded_.left.size()};
        r.pitchBank=pitchBank_.get();r.pitchIndex=pitchIndex_;
        r.kernel=pitchBank_?pitchBank_->kernel(pitchIndex_):&kernel_;
        r.edit=edit_;
        r.gain=gain_;r.pan=pan_;r.chokeGroup=choke_;r.mute=mute_;
        r.ownerToken=this;
        return r;
    }
    double outputRate() const noexcept {return outputRate_;}
    std::uint32_t sourceRate() const noexcept{return decoded_.sampleRate;}
    std::size_t frames() const noexcept{return decoded_.left.size();}

private:
    PreparedSample()=default;
    DecodedWav decoded_{};
    SampleEdit edit_{};
    SincKernel kernel_{};
    std::shared_ptr<const PitchKernelBank> pitchBank_{};
    std::uint8_t pitchIndex_{24};
    double outputRate_{0.0};
    float gain_{1.f},pan_{0.f};
    std::uint8_t choke_{0};
    bool mute_{false};
};

struct ExchangeStats {
    std::uint32_t adopted{0};
    std::uint32_t wrongOutputRate{0};
    std::uint32_t pendingBackpressure{0};
};

// Ownership rules:
// - publish(): caller retains unique_ptr on failure; success transfers ownership
//   of that pointer to this exchange.
// - Audio thread calls audioAdopt() at block boundaries, then renders.
// - Audio thread calls audioCollect() after rendering to enqueue assets no
//   longer referenced by voices. It NEVER deletes any asset.
// - Loader calls pollReclaim() and destroys the returned unique_ptr outside
//   process(). Destruction of SampleExchange requires BOTH threads stopped.
// - Never publish the same PreparedSample instance to multiple rows.
class SampleExchange final {
public:
    static constexpr std::size_t kRetireSlots=96;
    static constexpr std::size_t kReturnCapacity=128;

    SampleExchange() noexcept=default;
    SampleExchange(const SampleExchange&)=delete;
    SampleExchange& operator=(const SampleExchange&)=delete;

    ~SampleExchange() {
        // Destruct only after producer and audio have both stopped.
        for(auto& slot:pending_)delete slot.exchange(nullptr);
        for(auto*& ptr:current_) {delete ptr;ptr=nullptr;}
        for(auto*& ptr:retired_) {delete ptr;ptr=nullptr;}
        for(;;) {
            auto reclaimed=pollReclaim();
            if(!reclaimed)break;
        }
    }

    bool publish(std::size_t row,std::unique_ptr<PreparedSample>& asset) noexcept {
        if(row>=kRows||!asset)return false;
        PreparedSample* expected=nullptr;
        if(!pending_[row].compare_exchange_strong(
            expected,asset.get(),std::memory_order_release,
            std::memory_order_relaxed))return false;
        asset.release();
        return true;
    }

    // Loader may cancel an as-yet-unconsumed row update. CAS ensures that a
    // concurrently adopting audio thread cannot also take ownership.
    std::unique_ptr<PreparedSample> withdraw(std::size_t row) noexcept {
        if(row>=kRows)return {};
        auto* observed=pending_[row].load(std::memory_order_acquire);
        if(!observed)return {};
        if(!pending_[row].compare_exchange_strong(
            observed,nullptr,std::memory_order_acq_rel,
            std::memory_order_relaxed))return {};
        return std::unique_ptr<PreparedSample>(observed);
    }

    // Audio thread only, at block start. A valid match must be prepared
    // specifically for this host output rate, including its sinc kernel.
    ExchangeStats audioAdopt(SampleVoiceEngine& engine,double outputRate) noexcept {
        ExchangeStats stats;
        if(!std::isfinite(outputRate)||outputRate<=0.0)return stats;
        audioCollect(engine); // frees only internal slots, never object storage
        for(std::size_t row=0;row<kRows;++row) {
            // An atomic load alone does not confer ownership: the worker
            // could withdraw+delete at any moment. Never dereference a
            // pending pointer before atomically taking ownership.
            if(!pending_[row].load(std::memory_order_acquire))continue;
            const auto empty=freeRetiredSlot();
            if(empty==kRetireSlots) {
                ++stats.pendingBackpressure;
                continue;
            }
            auto* candidate=pending_[row].exchange(
                nullptr,std::memory_order_acq_rel);
            if(!candidate)continue;
            // We own candidate exclusively now. Returning a mismatched
            // asset through the reclaim queue is safer than leaving a
            // stale pointer published across host sample-rate changes.
            if(candidate->outputRate()!=outputRate) {
                retired_[empty]=candidate;
                ++stats.wrongOutputRate;
                continue;
            }
            // No producer can alter candidate after release/acquire handoff.
            if(!engine.setRow(row,candidate->rowSound())) {
                retired_[empty]=candidate; // never delete in callback
                continue;
            }
            auto* previous=current_[row];
            current_[row]=candidate;
            if(previous)retired_[empty]=previous;
            ++stats.adopted;
        }
        return stats;
    }

    // Audio-thread producer -> worker-thread consumer bounded SPSC queue.
    // If full, retain the asset and retry on a later process block.
    void audioCollect(const SampleVoiceEngine& engine) noexcept {
        for(auto*& item:retired_) {
            if(!item||engine.references(item))continue;
            const auto write=write_.load(std::memory_order_relaxed);
            const auto read=read_.load(std::memory_order_acquire);
            if(write-read>=kReturnCapacity)break;
            returned_[static_cast<std::size_t>(write%kReturnCapacity)]=item;
            write_.store(write+1,std::memory_order_release);
            item=nullptr;
        }
    }

    // Loader thread only. Reclaimed assets MUST be destroyed off audio.
    std::unique_ptr<PreparedSample> pollReclaim() noexcept {
        const auto read=read_.load(std::memory_order_relaxed);
        if(read==write_.load(std::memory_order_acquire))return {};
        auto* asset=returned_[static_cast<std::size_t>(read%kReturnCapacity)];
        read_.store(read+1,std::memory_order_release);
        return std::unique_ptr<PreparedSample>(asset);
    }

private:
    std::size_t freeRetiredSlot() const noexcept {
        for(std::size_t i=0;i<kRetireSlots;++i)
            if(!retired_[i])return i;
        return kRetireSlots;
    }

    std::array<std::atomic<PreparedSample*>,kRows> pending_{};
    // The following three arrays are audio-thread-owned, except during
    // quiescent destruction. Never read/write them on loader thread.
    std::array<PreparedSample*,kRows> current_{};
    std::array<PreparedSample*,kRetireSlots> retired_{};
    std::array<PreparedSample*,kReturnCapacity> returned_{};
    std::atomic<std::uint64_t> write_{0};
    std::atomic<std::uint64_t> read_{0};
};

} // namespace drumator
