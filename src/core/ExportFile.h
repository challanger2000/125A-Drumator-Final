#pragma once
// Control-thread-only transactional file output. A failed or aborted render
// never leaves a partial file at the intended destination.
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <utility>
#include <system_error>
namespace drumator {
class ExportFile final {
public:
  explicit ExportFile(std::filesystem::path path)
    : target_(std::move(path)), temp_(target_),backup_(target_) {
    temp_ += ".125a-export-part";
    backup_ += ".125a-export-backup";
  }
  ExportFile(const ExportFile&)=delete;
  ExportFile& operator=(const ExportFile&)=delete;
  ~ExportFile(){
    file_.close();
    if(!complete_){std::error_code ec;std::filesystem::remove(temp_,ec);}
  }
  bool open() {
    std::error_code ec;
    if(target_.empty()||temp_.empty()||
       std::filesystem::exists(temp_,ec)||ec)return false;
    file_.open(temp_,std::ios::binary|std::ios::out|std::ios::trunc);
    return file_.is_open();
  }
  bool write(const void* bytes,std::size_t len){
    if(!file_.is_open()||(!bytes&&len))return false;
    if(len){file_.write(static_cast<const char*>(bytes),
                        static_cast<std::streamsize>(len));}
    return static_cast<bool>(file_);
  }
  bool seek(std::streamoff offset){
    if(!file_.is_open())return false;
    file_.seekp(offset,std::ios::beg);
    return static_cast<bool>(file_);
  }
  bool finish() {
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
    if(existing){std::error_code unused;std::filesystem::remove(backup_,unused);}
    return true;
  }
private:
  std::filesystem::path target_,temp_,backup_;
  std::ofstream file_{};
  bool complete_{false};
};
}
