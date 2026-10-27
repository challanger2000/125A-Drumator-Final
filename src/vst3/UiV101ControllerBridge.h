#pragma once
// V1.0.1 isolated controller-action bridge; V1.0.0 editor is untouched.
// The caller resolves geometry through UiV101Layout::hit() first.
// A Deferred result REQUIRES a real view-level implementation, and must not
// be misrepresented as a successful controller operation.
#include "core/UiV101Layout.h"
#include "vst3/DrumatorController.h"
#include <cstddef>

namespace drumator {
struct UiV101ControllerBridge final {
 using L=UiV101Layout;
 enum class Route { Ignored, Applied, Failed, DeferredToView };
 static Route apply(Controller& controller,L::Hit h){
   if(h.action==L::Action::None||h.row>=controller.visibleRows())
     return Route::Ignored;
   switch(h.action){
     case L::Action::Mute:
       return controller.toggleRowGate(h.row,false)?Route::Applied:Route::Failed;
     case L::Action::Solo:
       return controller.toggleRowGate(h.row,true)?Route::Applied:Route::Failed;
     case L::Action::Step:{
       const auto p=controller.currentPattern();
       if(h.step<0||static_cast<std::size_t>(h.step)>=controller.patternLength(p))
         return Route::Ignored; // physically inactive beyond a 1-bar pattern
       return controller.toggleStep(p,h.row,static_cast<std::size_t>(h.step))?
              Route::Applied:Route::Failed;
     }
     case L::Action::Previous:
       return controller.stepSample(h.row,-1)==Controller::LoadStatus::Accepted?
              Route::Applied:Route::Failed;
     case L::Action::Next:
       return controller.stepSample(h.row,1)==Controller::LoadStatus::Accepted?
              Route::Applied:Route::Failed;
     case L::Action::Preview:
       return controller.previewSample(h.row)?Route::Applied:Route::Failed;
     // Selection, import, expanded editors, physical rotaries, popup menus
     // and velocity painting require their own view state + pointer lifecycle.
     case L::Action::Select:
     case L::Action::Fill:
     case L::Action::Load:
     case L::Action::SampleMenu:
     case L::Action::Edit:
     case L::Action::Volume:
     case L::Action::Pan:
     case L::Action::Pitch:
     case L::Action::Velocity:
     case L::Action::Settings:
     case L::Action::VelocityLane:
     case L::Action::SettingsLane:
       return Route::DeferredToView;
     case L::Action::None: return Route::Ignored;
   }
   return Route::Ignored;
 }
};
}
