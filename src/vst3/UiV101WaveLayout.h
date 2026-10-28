#pragma once
#include "core/UiWaveEditorLayout.h"
#include <array>
#include <cstddef>

namespace drumator {
// V1.0.1-only Wave toolbar skin and mouse geometry. The proven
// UiWaveEditorLayout remains unchanged for the legacy 1.0.0 test view.
struct UiV101WaveLayout final {
 using Rect=UiWaveEditorLayout::Rect;
 static constexpr double toolbarX=270.,buttonPitch=94.;
 static constexpr double buttonWidth=86.,buttonTop=457.,buttonBottom=489.;
 static constexpr std::array<Rect,8> toolbar{{
   {270.,buttonTop,356.,buttonBottom},
   {364.,buttonTop,450.,buttonBottom},
   {458.,buttonTop,544.,buttonBottom},
   {552.,buttonTop,638.,buttonBottom},
   {646.,buttonTop,732.,buttonBottom},
   {740.,buttonTop,826.,buttonBottom},
   {834.,buttonTop,920.,buttonBottom},
   {928.,buttonTop,1014.,buttonBottom}
 }};
 // Four identical navy mounting bezels, grouped by actual function:
 // view (ZOOM+/ZOOM-/FIT), audition (PREVIEW/STOP), RESET,
 // and commit (CANCEL/APPLY). Draw-only: each key retains its click box.
 // The 2px clearance between bezels ensures no merged outlines.
 static constexpr std::array<Rect,4> toolbarGroups{{
   {267.,453.,547.,494.},
   {549.,453.,735.,494.},
   {737.,453.,829.,494.},
   {831.,453.,1017.,494.}
 }};
 static constexpr int buttonAt(double x,double y) noexcept {
   for(std::size_t i=0;i<toolbar.size();++i)
     if(toolbar[i].hit(x,y))return static_cast<int>(i);
   return -1;
 }
 // Five equal inset readouts replace free-floating parameter text.
 // Label/value alignment is shared by the Windows pixel oracle.
 static constexpr Rect readout(std::size_t i) noexcept {
   const double x=265.+153.*static_cast<double>(i);
   return {x,499.,x+140.,545.};
 }
};
static_assert(UiV101WaveLayout::toolbar.size()==8);
static_assert(UiV101WaveLayout::toolbar[0].width()==86.);
static_assert(UiV101WaveLayout::toolbar[7].width()==86.);
static_assert(UiV101WaveLayout::toolbar[7].right<1050.);
static_assert(UiV101WaveLayout::toolbar[0].bottom-UiV101WaveLayout::toolbar[0].top==32.);
static_assert(UiV101WaveLayout::toolbar[1].left-UiV101WaveLayout::toolbar[0].right==8.);
static_assert(UiV101WaveLayout::buttonAt(313.,473.)==0);
static_assert(UiV101WaveLayout::buttonAt(971.,473.)==7);
static_assert(UiV101WaveLayout::buttonAt(360.,473.)==-1);
static_assert(UiV101WaveLayout::toolbarGroups.size()==4);
static_assert(UiV101WaveLayout::toolbarGroups[0].right+2.==
              UiV101WaveLayout::toolbarGroups[1].left);
static_assert(UiV101WaveLayout::toolbarGroups[1].right+2.==
              UiV101WaveLayout::toolbarGroups[2].left);
static_assert(UiV101WaveLayout::toolbarGroups[2].right+2.==
              UiV101WaveLayout::toolbarGroups[3].left);
static_assert(UiV101WaveLayout::toolbarGroups[0].left<
              UiV101WaveLayout::toolbar[0].left&&
              UiV101WaveLayout::toolbarGroups[0].right>
              UiV101WaveLayout::toolbar[2].right);
static_assert(UiV101WaveLayout::toolbarGroups[1].left<
              UiV101WaveLayout::toolbar[3].left&&
              UiV101WaveLayout::toolbarGroups[1].right>
              UiV101WaveLayout::toolbar[4].right);
static_assert(UiV101WaveLayout::toolbarGroups[2].left<
              UiV101WaveLayout::toolbar[5].left&&
              UiV101WaveLayout::toolbarGroups[2].right>
              UiV101WaveLayout::toolbar[5].right);
static_assert(UiV101WaveLayout::toolbarGroups[3].left<
              UiV101WaveLayout::toolbar[6].left&&
              UiV101WaveLayout::toolbarGroups[3].right>
              UiV101WaveLayout::toolbar[7].right);
static_assert(UiV101WaveLayout::toolbarGroups[0].top<
              UiV101WaveLayout::buttonTop &&
              UiV101WaveLayout::toolbarGroups[0].bottom>
              UiV101WaveLayout::buttonBottom);
static_assert(UiV101WaveLayout::readout(4).right<1050.);
} // namespace drumator
