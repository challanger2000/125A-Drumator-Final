# 125A Drumator — authoritative current status

**31 October 2026 | Version 1.0.1 | F19 NEXT PAGE fix Windows QA PASS; private Studio One verification pending.**

## Current private GUI correction (F19)

- **Problem:** With eleven instruments the final page previously showed only ROWS 11-11 / 11 and shrank the host window from 686px to 254px logical height. This produced an unwanted partial GUI within Studio One. An initial bottom-metal-faceplate-only correction (F18) did not remedy the window resize. The first fixed-height render then revealed an unacceptable large blank panel.
- **Final F19 layout:** All normal 10-row viewport pages remain at logical **1280×686** and never request another host resize merely to navigate NEXT/PREV. Pages are anchored so they remain fully occupied by actual instrument rows, even when the last page overlaps the previous page: **11 total = ROWS 1-10 then 2-11**; **16 total = ROWS 1-10 then 7-16**. Instrument controls, row identity and input hit targets use those true row indexes. Constrained peer views with host-resize refusal retain their existing compact recovery strategy.
- **Exact tested executable source commit:** `6d60a4da95e1b07f9e1673232d18d1a368169ec2`.
- **[Windows QA run #41152681580](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41152681580): SUCCESS** — 29/29 CTests, 47/47 Steinberg VST3 Validator and independent 125A Plugin Tester v0.3.2: **58 PASS / 0 WARNING / 0 FAIL**.
- **Screenshot evidence:** Real native VSTGUI rendering `v101-f19-11rows-next-page-130.bmp` shows ROWS 2-11 / 11 at the actual 130% zoom; 4-zoom screenshots and exhaustive 1-16 row paging tests also succeeded.
- **Exact Windows executable SHA256:** `11de6738ea6ad66d9c60dc88a481ce703923c1f82e09fff4387527ad6a5caee0`.
- **Private full VST3 test package:** `125A_Drumator_1.0.1_Next_Page_Fix_Test.zip`, 19 files including START-HERE and SHA256 manifest, ZIP SHA256 `5b33baf1d527673fb08fa75ae05ee90b4cc324b33b9328f7d67b65b5d398ef2e`. Created outside GitHub; not the published Gumroad ZIP.
- **Owner decision:** Keep the product **version 1.0.1**. No 1.0.2, new tag or unrelated DSP changes. Test the private Windows VST3 in Studio One, then consider replacing the existing Gumroad file within 1.0.1. There is no confirmation of the F19 Studio One result or updated Gumroad upload yet.

**Earlier F18 fix:** Metal-rim cropping alone passed its automated tests but did not fix the one-row host-window issue. Its former test ZIP is superseded by the F19 candidate; no separate F18 customer release occurred.

**30 October 2026 | Version 1.0.1 | Gumroad publication and hands-on Studio One test completed (owner-confirmed).**

## Canonical source and exact binary

- `main` remains the published 1.0.1 canonical branch; `develop/v1.0.1-gui-redesign` now intentionally differs with the unreleased Windows-validated F19 page-layout fix and test. Older version history remains reachable.
- **Exact last compiled/tested product source SHA:** `e630602a634bbd0773352a5235eb8b3c2a4cb7ab`.
- **Exact successful Windows Actions:** [#41140679693](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41140679693).
- **CTest:** 29 PASS / 0 FAIL. **Steinberg VST3 Validator:** 47 PASS / 0 FAIL. **Independent 125A Plugin Tester v0.3.2:** 58 PASS / 0 WARNING / 0 FAIL. The independent tester validated the packaged Windows binary from this exact run.
- **Windows VST3 executable SHA256:** `a76d9dca83e96b547eaa1cd2699be358108e0dc576f15b8fa160b89eb9f2efe2`.
- **Factory privacy:** Seller-specific personal GitHub links are not embedded in the shipping VST3 factory metadata.

## Current GUI (replaces all older screenshots and UI-package references)

The approved 1.0.1 UI layout now includes all of the following, along with earlier verified WAVE-editor LIVE label and cyan-Velocity-bar fixes:

1. Instrument header/actions order: **INSTRUMENT / SAMPLE — PLAY — FILL — EDIT**. The native pointer hitboxes, Fill popup anchor, source names and integration checks were updated together.
2. **LIMIT ON/OFF** and **ZOOM** share the header baseline and retain 83×32 button size, aligned as a group to the sequencer's right edge (x=1260), including Zoom popup options.
3. Footer **EXPORT**, **PREV PAGE**, and **NEXT PAGE** retain 95×25 size with equal **15px gaps**.
4. The **complete original 125A SVG master logo**, with SYSTEMS/AUDIO SOFTWARE lettering preserved, has its visible left edge aligned with PATTERN A at x=33.

These are GUI layout refinements, not a new audio/DSP or parameter-ID revision. Screenshots are taken from the genuine Windows native VSTGUI rendering, and the full native interaction and 4-zoom matrix passed in the successful Windows run.

## Replacement Gumroad customer package

- **File:** `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` (created as a conversation artifact, **not** checked into GitHub).
- **ZIP SHA256:** `f455403523951bdc829c73eaca41437f1ed351ec374c93dd0c291cf774e136d7`.
- **26 customer files:** full installable Windows x64 `125A Drumator.vst3` bundle, German and English illustrated searchable PDF manuals, DE/EN install guides, changelog, license and support notice, third-party code notices, build-identification and per-file SHA256 checksums.
- Image resource metadata may be removed; decoded image pixels and compiled VST3 binary are preserved and verified. No audit reports, build scripts, QA logs or private paths enter the customer archive.
- **Previous ZIP** `125A_Drumator_1.0.1_Gumroad_GUI_korrigiert.zip` is superseded; do not distribute the older UI package.

## Owner release confirmation and limitations

On **30 October 2026**, following preparation and successful automated QA of the updated customer ZIP, the owner confirmed **both** of the remaining steps had just been completed: a hands-on test of the latest VST3 in Studio One, and upload/publication of the Gumroad package. This is an owner-reported outcome; GitHub CI neither runs Studio One nor verifies a Gumroad storefront listing. No separate public Gumroad listing URL has been recorded. The earlier "not published" and "Studio One test pending" labels are superseded by this owner confirmation.

## Scope, publication and limitations

- Windows x64 VST3 only. 16 sample rows, 8 A-H patterns, 16/32 steps, SONG, integrated WAVE editor, WAV/FLAC import, MIDI/WAV Pattern/SONG export, optional limiter, 100/120/130/150% Zoom and **66 stable VST3 automation parameters**.
- The exact build passed CI and automated host-integrated tests. The owner additionally confirms having performed a hands-on Studio One test of the latest VST3; **that personal DAW session was not performed or independently documented by GitHub Actions**.
- Included VST3 SDK (MIT), VSTGUI (BSD-style) and dr_flac (MIT-0) third-party notices enable license-compliant redistribution of those components. Ownership/distribution rights of original branded artwork and the seller's consumer-facing terms remain the seller's responsibility.
- **Gumroad publication/upload has been confirmed by the owner**. No GitHub FINAL release tag is implied or created by this documentation update. The public listing content and exact downloadable bytes were not independently checked against the owner-reported release.
- These current details supersede historical audit and package records. [Documentation guide](docs/INDEX.md) retains older evidence for traceability.
