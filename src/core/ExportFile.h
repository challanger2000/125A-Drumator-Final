#pragma once
// Control-thread-only transactional output with an exclusively owned scratch
// directory. No exporter may truncate or remove another export's scratch file.
// All paths are UTF-16-safe std::filesystem::path on Windows.
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <utility>
#include <system_error>
#include <atomic>
#include <chrono>
#include <string>
#include <cstdint>
namespace drumator {
class ExportFile final {
public:
  explicit ExportFile(std::filesystem::path path)
    : target_(std::move(path)),backup_(target_) {
    backup_ += ".125a-export-backup";
  }
  ExportFile(const ExportFile&)=delete;
  ExportFile& operator=(const ExportFile&)=delete;
  ~ExportFile(){cleanup();}
  bool open(){
    if(opened_||owned_||target_.empty())return false;
    // create_directory is atomic and fails if the path already exists.
    // Each exporter creates its own unique folder beside the destination;
    // its payload and cleanup stay strictly inside this owned folder.
    static std::atomic<std::uint64_t> seq{0};
    const auto tick=static_cast<std::uint64_t>(
      std::chrono::steady_clock::now().time_since_epoch().count());
    for(unsigned attempt=0;attempt<32;++attempt){
      const auto id=seq.fetch_add(1,std::memory_order_relaxed);
      directory_=target_;
      directory_ += ".125a-export-part-" + std::to_string(tick) +
                    "-" + std::to_string(id);
      std::error_code ec;
      if(std::filesystem::create_directory(directory_,ec)){
        owned_=true;break;
      }
      if(ec!=std::errc::file_exists)return false;
    }
    if(!owned_)return false;
    temp_=directory_/"payload";
    file_.open(temp_,std::ios::binary|std::ios::out|std::ios::trunc);
    opened_=file_.is_open();
    return opened_;
  }
  bool write(const void* bytes,std::size_t len){
    if(!file_.is_open()||(!bytes&&len))return false;
    if(len)file_.write(static_cast<const char*>(bytes),
                      static_cast<std::streamsize>(len));
    return static_cast<bool>(file_);
  }
  bool seek(std::streamoff offset){
    if(!file_.is_open())return false;
    file_.seekp(offset,std::ios::beg);
    return static_cast<bool>(file_);
  }
  bool finish(){
    if(!file_.is_open())return false;
    file_.flush();const bool good=static_cast<bool>(file_);
    file_.close();if(!good||file_.fail())return false;
    std::error_code ec;
    const bool existing=std::filesystem::exists(target_,ec);
    if(ec)return false;
    if(existing){
      if(std::filesystem::exists(backup_,ec)||ec)return false;
      std::filesystem::rename(target_,backup_,ec);
      if(ec)return false;
    }
    std::filesystem::rename(temp_,target_,ec);
    if(ec){
      if(existing){
        std::error_code rollback;
        std::filesystem::rename(backup_,target_,rollback);
      }
      return false;
    }
    complete_=true;
    cleanup();
    if(existing){
      std::error_code unused;
      std::filesystem::remove(backup_,unused);
    }
    return true;
  }
private:
  void cleanup()noexcept{
    file_.close();
    if(!owned_)return; // Never touch a pre-existing or foreign scratch path.
    std::error_code ec;
    if(!complete_)std::filesystem::remove(temp_,ec);
    ec.clear();
    std::filesystem::remove(directory_,ec);
    owned_=false;
  }
  std::filesystem::path target_,temp_,backup_,directory_;
  std::ofstream file_{};
  bool owned_{false},opened_{false},complete_{false};
};
}
