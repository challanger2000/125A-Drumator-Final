#pragma once
// 125A Drumator - DAW-independent kit catalogue and portable .125akit format.
// A .125akit is the existing, checksum-verified KitCodec payload. Only its
// instrument assets are applied on load; steps/transport/song are NEVER loaded.
#include "core/KitState.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
namespace drumator {
class KitPresetCatalog final {
public:
 struct Entry {std::filesystem::path path; std::string title;};
 static constexpr std::size_t maxEntries=128;
 static constexpr const char* extension=".125akit";
 static std::filesystem::path bundleDirectory() noexcept {
#if defined(_WIN32)
  try {
   HMODULE module{};
   // This static function belongs to the same module as the controller UI.
   if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|
         GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
         reinterpret_cast<LPCWSTR>(&KitPresetCatalog::bundleDirectory),&module))
     return {};
   wchar_t path[32768]{};
   const DWORD n=GetModuleFileNameW(module,path,32768);
   if(!n||n>=32768)return {};
   const auto bin=std::filesystem::path(path).parent_path();
   return bin.parent_path()/"Resources"/"Samples";
  }catch(...){return {};}
#else
  return {};
#endif
 }
 static std::vector<Entry> list(const std::filesystem::path& root) {
  std::vector<Entry> out;
  std::error_code ec;
  if(!std::filesystem::is_directory(root,ec)||ec)return out;
  for(std::filesystem::directory_iterator it(root,ec),end;!ec&&it!=end;
      it.increment(ec)) {
   if(!it->is_regular_file(ec)){if(ec)break;continue;}
   auto name=it->path().filename().string();
   auto suffix=it->path().extension().string();
   std::transform(suffix.begin(),suffix.end(),suffix.begin(),
       [](unsigned char c){return static_cast<char>(std::tolower(c));});
   if(suffix!=extension)continue;
   if(out.size()==maxEntries)break;
   out.push_back({it->path(),it->path().stem().string()});
  }
  std::sort(out.begin(),out.end(),[](const Entry& a,const Entry& b){
   return a.title<b.title;
  });
  return out;
 }
 static bool read(const std::filesystem::path& file,KitState& output) {
  try {
   std::error_code ec;
   if(!std::filesystem::is_regular_file(file,ec)||ec)return false;
   const auto size=std::filesystem::file_size(file,ec);
   if(ec||size<16||size>kMaxKitBytes)return false;
   std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
   std::ifstream input(file,std::ios::binary);
   if(!input||!input.read(reinterpret_cast<char*>(bytes.data()),
                       static_cast<std::streamsize>(bytes.size())))return false;
   KitState tmp;
   if(!KitCodec::decode(bytes.data(),bytes.size(),tmp))return false;
   output=std::move(tmp);
   return true;
  }catch(...){return false;}
 }
 static bool save(const std::filesystem::path& file,const KitState& state) {
  try {
   std::vector<std::uint8_t> bytes;
   if(!KitCodec::encode(state,bytes))return false;
   std::ofstream out(file,std::ios::binary|std::ios::trunc);
   if(!out)return false;
   out.write(reinterpret_cast<const char*>(bytes.data()),
             static_cast<std::streamsize>(bytes.size()));
   return out.good();
  }catch(...){return false;}
 }
};
} // namespace drumator
