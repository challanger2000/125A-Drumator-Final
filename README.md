# 125A Drumator

A Windows x64 VST3 drum sampler and 32-step sequencer with a dark hardware-style UI.

**Version:** 1.0.1 · **Status:** Release candidate — final Gumroad publication not yet approved.
**Production branch under review:** `develop/v1.0.1-gui-redesign`.

## Features

- 16 drum instrument slots with WAV/FLAC one-shot import, stereo outputs, solo/mute and per-instrument gain/pan/pitch.
- Eight Patterns (A–H), 1/2-bar lengths and 32-step velocity sequencing.
- 64-position SONG arrangement with REST, start-bar and section lengths.
- LIVE sample editing (Start, End, Fades and Gain) with APPLY/CANCEL rollback.
- Optional stereo peak limiter, MIDI and WAV export for Pattern or SONG, including drag-and-drop.
- 100%, 120%, 130% and 150% native GUI zoom; default 130%.

**Important:** the latest F16 first-open VST3 host-window geometry correction and F17 sample-picker `< WAV > ...` arrangement are included in source, but still require **exact-build Studio One acceptance** after Windows QA.

## QA and release state

- Last fully approved automated baseline before F16/F17: [Windows Actions #41000904184](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41000904184), 29/29 CTests, 47/47 Steinberg Validator and independent 58 PASS / 0 WARN / 0 FAIL.
- Current F16/F17 candidate: [commit `25d15b6`](https://github.com/challanger2000/125A-Drumator-Final/commit/25d15b66cbf1089df773310c69afc770a5b14591), [Windows run #41079094604](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41079094604) (verify its final status; do not infer PASS from this page).
- Product checklist and release gates: [CURRENT.md](CURRENT.md) and [release-readiness audit](docs/audits/RELEASE-READINESS-20261030.md).
- Historical milestones preserved in [docs/history](docs/history/), not authoritative for the current product.

## Build and installation

Windows 11 x64, CMake, Visual Studio 2022, official pinned Steinberg VST3 SDK 3.8.1 with its exact submodules. Windows CI builds and validates the installable folder `125A Drumator.vst3`. To install, copy the complete VST3 bundle into `C:\Program Files\Common Files\VST3` and restart the host.

**Only source and engineering files live in this repository.** Customer Gumroad ZIPs must not include internal audits, QA outputs, build logs or "Release_Review" folders. No FINAL/release tag until owner acceptance, rights/EULA/support and exact-package review.

[Mandatory 125A engineering entry](https://github.com/challanger2000/125A-Engineering/blob/main/START-HERE.md) · [SDK baseline](docs/SDK-BASELINE.md) · [Contributor instructions](AGENTS.md)
