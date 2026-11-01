#include "core/KitPresetCatalog.h"
#include <iostream>
#include <stdexcept>
using namespace drumator;
static void ck(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){
 try{
  const auto root=std::filesystem::temp_directory_path()/"125A-KitCatalog-QA";
  std::error_code ec;
  std::filesystem::remove_all(root,ec);
  ck(std::filesystem::create_directories(root),"create test kits");
  KitState stored;
  stored.wav[0].clear();
  stored.names[0].clear();
  stored.grid[0]=107;
  stored.swing=37;
  stored.displayNames[0]="User Kick";
  const auto a=root/"01_Acoustic.125akit";
  const auto b=root/"02_Electronic.125akit";
  ck(KitPresetCatalog::save(a,stored),"write kit A");
  ck(KitPresetCatalog::save(b,stored),"write kit B");
  auto entries=KitPresetCatalog::list(root);
  ck(entries.size()==2&&entries[0].title=="01_Acoustic"&&
     entries[1].title=="02_Electronic","sorted browser entries");
  KitState decoded;
  ck(KitPresetCatalog::read(a,decoded)&&decoded.grid[0]==107&&
     decoded.swing==37&&decoded.displayNames[0]=="User Kick",
     "kit checksum/round-trip validation");
  auto outside=root/"not_a_kit.txt";
  ck(KitPresetCatalog::save(outside,stored),"create filtered file");
  ck(KitPresetCatalog::list(root).size()==2,"ignored non-kit file");
  {
   std::ofstream f(root/"03_Bad.125akit",std::ios::binary);
   f<<"not-a-valid-kit";
  }
  ck(!KitPresetCatalog::read(root/"03_Bad.125akit",decoded),
     "malformed kit imported");
  std::filesystem::remove_all(root,ec);
  std::cout<<"BC33 independent kit browser catalogue, checksum and file round trip PASS\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<"BC33 FAIL: "<<e.what()<<"\n";return 1;}
}
