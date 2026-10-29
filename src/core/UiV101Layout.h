#pragma once
// V1.0.1 independent GUI geometry prototype. V1.0.0 view is untouched.
// The painter and input handler must consume these identical rectangles.
#include <array>
#include <cstddef>
#include <cstdint>
#include "core/UiCompactLayout.h"

namespace drumator {
struct UiV101Layout final {
  static constexpr std::size_t instruments=16,visibleRows=10,steps=32;
  static constexpr double width=1280.,header=158.,rowHeight=48.,expansionHeight=48.,footer=48.;
  static constexpr double stepX=420.,stepPitch=19.,stepEnd=stepX+steps*stepPitch;
  // Compact footer: exact same rectangles are used for paint and mouse hits.
  // The raised page switches stay inside the inset panel, away from the
  // exposed corner screws and the instrument counter.
  // Full-width lower chassis strip, matching the top/control fascia.
  // Repaint both bottom screw heads ON TOP of the inset in the renderer.
  static constexpr double footerPanelLeft=8.,footerPanelRight=1272.;
  static constexpr double footerPanelTop=4.,footerPanelBottom=38.;

  // Only SONG and SONG EDIT are buttons. The existing SWING, HUMANIZE,
  // MASTER controls are rotary encoders, not further menu buttons.
  static constexpr std::size_t headerGroupCount=2;
  // Zoom presets are the actual existing Drumator choices, never visual guesses.
  static constexpr auto zoomPresets=UiCompactLayout::userZooms;
  static constexpr double knobRadius=18.;
  static constexpr std::array<double,3> knobCenters{{1062.,1122.,1182.}};
  struct Rect final {
    double x0,y0,x1,y1;
    constexpr bool hit(double x,double y)const noexcept{
      return x>=x0&&x<x1&&y>=y0&&y<y1;
    }
  };
  // Safe inside the upper-left graphite panel, measured against the
  // original industrial faceplate at 150%. The top metallic highlight
  // and the black stepped bevel below the logo must remain untouched.
  // Applies to the COMPLETE official 125A SVG, including both captions.
  static constexpr Rect brandMasterSafeArea{67.,13.,154.,61.};
  static constexpr Rect patternHeading{213.,80.,339.,96.};
  // All five footer action keys now use the same 95x25 hardware cap.
  // Left pair preserves an 11px gutter and keeps the ROW summary clear.
  // These exact shared rectangles also govern mouse hit testing.
  static constexpr Rect footerAdd{45.,10.,140.,35.};
  static constexpr Rect footerRemove{151.,10.,246.,35.};
  static constexpr Rect footerPrev{1020.,10.,1115.,35.};
  static constexpr Rect footerNext{1130.,10.,1225.,35.};
  static constexpr Rect footerPanel{footerPanelLeft,footerPanelTop,
                                   footerPanelRight,footerPanelBottom};
  static constexpr double footerScrewLeftX=14.,footerScrewRightX=width-14.;
  static constexpr double footerScrewRadius=7.;

