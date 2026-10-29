#pragma once
// Isolated V1.0.1 interactive control surface. NOT active in the shipped editor.
// Owns only UI-local state. Mutations go through the existing, proven Controller.
#include "core/UiV101Layout.h"
#include "vst3/UiV101ControllerBridge.h"
#include "vst3/DrumatorController.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace drumator {
class UiV101Interaction final {
public:
 using L=UiV101Layout;
 enum class Result { Ignored, Handled, Applied, DeferredToView, Failed };
 enum class SettingsAction { None, Output, Choke, Copy, Paste, Clear,
                             RandomVelocity, ResetVelocity, RandomRealtimeDown,
                             RandomRealtimeUp, RandomRealtimeDrag };
 // A DeferredToView result must carry its exact host-side action and target.
 // The view consumes this once to open a popup/dialog; never infer it from
 // a subsequent mouse position or a potentially changed selected row.
 enum class DeferredKind { None, PatternMenu, SongEditor, Fill, LoadSample,
                           SampleMenu,
                           SampleEditor, OutputMenu, ChokeMenu, Zoom };
 struct DeferredRequest {
   DeferredKind kind=DeferredKind::None;
   std::size_t row=L::instruments;
 };
 DeferredRequest takeDeferredRequest() noexcept {
   const auto request=deferred_;
   deferred_={};
   return request;
 }
 struct State {
   std::size_t firstRow=0,selectedRow=0,expandedRow=L::instruments;
   L::Expansion expansion=L::Expansion::None;
   bool zoomOpen=false;
   std::size_t zoomIndex=0;
 };
 explicit UiV101Interaction(Controller& c) : controller_(c) {}
 ~UiV101Interaction(){endGesture(false);}
 UiV101Interaction(const UiV101Interaction&)=delete;
 UiV101Interaction& operator=(const UiV101Interaction&)=delete;

 const State& state()const noexcept{return state_;}
 // Revert a layout change when the real host refuses an editor resize.
 void restoreLayout(const State& prior) noexcept {
   endGesture(false);
   state_=prior;
   deferred_={};
 }

 // One expanded row max. With 9 visible + 1 expansion, 150% <=1080 px.
 std::size_t shownRows()const noexcept {
   const auto n=controller_.visibleRows();
   const auto limit=state_.expansion==L::Expansion::None?L::visibleRows:L::visibleRows-1;
   return state_.firstRow<n?std::min(limit,n-state_.firstRow):0;
 }
 L::Expansions expandedRows()const noexcept{
   L::Expansions expansion{};
   if(state_.expandedRow<L::instruments)
     expansion[state_.expandedRow]=state_.expansion;
   return expansion;
 }
 double height()const noexcept {
   return L::editorHeight(state_.firstRow,shownRows(),expandedRows());
 }
 double userZoom()const noexcept{return L::zoomPresets[state_.zoomIndex];}
 // A reopened DAW editor must show the actual recalled zoom, not always 100%.
 void syncZoomFromHost(double value) noexcept {
   if(!std::isfinite(value))return;
   for(std::size_t i=0;i<L::zoomPresets.size();++i)
     if(std::abs(value-L::zoomPresets[i])<0.0001){
       state_.zoomIndex=i;
       priorZoomIndex_=i;
       return;
     }
 }

