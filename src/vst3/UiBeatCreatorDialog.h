#pragma once
// Beat Creator V1.5.2: compact, fixed hardware geometry shared by paint/hit.
// Established Drumator action keycaps: 95 x 25 (EXPORT / PREV / NEXT).
// Only the eight genre captions use wider 136 x 25 caps for legibility.
// All ROW/host heights use the same 640 x 240 hardware panel.
#include "core/UiV101Layout.h"
#include <cstddef>
namespace drumator {
struct UiBeatCreatorDialog final {
 using Rect=UiV101Layout::Rect;
 static constexpr double width=640.,height=240.;
 static constexpr bool compact(double) noexcept {return true;}
 static constexpr Rect panel(double hostHeight) noexcept {
  const double top=(hostHeight-height)/2.;
  return {(UiV101Layout::width-width)/2.,top,
          (UiV101Layout::width+width)/2.,top+height};
 }
 static constexpr Rect title(double h) noexcept {
  const auto p=panel(h);return {p.x0+18.,p.y0+8.,p.x1-18.,p.y0+35.};
 }
 static constexpr Rect styleHeading(double h) noexcept {
  const auto p=panel(h);return {p.x0+27.,p.y0+38.,p.x1-27.,p.y0+53.};
 }
 // The blue recessed mounting bezel and all eight caps use ONE layout.
 // Four 136px caps + three 10px gutters leave 9px on each side;
 // two 25px rows + one 6px gutter leave 3px top and bottom.
 static constexpr Rect styleBezel(double h) noexcept {
  const auto p=panel(h);
  return {p.x0+24.,p.y0+54.,p.x1-24.,p.y0+116.};
 }
 static constexpr Rect style(double h,std::size_t i) noexcept {
  const auto g=styleBezel(h);
  const double left=g.x0+9.+146.*static_cast<double>(i%4u);
  const double top=g.y0+3.+31.*static_cast<double>(i/4u);
  return {left,top,left+136.,top+25.};
 }
 // Complexity on one 25px baseline. Compact [-] 10px [value] 10px [+],
 // equal 35px keys and symmetric centers around the 52px numeric readout.
 static constexpr Rect complexityLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+29.,p.y0+119.,p.x0+166.,p.y0+144.};
 }
 static constexpr Rect complexityMinus(double h) noexcept {
  const auto p=panel(h);return {p.x0+185.,p.y0+119.,p.x0+220.,p.y0+144.};
 }
 static constexpr Rect complexityValue(double h) noexcept {
  const auto p=panel(h);return {p.x0+230.,p.y0+119.,p.x0+282.,p.y0+144.};
 }
 static constexpr Rect complexityPlus(double h) noexcept {
  const auto p=panel(h);return {p.x0+292.,p.y0+119.,p.x0+327.,p.y0+144.};
 }
 static constexpr Rect stepsLabel(double h) noexcept {
  const auto p=panel(h);return {p.x0+450.,p.y0+119.,p.x0+610.,p.y0+144.};
 }
 // Exactly the original Drumator EXPORT/PAGE geometry: 95 x 25.
 static constexpr Rect action(double h,std::size_t i) noexcept {
  const auto p=panel(h);
  const double left=p.x0+62.+105.*static_cast<double>(i);
  return {left,p.y0+152.,left+95.,p.y0+177.};
 }
 static constexpr Rect generate(double h) noexcept {return action(h,0);}
 static constexpr Rect preview(double h,std::size_t i) noexcept {return action(h,i+1u);}
 static constexpr Rect stop(double h) noexcept {return action(h,4);}
 static constexpr Rect notice(double h) noexcept {
  const auto p=panel(h);return {p.x0+28.,p.y0+179.,p.x1-28.,p.y0+201.};
 }
 static constexpr Rect apply(double h) noexcept {
  const auto p=panel(h);return {p.x0+215.,p.y0+207.,p.x0+310.,p.y0+232.};
 }
 static constexpr Rect cancel(double h) noexcept {
  const auto p=panel(h);return {p.x0+330.,p.y0+207.,p.x0+425.,p.y0+232.};
 }
};
static_assert(UiBeatCreatorDialog::panel(254.).y0>=0.&&
              UiBeatCreatorDialog::panel(254.).y1<=254.);
static_assert(UiBeatCreatorDialog::panel(254.).x0==
              UiBeatCreatorDialog::panel(686.).x0);
static_assert(UiBeatCreatorDialog::panel(254.).x1==
              UiBeatCreatorDialog::panel(686.).x1);
static_assert(UiBeatCreatorDialog::generate(254.).x1-
              UiBeatCreatorDialog::generate(254.).x0==95.);
static_assert(UiBeatCreatorDialog::cancel(254.).y1<=
              UiBeatCreatorDialog::panel(254.).y1);
} // namespace drumator
