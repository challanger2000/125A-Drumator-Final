# 125A Drumator Modern — separate GUI experiment

**Status:** independent development branch. Not a release; owner DAW approval and Windows QA required.

## Frozen product contract
- Reference: Windows x64 Drumator 1.5.0, `develop/beat-creator-10-styles-20261102` (base SHA `52dedc399424261a93905c622ca3ef42606e9207`).
- The approved `main` (Classic 1.0.1) and the V1.5.0 development branch are unchanged.
- No changes to sample playback, WAV/FLAC decoding, realtime scheduling, groove generation, DSP, MIDI, Song, Pattern state serialization, 66 VST3 automation parameter IDs or project recall.
- Keep the 32-step FL Studio-like pattern workflow, 17×34 actual Step hit rectangles, 19px step spacing, row navigation and standard gestures.
- Beat Creator: 10 styles/80 grooves, Complexity 35 by default and ±5 steps; no alterations.
- MIDI export/drag: a single SMF-1 containing 17 tracks at 960 PPQ; no alterations.

## Visual design
- Steinberg VSTGUI 4 via pinned SDK 3.8.1. **No JUCE dependency**.
- Medium-gray, slightly dimensional controls without vintage photo/chassis artwork in Modern.
- Three palettes: **Graphite, Slate (default), Silver**.
- Modern `DRUMATOR` wordmark uses the default native VSTGUI font, theme-aware ink, and a subtle 1.3 px shadow. The original metallic title PNG is rendered only in Classic; the unmodified official 125A Systems master logo remains separate.
- Orange active Steps; cyan only for secondary selections/knob pointers, restrained green status accents.
- UI-specific theme tabs in footer. Painting and hit testing share `UiV101Layout::modernThemeTabs` rectangles.
- Scoped per-editor palette is contained in the GUI paint thread; no Processor/DSP access.
- Preference file `%LOCALAPPDATA%/125A Systems/Drumator Modern.theme`. Palette selection is intentionally not stored in a Drumator kit or host audio state.
- Existing original photo mode is retained **internally as a visual regression oracle**, not exposed as a fourth Modern theme.

## Installation/identity separation
- Modern uses distinct VST3 Processor/Controller class IDs in `PluginIDs.h`.
- Factory display name: `125A Drumator Modern`; package name: `125A Drumator Modern.vst3`.
- Do **not** replace the Classic DLL or promote Modern to `main`; both are separate plugin identities.
- Old host projects remain bound to the Classic class IDs. Do not claim cross-identity project migration without an explicit host test.

## QA boundaries
- Compare exact source diff with the V1.5.0 release baseline; DSP/Processor/Controller files must remain unchanged.
- Native Windows QA must compile the Windows bundle, run complete CTests, Steinberg Validator, 125A Plugin Tester and REAL attached VSTGUI host tests.
- The original Classic exact-photo and original-Step-pixel fixtures remain in place; the new independent MOD01/MOD02 checks assert three native palette renderings, orange active Steps, theme click geometry, preference storage and unchanged Controller state.
- Save screenshots of the three themes from the actual loaded VSTGUI editor; do not use mockup imagery as software verification.
- **Unverified until run:** actual Studio One / Fender Studio Pro usability, multi-instance visual behavior, 100–150% zoom, Mouse gestures and DAW project reopening with this separate Modern ID.
- No distribution, Gumroad upload, release tag or FINAL claim without owner approval and applicable release QA.
