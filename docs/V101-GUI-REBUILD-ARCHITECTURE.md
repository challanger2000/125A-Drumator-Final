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

### Batched native GUI milestone — verified success (27 October 2026)

**Windows Actions #40568273913** on source/workflow trigger commit
`7cfbf4815dc23e0e8e5137326b78756758209526`: overall **SUCCESS**.
The development branch's auto-push trigger was immediately restored to
manual-only in `95313e2`; that follow-up changes only CI trigger policy,
not the tested plugin/UI implementation.

- MSVC x64, C++ / VST3 compilation: **PASS**.
- CTest: **24/24 PASS**, including Controller → native pixel scene and the
  complete new `testV101Interaction` with real VST3 host handler, prepared
  sample-rate PITCH drag, velocity undo/commit and zoom/pagination contracts.
- Official Steinberg Validator: **47/47 PASS**.
- Packaged host VSTGUI resource and native prototype screenshot QA: **PASS**.
  Captures include 100%, 130% raster, zoom popup, live VEL lane, SET lane;
  artifacts `125A-Drumator-VSTGUI-Native-Renderings` on this exact run.
- Independent pinned 125A Plugin Tester v0.3.2: **58 PASS / 0 WARNING /
  0 FAIL**, `RELEASE: PASS` *for the tested packaged V1.0.0-compatible
  plugin and its existing live GUI*, not the unintegrated V1.0.1 UI.

**Scope boundary**: this verifies the *isolated V1.0.1 native design renderer,
its Controller-bound scene and standalone pointer-action test*. The active
shipped VST3 editor is still the original V1.0.0 view. Therefore real host
interaction with the new view, its popups (SONG EDIT, FILL, OUT/CHOKE, load,
sample editor), resize negotiations, stateful refresh/reopen and Studio One
acceptance are **NOT RUN / PENDING**. No merge to main, release, or claim of
finished V1.0.1 editor is authorized by this run.

### Reference palette correction — hardware material pass (27 October 2026)

The native 100% preview was compared against the owner's original
`Drumator Rhythmusmaschine im Retro-Design` hardware illustration.

**Measured color discrepancy, not just subjective style:**

| Region | Previous V1.0.1 raster | Hardware reference (sample) | New VSTGUI palette |
|---|---|---|---|
| Blank faceplate | RGB 214, 211, 202 | RGB ~199, 191, 181 | RGB 201, 194, 183 |
| Upper controls panel | RGB 225, 223, 216 | RGB ~202, 195, 185 | RGB 207, 200, 189 |
| Row well / chassis | RGB 4, 14, 27 | RGB ~27, 40, 54 | RGB 27-30, 41-45, 56-61 |
| Inactive step face | RGB 41, 53, 69 | RGB ~36, 45, 56 | RGB 36, 51, 70 |

The screenshot has variable lighting; sampled reference numbers are indicative,
not a requirement of pixel-for-pixel duplication. Main colors are centralized
under `UiV101NativePreview::Skin`. Adjusted brushed warm steel, satin
button tops, silver-bevel shadow, deep navy row material, amber-lit keys,
metal panel separators and badge treatment. Exact original 125A logo vector
paths and metallic DRUMATOR title PNG remain unchanged. Original 101-frame
knob graphics, geometry, Controller and DSP are untouched.

The native raster test now asserts warm-steel color ordering at a blank
faceplate coordinate and navy color ordering at a bare row-chassis coordinate.
**This paint-only candidate requires one native Windows screenshot review;
it is not yet a visually accepted or production-integrated V1.0.1 editor.**

### Reference-color QA evidence — Windows SUCCESS (27 October 2026)

