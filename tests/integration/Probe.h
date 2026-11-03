#pragma once
#include <cstddef>
#include <atomic>
#include <functional>
namespace VSTGUI {class IDataPackage;}
namespace drumator::qa {
// Observe the real prepared file package before OLE enters a host-owned loop.
// Only generated QA sources use this hook; no DAW acceptance is simulated.
extern std::function<bool(VSTGUI::IDataPackage*)> exportDragObserver;
extern thread_local bool legacyTriggerOrdering; // regression negative, test EXE only
enum class AuditMutation { none, keepStaleExportTempo, suppressZeroStop, staleDuration, skipPeerCommit,
 basenameOnly, delayMidiOne, dropMidi, doubleMidi, zeroPrefix,
 ignoreSongLengths,
 skipImport, muteImportedProcessor, delayImportedProcessor,
 rejectPreviewStop, rejectPreviewStopOnce, patternQueryOOM, patternReplyOOM,
 oldFadeRenderer, oldKnobDrawCenters, loadRepaintOOM, refreshViewsOOM, disableWaveCache, bypassWaveRangeIndex, suppressMissingContextCarry,
 skipCycleSplit, skipCycleVelocityMap, skipCycleModulo,
 suppressCycleClockCache, freezeCycleTempo,
 keepCycleOnPartialDisable, ignorePartialTempo, repeatSongSnapshots,
 ignoreValidPartialPpq, keepComponentFirstOriginArmed };
extern std::atomic<AuditMutation> auditMutation;
// QA-only two-pass raster oracle: paint with and without actual knob faces.
extern std::atomic<bool> suppressRotaryFace;
extern std::atomic<unsigned> auditSongScheduleCalls;
extern std::atomic<unsigned> auditSongPatternSnapshots;
extern std::atomic<unsigned> auditViewRefreshFaultHits;
extern std::atomic<unsigned> auditWaveDecodeMisses,auditWaveFullReplies;
extern std::atomic<unsigned> auditWaveIndexedCalls;
extern std::atomic<const void*> auditImportTarget;
extern std::atomic<unsigned> auditImportCalls,auditDelayCalls;
enum Point { none, patternMidWrite, patternReadStarted, songTripleMidWrite,
             songAfterFirstPatternRead, restoreSnapshot,
             restoreMidCommit, restoreBeforeSelection, songMidPublish,
             auditAfterLengthRead, auditAfterAuditionAdopt, auditAfterAuditionStartTaken,
             auditAfterAuditionAssetPublished };
// Inserted only in generated test translation units, never the shipped VST3.
void probe(Point point) noexcept;
}
