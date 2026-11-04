#pragma once
// F06: the same real Windows CF_HDROP file package used by the live View.
// Multiple independent MIDI paths, never a folder or one 17-track SMF.
#include "core/DragExportFile.h"
#include "core/Utf8Path.h"
#include "vstgui/lib/cdropsource.h"
#include <cstdint>
#include <limits>
#include <string>
namespace drumator {
struct MidiDragPackage final {
 static VSTGUI::SharedPointer<VSTGUI::IDataPackage> create(
     const std::filesystem::path& choice) {
  const auto paths=DragExportFile::existingMidiRows(choice);
  if(paths.empty())return {};
  auto pkg=VSTGUI::makeOwned<VSTGUI::CDropSource>();
  if(!pkg)return {};
  for(const auto& file:paths){
   const auto utf8=pathToUtf8(file);
   if(utf8.empty()||utf8.find('\0')!=std::string::npos||
      utf8.size()>=std::numeric_limits<std::uint32_t>::max()||
      !pkg->add(utf8.c_str(),static_cast<std::uint32_t>(utf8.size()+1),
                VSTGUI::IDataPackage::kFilePath))return {};
  }
  if(pkg->getCount()!=paths.size())return {};
  for(std::uint32_t i=0;i<pkg->getCount();++i){
   const void* buffer=nullptr;
   VSTGUI::IDataPackage::Type type=VSTGUI::IDataPackage::kError;
   const auto bytes=pkg->getData(i,buffer,type);
   if(!buffer||bytes<2||type!=VSTGUI::IDataPackage::kFilePath||
      static_cast<const char*>(buffer)[bytes-1]!='\0')return {};
  }
  return pkg;
 }
};
}
