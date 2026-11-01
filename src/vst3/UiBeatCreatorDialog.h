#pragma once
// Stage 5.1: one adaptive modal whose painted geometry is also used for input.
// A 10-row VST3 editor can be 686px high, but a host-constrained peer/editor
// is legally as small as 158 header + 48 row + 48 footer = 254px.
// A 420px minimum was incorrect: keep every control accessible at 254px.
#include "core/UiV101Layout.h"
#include <cstddef>

namespace drumator {
struct UiBeatCreatorDialog final {
 using Rect=UiV101Layout::Rect;
 static constexpr bool compact(double height) noexcept {return height<420.;}
 static constexpr Rect panel(double height) noexcept {
  if(compact(height)){
   const double top=(height-240.)/2.;
   return {215.,top,1065.,top+240.};
  }
  const double top=(height-390.)/2.;
  return {330.,top,950.,top+390.};
 }
 static constexpr Rect title(double height)noexcept {
  const auto p=panel(height);
  return compact(height)?
   Rect{p.x0+18.,p.y0+4.,p.x1-18.,p.y0+31.}:
   Rect{p.x0+18.,p.y0+12.,p.x1-18.,p.y0+37.};
 }
 static constexpr Rect styleHeading(double height)noexcept{
  const auto p=panel(height);
  return compact(height)?
   Rect{p.x0+22.,p.y0+30.,p.x1-22.,p.y0+44.}:
   Rect{p.x0+27.,p.y0+48.,p.x1-27.,p.y0+73.};
 }
 static constexpr Rect style(double height,std::size_t i) noexcept {
  const auto p=panel(height);
  if(compact(height)){
   const double left=p.x0+22.+206.*static_cast<double>(i%4u);
   const double top=p.y0+45.+33.*static_cast<double>(i/4u);
   return {left,top,left+198.,top+28.};
  }
  const double left=p.x0+26.+294.*static_cast<double>(i/4u);
  const double top=p.y0+78.+35.*static_cast<double>(i%4u);
  return {left,top,left+276.,top+30.};
 }
 static constexpr Rect complexityLabel(double height)noexcept{
  const auto p=panel(height);
  return compact(height)?
   Rect{p.x0+20.,p.y0+113.,p.x0+234.,p.y0+141.}:
   Rect{p.x0+25.,p.y0+233.,p.x0+260.,p.y0+261.};
 }
 static constexpr Rect complexityMinus(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+250.,p.y0+113.,p.x0+290.,p.y0+142.}:
   Rect{p.x0+374.,p.y0+231.,p.x0+416.,p.y0+262.};
 }
 static constexpr Rect complexityValue(double h)noexcept {
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+295.,p.y0+113.,p.x0+374.,p.y0+141.}:
   Rect{p.x0+420.,p.y0+233.,p.x0+495.,p.y0+260.};
 }
 static constexpr Rect complexityPlus(double h)noexcept {
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+380.,p.y0+113.,p.x0+420.,p.y0+142.}:
   Rect{p.x0+499.,p.y0+231.,p.x0+541.,p.y0+262.};
 }
 static constexpr Rect stepsLabel(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+500.,p.y0+113.,p.x1-18.,p.y0+141.}:
   Rect{p.x0+542.,p.y0+233.,p.x1-20.,p.y0+260.};
 }
 static constexpr Rect generate(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+22.,p.y0+150.,p.x0+166.,p.y0+180.}:
   Rect{p.x0+26.,p.y0+277.,p.x0+170.,p.y0+311.};
 }
 static constexpr Rect preview(double h,std::size_t i)noexcept{
  const auto p=panel(h);
  if(compact(h)){
   const double x=p.x0+174.+133.*static_cast<double>(i);
   return {x,p.y0+150.,x+125.,p.y0+180.};
  }
  const double x=p.x0+180.+111.*static_cast<double>(i);
  return {x,p.y0+277.,x+105.,p.y0+311.};
 }
 static constexpr Rect stop(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+580.,p.y0+150.,p.x0+828.,p.y0+180.}:
   Rect{p.x0+513.,p.y0+277.,p.x0+594.,p.y0+311.};
 }
 static constexpr Rect notice(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+22.,p.y0+182.,p.x1-22.,p.y0+202.}:
   Rect{p.x0+26.,p.y0+314.,p.x1-26.,p.y0+336.};
 }
 static constexpr Rect apply(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+22.,p.y0+205.,p.x0+420.,p.y0+235.}:
   Rect{p.x0+26.,p.y0+340.,p.x0+304.,p.y0+374.};
 }
 static constexpr Rect cancel(double h)noexcept{
  const auto p=panel(h);
  return compact(h)?
   Rect{p.x0+428.,p.y0+205.,p.x0+828.,p.y0+235.}:
   Rect{p.x0+316.,p.y0+340.,p.x0+594.,p.y0+374.};
 }
};
static_assert(UiBeatCreatorDialog::panel(254.).y0>=0.&&
              UiBeatCreatorDialog::panel(254.).y1<=254.);
static_assert(UiBeatCreatorDialog::cancel(254.).y1<
              UiBeatCreatorDialog::panel(254.).y1);
static_assert(UiBeatCreatorDialog::stop(254.).x1<
              UiBeatCreatorDialog::panel(254.).x1);
} // namespace drumator
