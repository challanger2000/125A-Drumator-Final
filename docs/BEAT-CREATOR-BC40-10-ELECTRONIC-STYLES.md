# 125A Drumator – Beat Creator rebuild / BC40

## Branch and baseline
Isolated branch `develop/beat-creator-10-styles-20261102` was created **from main (project version 1.0.1)**.
The existing Library contains the commercially packaged v1.0.1 release:
- `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`
- `Drumator_1.0.1_Gumroad_NEU.zip`
These two archives have **identical SHA-256** `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb` and each contains 26 files, including the Windows x64 VST3, two manuals, build/checksum information and START-HERE-DE. ZIP CRC integrity check passed.
The earlier `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` is different (SHA-256 `f455403523951bdc829c73eaca41437f1ed351ec374c93dd0c291cf774e136d7`). The **October 31 package** is the verified latest available release baseline, not a missing attachment.
Therefore **do not ship this branch as a Gumroad update** before binary-baseline reconciliation and Studio One manual QA.
No changes were made to WAV export, host tempo, processor, controller, sample loader or VST3 GUI in this milestone.

## Scope – electronic styles only
The intended GUI is **one STYLE dropdown**, not a wall of buttons:

1. 80s Synthpop
2. Italo Disco
3. Synthwave
4. EBM
5. 90s Eurodance
6. House
7. Techno
8. Trance
9. Hip-Hop
10. Drum & Bass

These styles are 10 distinct **rhythmic** arrangements, not sample presets.
Rock, Metal, EDM/Festival, Trap, Hardstyle, Dubstep, Electro Funk, Breakbeat, UK Garage and Afro House are not included.

## Fixed legend, no bundled or loaded samples
Instrument slot 01 BD/Kick, 02 Snare, 03 Closed Hi-Hat,
04 Open Hi-Hat, 05 Low Tom, 06 High Tom, 07 Crash, 08 Clap.
All source samples remain entirely user-owned; style selection changes **no WAVs**.
Slots 09–16, user sample state, row mixer/pitch/choke, song and other patterns
are outside the generator's scope.

## Groove algorithm
- Eight independently authored structural motifs **per style** = 80.
- A: choose one whole-bar musical pattern based on a deterministic seed.
- Two bars: bar two develops, never mindlessly duplicates, the first.
- B: genre-aware phrase-ending variation that preserves foundational accents.
- C: stylistic **last half-bar** turnaround (not an indiscriminate last-step random roll).
- Complexity 0–100 controls density and embellishments without creating or deleting sound files.
- Velocity is restrained, instrument-aware and bounded 1–127.
- Open/closed hats cannot occupy the same step.
- Sample data, audio clock, MIDI/WAV export and live playback are intentionally unaffected by generating a preview.
- A/B/C apply is transactional and requires consent before replacing occupied patterns; it preserves user slots 9–16 and hidden second-bar steps.

## Quality gates
Standalone CTest `drumator_beat_creator_tests` validates all 10 styles,
each of their eight structurally distinct motifs at 35% complexity,
A/B/C deterministic across seeds, lengths and complexity levels, no empty kits,
no accidental CHH/OHH collisions, no sample/mixer mutations, 2-bar progression,
early break conservation and occupied-pattern confirmation.
`drumator_velocity_stroke_shape_tests` checks the preserved
BC30 paint/line/curve/accent calculations from the previous prototype.

## Pending VST3 integration (intentionally NOT done in BC40)
1. Compare the already recovered, verified 2026-10-31 Gumroad release archive to the new VST3 build; do not ask for the ZIP again.
2. Port the already reviewed Beat Creator controller/processor transaction with minimal changes.
3. Port the BC30 velocity **mouse gesture UI** (not just its isolated algorithm), preserving existing v1.0.1 features.
4. Implement compact genre dropdown, permanently visible slot legend, no Preview Kit button or bundled kit code.
5. End-to-end Studio One practice tests: 16/32 steps, undo, save/recall, A/B/C preview,
   44.1 kHz, song chaining and **actual DAW-tempo-correct WAV export**.
   The prior export tempo bug remains unresolved and must not be called fixed.
6. Compare complete v1.0.1 archive vs candidate resource tree before release.

No changes to Gumroad/stable release occur without separate approval.
