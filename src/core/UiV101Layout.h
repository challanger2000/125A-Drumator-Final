#pragma once
// V1.0.1 independent GUI geometry prototype. V1.0.0 view is untouched.
// The painter and input handler must consume these identical rectangles.
#include <array>
#include <cstddef>
#include <cstdint>

namespace drumator {
struct UiV101Layout final {
  static constexpr std::size_t instruments=16,visibleRows=10,steps=32;
  static constexpr double width=1280.,header=158.,rowHeight=48.,expansionHeight=48.,footer=48.;
  static constexpr double stepX=420.,stepPitch=19.,stepEnd=stepX+steps*stepPitch;
  static constexpr double knobRadius=18.;
  static constexpr std::array<double,3> knobCenters{{1062.,1122.,1182.}};
  struct Rect final {
    double x0,y0,x1,y1;
    constexpr bool hit(double x,double y)const noexcept{
      return x>=x0&&x<x1&&y>=y0&&y<y1;
    }
  };
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
  static_assert(stepRect(steps-1).x1<=stepEnd);
  static_assert(editorHeight(0,visibleRows,Expansions{})*1.5<=1080.);
};
}
