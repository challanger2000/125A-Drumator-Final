# 125A Drumator — authoritative current status

**30 October 2026 | Version 1.0.1 | Commercial customer-package CANDIDATE, NOT published.**

- **Branches:** `main` (canonical and GitHub default), `develop/v1.0.1-gui-redesign` (synchronized working branch). Both now point to the same 1.0.1 merge commit. Earlier 1.0.0 history is preserved, not active.
- **GUI-fix source SHA:** `2bc9ad0840bc8e9bf61e7cd3d87074699ad8f601`. **Exact Windows-QA/build commit SHA:** [`c18091ab3bdb0c80f7753ea6e3339463b26ad800`](https://github.com/challanger2000/125A-Drumator-Final/commit/c18091ab3bdb0c80f7753ea6e3339463b26ad800).
- **Windows QA:** [Actions #41133162498](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41133162498) — **SUCCESS** (29/29 CTests, Steinberg 47/47 Validator, 125A Tester 58/0/0).
- **Automation:** 66 stable automatable parameters; AV01–AV05 PASS; AUT08 Wave Editor Apply/Pitch PASS.
- **GUI correction:** Wave editor title, sample filename and LIVE ON/OFF have disjoint header bounds; long filenames display without hiding behind LIVE. Velocity strength bars use distinct cyan rather than amber Pattern-step color. New geometry and native actual-pixel checks PASS, tested at multiple zoom levels.
- **Factory privacy:** personal GitHub URL removed in `src/vst3/Factory.cpp`; vendor `125A Systems`, blank website/email in shipping VST3 metadata.
- **Current customer package:** `125A_Drumator_1.0.1_Gumroad_GUI_korrigiert.zip`, assembled from the exact successful Windows build, 26 files, ZIP SHA256 `9c51de67012a209202761785b0188e7bf4f518d353060b7962bc639bdd8d18fb`. Executable DLL SHA256 `2cd7a2e418b701d7196332607e1acab9a56030affb6ed68eae80f1185bfa22de`. Updated DE/EN PDFs show the corrected WAV-editor header. This ZIP **supersedes** `125A_Drumator_1.0.1_Gumroad_Kundenpaket_mit_Lizenzhinweisen.zip` with the older GUI.
- **Package preflight:** PASS. Binary, moduleinfo, customer texts, both searchable manuals/PDF metadata, PNG metadata and member SHA256 manifest checked; known private identifiers and the named DAW absent. `THIRD_PARTY_NOTICES.txt` now includes the pinned Steinberg VST3 SDK MIT notice, pinned VSTGUI BSD-style notice, and pinned dr_flac MIT No Attribution notice.
- **Customer documentation:** DE and EN manuals updated to describe all 66 parameters and current project recall.
- **Sales/publication:** Not uploaded to Gumroad; no release tag or FINAL label. All three known external code dependencies allow commercial distribution subject to their notices (included). Separate rights in non-code assets and business/customer terms remain outside automated code QA; publication needs owner authorization.

**Repository cleanup:** all four obsolete root audits were moved without changing their content into `docs/history/root-audits/`; the previous `main` 1.0.0 landing documents are retained in `docs/history/main-v100-*-archive.md`. The merge was ancestry-preserving, not a forced branch reset. Source files, assets, tests and CMake match the validated 1.0.1 product commit `c18091ab3bdb0c80f7753ea6e3339463b26ad800`. The merge itself changes history/documentation, not VST3 product logic.

This status, not dated audit reports, is authoritative. [Documentation guide](docs/INDEX.md). The source branch may have newer docs-only commits; the compiled tested DLL is pinned to the exact SHA above. No GitHub release is published.

This status supersedes earlier source/audit records. Technical docs and status changes after the executable SHA do not alter the tested binary.
