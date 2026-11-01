# 125A Drumator — authoritative v1.5 development status

**1 November 2026 — Stage 3: Original 8-piece factory audition kit, WAV listening pack and all Windows QA PASS.**

- **Stable released 1.0.1 remains at** \`main\` \`f4c5caa4be82626ea89459d9835c12138d7c6a1b\`, without any v1.5 changes or new product package.
- **Audition milestone source verified at** \`5f03e2d549f9a406aaabc7ddc71bf759abd10b6c\` on \`develop/v1.5.0\`.
- **[Windows Actions #41268791112](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41268791112) SUCCESS:** **32/32 CTests**, **47/47 Steinberg Validator**, **58 PASS / 0 WARNING / 0 FAIL independent 125A Plugin Tester**.
- Audition WAV artifact: **\`125A-Drumator-15-Beat-Creator-Audition-WAVs\`** (GitHub run #41268791112). Contains exactly **8 independently synthesized original** 44.1kHz/16bit mono drum WAVs, **24** 48kHz/24bit stereo A/B/C loops for all eight BeatCreator styles, and a START-HERE text. The ZIP archive was downloaded, CRC-verified, wave-format checked, and sample RMS/peak checked (no silent or full-scale clipped files).
- Audition render uses the existing **Drumator OfflineWavExport, StepScheduler, SampleVoiceEngine** on an **isolated temporary KitState**. No external/sample-pack audio assets were copied. User samples, loaded kits and patterns are never changed by the audition generation.
- **Stage 2 stays valid:** controller/processor \`prepareBeatCreator\`, explicit guarded \`applyBeatCreator\` for A/B/C, and grouped undo remain separately tested and available in development code.
- **Not yet implemented:** in-plugin Beat Creator button/dialog, **live** GUI/audio Preview, automatic sample-kit opt-in, owner Studio One musical listening/release signoff, GUI improvements. 1.5.0 has not been published or activated in the consumer VST3.
- QA DLL from stage 3 SHA256 \`49850e79c544d25b7434382f0ddfafae63e2cf454fc3e80f756fb19552cc066a\`. Development QA build still identifies as 1.0.1 by design until an actual 1.5.0 release candidate.
- Engineering: [Stage 3 audition source and test plan](docs/BEAT-CREATOR-FACTORY-AUDITION.md), [Stage 2 transaction](docs/BEAT-CREATOR-TRANSACTION.md), [roadmap](docs/ROADMAP-1.5.0.md).

---

## Previous stage: grouped Apply/Undo (historical)
# 125A Drumator — authoritative v1.5 development status

**1 November 2026 — Stage 2: Beat Creator A/B/C Preview, explicit Apply and grouped Undo — WINDOWS QA PASS.**

- **Stable product:** \`main\` remains at released v1.0.1, \`f4c5caa4be82626ea89459d9835c12138d7c6a1b\`. No code for v1.5 is merged or released there.
- **Validated v1.5 development code:** \`fd0ae69a1b6836e8741cbcc963ed0a2cc7e71364\`; branch \`develop/v1.5.0\`.
- **[Windows workflow #41267109608](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41267109608) SUCCESS:** 31/31 CTests (including real Controller/Processor \`BC15\` integration), Steinberg VST3 Validator 47/47, 125A Plugin Tester v0.3.2 58 PASS / 0 WARN / 0 FAIL.
- **Exact QA binary SHA256:** \`568fe581e9ac61a6caa1ad13b84b135e999e7a467459c67ca62890c08f45fdb2\`. The plugin factory still identifies as v1.0.1: this is **not** a v1.5 consumer build or release package.
- **Implemented:** \`BeatCreator::preview\` calculates Generate A, Variate B and Break C for 8 styles, 16/32-step duration; controller holds the preview without modifying the running kit. \`applyBeatCreator\` explicitly refuses occupied A/B/C unless authorized, verifies no intervening edits/recalled kit, and sends one strict binary compare-and-swap IPC. Processor atomically validates and updates all 3 patterns under its control-thread writer mutex. \`undoBeatCreator\` restores A/B/C and their prior lengths in one operation. Existing Pattern MENU UNDO routes into this group when it is active, and manual edits invalidate stale group history.
- **Protection:** samples, kit metadata, patterns D-H, user rows 9-16 and hidden second-bar steps remain unchanged. Test \`BC15\` verifies Processor save-state, transaction, stale guards, hidden data, ordinary Undo and recall.
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
