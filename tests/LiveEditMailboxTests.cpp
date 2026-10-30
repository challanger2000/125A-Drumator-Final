#include "core/LiveEditMailbox.h"
#include <cstdint>
#include <iostream>
using drumator::SampleEdit;
using drumator::LiveEditMailbox;
using drumator::LiveEditLease;
int main(){
 LiveEditMailbox mailbox;
 LiveEditMailbox::Snapshot state{};
 if(!mailbox.snapshot(state)||state.active)return 1;
 for(std::uint32_t i=0;i<50000u;++i){
   const SampleEdit edit{i,i+200u,i%60u,(i+7u)%60u,0.25f};
   mailbox.publish(true,edit);
   if(!mailbox.snapshot(state)||!state.active||
      state.edit.start!=edit.start||state.edit.end!=edit.end||
      state.edit.fadeIn!=edit.fadeIn||state.edit.fadeOut!=edit.fadeOut||
      state.edit.gain!=edit.gain)return 2;
 }
 mailbox.publish(false,SampleEdit{});
 if(!mailbox.snapshot(state)||state.active)return 3;
 LiveEditLease lease;
 const auto sampleRate=48000.0;
 const SampleEdit activeEdit{5,120,3,4,.5f};
 mailbox.publish(true,activeEdit);
 if(!mailbox.snapshot(state)||!lease.useDraft(&state,256,sampleRate)||
    lease.edit().start!=5)return 4;
 // Simulate a host that refuses all EditorLiveEnd messages and a View
 // disappearing: the last sound MUST expire without any more IPC.
 for(int i=0;i<800;++i){
   if(!mailbox.snapshot(state))return 5;
   (void)lease.useDraft(&state,256,sampleRate);
 }
 if(lease.useDraft(&state,256,sampleRate))return 6;
 // Explicit UI heartbeat restarts the same edit only while still alive.
 mailbox.publish(true,activeEdit);
 if(!mailbox.snapshot(state)||!lease.useDraft(&state,256,sampleRate))return 7;
 mailbox.publish(false,SampleEdit{});
 if(!mailbox.snapshot(state)||lease.useDraft(&state,256,sampleRate))return 8;
 std::cout<<"LiveEditMailbox 50000 coherent snapshots / cancel PASS\n";
 return 0;
}
