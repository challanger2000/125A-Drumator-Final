#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
namespace drumator {
struct UiCompactLayout final {
 static constexpr double width=1280.,header=160.,baseRow=130.,velocityRow=30.,footer=54.;
 static constexpr std::size_t perPage=4;
 // Move the complete header control group starting with PATTERN; logo and left controls stay fixed.
 static constexpr double headerRightShift=-12.;
 // Space the PATTERN LENGTH controls from the centered A-H group without moving either title or the pattern group.
 static constexpr double patternLengthLeftShift=14.;
 static constexpr double headerButtonTop=101.,headerButtonBottom=133.,
      groupTitleTop=49.,groupTitleBottom=65.,globalKnobY=119.,
      songEditorHeight=432.,patternStart=457.+headerRightShift,patternPitch=49.,
      patternButtonWidth=47.;
 static constexpr double stepX=420.,stepWidth=19.;
 static constexpr double knobRadius=25.,rowKnobY=88.;
 // Render geometry contract: labels clear the mounted 30px ring by 3px.
 static constexpr double rowKnobMountRadius=30.,rowKnobLabelTop=-46.,
                         rowKnobLabelBottom=-33.;
 static_assert(rowKnobLabelBottom <= -rowKnobMountRadius-3.);
 static_assert(rowKnobY+rowKnobMountRadius < 120.);
 static constexpr double scrollLeft=1263.,scrollRight=1279.,scrollTop=header+7.;
 struct Slot {double left,right;};
 static constexpr Slot barOne{273.-patternLengthLeftShift,356.-patternLengthLeftShift},
                       barTwo{359.-patternLengthLeftShift,442.-patternLengthLeftShift},
                       edit{208.,272.};
 static constexpr Slot patternTools{850.+headerRightShift,969.+headerRightShift},
                       songToggle{972.+headerRightShift,1042.+headerRightShift},
                       songEdit{1045.+headerRightShift,1125.+headerRightShift};
 static constexpr Slot rowMenu{20.,64.};
 static constexpr Slot select{18.,42.},sample{151.,308.},sampleMenu{188.,225.},previous{151.,174.},
   next{285.,308.},preview{312.,352.},fill{110.,148.},mute{46.,75.},
   solo{78.,107.},choke{136.,185.},output{67.,133.},
   velocityToggle{1234.,1258.},rowAdd{16.,110.},rowRemove{116.,210.};
 // V1.0.1 layout feasibility: shared geometry contract for the intended
 // unbroken instrument strips. This is a PROPOSAL only; original working
 // coordinates above remain active until draw and hit testing migrate together.
 struct V101RowPlan final {
  static constexpr double left=16.,right=1260.;
  static constexpr double controlStart=20.,controlEnd=412.;
  static constexpr double stepStart=420.,stepPitch=19.;
  static constexpr double stepEnd=stepStart+32.*stepPitch; // 1028 px
  static constexpr std::array<double,3> knobCenters{{1066.,1134.,1202.}};
  static constexpr double knobMountRadius=30.;
  static constexpr double expandLeft=1234.,expandRight=1258.;
  static constexpr double clearance=8.;
  static_assert(controlEnd+clearance<=stepStart);
  static_assert(stepEnd+clearance<=knobCenters[0]-knobMountRadius);
  static_assert(knobCenters[0]+knobMountRadius+clearance<=knobCenters[1]-knobMountRadius);
  static_assert(knobCenters[1]+knobMountRadius+clearance<=knobCenters[2]-knobMountRadius);
  static_assert(knobCenters[2]+knobMountRadius<expandLeft);
  static_assert(expandRight<=right);
  static_assert(stepEnd-stepStart==608.);
 };
 static constexpr std::array<double,4> rowKnobCenters{{1066.,1134.,1202.,376.}};
 static_assert(rowKnobCenters[2]+rowKnobMountRadius<velocityToggle.left);
 static constexpr std::array<double,3> globalKnobCenters{{79.,177.,1180.+headerRightShift}};
 // One geometry contract for the moved MAIN L/R LEDs, CLIP text and click reset.
 static constexpr double meterLeft=1255.+headerRightShift;
 static constexpr double meterRight=1267.+headerRightShift;
 static constexpr double meterClipLeft=1226.+headerRightShift;
 static constexpr double meterClipRight=1279.+headerRightShift;
 static constexpr double meterResetLeft=meterLeft-2.;
 static constexpr double meterResetRight=meterRight+13.;
 static_assert(barTwo.right<patternStart);
 // Preserve readable group separation and the HUMANIZE numeric readout.
 static_assert(patternStart-barTwo.right>=16.);
 static_assert(barOne.left>=globalKnobCenters[1]+72.+8.);
 static_assert(patternStart+7.*patternPitch+patternButtonWidth<patternTools.left);
 static_assert(patternTools.right<songToggle.left&&songToggle.right<songEdit.left);
 static_assert(songEdit.right<globalKnobCenters[2]-50.);
 static_assert(patternStart+(7.*patternPitch+patternButtonWidth)/2.==width/2.);
 static_assert(meterRight+7.<=width-12.&&meterClipRight<=width-12.);
 static_assert(meterLeft>globalKnobCenters[2]+72.);
 static constexpr std::array<double,4> userZooms{{1.,1.2,1.3,1.5}};
 static constexpr bool hit(double x,Slot slot) noexcept {return x>=slot.left&&x<slot.right;}
 // At the supported 300px minimum, tall modal menus must extend over the
 // header. Keep every item visible and std::clamp's lower <= upper contract.
 static constexpr double popupTop(double rowY,double clientHeight,double requiredHeight) noexcept {
   const double upper=std::max(0.,clientHeight-requiredHeight);
   return std::clamp(rowY+36.,std::min(header,upper),upper);
 }
 static constexpr double rowTop(std::size_t first,std::size_t index,
                                  const std::array<bool,16>& expanded) noexcept {
   double y=header;
   for(std::size_t i=first;i<first+index&&i<expanded.size();++i)
     y+=baseRow+(expanded[i]?velocityRow:0.);
   return y;
 }
 static constexpr double rowBottom(std::size_t first,std::size_t count,
                                    const std::array<bool,16>& expanded) noexcept {
   return rowTop(first,count,expanded);
 }
 static constexpr double editorHeight(std::size_t first,std::size_t count,
                                      const std::array<bool,16>& expanded) noexcept {
   return rowBottom(first,count,expanded)+footer;
 }
 static constexpr std::size_t rowAt(double y,std::size_t first,std::size_t count,
                                     const std::array<bool,16>& expanded) noexcept {
   for(std::size_t i=0;i<count;++i) {
     const auto t=rowTop(first,i,expanded);
     const auto n=t+baseRow+(expanded[first+i]?velocityRow:0.);
     if(y>=t&&y<n)return i;
   }
   return count;
 }
 static constexpr int stepAt(double x) noexcept {
   return x>=stepX&&x<stepX+32.*stepWidth?
          static_cast<int>((x-stepX)/stepWidth):-1;
 }
 static constexpr bool mainHit(double y,double rowY) noexcept {
   return y>=rowY&&y<rowY+baseRow;
 }
 static constexpr bool velocityHit(double y,double rowY) noexcept {
   return y>=rowY+baseRow&&y<rowY+baseRow+velocityRow;
 }
 static std::uint8_t velocityAt(double y,double rowY,bool erase=false) noexcept {
   if(erase)return 0;
   const auto fraction=std::clamp((rowY+baseRow+velocityRow-3.-y)/(velocityRow-6.),0.,1.);
   return static_cast<std::uint8_t>(1+std::lround(fraction*126.));
 }
 static int knobValue(double startY,double movedY,int initial,int minimum,int maximum) noexcept {
   // 2.5 logical pixels per discrete controller step, extra range clamped.
   return std::clamp(initial+static_cast<int>(std::lround((startY-movedY)/2.5)),minimum,maximum);
 }
 // Use the value's range to choose sensible drag sensitivity, rather
 // than forcing users to drag 500px just to traverse a 0..200% control.
 // Fine mode is latched at mouse-down, so adding/removing Shift mid-gesture
 // cannot unexpectedly jump the value.
 static constexpr double pixelsPerStep(int slot,bool fine=false) noexcept {
   const double unit=slot==2||slot==4?2.5:1.5;
   return fine?unit*4.:unit;
 }
 static int knobValueForSlot(int slot,double startY,double movedY,int initial,
                             bool fine=false) noexcept {
   const auto delta=static_cast<int>(std::lround(
       (startY-movedY)/pixelsPerStep(slot,fine)));
   return std::clamp(initial+delta,minValue(slot),maxValue(slot));
 }
 static constexpr int defaultValue(int slot) noexcept {
   return slot==0||slot==6?100:0;
 }
 static constexpr int minValue(int slot) noexcept {
   return slot==1?-100:slot==2?-24:0;
 }
 static constexpr int maxValue(int slot) noexcept {
   return slot==0||slot==6?200:slot==4?50:slot==1||slot==3||slot==5?100:24;
 }
};
}
