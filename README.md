# 125A Drumator

Windows x64 VST3 drum sampler and 32-step sequencer with a modern hardware-style interface.

**Version:** 1.0.1 · **Status:** Released on Gumroad (owner-confirmed); current Windows build additionally tested in Studio One by the owner.
**Published product branch:** [`main`](https://github.com/challanger2000/125A-Drumator-Final/tree/main), version 1.0.1. **Unreleased F19 fix branch:** [`develop/v1.0.1-gui-redesign`](https://github.com/challanger2000/125A-Drumator-Final/tree/develop/v1.0.1-gui-redesign); intentionally diverged pending the owner's next Studio One test. Older history preserved.

## Features

- 16 sample instrument rows with WAV/FLAC import, Mute/Solo, per-row VOL/PAN/PITCH and optional separate stereo outputs.
- Eight patterns (A–H), 16 or 32 steps, per-step velocity, swing and humanization.
- SONG arrangement with up to 64 entries, REST positions and selectable SONG START.
- Integrated non-destructive sample editor with START/END, fades, gain, LIVE and APPLY/CANCEL.
- Pattern/SONG MIDI and WAV export, including drag and drop; optional output peak limiter.
- GUI zoom 100/120/130/150 %, default 130 %.
- **66 automatable VST3 parameters:** Pattern, 16× Volume, 16× Pan, 16× Mute, 16× Pitch and Master.

## Unreleased NEXT PAGE correction — version 1.0.1 (development branch only)

- Owner-reported issue: the final page with a single instrument compressed the embedded Studio One window to a tiny height. F18's lower-metal-border correction alone did not address this.
- F19 retains the normal 686px window height and bottom metal chassis with no page-change host resize. The final page shows the last **ten real instrument rows** (for 11 instruments: 2–11; for 16: 7–16), so there are no empty inactive panels.
- **Exact product code:** `6d60a4da95e1b07f9e1673232d18d1a368169ec2` · [Windows Actions #41152681580](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41152681580) **SUCCESS** (29/29 CTests, 47/47 validator, 58/0/0 independent tester).
- **DLL SHA256:** `11de6738ea6ad66d9c60dc88a481ce703923c1f82e09fff4387527ad6a5caee0`.
- **Private test ZIP:** `125A_Drumator_1.0.1_Next_Page_Fix_Test.zip`, SHA256 `5b33baf1d527673fb08fa75ae05ee90b4cc324b33b9328f7d67b65b5d398ef2e`. Owner Studio One check outstanding. The seller's currently published Gumroad ZIP is not changed. **No version bump to 1.0.2.**

## Verified published 1.0.1 product build

- **Executable source commit:** [`e630602a634bbd0773352a5235eb8b3c2a4cb7ab`](https://github.com/challanger2000/125A-Drumator-Final/commit/e630602a634bbd0773352a5235eb8b3c2a4cb7ab).
- **Windows Actions:** [#41140679693](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41140679693), **SUCCESS**.
- **QA:** 29/29 CTests, Steinberg Validator 47/47, independent 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- **Executable DLL SHA256:** `a76d9dca83e96b547eaa1cd2699be358108e0dc576f15b8fa160b89eb9f2efe2`.
- **Latest GUI refinements:** instrument row order PLAY — FILL — EDIT, including updated hitboxes and popup; LIMIT/ZOOM aligned to common right edge; 15px footer gaps between EXPORT/PREV PAGE/NEXT PAGE; complete 125A branding aligned with PATTERN A.
- **Previous fixes retained:** WAVE EDITOR header labels and LIVE ON/OFF do not overlap, filenames are clipped, and cyan Velocity bars remain visibly distinct from Pattern steps.
- **Current customer ZIP:** `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip`, 26 files, SHA256 `f455403523951bdc829c73eaca41437f1ed351ec374c93dd0c291cf774e136d7`. Prepared outside the repository; includes exactly the successful Windows product DLL, native VST3 resources, current GUI illustrations in bilingual PDF guides, installation, license notices and SHA256 manifest. Supersedes the previous `125A_Drumator_1.0.1_Gumroad_GUI_korrigiert.zip` package.
- **Authoritative status:** [CURRENT.md](CURRENT.md).

## Distribution

Windows x64 VST3 only. Close the host, copy the complete `125A Drumator.vst3` bundle into `C:\\Program Files\\Common Files\\VST3`, and restart the host.

The Gumroad customer package is **not built from miscellaneous repository files**. It contains the exact validated executable, pixel-equivalent resource PNGs with nonessential image metadata removed, German/English PDF manuals, install guides, license/support notices, changelog and file checksums. Personal repository links, private paths, development reports, QA logs and Release_Review folders are excluded.

**Release status:** On 30 October 2026 the owner confirmed completing the current-binary Studio One hands-on test and publishing the updated customer ZIP on Gumroad. These owner reports are distinct from the independently recorded Windows CI results. No Gumroad listing URL or independent storefront verification is recorded here.

[German manual](docs/BEDIENUNGSANLEITUNG-DE.md) · [Contributor rules](AGENTS.md)
