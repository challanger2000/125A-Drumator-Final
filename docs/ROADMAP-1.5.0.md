# 125A Drumator 1.5.0 — Development scope (not released)

**Status (1 November 2026):** Stage 3 eight-sample original audition kit + all 24 WAV loops are rendered and Windows-QA verified ([Actions #41268791112](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41268791112): 32/32 CTests, 47/47 Validator, 58/58 Tester). Stage 2 guarded A/B/C Apply/grouped Undo also passed. Live GUI preview, sample-kit opt-in and Studio One musical acceptance are pending. Source `5f03e2d549f9a406aaabc7ddc71bf759abd10b6c` on `develop/v1.5.0`.
**Frozen sale version:** `main` stays the independently validated v1.0.1, starting from `f4c5caa4be82626ea89459d9835c12138d7c6a1b`. Do not merge 1.5 development into main, package or market it as released without dedicated approval.

## Product principle

**More useful, not more complicated.** The existing Drumator remains fully usable without opening the new feature. Preserve its hardware-like, dark 3D look and existing sequencer, instruments, automation, projects, MIDI/WAV export, and SONG workflows. Add only one unobtrusive `BEAT CREATOR` entry, with a small optional dialog. No sequencer inside the sequencer, no deep menu trees, no default complexity explosion.

## P1: Beat Creator — main distinctive feature

- Choose a genre (initial candidates: Industrial/NDH, EBM, Techno, House, Rock, Metal, Hip-Hop).
- One straightforward SIMPLE–COMPLEX knob; optionally a VARIATION setting and FILL when actually useful. Keep defaults immediately musical. A basic `STYLE` and `GENERATE` path must always remain usable without adjusting anything else.
- Generate coherent kick/snare/hat/other step placements with genre-specific rhythmic rules and sensible per-step velocity, not merely unconstrained random bits.
- Respect current 16/32-step length and current pattern A–H; preview candidate before destructive application, explicit APPLY and Undo. No unapproved modification of the existing song, kit, samples or stored patterns.
- Support repeatable deterministic test fixtures (seed in test logic, not necessarily exposed in UI) and tempo-/bar-aware placement.
- No MIDI drag/drop workflow or audio engine rewrite is required for the first milestone.

## P2: Minimal preview sample kit

Exactly one small, original/cleared-to-distribute factory audition kit; sample quality sufficient to identify groove, not intended as a new premium drum library.

| Instrument row (top down) | Role |
|---|---|
| 1 | BD |
| 2 | Snare |
| 3 | CHH |
| 4 | OHH |
| 5 | Low Tom |
| 6 | High Tom |
| 7 | Crash |
| 8 | Clap |

Rows 9–16 remain free for user instruments. Default generator targets role assignments, not specific sample files. Users can substitute their own WAV/FLAC samples freely while preserving every generated step/velocity. If a project already has samples, it must **never silently overwrite or replace them**; offer audition loading separately or explicit opt-in. A candidate generated groove should be audible even for a brand-new empty user session after explicitly choosing the preview kit. Produce original samples or verify an appropriate redistribution license and preserve notices; do not redistribute proprietary classic drum-machine recordings unlicensed.

## P3: Velocity usability

Keep mouse click and drag painting as they already exist. IMPORTANT: `Ctrl+drag` currently has an erase/minimum-value action; do **not** silently reuse it for curves. Investigate `Shift+drag` for straight-line/curve interpolation, plus optional SIMPLE/ACCENT adjustment. Do not break the existing knob fine-adjust or other shift gestures. Group each painting gesture as one undoable operation.

## P4: SONG workflow and timing validation

- Keep existing MIDI NoteOn 24–31 mapping to patterns A–H. Internal SONG chain remains available; evaluate only concrete friction points before UI changes.
- Compare host playback vs MIDI triggers vs exported MIDI/WAV for sample-onset position, fixed offset, loop stability and tempo/transport restarts across 44.1/48 kHz and 16-/32-step patterns. Record measured offset samples and regression expectations. Do not claim sample-accurate synchronization from hearing alone.
- Do not change previously working transport/midi scheduling without regression coverage.

## P5: GUI contrast (minimal)

Retain the modern dark 3D/hardware faceplate. Review text contrast and dark groups for legibility at supported zoom settings, but no flat redesign or repainting unrelated controls.

## Development and release gates

1. Preserve the 1.0.1 stable branch and customer ZIP. Work **only** on `develop/v1.5.0`; don't accidentally trigger public packages from development.
2. Begin with a pure tested core generator and explicit role mapping (no GUI interference); then integrate small dialog and safe Preview/Apply/Undo; then factory audition kit and usability items.
3. Every code increment: Windows x64 VST3 compile, CTests, Steinberg validator, independent tester and relevant integration/GUI pixel tests. Preserve project recall and automation compatibility.
4. Use actual Studio One hands-on tests for playback, sample swapping, multi-step painting, export timing and project reopening. CI success does not substitute for owner acceptance.
5. Version identifiers should become 1.5.0 **only once work starts intentionally** and must never change the safe `main` build. Package/release only after the v1.5.0 candidate is accepted, with START-HERE, DE/EN manual, legal/third-party notices and checksum manifest.

Stages 1-3 are implemented on the isolated development branch. None has been merged to `main` or approved as a v1.5.0 release.
