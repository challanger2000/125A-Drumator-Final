# Drumator V1.0.1 — GUI replacement architecture

Date: 2026-10-27. Development branch: `develop/v1.0.1-gui-redesign`.

## Engineering decision

Build a **new GUI layer around the existing working controller**, rather than moving old controls one by one through the V1.0.0 painter. The reference image is a *visual and spatial* reference, not a feature specification. It depicts controls that do not exist in the current Drumator and omits some that must remain accessible. Do not add invented transports, tempo, limiter, groove, scales or modes just because they appear in the concept image.

- Preserve V1.0.0 DSP, MIDI, Controller, processor, serialization, existing sample editor and song behavior.
- New design and input geometry share the **same authored rectangle model** (`src/core/UiV101Layout.h`). No duplicated numeric click positions in the painter.
- All 32 step positions, sample prev/load/next, M/S/FILL, PREVIEW/EDIT, VOL/PAN/PITCH and VEL/SET must be accessible using ordinary mouse actions.
- Routing (MAIN/OUT), CHOKE, sample menu, ROW COPY/PASTE/CLEAR and legacy realtime random velocity are retained in a visible **SET** expansion. Velocity editing and SET use one mutually exclusive additional row; no stacked expansion lanes.
- Prototype uses 1280 logical pixels, 48px continuous rows and **up to ten visible rows**, leaving the 150% unexpanded window within 1920x1080. All 16 instruments remain reachable. Visible row count and pagination are candidates until native GUI and host-resize audits pass.
- No artificial design screenshots passed off as a plugin run. Only offscreen VSTGUI renderings from a compiled native fixture are valid GUI evidence.

## Strict migration gates

1. **Geometry contract**: isolated `UiV101Layout.h` and dedicated `UiV101LayoutTests.cpp`; existing V1.0.0 painter and hits untouched.
2. **Native rendering**: a separately selectable V1.0.1 VSTGUI painter reads this geometry; screenshot from Windows Actions. Only material/typography and spatial layout at this gate.
3. **Input integration**: route every `UiV101Layout::Action` to existing Controller methods, handling pointer start/move/up and preview/drag/drop, preserving mouse-only alternatives.
4. **Secondary controls**: mutually exclusive VEL/SET lane; all output, choke, legacy and row actions remain discoverable and testable.
5. **Host lifecycle and QA**: full existing CTest/independent plugin tester and native 100/120/130/150% images. Extend tests for new positions; **do not weaken or silently edit existing V1.0.0 regression assertions**. Keep a legacy-mode integration suite if migration changes old-coordinate UI fixtures.
6. **Only then** activate the new view by default. No main merge or release without explicit final approval.

## Status after isolated native VSTGUI prototype and asset reuse (27 Oct 2026)

- **Gate 1 passed**: standalone geometry / mouse-hit CTest included in the 24/24 successful tests in Windows Actions #40552248030, along with Validator 47/47 and independent plugin tester PASS.
- **Gate 2 prototype exists**: native VSTGUI offscreen Windows rendering at 100% and raster-scaled 130%, distinct from actual host-side zoom and distinct from the active controller-backed plugin editor.
- **Real graphic resources now wired into the prototype**: the authoritative 125A SVG paths already present in V1.0.0 (`BrandLogoGeometry.h`), original `Metallisches DRUMATOR-Logo.png` and original `125A_Drumator_Knob.png` with all 101 frames. Step keycaps use the original V1.0 accepted paint colors and geometry adapted to the new 48px row. Original controller/processor sources remain unchanged.
- **Fail-closed native render**: the integration capture requires the original logo PNG and exactly 125x12625 rotary sprites. It must not quietly substitute painted placeholders for missing assets.
- **New V1.0.1 hardware buttons**: the silver keycap family is VSTGUI code intended to be carried into the actual V1.0.1 editor, **not** a claimed existing PNG from V1.0.0. Mouse activation is not connected yet.
- **Pending acceptance**: current sprite-integrated source has **not yet been Windows-built or visually reviewed**. The last validated commit predates the sprite integration. Perform one manually dispatched Actions build after the completed work package and inspect its genuine images and pixel checks before claiming success.
- **Cost control**: V1.0.1 branch removed from push-triggered full Windows QA in `c2afc3e`; run `core-ci.yml` by `workflow_dispatch` for coherent milestones. `main` and release push checks remain automated.