 // Called by the real editor only after setZoomChecked + host resize negotiation.
 // A failed resize must not leave the visible menu out of sync with the host.
 void confirmZoom(bool accepted)noexcept {
   if(!accepted)state_.zoomIndex=priorZoomIndex_;
 }
 L::Hit rowHit(double x,double y)const noexcept{
   return L::hit(x,y,state_.firstRow,shownRows(),expandedRows());
 }
 static constexpr L::Rect settingsRect(std::size_t index)noexcept {
   // Five identical 112px keys with four 10px gaps: 600px total.
   // Center this SET group in the 13..1260 chassis: midpoint x=636.
   constexpr std::array<double,5> x{{336.,458.,580.,702.,824.}};
   const double left=x[index];
   return {left,56.,left+112.,88.}; // relative to expanded ROW TOP
 }
 // The two one-shot edits live in VEL, beside the actual velocity bars.
 // LIVE VEL is a separate non-destructive playback variation, with its
 // own dark readout/short +/- controls after the 32-bar velocity lane.
 static constexpr L::Rect velocityRandom{132.,64.,260.,92.};
 static constexpr L::Rect velocityReset{272.,64.,400.,92.};
 static constexpr L::Rect realtimeMinus{1038.,58.,1072.,90.};
 static constexpr L::Rect realtimeValue{1082.,58.,1206.,90.};
 static constexpr L::Rect realtimePlus{1216.,58.,1250.,90.};
 static_assert(velocityRandom.x1+12.==velocityReset.x0);
 static_assert(velocityReset.x1< L::stepX);
 static_assert(realtimeMinus.x1+10.==realtimeValue.x0);
 static_assert(realtimeValue.x1+10.==realtimePlus.x0);
 static_assert(realtimeMinus.x0> L::stepEnd);
 static constexpr SettingsAction settingsAt(double x,double relativeY)noexcept{
   if(relativeY<48.||relativeY>=96.)return SettingsAction::None;
   for(std::size_t i=0;i<5;++i)
     if(settingsRect(i).hit(x,relativeY))
       return static_cast<SettingsAction>(static_cast<int>(i)+1);
   return SettingsAction::None;
 }
 static constexpr SettingsAction velocityAt(double x,double relativeY)noexcept{
   if(relativeY<48.||relativeY>=96.)return SettingsAction::None;
   if(velocityRandom.hit(x,relativeY))return SettingsAction::RandomVelocity;
   if(velocityReset.hit(x,relativeY))return SettingsAction::ResetVelocity;
   if(realtimeMinus.hit(x,relativeY))return SettingsAction::RandomRealtimeDown;
   if(realtimePlus.hit(x,relativeY))return SettingsAction::RandomRealtimeUp;
   if(realtimeValue.hit(x,relativeY))return SettingsAction::RandomRealtimeDrag;
   return SettingsAction::None;
 }
 // Real 32-step lane uses the same x/pitch as the main Step grid.
 static constexpr int laneStep(double x)noexcept {
   if(x<L::stepX||x>=L::stepEnd)return -1;
   const auto index=static_cast<std::size_t>((x-L::stepX)/L::stepPitch);
   return index<L::steps&&L::stepRect(index).hit(x,20.)?static_cast<int>(index):-1;
 }
 static std::uint8_t laneValue(double relativeY)noexcept{
   const double level=std::clamp((96.-relativeY)/48.,0.,1.);
   return static_cast<std::uint8_t>(std::clamp(std::lround(1.+126.*level),1L,127L));
 }
 void reset() {
   endGesture(false);
   state_=State{};
   deferred_={};
 }
 void reconcile(){
   const auto total=controller_.visibleRows();
   if(total==0){reset();return;}
   if(state_.firstRow>=total)state_.firstRow=total-1;
   if(state_.selectedRow>=total)state_.selectedRow=total-1;
   if(state_.expandedRow>=total)closeExpansion();
   const auto count=shownRows();
   if(state_.expandedRow<L::instruments&&
       (state_.expandedRow<state_.firstRow||
        state_.expandedRow>=state_.firstRow+count))closeExpansion();
 }
 bool page(int direction){
   reconcile();
   const auto total=controller_.visibleRows();
   if(total<=L::visibleRows)return false;
   endGesture(true);
   closeExpansion();
   constexpr auto stride=L::visibleRows;
   const auto next=direction>0?std::min(state_.firstRow+stride,total-1):
     (state_.firstRow>=stride?state_.firstRow-stride:0);
   if(next==state_.firstRow)return false;
   state_.firstRow=next;
   state_.selectedRow=next;
   state_.zoomOpen=false;
   return true;
 }
 Result down(double x,double y,bool erase=false,bool fine=false){
   deferred_={};
   reconcile();
   if(gesture_!=Gesture::None)endGesture(true);
   if(state_.zoomOpen){
     const int selected=L::zoomHit(x,y,true);
     state_.zoomOpen=false;
     if(selected==-2)return Result::Handled;
     if(selected>=0){
       priorZoomIndex_=state_.zoomIndex;
       state_.zoomIndex=static_cast<std::size_t>(selected);
       return defer(DeferredKind::Zoom); // host/editor must negotiate actual resize
     }
     return Result::Handled; // modal menu blocks click-through
   }
   if(L::zoomHit(x,y,false)==-2){state_.zoomOpen=true;return Result::Handled;}
   if(L::meterReset.hit(x,y))
     return controller_.clearMeterClips()?Result::Applied:Result::Failed;
   const double footerTop=height()-L::footer;
   // The pointer targets are precisely the painted, smaller 95x25 caps.
   // Outside them, the footer is inert (no old oversized hidden hotspots).
   if(L::footerAdd.hit(x,y-footerTop))
     return controller_.visibleRows()>=L::instruments?Result::Ignored:
            controller_.addRow()?Result::Applied:Result::Failed;
   if(L::footerRemove.hit(x,y-footerTop))
     return !controller_.canHideLast()?Result::Ignored:
            controller_.hideLastEmptyRow()?Result::Applied:Result::Failed;
   if(L::footerPrev.hit(x,y-footerTop))
     return page(-1)?Result::Handled:Result::Ignored;
   if(L::footerNext.hit(x,y-footerTop))
     return page(1)?Result::Handled:Result::Ignored;
   if(y>=L::headerButtonY&&
      y<L::headerButtonY+L::headerButtonHeight){
     for(std::size_t i=0;i<8;++i)
       if(L::patternButton(i).hit(x,y))
         return controller_.changePattern(i)?Result::Applied:Result::Failed;
     if(L::patternMenu.hit(x,y))return defer(DeferredKind::PatternMenu);
     if(L::barsButton.hit(x,y))
       return controller_.togglePatternLength(controller_.currentPattern())?
              Result::Applied:Result::Failed;
     if(L::headerGroup(0).hit(x,y))
       return controller_.toggleSongMode()?Result::Applied:Result::Failed;
     if(L::headerGroup(1).hit(x,y))return defer(DeferredKind::SongEditor);
   }
   for(std::size_t i=0;i<L::headerKnobCenters.size();++i)
     if(L::headerKnob(i).hit(x,y)){
       gesture_=Gesture::HeaderKnob;fineKnob_=fine;knobIndex_=static_cast<int>(i);
       knobAnchorY_=y;return Result::Handled;
     }
   const auto h=rowHit(x,y);
   if(h.action==L::Action::None)return Result::Ignored;
   if(h.row>=controller_.visibleRows())return Result::Ignored;
   if(h.action==L::Action::Velocity||h.action==L::Action::Settings){
     const auto requested=h.action==L::Action::Velocity?
       L::Expansion::Velocity:L::Expansion::Settings;
     if(state_.expandedRow==h.row&&state_.expansion==requested){
       closeExpansion();
     }else{
       state_.expandedRow=h.row;state_.expansion=requested;
       // Expanding reduces the visible count to nine. Keep the selected row
       // physically visible rather than silently hiding the last instrument.
       if(h.row>=state_.firstRow+shownRows())state_.firstRow=h.row-(L::visibleRows-2);
     }
     state_.selectedRow=h.row;return Result::Handled;
   }
   if(h.action==L::Action::Select){state_.selectedRow=h.row;return Result::Handled;}
   if(h.action==L::Action::Volume||h.action==L::Action::Pan||
      h.action==L::Action::Pitch){
     gesture_=Gesture::RowKnob;fineKnob_=fine;knobRow_=h.row;
     knobIndex_=h.action==L::Action::Volume?0:h.action==L::Action::Pan?1:2;
     knobAnchorY_=y;return Result::Handled;
   }
   if(h.action==L::Action::VelocityLane){
     if(state_.expansion!=L::Expansion::Velocity||
        state_.expandedRow!=h.row)return Result::Ignored;
     const double relativeY=y-L::rowTop(state_.firstRow,h.row-state_.firstRow,
                                          expandedRows());
     const auto velocityAction=velocityAt(x,relativeY);
     if(velocityAction==SettingsAction::RandomRealtimeDrag){
       gesture_=Gesture::RealtimeKnob;
       knobRow_=h.row;knobAnchorY_=y;fineKnob_=fine;
       return Result::Handled;
     }
     if(velocityAction!=SettingsAction::None)
       return settingsAction(h.row,velocityAction);
     return beginLane(h.row,x,relativeY,erase);
   }
   if(h.action==L::Action::SettingsLane){
     if(state_.expansion!=L::Expansion::Settings||
        state_.expandedRow!=h.row)return Result::Ignored;
     const double relativeY=y-L::rowTop(state_.firstRow,h.row-state_.firstRow,
                                          expandedRows());
     const auto settings=settingsAt(x,relativeY);
     return settingsAction(h.row,settings);
   }
   const auto out=UiV101ControllerBridge::apply(controller_,h);
   if(out==UiV101ControllerBridge::Route::DeferredToView){
     if(h.action==L::Action::Fill)return defer(DeferredKind::Fill,h.row);
     if(h.action==L::Action::Load)return defer(DeferredKind::LoadSample,h.row);
     if(h.action==L::Action::SampleMenu)return defer(DeferredKind::SampleMenu,h.row);
     if(h.action==L::Action::Edit)return defer(DeferredKind::SampleEditor,h.row);
   }
   return out==UiV101ControllerBridge::Route::Applied?Result::Applied:
          out==UiV101ControllerBridge::Route::Failed?Result::Failed:
          out==UiV101ControllerBridge::Route::DeferredToView?
          Result::DeferredToView:Result::Ignored;
 }
 Result move(double x,double y){
   if(gesture_==Gesture::Velocity){
     const auto p=controller_.currentPattern();
     if(p!=lanePattern_||controller_.editGeneration()!=laneGeneration_){
       endGesture(false);return Result::Ignored;
     }
     const int step=laneStep(x);
     if(step<0||static_cast<std::size_t>(step)>=controller_.patternLength(p))
       return Result::Ignored;
     if(controller_.velocity(p,laneRow_,static_cast<std::size_t>(step))==0)
       return Result::Ignored; // never create a note in Velocity lane
     const double top=L::rowTop(state_.firstRow,laneRow_-state_.firstRow,expandedRows());
     const auto next=erase_?std::uint8_t{1}:laneValue(y-top);
     return controller_.setStepVelocity(p,laneRow_,static_cast<std::size_t>(step),next,this)?
            Result::Applied:Result::Failed;
   }
   if(gesture_==Gesture::None)return Result::Ignored;
   const double step=(knobIndex_==2&&gesture_==Gesture::RowKnob?6.:4.)*
                     (fineKnob_?3.:1.);
   const auto increments=static_cast<int>(std::trunc((knobAnchorY_-y)/step));
   if(increments==0)return Result::Handled;
   const int index=knobIndex_;
   const bool ok=gesture_==Gesture::HeaderKnob?
     (index==0?controller_.adjustSwing(increments):
      index==1?controller_.adjustHumanize(increments):
               controller_.adjustMasterVolume(increments)):
     gesture_==Gesture::RealtimeKnob?
       controller_.adjustRandomVelocity(knobRow_,increments):
     (index==2?controller_.adjustRowPitch(knobRow_,increments):
               controller_.adjustRowMix(knobRow_,index==1,increments));
   if(ok){knobAnchorY_-=increments*step;return Result::Applied;}
   return Result::Failed;
 }
 Result up(){
   if(gesture_==Gesture::None)return Result::Ignored;
   endGesture(true);return Result::Handled;
 }
 void cancel(){endGesture(false);state_.zoomOpen=false;}
 // Controller state restore owns rollback. Clear UI-only pointer captures
 // without calling the old processor or throwing from the no-throw recall hook.
 void discardForRecall() noexcept {
   gesture_=Gesture::None;
   fineKnob_=false;
   knobIndex_=-1;
   knobRow_=L::instruments;
   laneRow_=L::instruments;
   deferred_={};
   state_.zoomOpen=false;
 }

private:
 enum class Gesture { None, RowKnob, HeaderKnob, RealtimeKnob, Velocity };
 Controller& controller_;
 State state_{};
 DeferredRequest deferred_{};
 Result defer(DeferredKind kind,std::size_t row=L::instruments) noexcept {
   deferred_={kind,row};
   return Result::DeferredToView;
 }
 std::size_t priorZoomIndex_=0;
 Gesture gesture_=Gesture::None;
 int knobIndex_=-1;
 std::size_t knobRow_=L::instruments,laneRow_=L::instruments,lanePattern_=0;
 double knobAnchorY_=0.;
 std::uint64_t laneGeneration_=0;
 bool erase_=false,fineKnob_=false;
 void closeExpansion()noexcept {
   state_.expansion=L::Expansion::None;
   state_.expandedRow=L::instruments;
 }
 void endGesture(bool commit){
   if(gesture_==Gesture::Velocity)controller_.endVelocityStroke(commit,this);
   gesture_=Gesture::None;fineKnob_=false;knobIndex_=-1;knobRow_=L::instruments;
   laneRow_=L::instruments;
 }
 Result beginLane(std::size_t row,double x,double relativeY,bool erase){
   const int step=laneStep(x);
   const auto p=controller_.currentPattern();
   if(step<0||static_cast<std::size_t>(step)>=controller_.patternLength(p))
     return Result::Ignored;
   if(controller_.velocity(p,row,static_cast<std::size_t>(step))==0)
     return Result::Ignored;
   laneRow_=row;lanePattern_=p;laneGeneration_=controller_.editGeneration();
   gesture_=Gesture::Velocity;erase_=erase;
   controller_.beginVelocityStroke(p,this);
   const auto v=erase?std::uint8_t{1}:laneValue(relativeY);
   if(!controller_.setStepVelocity(p,row,static_cast<std::size_t>(step),v,this)){
     endGesture(false);return Result::Failed;
   }
   return Result::Applied;
 }
 Result settingsAction(std::size_t row,SettingsAction action){
   bool ok=false;
   switch(action){
     case SettingsAction::None:return Result::Ignored;
     case SettingsAction::Output:return defer(DeferredKind::OutputMenu,row);
     case SettingsAction::Choke:return defer(DeferredKind::ChokeMenu,row);
     case SettingsAction::Copy:ok=controller_.copyRow(row);break;
     case SettingsAction::Paste:
       if(!controller_.canPasteRow())return Result::Ignored;
       ok=controller_.pasteRow(row);break;
     case SettingsAction::Clear:ok=controller_.clearRow(row);break;
     case SettingsAction::RandomVelocity:
       ok=controller_.randomizeRowVelocity(row);break;
     case SettingsAction::ResetVelocity:
       ok=controller_.resetRowVelocity(row);break;
     case SettingsAction::RandomRealtimeDown:
       ok=controller_.adjustRandomVelocity(row,-10);break;
     case SettingsAction::RandomRealtimeUp:
       ok=controller_.adjustRandomVelocity(row,10);break;
     case SettingsAction::RandomRealtimeDrag:return Result::Ignored;
   }
   return ok?Result::Applied:Result::Failed;
 }
};
}
