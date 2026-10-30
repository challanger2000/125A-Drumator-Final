#pragma once
// A real filesystem path is required for the Windows CF_HDROP drag source.
// This is UI/control-thread only; no export or filesystem work on audio callbacks.
// Successful drag files intentionally remain accessible after DoDragDrop returns:
// DAWs may open the path asynchronously. Do not silently delete on drop/close.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <system_error>

namespace drumator {
struct DragExportFile final {
 static constexpr const char* filename(int option) noexcept {
  switch(option) {
   case 0:return "125A Drumator Pattern.mid";
   case 1:return "125A Drumator Pattern.wav";
   case 2:return "125A Drumator Song.mid";
   case 3:return "125A Drumator Song.wav";
   default:return nullptr;
  }
 }
 static std::filesystem::path create(int option) {
  const auto* name=filename(option);
  if(!name)return {};
  std::error_code ec;
  const auto root=std::filesystem::temp_directory_path(ec);
  if(ec||root.empty())return {};
  static std::atomic<std::uint64_t> serial{0};
  const auto tick=static_cast<std::uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  for(unsigned attempt=0;attempt<32;++attempt) {
   const auto value=serial.fetch_add(1,std::memory_order_relaxed);
   const auto directory=root/("125A-Drumator-Drag-"+
       std::to_string(tick)+"-"+std::to_string(value)+"-"+
       std::to_string(attempt));
   ec.clear();
   if(std::filesystem::create_directory(directory,ec))return directory/name;
   if(ec && ec!=std::errc::file_exists)return {};
  }
  return {};
 }
 // Only the caller's exclusive directory may be cleaned after failed export.
 // Never delete a successful drag's file: host may still be importing it.
 static void discardFailed(const std::filesystem::path& file) noexcept {
  if(file.empty()||!filenameMatches(file))return;
  const auto dir=file.parent_path();
  if(dir.filename().string().rfind("125A-Drumator-Drag-",0)!=0)return;
  std::error_code ec;
  std::filesystem::remove(file,ec);
  ec.clear();
  std::filesystem::remove(dir,ec); // fails safely if nonempty
 }
 static bool filenameMatches(const std::filesystem::path& path) {
  const auto file=path.filename();
  for(int i=0;i<4;++i)if(file==filename(i))return true;
  return false;
 }
};
}
