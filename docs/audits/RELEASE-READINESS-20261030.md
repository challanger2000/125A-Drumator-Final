# 125A Drumator v1.0.1 — Release readiness / non-regression audit

**Date:** 2026-10-30  
**Product branch:** `develop/v1.0.1-gui-redesign`  
**Exact tested product SHA:** `b78e38c0c740c54d9570d31b96cb1cb7fcf15cf2`  
**CI:** [GitHub Actions #40997226195](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40997226195) **SUCCESS**  
**Tests:** **29/29 CTests**, **47/47 Steinberg Validator**, **58 PASS / 0 WARNING / 0 FAIL** in independent 125A Plugin Tester v0.3.2.  
**Exact tested Windows x64 VST3 DLL SHA-256:** `409e7374b350a09eebcdc65ed59586aed6256bfbf96e1becb679556a3cf3e895`  
**Release status:** **RELEASE CANDIDATE / NOT FINAL**. Source/QA results and owner DAW reports must not be presented as comprehensive release certification.

## Independent post-QA evidence review

- Native GUI covers MAIN, SONG, VEL, SET, WAVE, all popup families, four zoom choices (100/120/130/150%), and 16-row pages. Screenshot and code geometries have separate source oracles; Windows suite confirms many native click interactions.
- F14 reproduces and repairs the owner-observed `ROWS 5-5 / 5` dead-end. F15 tests the 1-16-row x 1-10-viewport navigation matrix, including repeated NEXT/PREV and host-refused resizes. Codex05 confirms two real host-attached native V1.0.1 Views remain navigable when one host rejects a window height change.
- LIVE WAVE editor WEL1-WEL5 tests actual sequenced main-audio PCM change on next trigger, separation from audition preview, session exclusivity, stale-session rejection, successful CANCEL rollback, and native LIVE toggle. W11F tests WAV and FLAC import and live editor source refreshing. Mailbox unit test checks 50,000 coherent snapshots.
- Pattern/song MIDI and WAV export has exact step timing and sample-position checks (48 kHz/24-bit WAV, 960 PPQ type-1 MIDI). Optional sample-peak limiter, stereo buses, state recall and save, host-automation pattern parameter, real-time lock/voice constraints, 64-voice offline stress and validator are covered by tests.
- The user confirmed WAV export and LIVE-WAV-Editor in Studio One in the current conversation. Individual user reports are not a signed all-controls QA checklist.
- Inspected local packaged Windows QA artifact: ZIP has 17 items including 16 resources and native x64 VST3; archive integrity PASS. `moduleinfo.json` identifies 125A Drumator 1.0.1, VST 3.8.1, Instrument/Drum. Same SHA-256 DLL as the independent tester.
- German and English manuals have been authored from the implementation spec and generated as searchable PDF candidate documents with native screenshots. These binaries are **outside Git** in a staging Gumroad package; no PDF source is falsely claimed to be part of this commit.

## Actual source / release findings

| ID | Severity | Finding | Final gate |
|---|---|---|---|
| RA-01 | HIGH | The normal live editor CANCEL path is proven, but if `Drumator.EditorLiveEnd` cannot be delivered, `DrumatorView::removed`/destructor currently discard the return value of `stopSampleEditorLive()`. The processor may keep transient LIVE edits for subsequent triggers. | Persistently rejecting host-IPC fault test plus a guaranteed safe processor-side cleanup mechanism. Do not declare unconditional CANCEL/close safety until verified. |
| RA-02 | MEDIUM | `src/core/DragExportFile.h` explicitly retains successful unique temp directories after a drag, to avoid racing an asynchronous DAW import; no age-based cleanup exists. | Implement safely scoped stale-file cleanup, verify delayed import and never delete unrelated temp data. |
| RA-03 | MEDIUM | Root `README.md`, `CURRENT.md`, `AGENTS.md` contained leading 1.0.0/older 1.0.1 "current" assertions. | This docs-only commit adds an explicit top-of-file current-1.0.1 disclaimer. Before public release, archive obsolete release claims comprehensively. |
| RA-04 | HIGH | No reproducible full DAW manual matrix for the exact latest artifact: 4 save/drag exports, independent bus activation, Pattern/SONG and saved project recall, live editor APPLY/CANCEL after sample replace, 44.1/48 kHz, 100/120/130/150%, long ASIO/transport loop, two views, resize refusal. | Record real Studio One test matrix per action with pass/fail, project and current DLL hash; fix reproduced failures. |
| RA-05 | MEDIUM | Source repository contains no commercial LICENSE/EULA document and no confirmed publicly documented support destination; branded/third-party graphics and code rights cannot be proven from a green validator. | Owner defines software license/redistribution rights, customer support route and confirms all shipped artwork/code is distributable before Gumroad publication. |
| RA-06 | INFORMATION | No Windows publisher-signature evidence or end-user SmartScreen policy recorded. | Decide code signing or disclose unsigned binary and provide verification SHA. |

**Do not claim all release gates green based only on CI.** Studio One DAW testing cannot run in this GitHub Windows CI context, and offline deadline numbers are not ASIO guarantees.

## User-facing feature reference for DE/EN manuals

Windows x64 VST3 instrument. 16 rows (up to 10 visible), 32 steps, A-H patterns, 1/2 bar lengths (16/32 steps), SONG 1-64 entries of 8/16/32 sixteenths, REST and SONG START limited to 4/4 grid, WAV/FLAC one-shot import, non-destructive LIVE Sample Editor START/END/Fades/0-400% Gain, VEL/LIVE VEL and SET copy/paste/clear/bus/choke, up to 16 stereo outputs activated by host, Limiter at -0.3 dBFS sample-peak, 100/120/130/150% zoom (new-instance 130%), MIDI type 1 960 PPQ ch 10 notes 36-51, 48 kHz stereo 24-bit WAV, no auto loop tempo-matching, only Pattern A-H exposed as host automation. Embedded WAV limit 16 MiB per row / 64 MiB total.

## Further action and disposition

1. Close and regression-test RA-01 and RA-02 on a new product commit. Repeat full 29 (or more) CTests, 47 validator and 58 independent tester.
2. Owner performs exact-build real DAW matrix (RA-04) and reports any findings. Do not mark unexecuted cases PASS.
3. Confirm rights/EULA/support/signing decision (RA-05/06), finalize bilingual PDF manuals with the final QA hash, and package a **new exact audited VST3** and customer-facing metadata.
4. Review native GUI renders and buyer manuals one last time. Only then tag `v1.0.1` / FINAL and upload to Gumroad; no premature release/tag.

**Documentation-only change:** this status document and the root status banners alter no product source or VST3 package and do not invalidate the product binary SHA above.

## 30 October 2026 — product RC close-out of RA-01 / RA-02

**Current tested PRODUCT code:** `c90297b5ab9f44716e4d06f49dc913c643560705`; [Windows QA #41000904184](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41000904184) **SUCCESS**. Precisely **29/29 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL** in independent Plugin Tester v0.3.2. Tester-verified DLL SHA-256 `c7e07da90f9263560854e2ad5801263ffebd74a5b5687d4ca326088f11fc2164`.

**RA-01 — code mitigation and automated test PASS; live host-failure recreation NOT RUN.** `LiveEditLease` (audio callback only) expires unrenewed LIVE edits after 4 seconds of processed audio. The View periodically renews at approx. 400 ms using existing VST3 message transport; a crashed/detached editor cannot hold provisional audio indefinitely when `EditorLiveEnd` fails. The control actor allows a new session once a prior orphan has exceeded its wall-clock timeout. No audio-thread file I/O, lock, allocation or wall-clock call is introduced. `LiveEditMailboxTests` simulates no more END/renewal, checks forced expiry and subsequent renewal; the complete audio integration and independent tester pass. Real permanently rejected Studio One `EditorLiveEnd` was not provoked; expiry occurs after processed-audio time rather than instantaneous host destruction.

**RA-02 — scoped cleanup implemented and automated test PASS; DAW media ownership NOT PROVEN.** Before a new drag export, `DragExportFile::pruneStale` can remove at most 32 own folders per pass after 14 days, only with an exact three-decimal-field `125A-Drumator-Drag-...` directory and a single recognized matching regular file. Recent, symlink, foreign-content and malformed folders are excluded. The test sets 30-day old fixtures and verifies untouched recent/unrelated entries. **Important user caveat:** Windows/OLE drag does not tell the plug-in whether a DAW copied the file into a permanent project folder or still references the temp path. For releases, instruct customers to **copy/consolidate all dragged media** into their project before 14 days. Real Studio One copy/reference test remains required.

**RA-03 — top-level product status clearly superseded.** Current banner added above historical headings in README, CURRENT and AGENTS. Historic audit evidence is retained for traceability, not applicable as current FINAL status.

**RA-04 / RA-05 / RA-06 remain owner release gates:** exact-build Studio One/ASIO final matrix, active external outputs and four MIDI/WAV Save-and-Drag paths with DAW media ownership, rights/license/support terms, decision about signed/unsigned binary and actual customer ZIP verification.

The bilingual PDF manuals and prerelease Gumroad ZIP are generated as conversation artifacts, not committed as binaries into GitHub. The new exact DLL was included and the package ZIP CRC verified. No release tag or Gumroad listing has been published. **FINAL remains on hold.**
