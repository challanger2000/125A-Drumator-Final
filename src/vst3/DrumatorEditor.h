#pragma once
#include "vstgui/plugin-bindings/vst3editor.h"
#include "vstgui/lib/cframe.h"
#include <cmath>
namespace drumator {
// The pinned VSTGUI exposes the combined host DPI and user zoom factor
// as protected API. Expose it only through our concrete editor subclass.
class DrumatorEditor final : public VSTGUI::VST3Editor {
public:
 using VSTGUI::VST3Editor::VST3Editor;
 double effectiveScale() const {return getAbsScaleFactor();}
#ifdef VST3_CONTENT_SCALE_SUPPORT
 Steinberg::tresult PLUGIN_API setContentScaleFactor(ScaleFactor factor) override {
   if(!std::isfinite(factor)||factor<=0.)return Steinberg::kResultFalse;
   const auto previous=contentScaleFactor;
   if(std::abs(previous-factor)<0.00001)return Steinberg::kResultTrue;
   if(auto* activeFrame=getFrame()) {
     contentScaleFactor=factor;
     if(!activeFrame->setZoom(getAbsScaleFactor())) {
       contentScaleFactor=previous;
       return Steinberg::kResultFalse;
     }
     return Steinberg::kResultTrue;
   }
   // Before attach the host consults getSize; update its reported rectangle.
   Steinberg::ViewRect bounds{};
   if(getSize(&bounds)!=Steinberg::kResultOk)return Steinberg::kResultFalse;
   const double ratio=factor/previous;
   bounds.right=bounds.left+static_cast<Steinberg::int32>(
       std::lround(bounds.getWidth()*ratio));
   bounds.bottom=bounds.top+static_cast<Steinberg::int32>(
       std::lround(bounds.getHeight()*ratio));
   if(VSTGUI::VST3Editor::onSize(&bounds)!=Steinberg::kResultOk)
     return Steinberg::kResultFalse;
   contentScaleFactor=factor;
   return Steinberg::kResultTrue;
 }
#endif
 // VSTGUI updates zoomFactor before negotiating a host resize. On rejection
 // its frame restores the old transform but the editor value remains stale.
 bool setZoomChecked(double factor) {
   if(!std::isfinite(factor)||factor<=0.)return false;
   const double previous=getZoomFactor();
   if(std::abs(previous-factor)<0.00001)return true;
   if(getFrame()==nullptr) {
     // F16: Studio One queries IPlugView::getSize() BEFORE attached().
     // VSTGUI::setZoomFactor() without a CFrame changes only zoomFactor,
     // not its physical ViewRect. Consequently the host builds a 1280px
     // title/close bar while the attached plug-in later paints 1664px.
     // Synchronize the PRE-ATTACH ViewRect so the host chrome, plug-in
     // client and eventual CFrame all start at the same pixel width.
     Steinberg::ViewRect bounds{};
     if(VSTGUI::VST3Editor::getSize(&bounds)!=Steinberg::kResultOk)
       return false;
     const double ratio=factor/previous;
     const auto scaledWidth=static_cast<Steinberg::int32>(
         std::lround(bounds.getWidth()*ratio));
     const auto scaledHeight=static_cast<Steinberg::int32>(
         std::lround(bounds.getHeight()*ratio));
     if(scaledWidth<=0||scaledHeight<=0)return false;
     bounds.right=bounds.left+scaledWidth;
     bounds.bottom=bounds.top+scaledHeight;
     if(VSTGUI::VST3Editor::onSize(&bounds)!=Steinberg::kResultOk)
       return false;
     setZoomFactor(factor);
     return true;
   }
   setZoomFactor(factor);
   if(const auto* activeFrame=getFrame()) {
     if(std::abs(activeFrame->getTransform().m11-getAbsScaleFactor())>0.001) {
       zoomFactor=previous;
       return false;
     }
   }
   return true;
 }
};
}
