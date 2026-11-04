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
 static constexpr double width=820.,height=252.;
 static constexpr Rect panel(double h) noexcept {
  const double top=(h-height)*.5;
  return {(UiV101Layout::width-width)*.5,top,
          (UiV101Layout::width+width)*.5,top+height};
 }
 static constexpr Rect title(double h) noexcept {
  const auto p=panel(h);return {p.x0+20.,p.y0+9.,p.x1-20.,p.y0+36.};
 }
 static constexpr Rect styleLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+24.,p.y0+45.,p.x0+96.,p.y0+72.};
 }
 static constexpr Rect styleSelect(double h) noexcept {
  const auto p=panel(h);return {p.x0+162.,p.y0+44.,p.x0+458.,p.y0+72.};
 }
 static constexpr Rect notice(double h) noexcept {
  const auto p=panel(h);return {p.x0+472.,p.y0+44.,p.x1-24.,p.y0+72.};
 }
 // The modal style list is a two-column dark selector, not 286px stretched
 // hardware buttons. The full 10-item list fits even the 254px host viewport.
 static constexpr Rect styleMenu(double h) noexcept {
  const auto p=panel(h);return {p.x0+24.,p.y0+74.,p.x1-24.,p.y0+211.};
 }
 // BC52: shared painted/clickable geometry for the two-column selector.
 // Equal 377px columns, 5px left/right insets and 8px center gap.
 static constexpr Rect styleOption(double h,std::size_t i) noexcept {
  const auto p=styleMenu(h);
  constexpr double inset=5.,gutter=8.;
  const double cellWidth=(p.x1-p.x0-2.*inset-gutter)*.5;
  const double x=p.x0+inset+static_cast<double>(i/5u)*(cellWidth+gutter);
  const double y=p.y0+4.+static_cast<double>(i%5u)*25.;
  return {x,y,x+cellWidth,y+23.};
 }
 static constexpr Rect complexityLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+24.,p.y0+81.,p.x0+156.,p.y0+108.};
 }
 static constexpr Rect complexityMinus(double h) noexcept {
  const auto p=panel(h);return {p.x0+162.,p.y0+82.,p.x0+198.,p.y0+108.};
 }
 static constexpr Rect complexityValue(double h) noexcept {
  const auto p=panel(h);return {p.x0+207.,p.y0+82.,p.x0+263.,p.y0+108.};
 }
 static constexpr Rect complexityPlus(double h) noexcept {
  const auto p=panel(h);return {p.x0+272.,p.y0+82.,p.x0+308.,p.y0+108.};
 }
 static constexpr Rect lengthLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+490.,p.y0+82.,p.x0+579.,p.y0+108.};
 }
 // Native 83x32 Drumator caps: no horizontally stretched select control.
 static constexpr Rect barOne(double h) noexcept {
  const auto p=panel(h);return {p.x0+595.,p.y0+79.,p.x0+678.,p.y0+111.};
 }
 static constexpr Rect barTwo(double h) noexcept {
  const auto p=panel(h);return {p.x0+690.,p.y0+79.,p.x0+773.,p.y0+111.};
 }
 // Keep the existing A/B/C summary preview, using all space left of the legend.
 static constexpr Rect previewRow(double h,std::size_t i) noexcept {
  const auto p=panel(h);
  const double top=p.y0+120.+29.*static_cast<double>(i);
  return {p.x0+24.,top,p.x1-164.,top+21.};
 }
 static constexpr double previewStepPitch(double h) noexcept {
  const auto r=previewRow(h,0);return (r.x1-r.x0-151.)/32.;
 }
 static constexpr Rect previewStep(double h,std::size_t pat,std::size_t step) noexcept {
  const auto r=previewRow(h,pat);
  const double x=r.x0+138.+previewStepPitch(h)*static_cast<double>(step);
  return {x,r.y0+4.,x+11.,r.y1-4.};
 }
 // One vertical, compact eight-instrument legend; no redundant footer legend.
 static constexpr Rect legendPanel(double h) noexcept {
  const auto p=panel(h);return {p.x1-152.,p.y0+116.,p.x1-22.,p.y0+216.};
 }
 static constexpr Rect legend(double h,std::size_t row) noexcept {
  const auto p=legendPanel(h);
  const double top=p.y0+3.+12.*static_cast<double>(row);
  return {p.x0+8.,top,p.x1-6.,top+11.};
 }
 static constexpr Rect generate(double h) noexcept {
  const auto p=panel(h);return {p.x0+245.,p.y0+221.,p.x0+340.,p.y0+246.};
 }
 static constexpr Rect apply(double h) noexcept {
  const auto p=panel(h);return {p.x0+362.,p.y0+221.,p.x0+457.,p.y0+246.};
 }
 static constexpr Rect cancel(double h) noexcept {
  const auto p=panel(h);return {p.x0+479.,p.y0+221.,p.x0+574.,p.y0+246.};
 }
 // BC65: user explicitly chose original +/- 5-point steps (start at 35).
 // Clamp to 0..100; direct entry accepts any integer in that range.
 static constexpr std::uint8_t steppedComplexity(std::uint8_t old,int dir) noexcept {
  const auto delta=dir<0?-5:dir>0?5:0;
  const auto value=static_cast<int>(old)+delta;
  return static_cast<std::uint8_t>(value<0?0:value>100?100:value);
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
static_assert(UiBeatCreatorDialog::legend(254.,7).y1<
              UiBeatCreatorDialog::generate(254.).y0);
static_assert(UiBeatCreatorDialog::previewRow(254.,2).x1+12.<=
              UiBeatCreatorDialog::legendPanel(254.).x0);
static_assert(UiBeatCreatorDialog::previewStep(254.,0,31).x1<
              UiBeatCreatorDialog::previewRow(254.,0).x1);
} // namespace drumator
