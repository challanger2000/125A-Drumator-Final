# 125A Drumator — authoritative current status

**30 October 2026 | Version 1.0.1 | Commercial customer-package CANDIDATE, NOT published.**

- **Source branch:** `develop/v1.0.1-gui-redesign`.
- **Exact compiled product source SHA:** [`1acfe3698bcbe6eb88e1faee6ff8e538edbc390d`](https://github.com/challanger2000/125A-Drumator-Final/commit/1acfe3698bcbe6eb88e1faee6ff8e538edbc390d).
- **Windows QA:** [Actions #41125370883, attempt 2](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41125370883) — **SUCCESS** (29/29 CTests, Steinberg 47/47 Validator, 125A Tester 58/0/0).
- **Automation:** 66 stable automatable parameters; AV01–AV05 PASS; AUT08 Wave Editor Apply/Pitch PASS.
- **Factory privacy:** personal GitHub URL removed in `src/vst3/Factory.cpp`; vendor `125A Systems`, blank website/email in shipping VST3 metadata.
- **Customer-package candidate (with third-party license notices):** `125A_Drumator_1.0.1_Gumroad_Kundenpaket_mit_Lizenzhinweisen.zip`, assembled outside Git from this exact tested build. 26 files, ZIP SHA256 `81eb84f1529c3a4aefd797689630ce03a0252c01ba9730e9449f2a2b9ede08dc`. The executable SHA256 remains `cf66bf3c6360eae6b8ca5bcd527806e8b3380d2fce227f766fb7ff68da3ff6cc`.
- **Package preflight:** PASS. Binary, moduleinfo, customer texts, both searchable manuals/PDF metadata, PNG metadata and member SHA256 manifest checked; known private identifiers and the named DAW absent. `THIRD_PARTY_NOTICES.txt` now includes the pinned Steinberg VST3 SDK MIT notice, pinned VSTGUI BSD-style notice, and pinned dr_flac MIT No Attribution notice.
- **Customer documentation:** DE and EN manuals updated to describe all 66 parameters and current project recall.
- **Sales/publication:** Not uploaded to Gumroad; no release tag or FINAL label. All three known external code dependencies allow commercial distribution subject to their notices (included). Separate rights in non-code assets and business/customer terms remain outside automated code QA; publication needs owner authorization.

This status supersedes earlier source/audit records. Technical docs and status changes after the executable SHA do not alter the tested binary.
