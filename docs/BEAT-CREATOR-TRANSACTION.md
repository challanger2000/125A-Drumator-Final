# 125A Drumator 1.5 — Beat Creator stage 2: Verified A/B/C transaction

**Code:** \`fd0ae69a1b6836e8741cbcc963ed0a2cc7e71364\` on \`develop/v1.5.0\`.
**QA:** [Actions #41267109608](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41267109608) — Windows 31/31 CTests, Steinberg 47/47 validator, independent 125A Plugin Tester v0.3.2 58 PASS, zero warnings and failures.

## What is implemented

- \`src/core/BeatCreatorTransaction.h\`: validates the in-memory preview and old pattern state and creates an A/B/C before/after pair. Respects 16/32 steps; preserves user instruments 9–16 and hidden second-bar values when generating one-bar beats.
- \`src/core/AtomicPatternBank.h::replaceFirstThreeIfMatches\`: validates three before-images and lengths while holding one writer mutex, then applies all three new data arrays. Rejects malformed or stale input without a partial commit. Atomicity is at the **Processor control state** level, while realtime audio coherency remains per individual pattern.
- \`src/vst3/DrumatorProcessor.cpp\`: strict binary \`Drumator.BeatCreatorTriple\` command, expected/next patterns and durations validated and applied under \`kitMutex_\`. Zero file or sample changes.
- \`src/vst3/DrumatorController.cpp\`: \`prepareBeatCreator\` stages the preview without audio or project changes. \`cancelBeatCreator\` discards it. \`applyBeatCreator(allowOccupied)\` refuses any occupied A/B/C unless explicit permission is passed, rejects modified or recalled projects, and records a one-level group Undo. \`undoBeatCreator\` restores all three originals and their previous lengths. Existing Pattern MENU Undo selects grouped Undo while history is valid. Later manual edits of A/B/C invalidate group Undo safely.
- \`tests/BeatCreatorTransactionTests.cpp\`: validates occupied consent, all eight row roles, hidden data and malformed candidates.
- \`tests/integration/WindowsIntegration.cpp::testBeatCreatorV15GroupApplyUndo\`: exercises actual connected Controller, Processor and VST3 saved state: PREVIEW stays read-only; guarded Apply and group Undo round-trip; samples and rows 9–16 remain safe; current edits cannot be reverted by stale group history; project recall invalidates previews.

## Next milestones

1. Small audible factory preview kit with original/redistributable WAV samples (BD, Snare, CHH, OHH, Low Tom, High Tom, Crash, Clap), loaded only with user permission.
2. Separate audition transport that can hear draft A/B/C without altering project/host data. Evaluate genre quality in Studio One; do not claim audio quality from tests of data shape.
3. Optional Beat Creator GUI as **one button on the existing header**, with a small window. Keep complexity minimal; preserve original 3D GUI until functional review.
4. Full GUI/control interaction, project recall and end-to-end Studio One acceptance before version bump to 1.5.0 or any sale.

**Never merge this development branch to \`main\` or package it as a release without approval.**
