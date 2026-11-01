#pragma once
// Beat Creator GUI V1.5: ONE 850x240 hardware panel at every host height.
// Coordinate functions are shared by painter and hit testing. The host may
// refuse a resize, leaving only 254px of usable client height: never change
// columns, size, color or control placement when additional ROWs appear.
#include "core/UiV101Layout.h"
#include <cstddef>

namespace drumator {
struct UiBeatCreatorDialog final {
 using Rect=UiV101Layout::Rect;
 static constexpr double width=850.,height=240.;
 static constexpr bool compact(double) noexcept {return true;}
 static constexpr Rect panel(double hostHeight) noexcept {
  const double top=(hostHeight-height)/2.;
  return {215.,top,1065.,top+height};
 }
 static constexpr Rect title(double h) noexcept {
  const auto p=panel(h);return {p.x0+18.,p.y0+8.,p.x1-18.,p.y0+36.};
 }
 static constexpr Rect styleHeading(double h) noexcept {
  const auto p=panel(h);return {p.x0+22.,p.y0+37.,p.x1-22.,p.y0+54.};
 }
 static constexpr Rect style(double h,std::size_t i) noexcept {
  const auto p=panel(h);
  const double left=p.x0+22.+204.*static_cast<double>(i%4u);
  const double top=p.y0+55.+32.*static_cast<double>(i/4u);
  return {left,top,left+192.,top+28.};
 }
 static constexpr Rect complexityLabel(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+22.,p.y0+118.,p.x0+226.,p.y0+145.};
 }
 static constexpr Rect complexityMinus(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+250.,p.y0+118.,p.x0+290.,p.y0+145.};
 }
 static constexpr Rect complexityValue(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+298.,p.y0+118.,p.x0+373.,p.y0+145.};
 }
 static constexpr Rect complexityPlus(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+382.,p.y0+118.,p.x0+422.,p.y0+145.};
 }
 static constexpr Rect stepsLabel(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+606.,p.y0+118.,p.x0+826.,p.y0+145.};
 }
 // Identical 154x30 keycaps for GENERATE, PLAY A/B/C and STOP.
 static constexpr Rect action(double h,std::size_t i) noexcept {
  const auto p=panel(h);
  const double left=p.x0+22.+162.*static_cast<double>(i);
  return {left,p.y0+150.,left+154.,p.y0+180.};
 }
 static constexpr Rect generate(double h) noexcept {return action(h,0);}
 static constexpr Rect preview(double h,std::size_t i) noexcept {
  return action(h,i+1u);
 }
 static constexpr Rect stop(double h) noexcept {return action(h,4);}
 static constexpr Rect notice(double h) noexcept {
  const auto p=panel(h);return {p.x0+22.,p.y0+181.,p.x1-22.,p.y0+202.};
 }
 // Normal 154x30 hardware keycaps, not 398/400px silver fallback bars.
 static constexpr Rect apply(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+265.,p.y0+205.,p.x0+419.,p.y0+235.};
 }
 static constexpr Rect cancel(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+431.,p.y0+205.,p.x0+585.,p.y0+235.};
 }
};
static_assert(UiBeatCreatorDialog::panel(254.).y0>=0. &&
              UiBeatCreatorDialog::panel(254.).y1<=254.);
static_assert(UiBeatCreatorDialog::cancel(254.).y1<
              UiBeatCreatorDialog::panel(254.).y1);
static_assert(UiBeatCreatorDialog::stop(254.).x1<
              UiBeatCreatorDialog::panel(254.).x1);
static_assert(UiBeatCreatorDialog::panel(254.).x0==
              UiBeatCreatorDialog::panel(686.).x0 &&
              UiBeatCreatorDialog::panel(254.).x1==
              UiBeatCreatorDialog::panel(686.).x1);
} // namespace drumator
