#pragma once
// 125A Drumator Modern: GUI-only palette and user preference.
// Deliberately independent of VST3 ParamIDs, DSP, automation and kit state.
#include "vstgui/lib/ccolor.h"
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace drumator::v101 {
enum class ModernTheme : std::uint8_t { Graphite=0, Slate=1, Silver=2, Classic=3 };

struct ModernPalette final {
 VSTGUI::CColor chassis, header, rowA, rowB, well, edge, button, label, dim,
                  orange, orangeLight, cyan, green;
};
inline ModernPalette modernPalette(ModernTheme theme) noexcept {
 using C=VSTGUI::CColor;
 switch(theme){
  case ModernTheme::Graphite:
   return {C{45,54,64,255},C{52,63,75,255},C{59,72,84,255},
           C{63,76,89,255},C{29,40,53,255},C{101,118,132,255},
           C{87,102,116,255},C{234,239,243,255},C{174,190,201,255},
           C{244,134,38,255},C{255,189,86,255},C{71,180,209,255},
           C{70,186,121,255}};
  case ModernTheme::Silver:
   return {C{191,200,206,255},C{86,100,113,255},C{108,124,136,255},
           C{113,129,140,255},C{53,69,82,255},C{122,143,156,255},
           C{139,154,165,255},C{249,251,252,255},C{212,225,232,255},
           C{235,126,32,255},C{255,192,94,255},C{74,185,214,255},
           C{67,183,119,255}};
  case ModernTheme::Slate:
  default:
   return {C{115,126,136,255},C{88,105,118,255},C{80,97,109,255},
           C{85,102,114,255},C{42,58,71,255},C{131,151,163,255},
           C{118,135,147,255},C{243,248,251,255},C{192,209,218,255},
           C{243,140,43,255},C{255,198,94,255},C{67,188,213,255},
           C{65,183,122,255}};
 }
}

struct ModernDrawStyle final {
 // Scoped setting is restored even if VSTGUI paint throws. Each editor selects
 // its own palette; interleaved editors cannot leak colors across instances.
 static inline thread_local ModernTheme current=ModernTheme::Classic;
 struct Scope final {
  ModernTheme previous;
  explicit Scope(ModernTheme selected) noexcept:previous(current){current=selected;}
  ~Scope(){current=previous;}
  Scope(const Scope&)=delete;
  Scope& operator=(const Scope&)=delete;
 };
 static bool enabled() noexcept{return current!=ModernTheme::Classic;}
 static ModernPalette colors() noexcept{return modernPalette(current);}
};

// GUI-only per-user preference, never serialized into a drum kit or VST3
// processor state. Disk access occurs solely in editor create/click handlers.
inline std::filesystem::path modernThemePreferencePath() {
 const char* root=std::getenv("LOCALAPPDATA");
 if(!root||!*root)return {};
 return std::filesystem::path(root)/"125A Systems"/"Drumator Modern.theme";
}
inline ModernTheme loadModernThemePreference() noexcept {
 try{
  const auto path=modernThemePreferencePath();
  if(path.empty())return ModernTheme::Slate;
  std::ifstream in(path,std::ios::binary);
  char ch=0;
  if(in.get(ch)&&ch>='0'&&ch<='2')
   return static_cast<ModernTheme>(ch-'0');
 }catch(...){}
 return ModernTheme::Slate;
}
inline bool saveModernThemePreference(ModernTheme theme) noexcept {
 if(theme==ModernTheme::Classic)return false;
 try{
  const auto path=modernThemePreferencePath();
  if(path.empty())return false;
  std::error_code ec;
  std::filesystem::create_directories(path.parent_path(),ec);
  if(ec)return false;
  std::ofstream out(path,std::ios::binary|std::ios::trunc);
  if(!out)return false;
  out.put(static_cast<char>('0'+static_cast<int>(theme)));
  out.flush();
  return out.good();
 }catch(...){return false;}
}
} // namespace drumator::v101
