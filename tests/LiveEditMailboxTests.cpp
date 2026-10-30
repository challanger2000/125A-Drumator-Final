#include "core/LiveEditMailbox.h"
#include <cstdint>
#include <iostream>
using drumator::SampleEdit;
using drumator::LiveEditMailbox;
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
 std::cout<<"LiveEditMailbox 50000 coherent snapshots / cancel PASS\n";
 return 0;
}