Current working gate: **2 — integrate authentic GUI assets and inspect native pixels**, while preserving a separate known-working V1.0.0 view. Then proceed to gate 3 in batches.

### Controller-backed visual-state bridge (unbuilt candidate)

The separate native painter no longer owns fake drum names, hard-coded lit steps,
fictional mute/solo states or knob values. The integration fixture now:

1. Creates an actual ten-row embedded PCM kit via `KitCodec` and
   `Controller::setComponentState`, with selected Pattern C, explicit names,
   swing, humanize, master, mix, mute, solo and step velocities.
2. Captures one typed `UiV101NativePreview::Scene` exclusively through real
   Controller getters (`currentPattern`, `patternLength`, `velocity`,
   `label`, `hasSample`, `isRowMuted`, `isRowSolo`, row mix and globals).
3. Draws that scene using original 125A logo/title/knob assets and legacy
   step-key colors in the native offscreen renderer.
4. Adds a native integration oracle: the actual Controller toggles one step,
   the new Scene reflects it, and the VSTGUI key pixel must visibly change
   between the two renderings.

### Isolated native mouse-action bridge (unbuilt candidate)

The new `src/vst3/UiV101ControllerBridge.h` accepts the shared
`UiV101Layout::Hit` result and routes real mouse-target actions:

- `Step` → `Controller::toggleStep`; ignores hidden rows and steps
  beyond the active 16/32-step pattern length
- `Mute` / `Solo` → `Controller::toggleRowGate`
- `Previous` / `Next` → existing sample-navigation methods
- `Preview` → existing sample audition method
- `Fill`, `Load`, `Edit`, row selection, all rotary adjustments and
  `VEL`/`SET` report **DeferredToView**; a future actual GUI View must
  supply popup/modal and mouse-drag lifecycle rather than silently acting.

One native regression derives actual step, mute, fill and rotary hits from
geometry and confirms the real Controller state changes **only** for
implemented operations. This remains an isolated integration path, not the
active V1.0.1 editor; no DAW click is yet connected.

**Status: source committed; Windows compile, tests and DAW interaction
NOT RUN.** The next full QA should be explicitly launched once for the
combined authentic assets, controller scene and input bridge.

**Status: committed, not compiled or QA-approved.** The former static
layout screenshots are superseded, and no screenshot for this code has been
generated yet. Nothing has been activated in the packaged plugin, and the
existing 1.0.0 editor/controller/processor production code was not changed.
The next manually dispatched full QA should validate this entire package
before any visual acceptance.

### V1.0.1 interaction + native expansion milestone (source candidate, QA NOT RUN)

`src/vst3/UiV101Interaction.h` now manages a standalone pointer and
presentation state for the new 1280px GUI. No shipped view is switched over.
It implements real Controller-backed pattern A–H selection, 1/2 bar selection,
SONG toggle, row and global rotary drags, selective Velocity-lane painting
with undoable begin/commit/cancel gestures and protection for unoccupied steps,
single-row-exclusive VEL/SET expansion, SET copy/paste/clear/random velocity,
zoom selection with explicit host-resize rollback, and two-way pagination
across 16 instruments. OUT and CHOKE selectors plus Song Editor, FILL, import,
sample editor and other host-dependent actions still require real view popups.

**Height contract:** Up to ten collapsed instrument rows; no more than nine
while one extra 48px VEL/SET lane is expanded. Ten-row overflow automatically
shifts first row to keep the selected expanded row visible. Both states
measure 686 logical px and therefore 1029 px at 150%; no overlap with footer.

