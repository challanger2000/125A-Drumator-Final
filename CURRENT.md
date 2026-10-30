# 125A Drumator — authoritative current status

**30 October 2026 | Version 1.0.1 | Gumroad publication and hands-on Studio One test completed (owner-confirmed).**

## Canonical source and exact binary

- `main` is the canonical default branch; `develop/v1.0.1-gui-redesign` is the synchronized working branch. Both refer to the current 1.0.1 source. The 1.0.0 version remains reachable in Git history.
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
