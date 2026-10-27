#pragma once
// Shared, read-only controller -> V1.0.1 paint scene.
// No synthetic kit data: the DAW and the Windows pixel fixture use this same adapter.
#include "vst3/UiV101NativePainter.h"
#include "vst3/DrumatorController.h"

namespace drumator::v101 {
inline UiV101NativePreview::Scene captureControllerScene(Controller& controller) {
 UiV101NativePreview::Scene scene{};
 scene.pattern=controller.currentPattern();
 scene.visibleRows=controller.visibleRows();
 scene.patternLength=controller.patternLength(scene.pattern);
 scene.songEnabled=controller.songEnabled();
 scene.canRemoveRow=controller.canHideLast();
 scene.swing=controller.swingPercent();
 scene.humanize=controller.humanizePercent();
 scene.master=controller.masterVolume();
 for(std::size_t row=0;row<scene.rows.size();++row) {
   auto& dst=scene.rows[row];
   dst.name=controller.label(row);
   dst.hasSample=controller.hasSample(row);
   dst.muted=controller.isRowMuted(row);
   dst.solo=controller.isRowSolo(row);
   dst.gain=controller.rowGain(row);
   dst.randomRealtime=controller.rowRandomVelocity(row);
   dst.pan=controller.rowPan(row);
   dst.pitch=controller.rowPitch(row);
   for(std::size_t step=0;step<dst.velocities.size();++step)
     dst.velocities[step]=controller.velocity(scene.pattern,row,step);
 }
 return scene;
}
} // namespace drumator::v101
