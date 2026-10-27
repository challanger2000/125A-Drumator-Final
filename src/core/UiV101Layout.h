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
  // One common vertical contract for every header action. The previous
  // geometry mixed 107..141 Pattern/BARS with 105..135 SONG controls,
  // causing misaligned buttons and clicks at the old low position.
  static constexpr double headerButtonY=99.,headerButtonHeight=32.;
  static constexpr Rect patternButton(std::size_t i)noexcept{
    const double x=33.+39.*static_cast<double>(i);
    return {x,headerButtonY,x+35.,headerButtonY+headerButtonHeight};
  }
  static constexpr Rect patternMenu{364.,headerButtonY,414.,
                                    headerButtonY+headerButtonHeight};
  static constexpr Rect barsButton{431.,headerButtonY,514.,
                                   headerButtonY+headerButtonHeight};
  static constexpr std::array<Rect,headerGroupCount> headerGroups{{
    {527.,headerButtonY,610.,headerButtonY+headerButtonHeight},
    {618.,headerButtonY,716.,headerButtonY+headerButtonHeight}
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
  static constexpr Rect zoomTrigger{1140.,14.,1248.,44.};
  static constexpr Rect zoomOption(std::size_t index) noexcept{
    const double top=49.+26.*static_cast<double>(index);
    return {1134.,top,1248.,top+24.};
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
    load{155.,8.,281.,40.},next{281.,8.,304.,40.},
    preview{310.,8.,347.,40.},edit{354.,8.,406.,40.},
    stepWell{417.,4.,1031.,44.},
    velocity{1221.,4.,1258.,24.},settings{1221.,26.,1258.,46.};
  enum class Expansion:std::uint8_t{None,Velocity,Settings};
  enum class Action:std::uint8_t{
    None,Select,Mute,Solo,Fill,Previous,Load,Next,Preview,Edit,
    Step,Volume,Pan,Pitch,Velocity,Settings,VelocityLane,SettingsLane
  };
  struct Hit {std::size_t row=instruments;Action action=Action::None;int step=-1;};
  using Expansions=std::array<Expansion,instruments>;
  static constexpr Rect stepRect(std::size_t i) noexcept{
    const double x=stepX+i*stepPitch;
    return {x,9.,x+17.,40.};
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
  static_assert(previous.x1==load.x0&&load.x1==next.x0);
};
// MSVC evaluates constexpr function bodies reliably after the class is complete.
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
static_assert(UiV101Layout::zoomPresets[0]==1.0 &&
              UiV101Layout::zoomPresets[1]==1.2 &&
              UiV101Layout::zoomPresets[2]==1.3 &&
              UiV101Layout::zoomPresets[3]==1.5);
static_assert(UiV101Layout::zoomOption(3).y1<=UiV101Layout::header);
static_assert(UiV101Layout::stepRect(UiV101Layout::steps-1).x1<=UiV101Layout::stepEnd);
static_assert(UiV101Layout::editorHeight(0,UiV101Layout::visibleRows,
              UiV101Layout::Expansions{})*1.5<=1080.);
}
