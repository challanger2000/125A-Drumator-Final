#pragma once
// One modal surface for Stage 5. Every painted rectangle has a matching hitbox.
// Pure geometry; no duplicate sequencer and no persistent GUI state.
#include "core/UiV101Layout.h"
#include <cstddef>

namespace drumator {
struct UiBeatCreatorDialog final {
 using Rect=UiV101Layout::Rect;
 static constexpr Rect panel(double height) noexcept {
  const double top=(height-390.)/2.;
  return {330.,top,950.,top+390.};
 }
 static constexpr Rect style(double height,std::size_t i) noexcept {
  const auto p=panel(height);
  const double left=p.x0+26.+294.*static_cast<double>(i/4u);
  const double top=p.y0+78.+35.*static_cast<double>(i%4u);
  return {left,top,left+276.,top+30.};
 }
 static constexpr Rect complexityMinus(double h)noexcept{
  const auto p=panel(h);return {p.x0+374.,p.y0+231.,p.x0+416.,p.y0+262.};
 }
 static constexpr Rect complexityPlus(double h)noexcept{
  const auto p=panel(h);return {p.x0+499.,p.y0+231.,p.x0+541.,p.y0+262.};
 }
 static constexpr Rect generate(double h)noexcept{
  const auto p=panel(h);return {p.x0+26.,p.y0+277.,p.x0+170.,p.y0+311.};
 }
 static constexpr Rect preview(double h,std::size_t i)noexcept{
  const auto p=panel(h);
  const double x=p.x0+180.+111.*static_cast<double>(i);
  return {x,p.y0+277.,x+105.,p.y0+311.};
 }
 static constexpr Rect stop(double h)noexcept{
  const auto p=panel(h);return {p.x0+513.,p.y0+277.,p.x0+594.,p.y0+311.};
 }
 static constexpr Rect apply(double h)noexcept{
  const auto p=panel(h);return {p.x0+26.,p.y0+340.,p.x0+304.,p.y0+374.};
 }
 static constexpr Rect cancel(double h)noexcept{
  const auto p=panel(h);return {p.x0+316.,p.y0+340.,p.x0+594.,p.y0+374.};
 }
};
} // namespace drumator
