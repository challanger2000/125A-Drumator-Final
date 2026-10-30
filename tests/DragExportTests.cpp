#include "core/DragExportFile.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
using drumator::DragExportFile;
#define REQUIRE(x) do { if(!(x)){std::cerr<<__LINE__<<" " #x "\n";return 1;} }while(false)
int main() {
 REQUIRE(DragExportFile::create(-1).empty());
 REQUIRE(DragExportFile::create(4).empty());
 std::set<std::filesystem::path> directories;
 for(int i=0;i<4;++i) {
  auto file=DragExportFile::create(i);
  REQUIRE(!file.empty());
  REQUIRE(file.filename()==DragExportFile::filename(i));
  REQUIRE(std::filesystem::exists(file.parent_path()));
  REQUIRE(!std::filesystem::exists(file));
  REQUIRE(directories.insert(file.parent_path()).second);
  // Exporter writes real bytes before publication in CF_HDROP.
  {std::ofstream out(file,std::ios::binary);REQUIRE(out.good());out<<"MIDI/WAV"; }
  REQUIRE(std::filesystem::file_size(file)==8);
  // An unrelated file in our directory must block directory removal.
  const auto sentinel=file.parent_path()/"unrelated.txt";
  {std::ofstream out(sentinel);out<<"KEEP";}
  DragExportFile::discardFailed(file);
  REQUIRE(!std::filesystem::exists(file));
  REQUIRE(std::filesystem::exists(sentinel));
  REQUIRE(std::filesystem::exists(file.parent_path()));
  REQUIRE(std::filesystem::remove(sentinel));
  REQUIRE(std::filesystem::remove(file.parent_path()));
 }
 // Real drag exports deliberately outlive the drag/drop API return.
 auto retained=DragExportFile::create(0);
 REQUIRE(!retained.empty());
 {std::ofstream out(retained);out<<"content";}
 REQUIRE(std::filesystem::exists(retained));
 REQUIRE(std::filesystem::file_size(retained)==7);
 DragExportFile::discardFailed(retained);
 REQUIRE(!std::filesystem::exists(retained));
 REQUIRE(!std::filesystem::exists(retained.parent_path()));
 // Conservative expiry policy: stale controlled folders only.
 {
  namespace fs=std::filesystem;
  std::error_code ec;
  const auto base=fs::temp_directory_path()/("125A-Drumator-QA-"+
       std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
  REQUIRE(fs::create_directory(base));
  const auto own=base/"125A-Drumator-Drag-123-1-0";
  const auto recent=base/"125A-Drumator-Drag-123-2-0";
  const auto foreign=base/"125A-Drumator-Drag-123-3-0";
  const auto malformed=base/"125A-Drumator-Drag-foreign";
  REQUIRE(fs::create_directory(own));
  REQUIRE(fs::create_directory(recent));
  REQUIRE(fs::create_directory(foreign));
  REQUIRE(fs::create_directory(malformed));
  const auto ownFile=own/DragExportFile::filename(1);
  const auto recentFile=recent/DragExportFile::filename(0);
  const auto foreignFile=foreign/DragExportFile::filename(2);
  const auto malformedFile=malformed/DragExportFile::filename(3);
  for(const auto& p:{ownFile,recentFile,foreignFile,malformedFile}){
   std::ofstream f(p);f<<"test";
  }
  {std::ofstream f(foreign/"other.txt");f<<"keep";}
  const auto old=fs::file_time_type::clock::now()-std::chrono::hours(24*30);
  for(const auto& p:{ownFile,own,foreignFile,foreign,malformedFile,malformed})
   fs::last_write_time(p,old,ec);
  REQUIRE(!ec);
  REQUIRE(!DragExportFile::ownDirectoryName(malformed));
  REQUIRE(DragExportFile::ownDirectoryName(own));
  REQUIRE(DragExportFile::pruneStale(base)==1);
  REQUIRE(!fs::exists(own));
  REQUIRE(fs::exists(recentFile));
  REQUIRE(fs::exists(foreignFile)&&fs::exists(foreign/"other.txt"));
  REQUIRE(fs::exists(malformedFile));
  REQUIRE(DragExportFile::pruneStale(base)==0);
  fs::remove_all(base,ec);
  REQUIRE(!ec);
 }
 std::cout<<"Drag export filesystem lifecycle PASS\n";
}
