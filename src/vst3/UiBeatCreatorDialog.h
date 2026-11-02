#pragma once
// BC42: one compact ten-style dropdown, three read-only pattern previews.
// Geometry is shared by mouse hit testing and native hardware painting.
// Deliberately no bundled kit, preview sample or sound browser.
#include "core/UiV101Layout.h"
#include <cstddef>
#include <cstdint>
namespace drumator {
struct UiBeatCreatorDialog final {
 using Rect=UiV101Layout::Rect;
 static constexpr double width=640.,height=240.;
 static constexpr Rect panel(double h) noexcept {
  const double top=(h-height)*.5;
  return {(UiV101Layout::width-width)*.5,top,
          (UiV101Layout::width+width)*.5,top+height};
 }
 static constexpr Rect title(double h) noexcept {
  const auto p=panel(h);return {p.x0+20.,p.y0+9.,p.x0+335.,p.y0+36.};
 }
 static constexpr Rect styleSelect(double h) noexcept {
  const auto p=panel(h);return {p.x0+24.,p.y0+42.,p.x0+335.,p.y0+69.};
 }
 static constexpr Rect notice(double h) noexcept {
  const auto p=panel(h);return {p.x0+350.,p.y0+42.,p.x1-22.,p.y0+69.};
 }
 static constexpr Rect styleMenu(double h) noexcept {
  const auto p=panel(h);return {p.x0+24.,p.y0+70.,p.x1-24.,p.y0+193.};
 }
 static constexpr Rect styleOption(double h,std::size_t i) noexcept {
  const auto p=styleMenu(h);
  const double x=p.x0+4.+(i/5u)*294.;
  const double y=p.y0+2.+(i%5u)*24.;
  return {x,y,x+286.,y+23.};
 }
 static constexpr Rect complexityLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+25.,p.y0+78.,p.x0+160.,p.y0+103.};
 }
 static constexpr Rect complexityMinus(double h) noexcept {
  const auto p=panel(h);return {p.x0+166.,p.y0+78.,p.x0+202.,p.y0+103.};
 }
 static constexpr Rect complexityValue(double h) noexcept {
  const auto p=panel(h);return {p.x0+212.,p.y0+78.,p.x0+268.,p.y0+103.};
 }
 static constexpr Rect complexityPlus(double h) noexcept {
  const auto p=panel(h);return {p.x0+278.,p.y0+78.,p.x0+314.,p.y0+103.};
 }
 static constexpr Rect stepsLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+388.,p.y0+78.,p.x1-25.,p.y0+103.};
 }
 static constexpr Rect previewRow(double h,std::size_t i) noexcept {
  const auto p=panel(h);
  const double top=p.y0+109.+23.*static_cast<double>(i);
  return {p.x0+25.,top,p.x1-25.,top+21.};
 }
 static constexpr Rect legend(double h,unsigned row) noexcept {
  const auto p=panel(h);const double top=p.y0+182.+11.*row;
  return {p.x0+24.,top,p.x1-24.,top+11.};
 }
 static constexpr Rect generate(double h) noexcept {
  const auto p=panel(h);return {p.x0+157.,p.y0+210.,p.x0+252.,p.y0+235.};
 }
 static constexpr Rect apply(double h) noexcept {
  const auto p=panel(h);return {p.x0+272.,p.y0+210.,p.x0+367.,p.y0+235.};
 }
 static constexpr Rect cancel(double h) noexcept {
  const auto p=panel(h);return {p.x0+387.,p.y0+210.,p.x0+482.,p.y0+235.};
 }
 static bool parseComplexity(const char* t,std::uint8_t& out) noexcept {
  if(!t)return false;
  const auto ws=[](char c){return c==' '||c=='\t';};
  while(ws(*t))++t;
  if(*t<'0'||*t>'9')return false;
  unsigned n=0;
  while(*t>='0'&&*t<='9'){
   n=n*10u+static_cast<unsigned>(*t++-'0');
   if(n>100u)return false;
  }
  while(ws(*t))++t;
  if(*t=='%')++t;
  while(ws(*t))++t;
  if(*t!='\0')return false;
  out=static_cast<std::uint8_t>(n);
  return true;
 }
};
static_assert(UiBeatCreatorDialog::panel(254.).y0>=0.);
static_assert(UiBeatCreatorDialog::panel(254.).y1<=254.);
static_assert(UiBeatCreatorDialog::styleOption(254.,9).y1<=
              UiBeatCreatorDialog::styleMenu(254.).y1);
static_assert(UiBeatCreatorDialog::styleOption(254.,9).x1<=
              UiBeatCreatorDialog::styleMenu(254.).x1);
static_assert(UiBeatCreatorDialog::legend(254.,1).y1<
              UiBeatCreatorDialog::generate(254.).y0);
} // namespace drumator
