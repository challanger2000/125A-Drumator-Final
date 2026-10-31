# 125A Drumator

Windows x64 VST3 drum sampler and 32-step sequencer with a modern hardware-style interface.

**Version:** 1.0.1. The F20/F21 layout corrections are included in the updated 1.0.1 customer package prepared for the seller. **No 1.0.2 release.**

**Canonical source:** [`main`](https://github.com/challanger2000/125A-Drumator-Final/tree/main). The `develop/v1.0.1-gui-redesign` branch is synchronized to the same source and status. Former development versions and failed F18/F19 attempts are retained in Git history but are not the current executable.

## Features

- 16 sample instrument rows with WAV/FLAC import, Mute/Solo, per-row VOL/PAN/PITCH and optional separate stereo outputs.
- Eight patterns (A–H), 16 or 32 steps, per-step velocity, swing and humanization.
- SONG arrangement with up to 64 entries, REST positions and selectable SONG START.
- Integrated non-destructive sample editor with START/END, fades, gain, LIVE and APPLY/CANCEL.
- Pattern/SONG MIDI and WAV export, including drag and drop; optional output peak limiter.
- GUI zoom 100/120/130/150 %, default 130 %.
- **66 automatable VST3 parameters:** Pattern, 16× Volume, 16× Pan, 16× Mute, 16× Pitch and Master.


## Verified 1.0.1 product build and customer ZIP

- **Executable source commit:** [`3748b1e9e1867357362304c51bec2cb99ac9657b`](https://github.com/challanger2000/125A-Drumator-Final/commit/3748b1e9e1867357362304c51bec2cb99ac9657b).
- **Windows QA:** [Actions #41157069202](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41157069202), SUCCESS: **29/29 CTests**, **47/47 Steinberg VST3 Validator**, **58 PASS / 0 WARNING / 0 FAIL** independent 125A Plugin Tester v0.3.2.
- **DLL SHA256:** `06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278`.
- **Updated Gumroad package:** `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`, 26 customer files, SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`. It is prepared outside the repository. **The seller must replace the prior Gumroad download; this commit is not a storefront upload.**
- **GUI footer alignment:** the entire footer bezel is exactly x12..1260 (1248px wide), matching the instrument chassis, instead of the former x16..1264. Buttons, hitboxes, SONG rail and all audio code are unchanged.
- **Correct paging:** ten rows per initial page; the last page has only genuinely remaining rows (16 instruments: 1–10 then 11–16; 11 instruments: 1–10 then only 11). Window size stays fixed for normal 10-row pages when navigating NEXT/PREV, eliminating the earlier host resize problem. Missing rows are not simulated with duplicate controls.
- **Other 1.0.1 GUI refinements:** PLAY – FILL – EDIT; aligned LIMIT and ZOOM; symmetric right-footer button gaps; complete original 125A branding; resolved WAVE-editor text overlap; cyan Velocity steps; unchanged 66 VST3 automation parameters.
- **DAW verification:** The owner confirmed the corrected F20 10+6 page navigation in Studio One. This final F21 build changes only footer-bezel geometry and passed automated Windows/host tests, but no separate manual Studio One session is independently recorded for this last DLL.
- **Authoritative current status:** [CURRENT.md](CURRENT.md).

## Distribution

Windows x64 VST3 only. Close the DAW, replace the complete `125A Drumator.vst3` folder in `C:\\Program Files\\Common Files\\VST3`, then restart the DAW.

The customer package includes the **exact validated VST3**, its complete resource bundle, two refreshed and illustrated searchable PDF manuals (DE/EN), START-HERE and installation instructions, changelog, third-party code notices, support/license information and an SHA256 checksum for every delivered file. Development reports, internal test logs, private paths, and review artifacts are excluded.

The earlier Gumroad 1.0.1 download was owner-confirmed published on 30 October 2026. The ZIP named above is its **replacement within the same version** and has been created locally; no subsequent storefront upload is claimed here.

[German manual](docs/BEDIENUNGSANLEITUNG-DE.md) · [Contributor rules](AGENTS.md)
