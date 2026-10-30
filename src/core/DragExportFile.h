#pragma once
// Real Windows CF_HDROP files are retained for asynchronous DAW import.
// A conservative, scoped scavenger removes only our own intact, old folders;
// never remove anything the host may still reference, nor foreign temp data.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <system_error>
namespace drumator {
struct DragExportFile final {
 static constexpr const char* kPrefix="125A-Drumator-Drag-";
 static constexpr auto kRetention=std::chrono::hours(24*14);
 static constexpr const char* filename(int option) noexcept {
  switch(option) {
   case 0:return "125A Drumator Pattern.mid";
   case 1:return "125A Drumator Pattern.wav";
   case 2:return "125A Drumator Song.mid";
   case 3:return "125A Drumator Song.wav";
   default:return nullptr;
  }
 }
 static bool ownDirectoryName(const std::filesystem::path& path) {
  const auto n=path.filename().string(),p=std::string{kPrefix};
  if(n.compare(0,p.size(),p)!=0)return false;
  const auto tail=n.substr(p.size());
  // Our names always have three nonempty decimal fields, exactly.
  unsigned separators=0, digits=0;
  for(const char c:tail) {
   if(c=='-'){
    if(digits==0)return false;
    ++separators;digits=0;
   } else if(c>='0'&&c<='9')++digits;
   else return false;
  }
  return separators==2&&digits>0;
 }
 static bool filenameMatches(const std::filesystem::path& path) {
  for(int i=0;i<4;++i)if(path.filename()==filename(i))return true;
  return false;
 }
 // Return the number of own stale folders removed. This must run on the
 // control thread and may have bounded filesystem cost, NEVER on audio.
 static std::size_t pruneStale(const std::filesystem::path& root,
                               std::chrono::hours retention=kRetention) noexcept {
  namespace fs=std::filesystem;
  std::error_code ec;
  if(root.empty()||!fs::is_directory(root,ec)||ec)return 0;
  const auto now=fs::file_time_type::clock::now();
  const auto cutoff=now-std::chrono::duration_cast<fs::file_time_type::duration>(retention);
  std::size_t removed=0,visited=0;
  for(fs::directory_iterator it(root,fs::directory_options::skip_permission_denied,ec),end;
      it!=end&&!ec&&visited<4096&&removed<32;it.increment(ec),++visited) {
   std::error_code local;
   const auto dir=it->path();
   if(!ownDirectoryName(dir))continue;
   const auto meta=fs::symlink_status(dir,local);
   if(local||meta.type()!=fs::file_type::directory)continue;
   const auto dirTime=fs::last_write_time(dir,local);
   if(local||dirTime>=cutoff)continue;
   fs::directory_iterator child(dir,fs::directory_options::skip_permission_denied,local);
   if(local)continue;
   if(child==fs::directory_iterator{}) {
    if(fs::remove(dir,local)&&!local)++removed;
    continue;
   }
   const auto file=child->path();
   if(!filenameMatches(file))continue;
   const auto type=fs::symlink_status(file,local);
   if(local||type.type()!=fs::file_type::regular)continue;
   const auto age=fs::last_write_time(file,local);
   if(local||age>=cutoff)continue;
   child.increment(local);
   if(local||child!=fs::directory_iterator{})continue; // no foreign files
   if(!fs::remove(file,local)||local)continue;
   local.clear();
   if(fs::remove(dir,local)&&!local)++removed;
  }
  return removed;
 }
 static std::filesystem::path create(int option) {
  const auto* name=filename(option);
  if(!name)return {};
  std::error_code ec;
  const auto root=std::filesystem::temp_directory_path(ec);
  if(ec||root.empty())return {};
  (void)pruneStale(root); // after 14 days, before allocating a new drag
  static std::atomic<std::uint64_t> serial{0};
  const auto tick=static_cast<std::uint64_t>(
    std::chrono::high_resolution_clock::now().time_since_epoch().count());
  for(unsigned attempt=0;attempt<32;++attempt) {
   const auto value=serial.fetch_add(1,std::memory_order_relaxed);
   const auto directory=root/(std::string{kPrefix}+
       std::to_string(tick)+"-"+std::to_string(value)+"-"+
       std::to_string(attempt));
   ec.clear();
   if(std::filesystem::create_directory(directory,ec))return directory/name;
   if(ec && ec!=std::errc::file_exists)return {};
  }
  return {};
 }
 static void discardFailed(const std::filesystem::path& file) noexcept {
  if(file.empty()||!filenameMatches(file))return;
  const auto dir=file.parent_path();
  if(!ownDirectoryName(dir))return;
  std::error_code ec;
  if(std::filesystem::symlink_status(file,ec).type()!=std::filesystem::file_type::regular||ec)return;
  std::filesystem::remove(file,ec);
  ec.clear();
  std::filesystem::remove(dir,ec); // blocked by nonempty foreign contents
 }
};
}
