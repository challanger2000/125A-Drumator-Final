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

**Status: committed, not compiled or QA-approved.** The former static
layout screenshots are superseded, and no screenshot for this code has been
generated yet. Nothing has been activated in the packaged plugin, and the
existing 1.0.0 editor/controller/processor production code was not changed.
The next manually dispatched full QA should validate this entire package
before any visual acceptance.