The native QA renderer now uses `UiV101Interaction::State` to draw the
controller-backed 9-row VEL and SET expansions and genuine button/velocity
pixels. New offscreen capture variants:
`v101-controller-velocity-lane-100.bmp` and
`v101-controller-settings-lane-100.bmp`. The capture code asserts a
specific orange velocity pixel and a text-free silver SET-button pixel:
**screenshots must reflect the requested state, not merely exist**.

A dedicated `testV101Interaction` Windows integration test exercises real
Controller mutations for header/pattern, song, rotary drag, 1-bar step
exclusion, M/S, ninth/tenth-row viewport behavior, velocity rollback and
commit, blank-step rejection, VEL/SET exclusivity, SET clipboard and clear,
popup deferral, zoom rejection/retry, and 16-row forward/back pagination.

| Area | New regression | Windows result | DAW result |
|---|---|---|---|
| Controller-derived scene + original brand/knob PNG | real KitCodec, native pixel transition | NOT RUN | NOT RUN |
| Header pattern/song/length and rotary drag | testV101Interaction | NOT RUN | NOT RUN |
| Step hit, 16/32 limit, mute and solo | testV101Interaction | NOT RUN | NOT RUN |
| Velocity press, cancel/commit, empty-step protection | testV101Interaction | NOT RUN | NOT RUN |
| One expanded row, 150% bound, navigation 16 rows | testV101Interaction + CTest geometry | NOT RUN | NOT RUN |
| SET copy/paste/clear, OUT/CHOKE modal deferral | testV101Interaction | NOT RUN | NOT RUN |
| Zoom modal and host-rejected zoom rollback | state test only; host resize not wired | NOT RUN | NOT RUN |
| SONG EDIT, FILL, sample/load/editor dialogs | no implementation yet | NOT RUN | NOT RUN |

This is a coherent package ready for **one manually dispatched** Windows
Actions run. The development branch intentionally has no automatic push CI.
Tests, bitmap captures and packaged VST3 acceptance **must not be described
as PASS until that manual run succeeds**. Even with green CI this remains
a separate view prototype rather than the installed plugin's live GUI.

### First batched Windows QA attempts (27 October 2026)

- **Actions #40563250423**: production and test compilation PASS; 23 of 24 CTests PASS. The new V101 interaction test FAILed on host-automatable Pattern C→D because the isolated test fixture omitted a VST3 `IComponentHandler`. Other CI stages were skipped, NOT PASS.
- **Actions #40564564947**: after adding the required test-only `IComponentHandler`, compilation FAILed under `/W4 /WX` on MSVC C4458 (parameter `iid` shadows interface `iid`). The parameter was renamed to `interfaceId` in commit `f1b0b3c`. No DSP, Controller or shipped Editor changes.
- **Current HEAD**: C4458 source correction committed; **NOT YET BUILT**. No further full Actions run is warranted for the single test-parameter rename. The next manual QA milestone must report its actual result and verify the previously blocked interaction scenarios, real resource loading, VEL/SET screenshots, Steinberg Validator and independent plugin tester. Do not describe the V1.0.1 prototype as QA PASS.
- The temporary push branch trigger used to initiate each run was reverted immediately; development pushes remain manual-only.

- **Actions #40566872805**: Windows compilation PASS; 23/24 CTests PASS. V101 interactive regression passed the previously blocked host Pattern D change, song/mute/solo, SWING, VOL and PAN checks but stopped at `V101 PITCH drag wrong`. Proven cause from `DrumatorProcessor.cpp`: `Drumator.RowPitch` requires `outputRate_>0` for `PreparedSample::fromWav`, whereas VOL/PAN can run with inactive audio. The test fixture did not call `prepareAudio(*f.p)`; this was corrected in commit `d72b081` with no shipping-code change. Remaining interactive and screenshot tests are **NOT YET VERIFIED**.