**Run [#40571877383](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40571877383)**,
code/trigger commit `644a02402d9fda1a9aefbb87becb32d70edc1299`:

- Windows VST3/test build: **PASS**.
- **24/24** CTests, including the native-render material RGB contract and
  the separate controller/input integration tests: **PASS**.
- Steinberg validator: **47/47 PASS**.
- Native 100/130% screenshot fixtures, menu, VEL/SET: **PASS**.
- Independent plugin tester: **58 PASS / 0 WARN / 0 FAIL**.
- Preview images: workflow artifact `125A-Drumator-VSTGUI-Native-Renderings`.
- Manual-only GUI-branch CI push filter restored in commit `ec33d6d`
  (workflow-only; image renderer code unchanged).

**Owner visual approval still pending.** Even with sampled colors matching the
reference more closely, the hard-edged rectangular button/bevel geometry may
still look like a retro desktop UI rather than expensive rack hardware.
Any subsequent design refinements should be grouped in one coherent package,
then built once, not in a series of trivial Action runs. The newly colored
renderer remains a standalone QA prototype, not the VST3 editor opened in DAW.

### Hardware material pass following owner review (27 October 2026)

Native VSTGUI renderer `tests/integration/UiV101NativePreview.h`
has received a **coherent material treatment** rather than another color-only
revision. A deterministic three-point satin gradient (`satin`), pixel-corner
machined button silhouettes (`rounded`), corner fasteners (`screw`),
recessed bronze/graphite header sections, bezel-lit key edges, and subtle
horizontal steel brushing are all drawn by VSTGUI. Dark navy row modules now
have dark gaskets, an inset directional gradient, upper catch light and
lower shadow. Original amber step lenses have dimensional bezels and a warm
light source; the existing original 125A rotary sprite gets a machined mounting
socket instead of a flat circle.

**No geometry, step pitch, hit rectangles, Controller/DSP, pattern semantics,
sample state or shipping V1.0.0 editor was changed.** New drawing helpers
operate entirely inside the isolated native visual fixture. The previously
verified warm-steel / navy RGB pixel contract and native 100/130%/VEL/SET
capture oracles are intentionally retained, not weakened.

**QA for this material pass: pending one batched Windows workflow run.
Visual acceptance pending inspection of its genuine screenshot.**

### Actual screenshot design review and socket correction

The first dimensional-material Windows Actions run **#40574975025**
completed with Windows/CTest/Validator/native-raster and independent plugin
tester jobs SUCCESS, but real 100% screenshot review revealed that the
new rotary mounting sockets (radius 28–30px) were visually oversized:
row sockets nearly touched across 48px lanes and header sockets obscured
the SWING/HUMANIZE/MASTER engraving. This is a **real visual regression**,
not a functional QA failure.

Source fix `e8a01d3`: reduce row socket outer radius to 23px and header
socket to 22px; suppress overly pronounced steel brush lines. Follow-up
`a8a2271`: compile-time geometry contracts guarantee the 48px row
socket cannot overlap its neighbor and header mount stays within its
designated segment. No GUI hit rectangles or DSP/Controller changes.

**Corrected source not yet Windows-rendered at time of this note**;
one combined QA/capture pass must verify the actual pixel result.

### Post-review hardware material QA accepted by CI (27 October 2026)

**Windows workflow #40576509346** (`cbff75b94db5257b5619ce4cd31ae463a0b64ecd`) completed
**SUCCESS** after correcting the oversized ring geometry:

- MSVC Windows VST3 and integration test build: PASS.
- CTest **24/24 PASS**, including new Controller/VEL/SET and native screen oracles.
- Official Steinberg VST3 Validator **47/47 PASS**.
- Native VSTGUI 100% and 130%, velocity, settings and zoom captures: PASS.
- Independent 125A Plugin Tester v0.3.2: **58 PASS / 0 WARNING / 0 FAIL**.
- Native artifact `125A-Drumator-VSTGUI-Native-Renderings`, exact run.
- The preview has been **visually inspected against the earlier hardware-pass
  screenshot**: rotary mounting sockets are smaller, no longer collide across
  instrument rows and no longer obscure SWING/HUMANIZE/MASTER labels. The satin
  brushing is more restrained.
- Auto-push on V1.0.1 was restored to manual-only by workflow-only commit
  `49a8725` after the run began.

**Acceptance boundary:** Technically QA-green native prototype is *not*
an owner-approved final GUI or the active DAW editor. Material direction now
merits owner design review. The next engineering work remains integration
into the actual plugin VSTGUI view and completion of deferred native menu
and editor interactions; no code has been merged to main or released.

### Hardware-header alignment, depth and title revision (27 October 2026)

Owner review identified the PATTERN metal panel beginning at x=21 while the
navy sequencer began at x=12, plus a control strip that sat visibly too low
and mismatched hardware buttons in the same row. The isolated V1.0.1 renderer
now uses the shared 12..1260 chassis silhouette, a raised y=78..137
machined header recess, and one `UiV101Layout` button strip:
`headerButtonY=99`, `headerButtonHeight=32`. Pattern A–H, PTNR MENU,
1/2 BARS, SONG and SONG EDIT are all painted from those same hit rectangles,
not from unrelated hand-adjusted offsets. Global rotary centers were moved
from y=119 to y=114 and their sprite face limited to 44px to avoid the
upper engraving and dark column-header strip.

The old dark-skin `Metallisches DRUMATOR-Logo.png` is **no longer used
in V1.0.1 native rendering**; an eight-glyph engraved steel DRUMATOR
heading replaces it. The old asset remains untouched in the shipping
V1.0.0 VST3 for compatibility. The original 125A emblem and authentic
101-frame knobs stay unchanged.

`testV101Interaction` now asserts common geometry across every header key,
rejects clicks in the previous low zone, and marks PTNR MENU as a deferred
view popup. Original Pattern and SONG Controller behavior remains under
the existing integration regression. **Status: source committed; Windows
CI raster and user design approval not yet obtained.** `main` and
`release/v1.0.0` unchanged.

### Raised silver-header QA — verified on Windows (27 October 2026)

**Successful native screenshot + QA run:** [#40582025305](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40582025305), tested trigger SHA `a18646628786049b24eeca65e00fa8f11db563aa`.

- MSVC Windows x64 plugin / native integration build: **PASS**.
- 24/24 CTests: **PASS**, including unified 32px PATTERN/2 BARS/SONG click hit map, PTNR MENU deferral and controller-backed input regressions.
- Official Steinberg validator: **47/47 PASS**.
- Independent 125A Plugin Tester v0.3.2: **58 PASS / 0 WARNING / 0 FAIL**.
- Generated native preview 100% and raster 130%, zoom overlay, VEL and SET; exact run artifact `125A-Drumator-VSTGUI-Native-Renderings`. Visually verified the header's left border starts at the same x=12 as the navy sequencer and every header action has a uniform 32px silver key.
- Workflow-only commit `b4993a1` immediately restored the on-demand CI policy; no auto-trigger on ordinary V1.0.1 work.

**Failed intermediate attempts are not hidden**: #40580416043 exposed an obsolete 30px SONG height assertion, corrected in `d2f1ccd`; #40581398480 hit /WX C4127 due to a constant checked through the test macro, corrected using compile-time `static_assert` in `a4bea1f`. Final run above verifies that corrected exact source.

**Owner design approval pending.** The warm engraved DRUMATOR title is a new paint-only replacement in the *separate native V1.0.1 prototype*; the existing V1.0.0 plugin editor, its dark-title PNG resource, audio engine and main/release branches have not been changed. Actual DAW V1.0.1 editor activation and deferred menu functions remain a separate engineering gate.

### Futuristic user-logo asset — source integration (27 October 2026)

The owner supplied `resource/Futuristisches Metalllogo DRUMATOR.png`
on `develop/v1.0.1-gui-redesign`. This exact filename has been retained.
The dark chrome DRUMATOR wordmark, glowing triangular A and symmetric amber
lightbars replace the temporary V1.0.1 per-glyph metal engraving in the
**isolated native VSTGUI renderer only**. The old title resources are
preserved for the shipping V1.0.0 editor.

`CMakeLists.txt` includes the new PNG in VSTGUI bundle resources. The new
`measureTitle` function reads its alpha-visible bounding box once on
image load, and `hardwareTitle` draws the original image at uniform scale
inside logical x=192..1118, y=7..76. It preserves both side flashes and
never crops lettering or changes interactive hit positions. Title metrics
are intended to be cached by the eventual live editor, not recomputed
on every paint.

The native Windows test fails closed if the resource is missing, has
no visible alpha data, is opaque in the upper-left source corner, occupies
an overlapping header rectangle, or its orange lightbars fail to appear
in the final 100% VSTGUI header pixel capture. This is a new visual
regression contract, not only a successful PNG file presence check.

**QA status for this new logo candidate: NOT YET RUN.** Do not call the
live VST3 editor migrated: the native renderer remains isolated, and the
host-side popups/Studio One interaction acceptance remains future work.

### Futuristic DRUMATOR PNG — Windows verified (27 October 2026)

Native design renderer source `tests/integration/UiV101NativePreview.h` now
loads the unmodified user asset
`resource/Futuristisches Metalllogo DRUMATOR.png`. The previously
painted character-by-character metal title is gone. Original pixel alpha
and both orange lightbar details are preserved. Alpha-visible bounds are
measured once per image load; the rendered logo is uniformly scaled and
centered inside x=192..1118, y=7..76.

**First build #40586611433:** 24/24 CTests and Steinberg Validator 47/47
passed, but the isolated native screenshot explicitly failed to find the
logo resource. This exposed an actual CI packaging oversight: the custom
PowerShell bundle assembly manually copies assets, separately from
CMake's VSTGUI resource list. The new PNG was absent from this copy.

**Corrected build [#40587499875](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40587499875):
SUCCESS.** PowerShell now explicitly copies the original-name PNG to
`Contents/Resources/`, includes it in the exact VST3 tree manifest,
and enforces nonempty resource size. Test evidence:

- MSVC Windows and native VSTGUI renderer compile PASS.
- CTest 24/24 PASS, including PNG alpha-transparent corner, non-empty visible
  pixels, non-overlapping header bounding box and true amber-glow header pixels.
- Steinberg validator 47/47 PASS.
- Independent Plugin Tester v0.3.2: 58 PASS, 0 WARNING, 0 FAIL.
- Native artifact `125A-Drumator-VSTGUI-Native-Renderings` from run
  #40587499875: personally reviewed the actual 1280×686 header image.
  The new dark chrome wordmark is sharp; both amber bars fit between the 125A
  badge and the zoom selector and do not overlap other controls.
- Manual-only development-branch push policy restored immediately in
  workflow-only commit `97bacee`.

**Scope:** This is a verified isolated Windows VSTGUI prototype and a
resource safely packaged into the development VST3 bundle, **not** a claim
that the shipped V1.0.0 view displays the new asset. The controller-connected
new V1.0.1 view has not yet been attached to the actual DAW plugin/editor.
`main` and release branch remain unchanged.

### V1.0.1 footer fastener clearance and compact pagination (27 October 2026)

After owner screenshot review, both lower cabinet screws were obscured by
the footer paint, and the two page buttons were too large and misaligned.
The inset footer panel now spans x=32..1248 (instead of 14..1260), with
original case fasteners at x=14 and x=1266 explicitly painted **after**
the footer. The existing original screw art is untouched.

The page keys now use identical **95x25** satin hardware caps,
`UiV101Layout::footerPrev` x=1020..1115 and
`footerNext` x=1130..1225, both y=10..35 relative to the
48px footer. `UiV101Interaction` uses precisely the same rectangles;
the old 120/130x31 invisible click regions are removed. The counter
is repositioned inside the inset panel without altering any song,
instrument, DSP or controller data.

Compile-time geometry assertions protect screws and footer clearances;
native regression tests assert correct page navigation, inactive old
hit zones, and **both visible screws in the rendered VSTGUI pixels**.
The earlier centered 125A master SVG is included in this source
candidate and remains unreleased. **Native Windows CI not yet run** for
this footer package; evaluate it with one grouped QA/screenshot build.

**First QA attempt [#40590579238](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40590579238):** MSVC build PASS,
23/24 CTests PASS. The single new pagination interaction test used
the 10-row page's footer Y after navigating to a 6-row second page.
That stale test coordinate caused `V101 compact previous page not symmetrical`.
The test now recomputes `pages.height()-L::footer` after pagination
(commit `79b6f884`). Product/UI page code required no change. The native
pixel screw oracle and release validator for this candidate remain
unverified until the subsequent full run.

### Compact footer and case-screw QA verified (27 October 2026)

Final batched run [#40591425134](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40591425134)
is **SUCCESS**:

- Windows native renderer/MSVC compilation PASS.
- CTest **24/24 PASS**, including the shared 95x25 PREV/NEXT hit regions,
  no legacy oversized click hotzone, dynamic footer position after a
  transition from 10 to 6 visible instruments, and both case screws
  demonstrably visible in the 100% Windows VSTGUI pixel capture.
- Steinberg Validator **47/47 PASS**.
- Independent plugin tester **58 PASS / 0 WARNING / 0 FAIL**.
- Actual rendered images: artifact `125A-Drumator-VSTGUI-Native-Renderings`,
  run #40591425134, including 100 and 130 percent previews.
  The bottom-left and bottom-right screws are now completely unobstructed;
  the page keys align inside the narrower, evenly inset metal footer.
- CI auto-push was immediately reset to on-demand in workflow-only
  commit `28539ff`. No merge to V1.0.0 or main.

The prior failed run #40590579238 was caused by the test using the
**first-page footer height for its click after navigating to a shorter
second page**, corrected in `79b6f884`; the live pointer/paint code
did not need an additional fix.

**Design note:** The 100/130% screenshots are real native previews of
the isolated V1.0.1 layer, not an installed DAW replacement. Owner visual
approval and production editor integration remain separate milestones.
