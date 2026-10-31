# 125A Drumator — authoritative current status

**31 October 2026 — Version 1.0.1 — F20 10+6 page correction owner-confirmed; F21 4px footer bezel alignment in development, QA pending.**

## Unreleased F21 footer alignment (4 logical pixels)

- Studio One owner screenshots confirm the 10+6 paging of F20.
- Footer bezel originally spans x16..1264 while the instrument chassis spans x12..1260. Move **only** the bezel by 4 logical pixels left. Same 1248px width; all five footer buttons and their hitboxes remain unchanged.
- This is a graphics-only correction for **version 1.0.1**, not version 1.0.2. Source requires new QA before updating the Gumroad ZIP.

## Exact current development candidate (F20)

- **Normal page navigation:** 16 rows → page 1 = 1–10, page 2 = 11–16; 11 rows → page 1 = 1–10, page 2 = **only 11**. Never repeat the previous page's instrument controls.
- **DAW window:** normal VSTGUI logical size stays 1280×686 while switching NEXT/PREV (no VST3 host resize request). The unoccupied lower portion of a partial page remains noninteractive dark chassis background; the footer stays attached to the bottom. Host-constrained peer-window fallback is unchanged.
- **Windows-tested exact product source:** `e983a7fc5655a38773ef2bdad4ef81b2d9a5a221`.
- **[GitHub Actions #41153911888](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41153911888): SUCCESS.** 29/29 CTests PASS, 47/47 Steinberg Validator PASS, independent 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- **Product DLL SHA256:** `a509aa1c1924b6ba84b72812526e3a9f6afbbca5b80d35017a7a6125da2bfddb`.
- **Private Windows x64 VST3 test ZIP:** `125A_Drumator_1.0.1_Next_Page_10_plus_6_Test.zip` (19 files, full .vst3, START-HERE, SHA256SUMS); ZIP SHA256 `3aa8d00a22a45870a80a52f0b56d2d78ef5159096ff4cce3086e0bce47140945`.
- **Native render proof:** `v101-current-page2-last-six-130.bmp` shows rows 11–16 only; `v101-f20-11rows-next-page-130.bmp` shows row 11 only. Real 130% host-size/click tests PASS.
- **Still pending:** Owner Studio One test and any updated Gumroad ZIP. The published 1.0.1 ZIP and `main` remain unchanged.

## Prior failed attempts

- F18 only repaired lower decorative photo cropping; it did not eliminate the host's tiny window.
- F19 fixed the host window but mistakenly duplicated instrument rows by overlapping pages (2–11 and 7–16). **Rejected; previous F19 test ZIP must not be distributed.**
- The new F20 retains both proper page partitioning and stable host geometry. **No 1.0.2 version bump.**

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