  // One common vertical contract for every header action. The previous
  // geometry mixed 107..141 Pattern/BARS with 105..135 SONG controls,
  // causing misaligned buttons and clicks at the old low position.
  static constexpr double headerButtonY=99.,headerButtonHeight=32.;
  static constexpr Rect patternButton(std::size_t i)noexcept{
    const double x=33.+39.*static_cast<double>(i);
    return {x,headerButtonY,x+35.,headerButtonY+headerButtonHeight};
  }
  // Four large machined header keys, all 83x32, with 8px gutters.
  // Keep A-H's compact 35px keycaps and the three real rotary controls.
  static constexpr double headerActionWidth=83.,headerActionGap=8.;
  static constexpr Rect patternMenu{364.,headerButtonY,447.,
                                    headerButtonY+headerButtonHeight};
  static constexpr Rect barsButton{455.,headerButtonY,538.,
                                   headerButtonY+headerButtonHeight};
  static constexpr std::array<Rect,headerGroupCount> headerGroups{{
    {546.,headerButtonY,629.,headerButtonY+headerButtonHeight},
    {637.,headerButtonY,720.,headerButtonY+headerButtonHeight}
  }};
  static constexpr double headerKnobY=114.,headerKnobRadius=18.;
  static constexpr std::array<double,3> headerKnobCenters{{785.,907.,1029.}};
  static constexpr Rect headerKnob(std::size_t i) noexcept{
    return {headerKnobCenters[i]-headerKnobRadius,
            headerKnobY-headerKnobRadius,
            headerKnobCenters[i]+headerKnobRadius,
            headerKnobY+headerKnobRadius};
  }
  static constexpr Rect headerGroup(std::size_t i) noexcept{
    return headerGroups[i];
  }
  static constexpr Rect meterReset{1132.,98.,1248.,134.};
  static constexpr Rect footerExport{915.,10.,1010.,35.}; // footer-relative
  // Standard 83x32 header key, with 10px clearance from zoom popup.
  static constexpr Rect limiterTrigger{1069.,54.,1152.,86.};
  // Exact same 83x32 hardware cap as MENU/BARS/SONG/SONG EDIT.
  // Keep right edge and optical center (near 1208,29) unchanged; the 4px
  // gap before the existing zoom option popup is retained.
  static constexpr Rect zoomTrigger{1165.,13.,1248.,45.};
  static constexpr Rect zoomOption(std::size_t index) noexcept{
    const double top=49.+26.*static_cast<double>(index);
    return {1162.,top,1248.,top+24.};
  }
  // -2 = open/close trigger, 0..3 = zoom preset, -1 = no hit.
  // The menu is modal while open; the renderer draws it over header controls.
  static constexpr int zoomHit(double x,double y,bool open) noexcept{
    if(zoomTrigger.hit(x,y))return -2;
    if(open){
      for(std::size_t i=0;i<zoomPresets.size();++i)
        if(zoomOption(i).hit(x,y))return static_cast<int>(i);
    }
    return -1;
  }
  // Coordinates relative to row top; single-height instrument channel.
  static constexpr Rect select{8.,4.,19.,45.},mute{20.,8.,47.,40.},
    solo{52.,8.,79.,40.},fill{84.,8.,124.,40.},
    sample{132.,8.,304.,40.},previous{132.,8.,155.,40.},
    load{155.,8.,257.,40.},sampleMenu{257.,8.,281.,40.},next{281.,8.,304.,40.},
    preview{310.,8.,347.,40.},edit{354.,8.,406.,40.},
    stepWell{417.,4.,1031.,44.},
    velocity{1221.,4.,1258.,24.},settings{1221.,26.,1258.,46.};
  enum class Expansion:std::uint8_t{None,Velocity,Settings};
  enum class Action:std::uint8_t{
    None,Select,Mute,Solo,Fill,Previous,Load,SampleMenu,Next,Preview,Edit,
    Step,Volume,Pan,Pitch,Velocity,Settings,VelocityLane,SettingsLane
  };
  struct Hit {std::size_t row=instruments;Action action=Action::None;int step=-1;};
  using Expansions=std::array<Expansion,instruments>;
  static constexpr Rect stepRect(std::size_t i) noexcept{
    const double x=stepX+i*stepPitch;
    return {x,9.,x+17.,40.};
  }
  // Actual paint is stepCap(x+1..x+18). Quarter captions use exactly
  // the SAME painted-cap center, not a two-step-wide 38px label field.
  static constexpr Rect stepNumberRect(std::size_t i) noexcept{
    const double x=stepX+1.+i*stepPitch;
    return {x,140.,x+17.,157.};
  }
  static constexpr Rect knobRect(std::size_t i)noexcept{
    const double x=knobCenters[i];
    return {x-knobRadius,6.,x+knobRadius,42.};
  }
  static constexpr double rowTop(std::size_t first,std::size_t index,
                                  const Expansions& expanded)noexcept{
    double y=header;
    for(std::size_t i=0;i<index&&first+i<instruments;++i)
      y+=rowHeight+(expanded[first+i]==Expansion::None?0.:expansionHeight);
    return y;
  }
  static constexpr double editorHeight(std::size_t first,std::size_t count,
                                        const Expansions& expanded)noexcept{
    return rowTop(first,count,expanded)+footer;
  }
  static constexpr Expansion toggle(Expansion current,Expansion requested)noexcept{
    return current==requested?Expansion::None:requested;
  }
  static constexpr Hit rowHit(std::size_t row,double x,double y,Expansion expanded)noexcept{
    if(y>=rowHeight&&y<rowHeight+expansionHeight&&x>=20.&&x<1258.){
      if(expanded==Expansion::Velocity)return {row,Action::VelocityLane,-1};
      if(expanded==Expansion::Settings)return {row,Action::SettingsLane,-1};
      return {};
    }
    if(y<0.||y>=rowHeight)return {};
    if(select.hit(x,y))return {row,Action::Select,-1};
    if(mute.hit(x,y))return {row,Action::Mute,-1};
    if(solo.hit(x,y))return {row,Action::Solo,-1};
    if(fill.hit(x,y))return {row,Action::Fill,-1};
    if(previous.hit(x,y))return {row,Action::Previous,-1};
    if(load.hit(x,y))return {row,Action::Load,-1};
    if(sampleMenu.hit(x,y))return {row,Action::SampleMenu,-1};
    if(next.hit(x,y))return {row,Action::Next,-1};
    if(preview.hit(x,y))return {row,Action::Preview,-1};
    if(edit.hit(x,y))return {row,Action::Edit,-1};
    if(y>=9.&&y<40.&&x>=stepX&&x<stepEnd){
      const auto i=static_cast<std::size_t>((x-stepX)/stepPitch);
      if(i<steps&&stepRect(i).hit(x,y))return {row,Action::Step,static_cast<int>(i)};
    }
    for(std::size_t i=0;i<knobCenters.size();++i)
      if(knobRect(i).hit(x,y))
        return {row,i==0?Action::Volume:i==1?Action::Pan:Action::Pitch,-1};
    if(velocity.hit(x,y))return {row,Action::Velocity,-1};
    if(settings.hit(x,y))return {row,Action::Settings,-1};
    return {};
  }
  static constexpr Hit hit(double x,double y,std::size_t first,std::size_t count,
                            const Expansions& expanded)noexcept{
    for(std::size_t i=0;i<count&&first+i<instruments;++i){
      const double top=rowTop(first,i,expanded);
      const double bottom=top+rowHeight+
        (expanded[first+i]==Expansion::None?0.:expansionHeight);
      if(y>=top&&y<bottom)return rowHit(first+i,x,y-top,expanded[first+i]);
    }
    return {};
  }
  static_assert(stepEnd==1028.);
  static_assert(edit.x1+14.<=stepX);
  static_assert(stepEnd+10.<=knobCenters[0]-knobRadius);
  static_assert(knobCenters[0]+knobRadius<knobCenters[1]-knobRadius);
  static_assert(knobCenters[1]+knobRadius<knobCenters[2]-knobRadius);
  static_assert(knobCenters[2]+knobRadius+18.<=velocity.x0);
  static_assert(previous.x1==load.x0&&load.x1==sampleMenu.x0&&
                sampleMenu.x1==next.x0);
};
// MSVC evaluates constexpr function bodies reliably after the class is complete.
static_assert(UiV101Layout::patternMenu.x1-UiV101Layout::patternMenu.x0==
              UiV101Layout::headerActionWidth);
static_assert(UiV101Layout::barsButton.x1-UiV101Layout::barsButton.x0==
              UiV101Layout::headerActionWidth);
static_assert(UiV101Layout::headerGroups[0].x1-UiV101Layout::headerGroups[0].x0==
              UiV101Layout::headerActionWidth);
static_assert(UiV101Layout::headerGroups[1].x1-UiV101Layout::headerGroups[1].x0==
              UiV101Layout::headerActionWidth);
static_assert(UiV101Layout::barsButton.x0-UiV101Layout::patternMenu.x1==
              UiV101Layout::headerActionGap);
static_assert(UiV101Layout::headerGroups[0].x0-UiV101Layout::barsButton.x1==
              UiV101Layout::headerActionGap);
static_assert(UiV101Layout::headerGroups[1].x0-UiV101Layout::headerGroups[0].x1==
              UiV101Layout::headerActionGap);
static_assert(UiV101Layout::headerGroups[0].x0>
              UiV101Layout::barsButton.x1);
static_assert(UiV101Layout::headerGroups[0].y0==
              UiV101Layout::headerButtonY);
static_assert(UiV101Layout::headerGroups[0].y1==
              UiV101Layout::headerButtonY+UiV101Layout::headerButtonHeight);
static_assert(UiV101Layout::patternButton(0).y0==
              UiV101Layout::barsButton.y0);
static_assert(UiV101Layout::patternButton(7).y1==
              UiV101Layout::headerGroups[1].y1);
static_assert(UiV101Layout::headerButtonY+
              UiV101Layout::headerButtonHeight<UiV101Layout::header-20.);
static_assert(UiV101Layout::patternButton(7).x1<
              UiV101Layout::patternMenu.x0);
static_assert(UiV101Layout::headerGroups[1].x1+26.<=
              UiV101Layout::headerKnobCenters[0]-UiV101Layout::headerKnobRadius);
static_assert(UiV101Layout::headerKnobCenters[2]+UiV101Layout::headerKnobRadius<1132.);
static_assert(UiV101Layout::meterReset.x0>=1132.&&
              UiV101Layout::meterReset.y0>UiV101Layout::zoomTrigger.y1);
static_assert(UiV101Layout::zoomTrigger.x1-UiV101Layout::zoomTrigger.x0==
              UiV101Layout::headerActionWidth&&
              UiV101Layout::zoomTrigger.y1-UiV101Layout::zoomTrigger.y0==
              UiV101Layout::headerButtonHeight,
              "ZOOM must match the existing 83x32 vintage hardware buttons");
static_assert(UiV101Layout::zoomTrigger.x1<=UiV101Layout::width-30.&&
              UiV101Layout::zoomTrigger.y1<UiV101Layout::zoomOption(0).y0,
              "ZOOM cap must preserve right chassis clearance and popup gap");
static_assert(UiV101Layout::zoomPresets[0]==1.0 &&
              UiV101Layout::zoomPresets[1]==1.2 &&
              UiV101Layout::zoomPresets[2]==1.3 &&
              UiV101Layout::zoomPresets[3]==1.5);
static_assert(UiV101Layout::zoomOption(3).y1<=UiV101Layout::header);
static_assert(UiV101Layout::stepRect(UiV101Layout::steps-1).x1<=UiV101Layout::stepEnd);
// The full-width strip extends BEHIND both lower screw heads. The painter
// restores the screw faces AFTER the inset instead of narrowing the strip.
static_assert(UiV101Layout::footerPanelLeft <
              UiV101Layout::footerScrewLeftX);
static_assert(UiV101Layout::footerPanelRight >
              UiV101Layout::footerScrewRightX);
static_assert(UiV101Layout::footerPrev.x0>=UiV101Layout::footerPanel.x0+10.&&
              UiV101Layout::footerNext.x1<=UiV101Layout::footerPanel.x1-10.);
static_assert(UiV101Layout::footerAdd.x0>=UiV101Layout::footerPanel.x0+10.&&
              UiV101Layout::footerRemove.x1<UiV101Layout::footerPrev.x0);
static_assert(UiV101Layout::footerAdd.x1+10.<=UiV101Layout::footerRemove.x0);
static_assert(UiV101Layout::footerAdd.y0==UiV101Layout::footerPrev.y0&&
              UiV101Layout::footerRemove.y1==UiV101Layout::footerNext.y1);
static_assert(UiV101Layout::footerPrev.x1+10.<=UiV101Layout::footerNext.x0);
static_assert(UiV101Layout::footerPrev.y0>=UiV101Layout::footerPanel.y0&&
              UiV101Layout::footerNext.y1<=UiV101Layout::footerPanel.y1);
static_assert(UiV101Layout::footerPrev.x1-UiV101Layout::footerPrev.x0==95.&&
              UiV101Layout::footerNext.y1-UiV101Layout::footerNext.y0==25.);
static_assert(UiV101Layout::footerAdd.x1-UiV101Layout::footerAdd.x0==95.&&
              UiV101Layout::footerRemove.x1-UiV101Layout::footerRemove.x0==95.&&
              UiV101Layout::footerExport.x1-UiV101Layout::footerExport.x0==95.);
static_assert(UiV101Layout::footerRemove.x1+12.<=260.); // ROW summary

static_assert(UiV101Layout::editorHeight(0,UiV101Layout::visibleRows,
              UiV101Layout::Expansions{})*1.5<=1080.);
}
