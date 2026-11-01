# 125A Drumator — authoritative v1.5 development status

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
