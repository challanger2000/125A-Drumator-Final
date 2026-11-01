# 125A Drumator — authoritative v1.5 development status

## Stage 6.0 — BC30 expressive velocity editor (1 November 2026)

- **Exact tested source:** `4cbe891f9942547e4e8996c51a26b627b96f6c8f` on `develop/v1.5.0`; [Windows Actions #41294300629](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41294300629) **SUCCESS**: **33/33 CTests**, **47/47 Steinberg official VST3 Validator**, **58 PASS / 0 WARNING / 0 FAIL independent 125A Tester v0.3.2**.
- **Interaction:** single-bar mouse clicks still set per-note velocity; ordinary mouse dragging paints all occupied steps crossed, including steps skipped by sparse mouse-move events (with linear value interpolation); **Shift+drag** creates a line, **Shift+Alt+drag** creates a smooth ease-in/ease-out curve, **Alt+click/drag** accents only existing notes to at least velocity 120. Ctrl drag continues to set occupied velocity to 1. New gesture modes are latched on mouse-down; Shift fine-control of knobs remains unchanged.
- **Safety/Undo:** no new notes in empty cells, no inactive 17–32 bar edits in one-bar mode, no other-row or other-pattern mutation; retracting an unfinished line restores original values beyond new span; pointer cancel restores original snapshot; entire completed stroke is one undo group. No menus or extra knobs. Three small reminder labels appear in the expanded VEL row.
- **Tests:** BC30 pure `VelocityStrokeShapeTests.cpp` covers sparse pointer interpolation, 16/32 limits, blank steps, line/retraction, curve, accent. Native Controller/Processor V101 interaction covers Shift/Alt state and Undo. The actual attached VSTGUI mouse event regression validates Shift drag and a single Undo. GUI and default patterns otherwise unchanged.
- **Development build:** [Windows VST3 ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41294300629/artifacts/13078048138). No merge into the sold `main` branch or Gumroad replacement. Actual owner Studio One UX/sonic acceptance remains separate from Windows CI.

---

## Stage 5.9 — BC29 eight CC0 recorded genre kits: WINDOWS QA PASS (1 November 2026)

- **Exact successfully tested source:** `fdbcf4adeb95712802950f5497adc1ea03e8eff4` on `feature/v1.5-eight-genre-kits`. [GitHub Actions #41289563236](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41289563236) **SUCCESS**: **32/32 CTests**, **47/47 official Steinberg VST3 Validator**, **58 PASS / 0 WARNING / 0 FAIL independent 125A Plugin Tester v0.3.2**.
- **Eight distinct genre-specific recorded kits / 64 processed PCM16 WAVs** from immutable-commit-pinned CC0 sample sets: Michael Fischer TR-808 recordings ([TidalCycles](https://github.com/tidalcycles/sounds-tr808-fischer)) and Karoryfer *Big Rusty Drums* ([source](https://github.com/sfzinstruments/karoryfer.big-rusty-drums)). See `docs/GENRE-KITS-SOURCES.md`, `assets/beatcreator/SOURCE-MANIFEST.json` for per-file source paths/hashes and exact origin revisions. Some recordings recur across compatible styles; this is 8 curated kits, not 64 independently unique raw captures.
- **Real VST3 integration:** PLAY A/B/C uses the selected genre's actual recorded kit. On GUI APPLY, only previously empty rows 1–8 receive those same sounds. The Controller and Processor independently load and validate the same packaged resources, and the existing atomic A/B/C edit + no-overwrite protection remains. The 64 WAV files live inside `125A Drumator.vst3/Contents/Resources/BeatCreator/`, with provenance; no external sample library installation is needed. The old synthetic factory implementation remains as a legacy/unit-test helper, not the selected-genre GUI path.
- **Verification:** native BC16/BC17/BC18/BC19 regression tests, all 32 CTests, packaged VST3/Validator and independent Plugin Tester passed; kit loader unit QA loads and decodes all eight kits. The standalone [candidate kit ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41288910518/artifacts/13075989066) was separately generated and audited; the [new Windows VST3 bundle](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41289563236/artifacts/13076981140) includes those resources.
- **Release boundary:** branch is still pre-release. No claim of human musical listening, acoustic genre authenticity or owner Studio One acceptance. `main` and the sold Gumroad 1.0.1 remain untouched. No commercial 1.5 release is authorized by this source/QA milestone.

---

## Stage 5.8 — BC27 musician-oriented style audit (1 November 2026)

- **Validated exact commit:** `7177afd1201151f4b1aa466e1167d6d2a052ca0b`, `develop/v1.5.0`; [GitHub Actions #41286921291](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41286921291) **SUCCESS**: **32/32 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL independent Plugin Tester v0.3.2**. Built Windows VST3 DLL SHA256 `15ad2f5553bac4b534ece863849daa5c176e6f2512644bb55760ebb2db339b91`.
- **Musical/product judgment (code and rhythmic theory, not an undocumented human listening test):** Keep Industrial/NDH, EBM, Techno, Rock and Hip-Hop structures as they are, because they already have coherent genre-specific foundations. Improve House (two offbeat open hats, different from Techno's single open hat), Drum & Bass (low-velocity ghost snare at step 7 at ordinary 35% Complexity, plus step 15 at higher Complexity), Metal (0%-Complexity kick skeleton reduced from rapid double-kick to a few anchor kicks), and acoustic Rock/Metal break C (tom-roll note density increases with Complexity instead of always being a four-hit roll). All eight genres retain the common strong snare backbeat and the preview/apply/undo flow. No new GUI options, no automatic tempo changes, no changes to existing kit samples.
- **New BC27 test:** across 48 seeds verifies House offbeat open hats and Techno differentiation, DnB ghost strength 30–55 vs strong backbeat, all electronic quarter-note kicks, sparse-vs-dense Metal, genre-faithful Hip-Hop and Rock kick placement, sparse 0%-Complexity tom fills and zero simultaneous open/closed hi-hats. Earlier BC26 64-seed note-placement-diversity tests remain active.
- **Current installation package:** [Windows VST3 ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41286921291/artifacts/13075907546); [15 genre/variation audition WAV examples](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41286921291/artifacts/13076120375).
- **User release boundary:** `main` remains the sold 1.0.1 version; no release, Gumroad replacement or merge has been made. Successful static/music-theory and real VST3 integration QA does **not** equal proof of subjective musical excellence, which still requires actual hearing with suitable samples.

---

## Stage 5.7 — Genre-specific Beat Creator groove engine (1 November 2026)

- **Latest tested SOURCE code:** `8386945f34c55babb6488aa53909f2c2aba2e22d` on `develop/v1.5.0`, [GitHub Actions #41284886394](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41284886394) **SUCCESS**, **32/32 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL independent 125A Tester v0.3.2**; exact VST3 DLL SHA256 `d0bffe170421819f2e87f9e3502b34792e5ae54b168e9a98591c966659850e9c`.
- **Musical changes:** 3 distinct genre templates for each of 8 genres (Industrial/NDH, EBM, Techno, House, Rock, Metal, Hip-Hop, Drum & Bass), plus 3 controlled 16th-note ornament variants at normal Complexity (up to 9 distinct step patterns/style); consecutive GENERATE seed increments cannot produce the same core motif at the 35% default. Two-bar (32-step) phrases use a different template in their second bar. B changes a genre-dependent closing note rather than always toggling the last kick. C has three separate genre-appropriate last-four-step fills (acoustic tom rolls, industrial/electronic accents, hip-hop sparse fills, drum & bass pickups), preserving the rest of A and the bar's core backbeat.
- **BC26 deterministic tests:** explicitly compare occupied steps, not just random velocity, across 64 seeds per style at Complexity 35/75, enforce 6+ distinct note-placement structures, prevent identical consecutive default-complexity structures, validate second-bar developments and multiple fill outcomes. Existing A/B/C selection, guarded APPLY, group UNDO, copied patterns D–H, WAV/host engine and all GUI layout stay unchanged.
- **Earlier Stage 5.6 on same source:** clicking the existing Complexity percentage activates a native inline edit in the same 52×25 area. Enter accepts 0–100 (including 67%), rejects invalid values; ESC/blur/cancel do not commit; `−/+ ` continue to change 10 percentage points. Closing the modal removes the native EDIT safely. Both the centred style bezel and aligned numeric controls remain as in BC23/BC24.
- **Installation / hearing:** [Current Windows VST3 ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41284886394/artifacts/13073764936); [15 Beat Creator audition WAVs](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41284886394/artifacts/13074443621); [native GUI shots](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41284886394/artifacts/13075290245).
- **Release boundary:** no merge into `main`, no release or Gumroad update. Musical approval still needs an actual Studio One listening test against the intended grooves and user samples. These tests verify note placement / stability, not subjective sound quality.

---

## Stage 5.5 — Beat Creator buttons match established Drumator hardware (1 November 2026)

- **Exact validated code:** `27106d07bb59ad9342bae63460c456366b130d74` on `develop/v1.5.0`; [GitHub Actions #41281044652](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41281044652) SUCCESS: **32/32 CTests**, **47/47 Steinberg VST3 Validator**, **58 PASS / 0 WARNING / 0 FAIL independent 125A Tester v0.3.2**. Windows VST3 DLL SHA256 `4189f36c5a2dd86aa16995c63fec830c5826c7538f4d5c92bebfcd1f95c7ef74`.
- **Owner's explicit GUI standard applied:** `GENERATE`, `PLAY A/B/C`, `STOP`, `APPLY`, `CANCEL` now use **95×25 logical pixels**, exactly matching existing `EXPORT`, `PREV PAGE`, `NEXT PAGE` caps. APPLY caption shortened to `APPLY`, overwrite confirmation to `CONFIRM`. Eight genre buttons are **136×25** only because `INDUSTRIAL / NDH` and `DRUM & BASS` need more characters. No giant 154/192px controls.
- **Panel:** fixed **640×240** logical pixels centered in 1280px-wide editor at all host window heights, including the minimal 254px client area. Four style columns / two rows, exact VSTGUI bitmap-backed dark button skin, same Wave-Editor metal faceplate crop, original common paint/hit rectangles.
- **BC21/BC22 proof:** built-in layout test compares Generate against `UiV101Layout::footerAdd` width/height, confirms all 17 click targets do not overlap at 254/286/390/420/686px; real native VSTGUI captures at 254px and 686px and pixel-checks photographic faceplate plus dark cap paint. Pattern generator, WAV install, names, transport, undo and engine unchanged.
- **Windows install artifact:** [complete VST3 ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41281044652/artifacts/13072979472); [rendered GUI screenshots](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41281044652/artifacts/13073910066).
- **Not released:** `main` remains v1.0.1 FINAL; no new Gumroad package. Awaiting owner's Studio One visual approval. The temporary factory sample quality and modal instant-close 200ms feedback limitation remain separate future considerations.

---

## Stage 5.3 — 200ms active-color button flash (1 November 2026)

- **Validated Windows source:** `9c386d28f76caa3dd9c9002d657fc86596f18f38`, branch `develop/v1.5.0`; **[Actions #41276387281](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41276387281)** SUCCESS: **32/32 CTests**, **47/47 Steinberg Validator**, **58 PASS / 0 WARNING / 0 FAIL 125A Independent Tester v0.3.2**. Tested DLL SHA256 `2a55f38194e31d8e7e18a3a3e64de5c27a60b6176cfee93f06c8d947cfdde834`.
- **Implementation:** for 200ms following left click, all shared `UiV101NativePreview::cap` hardware buttons render with the **same exact ACTIVE palette/sprite** as latched controls, then return automatically to normal. Includes Beat Creator GENERATE/APPLY/STOP/PLAY/style/complexity, MAIN and SONG buttons, Wave-Editor toolbar/LIVE, SET and footer actions, native popup caps. Persistent toggle states remain unchanged; disabled caps do not flash; separate plugin views cannot leak flash state.
- **BF20 real Windows pixel regression:** checks exact click-vs-active image parity on representative sizes and labels (GENERATE/APPLY/Pattern/SET/footer/Wave), pressed-sprite priority, disabled-state nonillumination, click outside, and nested paint scope isolation. The existing 33ms meter timer performs the redraw/expiry.
- **Scope caveat:** the feedback applies to `cap` buttons actually visible at repaint time. Instant-dismissals such as successful Beat Creator and Wave-Editor APPLY/CANCEL, or popup choices that disappear immediately, cannot visibly hold the 200ms highlight with the current synchronous close behavior. SONG-chain tiles, step pads and rotary knobs are custom interactive elements with their own selection/value painting rather than `cap` buttons. Do not describe the change as every interactive pixel flashing.
- **Artifact:** [Complete Windows VST3 QA ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41276387281/artifacts/13071438804). `main` remains released 1.0.1, unchanged. This is not a v1.5 Gumroad release.

---

## Stage 5.2 — compact host GUI + playable kit on APPLY: WINDOWS QA PASS (1 November 2026)

- **Exact validated source:** `87e205d9c2ac07780bf7fddc5de9370395879140` on `develop/v1.5.0`; **[GitHub Actions #41274961846](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41274961846)** SUCCESS: **32/32 CTests**, **47/47 Steinberg Validator**, **58 PASS / 0 WARNING / 0 FAIL 125A Plugin Tester v0.3.2**.
- **Verified Windows VST3 DLL SHA256:** `4f7c52889823f5f6a73fbfcf51f53fbf3e3771035d644b3fba739073db3308cd`.
- **Owner Studio One findings addressed:** (1) `BEAT CREATOR NEEDS A TALLER HOST WINDOW` on the default small-row editor; (2) APPLY previously transferred only MIDI-style steps, not any WAV/labels, with only four visible rows.
- **Small host view:** single Beat Creator modal automatically becomes a compact 240px dialog inside a 254px minimum plugin client. No request to enlarge the host window. BC18 tests click real VSTGUI widgets at 254px; all 17 clickable modal rectangles are collision-checked for 254/286/390/420/686px.
- **PLAYABLE GUI APPLY:** the same A/B/C pattern commit now installs the eight original temporary audition WAVs and legible roles **BD / Snare / CHH / OHH / Low Tom / High Tom / Crash / Clap** into **empty first-eight slots only**, and exposes at least eight instrument ROWs. Existing loaded WAVs, filenames, custom display labels, kit settings, D-H patterns, other rows and Song are protected. Processor prepares all assets off the real-time thread and refuses stale/occupied slots rather than overwriting.
- **New BC19 integration:** checks visible ROWs=8, factory WAVs/name persistence, preserved user sample and custom name, protection of noncreator rows, and grouped pattern Undo. BC17's GUI APPLY regression now verifies serialized WAV+label installation. Real owner DAW listening and musical acceptance remain necessary (the synthesized kit is an acknowledged low-quality placeholder).
- **Undo semantics:** the existing grouped Undo reverts A/B/C steps. Installed sample assets remain like ordinary Drumator sample assignments; Undo does NOT silently delete newly imported audio. CANCEL before APPLY still changes no saved kit.
- **Download validated dev VST3:** [Windows artifact 13071366425](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41274961846/artifacts/13071366425), contains complete `125A Drumator.vst3` folder and resources. QA build identifies as v1.0.1 in host factory metadata by design, not a v1.5 release.
- **Release unchanged:** `main` remains the published v1.0.1; no merge or Gumroad publication. Future: owner Studio One hands-on test, improved factory drums and rhythmic feel, final GUI audit, v1.5 review.

---

## Stage 5 — Beat Creator GUI/A-B-C listening: WINDOWS QA PASS (1 November 2026)

- **Validated exact code:** `78dbad14674fc2be43469b1d92a3634a80a3f8c3`, branch `develop/v1.5.0`.
- **[Windows Actions #41272670153](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41272670153): SUCCESS** — **32/32 CTests**, **47/47 official Steinberg VST3 Validator**, **58 PASS / 0 WARNING / 0 FAIL 125A Plugin Tester v0.3.2**.
- **Tested VST3 DLL SHA256:** `f101785f24ee8883dcd3fc54949e4b1490bbd98c36fa31a880fecbfbc5762d17`.
- **Actual V1.0.1 View integration:** one BEAT CREATOR header button between SONG EDIT and three tightened hardware-style rotaries. Small dark modal, eight genre choices, simple 0–100% complexity +/- control, GENERATE, PLAY A/B/C (one-shot), STOP, guarded APPLY, CANCEL. 16/32-step duration follows current pattern. No duplicate sequencer or extra sample kit import.
- **Safety:** generates into temporary data; previews through Stage 4 ephemeral audition voice. No saved kit/samples/SONG/user patterns changed before APPLY. Occupied A/B/C require explicit second APPLY click; grouped Undo restores all three. D-H and rows 9-16 protected.
- **BC17 real Windows VSTGUI test:** clicked Beat Creator, selected genre, increased complexity, GENERATED draft, PLAY A yielded real audio, STOP silenced, first APPLY on occupied pattern left project untouched, second APPLY committed A/B/C, grouped UNDO restored, CANCEL discarded second draft. PASS.
- **GUI/layout tests:** newly tightened header rotaries and 83x32 Beat Creator button have shared paint/hit geometry with nonoverlap assertions; entire 32/32 CTest suite passed.
- **Not yet approved as v1.5 consumer release:** real owner Studio One functionality and musical-quality acceptance, final GUI design audit, optional preview-kit opt-in integration, remaining roadmap items, signed-off packaging and release are outstanding. No v1.5 Gumroad upload has occurred. The QA build still identifies as 1.0.1 by design.
- **Stable sales branch:** `main` remains the released v1.0.1 at `f4c5caa4be82626ea89459d9835c12138d7c6a1b` and was not changed.


---

## Stage 4 — audible A/B/C audition transport: WINDOWS QA PASS (1 November 2026)

- **Verified code:** `878a2e6eb70341936845b3c92264d4481274a740` on `develop/v1.5.0`.
- **[Windows Actions #41271108699](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41271108699): SUCCESS.** 32/32 CTests, including BC16 connected Controller/Processor audio-preview regression; 47/47 Steinberg validator; independent 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- **Verified Windows DLL SHA256:** `f58ae0afe7c83d8cd11604567ffd570858d1d57a4a1337779bdb11af02361910`.
- **Implemented:** `playBeatCreatorDraft(0/1/2, bpm)` plays A Generate, B Variate and C Break at 16 or 32 steps through a distinct audition voice, with `stopBeatCreatorDraft()` and `cancelBeatCreator()`. The in-memory audition uses the **existing** `OfflineWavExport`, `StepScheduler`, `SampleVoiceEngine` with the independently synthesized eight-piece factory audition kit. No temporary disk writes, user kit/sample/step changes, SONG changes, or VST3 project-state mutations.
- **BC16 test:** verifies START audio with stopped host, STOP silences, CANCEL rejects stale drafts, invalid selections are rejected, and processor saved kit/grid and lengths remain unchanged for A/B/C in both 16- and 32-step mode.
- **Not yet completed:** Beat Creator UI/button/dialog and real owner Studio One audition/quality acceptance. No automatically installed sample kit, no v1.5 consumer package or Gumroad publication. Stage 4 is a **verified technical milestone**, not an end-user feature until its controls are connected.
- **Frozen release:** `main` continues to serve released **v1.0.1** unchanged. Only `develop/v1.5.0` contains 1.5 development.

---

## Previous stage: Stage 3 factory audition kit (historical)

**1 November 2026 — Stage 3: Original 8-piece factory audition kit, WAV listening pack and all Windows QA PASS.**

- **Stable released 1.0.1 remains at** `main` `f4c5caa4be82626ea89459d9835c12138d7c6a1b`, without any v1.5 changes or new product package.
- **Audition milestone source verified at** `5f03e2d549f9a406aaabc7ddc71bf759abd10b6c` on `develop/v1.5.0`.
- **[Windows Actions #41268791112](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41268791112) SUCCESS:** **32/32 CTests**, **47/47 Steinberg Validator**, **58 PASS / 0 WARNING / 0 FAIL independent 125A Plugin Tester**.
- Audition WAV artifact: **`125A-Drumator-15-Beat-Creator-Audition-WAVs`** (GitHub run #41268791112). Contains exactly **8 independently synthesized original** 44.1kHz/16bit mono drum WAVs, **24** 48kHz/24bit stereo A/B/C loops for all eight BeatCreator styles, and a START-HERE text. The ZIP archive was downloaded, CRC-verified, wave-format checked, and sample RMS/peak checked (no silent or full-scale clipped files).
- Audition render uses the existing **Drumator OfflineWavExport, StepScheduler, SampleVoiceEngine** on an **isolated temporary KitState**. No external/sample-pack audio assets were copied. User samples, loaded kits and patterns are never changed by the audition generation.
- **Stage 2 stays valid:** controller/processor `prepareBeatCreator`, explicit guarded `applyBeatCreator` for A/B/C, and grouped undo remain separately tested and available in development code.
- **Not yet implemented:** in-plugin Beat Creator button/dialog, **live** GUI/audio Preview, automatic sample-kit opt-in, owner Studio One musical listening/release signoff, GUI improvements. 1.5.0 has not been published or activated in the consumer VST3.
- QA DLL from stage 3 SHA256 `49850e79c544d25b7434382f0ddfafae63e2cf454fc3e80f756fb19552cc066a`. Development QA build still identifies as 1.0.1 by design until an actual 1.5.0 release candidate.
- Engineering: [Stage 3 audition source and test plan](docs/BEAT-CREATOR-FACTORY-AUDITION.md), [Stage 2 transaction](docs/BEAT-CREATOR-TRANSACTION.md), [roadmap](docs/ROADMAP-1.5.0.md).

---

## Previous stage: grouped Apply/Undo (historical)
# 125A Drumator — authoritative v1.5 development status

**1 November 2026 — Stage 2: Beat Creator A/B/C Preview, explicit Apply and grouped Undo — WINDOWS QA PASS.**

- **Stable product:** `main` remains at released v1.0.1, `f4c5caa4be82626ea89459d9835c12138d7c6a1b`. No code for v1.5 is merged or released there.
- **Validated v1.5 development code:** `fd0ae69a1b6836e8741cbcc963ed0a2cc7e71364`; branch `develop/v1.5.0`.
- **[Windows workflow #41267109608](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41267109608) SUCCESS:** 31/31 CTests (including real Controller/Processor `BC15` integration), Steinberg VST3 Validator 47/47, 125A Plugin Tester v0.3.2 58 PASS / 0 WARN / 0 FAIL.
- **Exact QA binary SHA256:** `568fe581e9ac61a6caa1ad13b84b135e999e7a467459c67ca62890c08f45fdb2`. The plugin factory still identifies as v1.0.1: this is **not** a v1.5 consumer build or release package.
- **Implemented:** `BeatCreator::preview` calculates Generate A, Variate B and Break C for 8 styles, 16/32-step duration; controller holds the preview without modifying the running kit. `applyBeatCreator` explicitly refuses occupied A/B/C unless authorized, verifies no intervening edits/recalled kit, and sends one strict binary compare-and-swap IPC. Processor atomically validates and updates all 3 patterns under its control-thread writer mutex. `undoBeatCreator` restores A/B/C and their prior lengths in one operation. Existing Pattern MENU UNDO routes into this group when it is active, and manual edits invalidate stale group history.
- **Protection:** samples, kit metadata, patterns D-H, user rows 9-16 and hidden second-bar steps remain unchanged. Test `BC15` verifies Processor save-state, transaction, stale guards, hidden data, ordinary Undo and recall.
- **Not implemented:** Beat Creator GUI/button/dialog, audio-preview transport, legally cleared factory kit, Studio One audition tests, measured musical quality. There is **no v1.5 customer release**. GUI design comes last by owner instruction.
- Technical details: [Stage 2 transaction notes](docs/BEAT-CREATOR-TRANSACTION.md), [foundation](docs/BEAT-CREATOR-FOUNDATION.md), [roadmap](docs/ROADMAP-1.5.0.md).

---

## Previous stage: core foundation (historical, superseded)

# 125A Drumator — active development

**1 November 2026 — Drumator 1.5.0 Beat Creator core: Windows QA PASS, GUI/integration NOT YET IMPLEMENTED.**

- Dedicated branch: `develop/v1.5.0`, initially based on released v1.0.1 commit `f4c5caa4be82626ea89459d9835c12138d7c6a1b`.
- **Stable published v1.0.1 on `main` remains untouched**, source commit unchanged. Nothing here is merged into the product branch or approved for sale.
- **Validated source SHA:** `6ac507df17ca83552dcf23b7df980b6918ae6e9e`.
- **[Windows Actions #41265815951](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41265815951) SUCCESS:** 30/30 CTests (including the new Beat Creator tests), Steinberg Validator 47/47, independent 125A Plugin Tester 58 PASS / 0 WARN / 0 FAIL.
- **Current generated DLL SHA256:** `34e6a78b223e3a6f8cd1623ae0f23b55348ccd5625a06957e6195a1f30c267ce` (regression binary still reports project version 1.0.1; it does **not** contain a Beat Creator GUI or connected controller).
- **Implemented (pure data core):** `src/core/BeatCreator.h` with eight styles, 16/32 steps, `generate` (A), `variate` (B), `makeBreak` (C), `preview` and occupied-pattern detection. A/B/C remain in memory. Stable mapping: BD, Snare, CHH, OHH, Low Tom, High Tom, Crash, Clap (top rows 1–8). Rows 9–16 are not touched. Regression cases cover 2,560 combinations and native array safety.
- **Not implemented:** a plugin-facing Beat Creator control, audio audition, sample kit, grouped atomic APPLY/UNDO, UI redesign or user Studio One listening test. The core has only structural/technical validation, not musical listening sign-off.
- **Read next:** [foundation implementation notes](docs/BEAT-CREATOR-FOUNDATION.md) and [1.5.0 roadmap](docs/ROADMAP-1.5.0.md).
- No v1.5 VST3 sale package, version bump or deployment is authorized.

---

## Frozen v1.0.1 product baseline (reference only)

**31 October 2026 | Version 1.0.1 | F21 footer alignment complete; verified updated Gumroad customer ZIP prepared.**

## Authoritative source and build

- **Main and previous 1.0.1 development branch:** same complete 1.0.1 product state before creating the 1.5.0 branch. Do not use superseded F18/F19/F20 candidate ZIPs.
- **Exact product code verified:** `3748b1e9e1867357362304c51bec2cb99ac9657b`.
- **[Windows Actions #41157069202](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41157069202): SUCCESS.** 29/29 CTests, Steinberg VST3 Validator 47/47, independent 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- **Product DLL SHA256:** `06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278`.
- **Updated complete customer ZIP:** `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`; 26 files; SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`. Prepared outside the repository and validated for archive integrity and all per-file hashes. Both illustrated DE/EN PDFs use the actual new GUI screenshot.
- **Storefront status:** The previous 1.0.1 Gumroad package was confirmed uploaded by the owner on 30 October 2026. This new corrected ZIP **must replace that file in Gumroad**; upload by the owner has not been verified or performed here.

## Final GUI corrections within 1.0.1

- **F20 (verified in owner's Studio One screenshot):** Regular instrument pages are nonoverlapping: 1–10 then 11–16, or only row 11 for an 11-row kit. Normal 10-row viewport keeps its 686px logical height across NEXT/PREV. Unoccupied areas are inactive background. Constrained peer-window recovery remains intact.
- **F21:** Footer bezel shifted exactly 4 logical pixels left, from x16..1264 to **x12..1260**, matching the row chassis. Footer stays 1248px wide. All button rectangles, interaction hitboxes, songs, DSP and window size logic remain untouched.
- **Visual regression:** Pixel difference between prior F20 and F21 actual Windows renders is confined to the footer bezel region (130% main, 16-row second page, and 11-row second page); there are no other changed pixels.
- **Earlier 1.0.1 work retained:** PLAY/FILL/EDIT order, corrected headings and colors, aligned right-header controls, Logo aligned to PATTERN A, 66 stable automation parameters.
- **Version decision:** **1.0.1**. No 1.0.2, no new release tag.
- **QA scope:** Windows automated host/native render CI and independent plugin validation passed. The owner's previous Studio One check covers F20. A fresh manual DAW check of this F21 DLL was not performed; F21 only moves the decorative footer panel.

## Superseded items (historical only)

- F18 corrected a photo crop but did not solve DAW resizing.
- F19 used overlapping 2–11/7–16 pages and was rejected.
- Earlier `125A_Drumator_1.0.1_Next_Page_10_plus_6_Test.zip` and `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` are superseded for distribution.
- Older audit reports remain only as historical evidence; README and this file identify the current build.
