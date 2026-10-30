# 125A Drumator

Windows x64 VST3 drum sampler and 32-step sequencer with a modern hardware-style interface.

**Version:** 1.0.1 · **Status:** Commercial customer package prepared; not yet published.
**Development branch:** `develop/v1.0.1-gui-redesign`.

## Features

- 16 sample instrument rows with WAV/FLAC import, Mute/Solo, per-row VOL/PAN/PITCH and optional separate stereo outputs.
- Eight patterns (A–H), 16 or 32 steps, per-step velocity, swing and humanization.
- SONG arrangement with up to 64 entries, REST positions and selectable SONG START.
- Integrated non-destructive sample editor with START/END, fades, gain, LIVE and APPLY/CANCEL.
- Pattern/SONG MIDI and WAV export, including drag and drop; optional output peak limiter.
- GUI zoom 100/120/130/150 %, default 130 %.
- **66 automatable VST3 parameters:** Pattern, 16× Volume, 16× Pan, 16× Mute, 16× Pitch and Master.

## Verified product build

- **Executable source commit:** `1acfe3698bcbe6eb88e1faee6ff8e538edbc390d` (factory metadata excludes personal repository URL).
- **Windows Actions:** [#41125370883, attempt 2](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41125370883), **SUCCESS**.
- **QA:** 29/29 CTests, official Steinberg Validator 47/47, independent 125A Plugin Tester v0.3.2 58 PASS / 0 WARNING / 0 FAIL.
- Automation regressions AV01–AV05 and original Wave Editor/Pitch regression AUT08: PASS.
- Current production and publication gate: [CURRENT.md](CURRENT.md).

## Distribution

Windows x64 VST3 only. Close the host, copy the complete `125A Drumator.vst3` bundle into `C:\\Program Files\\Common Files\\VST3`, and restart the host.

The Gumroad customer package is **not built from miscellaneous repository files**. It contains the exact validated executable, pixel-equivalent resource PNGs with nonessential image metadata removed, German/English PDF manuals, install guides, license/support notices, changelog and file checksums. Personal repository links, private paths, development reports, QA logs and Release_Review folders are excluded.

Do not equate a green CI with owner approval, legal redistribution clearance or actual storefront publication.

[German manual](docs/BEDIENUNGSANLEITUNG-DE.md) · [Contributor rules](AGENTS.md)
