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
 std::cout<<"Drag export filesystem lifecycle PASS\n";
}
