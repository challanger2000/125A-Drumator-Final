# 125A Drumator

Windows x64 VST3 drum sampler and 32-step sequencer with a modern hardware-style interface.

**Version:** 1.0.1 · **Status:** Commercial customer package prepared; not yet published.
**Current code:** [`develop/v1.0.1-gui-redesign`](https://github.com/challanger2000/125A-Drumator-Final/tree/develop/v1.0.1-gui-redesign). **Important:** GitHub's default `main` branch still holds older 1.0.0 source. Do not build 1.0.1 from `main`; these branches have not been merged.

## Features

- 16 sample instrument rows with WAV/FLAC import, Mute/Solo, per-row VOL/PAN/PITCH and optional separate stereo outputs.
- Eight patterns (A–H), 16 or 32 steps, per-step velocity, swing and humanization.
- SONG arrangement with up to 64 entries, REST positions and selectable SONG START.
- Integrated non-destructive sample editor with START/END, fades, gain, LIVE and APPLY/CANCEL.
- Pattern/SONG MIDI and WAV export, including drag and drop; optional output peak limiter.
- GUI zoom 100/120/130/150 %, default 130 %.
- **66 automatable VST3 parameters:** Pattern, 16× Volume, 16× Pan, 16× Mute, 16× Pitch and Master.

## Verified product build

- **Executable source commit:** `c18091ab3bdb0c80f7753ea6e3339463b26ad800` (factory metadata excludes personal repository URL).
- **Windows Actions:** [#41133162498, ](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41133162498), **SUCCESS**.
- **QA:** 29/29 CTests, official Steinberg Validator 47/47, independent 125A Plugin Tester v0.3.2 58 PASS / 0 WARNING / 0 FAIL.
- Automation regressions AV01–AV05 and original Wave Editor/Pitch regression AUT08: PASS.
- **GUI fixes:** WAVE EDITOR heading, sample name and LIVE ON/OFF key are non-overlapping at all 100–150% zoom levels; long names are clipped/ellipsized. Cyan Velocity bars are distinct from amber Pattern step keys, with a real raster oracle and layout regression.
- **Authoritative release status:** [CURRENT.md](CURRENT.md); [documentation guide](docs/INDEX.md) separates active instructions and historical audits.

## Distribution

Windows x64 VST3 only. Close the host, copy the complete `125A Drumator.vst3` bundle into `C:\\Program Files\\Common Files\\VST3`, and restart the host.

The Gumroad customer package is **not built from miscellaneous repository files**. It contains the exact validated executable, pixel-equivalent resource PNGs with nonessential image metadata removed, German/English PDF manuals, install guides, license/support notices, changelog and file checksums. Personal repository links, private paths, development reports, QA logs and Release_Review folders are excluded.

Do not equate a green CI with owner approval, legal redistribution clearance or actual storefront publication.

[German manual](docs/BEDIENUNGSANLEITUNG-DE.md) · [Contributor rules](AGENTS.md)
