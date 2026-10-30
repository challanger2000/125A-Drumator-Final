# 125A Drumator V1.0.1 — LIVE Wave Editor and state-transition GUI audit
**Date:** 30 October 2026  
**Repository:** `challanger2000/125A-Drumator-Final`  
**Branch:** `develop/v1.0.1-gui-redesign`  
**Baseline:** Windows PASS `214b74dd925f54a5767f52be0b95cc4ed02bcbe8`, Actions 40993792076 (28/28 CTests, 47/47 Validator, independent 58/0/0)  
**Current product feature commit:** `99ebe6e8c67fb0aca5f7e7901af43c6e162e9e4d`  
**First LIVE candidate Actions 40995697627:** 28/29 CTests PASS, validator/tester NOT RUN. Integration failed **only** at old W11F exact status text oracle: it expected "SAMPLE READY FOR EDITING" where new editor correctly displayed LIVE ON. Its existing runtime WEL1–WEL5 PCM/session/GUI assertions passed. This audit correction updates that oracle, and adds F15 state exhaustiveness. Do not mark latest candidate PASS until a new Windows run finishes.

## Why the former "full audit" was inadequate

The former F14 navigation test used only a successful click sequence with 16 rows: page one -> page two -> page one. It did **not** test row-count shrink, retained page offsets, repeated NEXT on the last page, or host-constrained viewports. User reproduced `ROWS 5-5 / 5` with both navigation buttons unable to escape. F14 repaired this state, but inspection uncovered an additional defect: the old code allowed repeated NEXT to reach a noncanonical one-row page (16 total rows, 10 visible). F15 replaces the last-page formula with `floor((total-1)/pageCapacity)*pageCapacity` and adds a real attached View test plus a 16*10 (row-count*viewport-height) transition matrix, including forward, backward, stranded and shrink cases.

## Wave LIVE contract and safety review

1. **Audio**: the next sample trigger of an edited row uses the new Start/End/Fade/Gain. Already sounding voices retain their launch `SampleEdit` to avoid mid-tail discontinuities. The actual `SampleVoiceEngine` is updated at each audio block; separate PREVIEW audition remains unchanged.
2. **Threading**: UI/controller sends `EditorLiveBegin`, `EditorLiveUpdate`, `EditorLiveEnd` through VST3 messages. A single-writer, versioned `LiveEditMailbox` uses fixed atomics; the audio callback makes two bounded snapshot attempts, has no mutex/heap/I/O/PCM decoding, and safely retains its last valid edit under contention. No digital audio latency is introduced by edit publication.
3. **Lifetime**: a 64-bit session token plus source revision gates every transient edit; a second editor cannot steal the active session. A stale `END` cannot cancel a newer session. New WAV import/clear/commit, full kit recall or editor re-open invalidates or replaces prior session. A failed LIVE stop blocks voluntary CANCEL/APPLY so the user is not falsely told a transient edit was discarded.
4. **Project state**: `kit_.sampleEdits` and serialized VST3 state are untouched until APPLY; CANCEL and LIVE OFF return to the committed edit for subsequent voices. Processor commit and recall explicitly revoke transient tokens. Dirty/undo are not requested by LIVE alone.
5. **UI geometry**: a separate LIVE toggle at logical x=886..1010, y=114..146 sits inside the 1280x620 metal modal header. It does not resize or shift the established eight 86x32 Wave toolbar keys and their shared mouse hit zones. Default is LIVE ON when a session is accepted, with explicit LIVE OFF on a second-editor conflict.

## Requirement-to-evidence inventory

| Control group / interactive elements | Input to actual behavior | Evidence / test family | Status |
|---|---|---|---|
| MAIN Pattern A–H, select and length | Hit map -> Controller -> pattern bank / MIDI host | V101 menu matrix, F15 pattern host, packaged MIDI PCM | Windows regression |
| Pattern MENU: copy, paste, clear, undo, random velocity, velocity 100, duplicate | Popup index/selection -> Controller -> pattern data | `testV101MenuMatrixAndGeometry`, P18/P19 | Windows regression |
| Song ON/OFF and SONG EDIT | V101 header -> Controller/song board -> real chain | SONGSTART, SONG UI native screenshot, actual playback | Windows regression |
| SWING / HUMANIZE / MASTER | rotary gestures -> controller -> DSP / state | Packaged audio host, per-knob UI, recall | Windows regression |
| LIMIT ON/OFF, meters, zoom 100/120/130/150 | button -> DSP, meter IPC, host resize + UI | G02, Z02, native screenshots, limiter PCM | Windows regression |
| Rows SELECT/MUTE/SOLO/FILL, SAMPLE arrows/load/remove, PLAY, EDIT | hit boxes -> processor and WAV / PCM | V101 all-row isolation, sample browse, preview, fault tests | Windows regression |
| 32 step keys, VOL/PAN/PITCH, VEL lanes, SET output/choke/copy/paste/clear | per-row controls -> Controller -> audio/MIDI, UI states | GUI menu matrix, all-16-row, velocity 100 and routing cases | Windows regression |
| ADD/REMOVE ROW, PREV/NEXT PAGE | row count and page state -> bounds/host layout | F14 real 5-of-5, **new F15 1..16 x 1..10**, host resize rejection | F15 rerun pending |
| SONG 64 slots, page navigation, REST/A-H, length 1/2/1/2 bars, start/reset, add/remove last | SONG editor -> SongChain -> scheduled audio | SONGSTART, V101 native GUI, SONG-page tests | Windows regression |
| WAV Zoom +/-/FIT, waveform scroll, Start/End/Fades/Gain | shared hit map -> edit draft / waveform envelope | W11D/F, waveform extrema oracle, native screenshots | Windows regression |
| WAV PREVIEW/STOP, RESET, CANCEL/APPLY | audition and commit/rollback vs saved kit | WE01–03, W11D, fault injection RV02/A08 | Windows regression |
| **NEW WAV LIVE ON/OFF** | host control messages -> atomics -> main drum-engine PCM | **WEL1** transient real PCM, **WEL2** second-view conflict, **WEL3** CANCEL PCM revert, **WEL4** stale session, **WEL5** native click toggle, mailbox 50,000 snapshots | First candidate PASS; rerun pending |
| EXPORT Pattern MIDI/WAV + SONG MIDI/WAV, Save and drag | popup -> export -> OS/DAW import | OfflineWavExport duration and onset, DragExport tests, packaged host | Windows PASS; real Studio One drag import NOT RUN this audit |
| Project open/close/reopen, multi-view, corrupt/legacy recall, 44.1/48k, host Stop/Cycle, large MIDI load | full controller + processor + saved state | Native WindowsIntegration, CTests, Steinberg Validator, plugin tester | Windows regression; real Studio One NOT RUN this audit |

**Meaning of status:** "Windows regression" is an automated test family retained in the exact-code CI suite, not a claim of a newly witnessed DAW result or complete per-control sound measurement. PASS must be confirmed from the new workflow log. Proven GUI/audio claims are narrower than the release requirement.

## New negative controls / what must fail

- Reintroducing `if(total<=limit)return false` makes F14 fail after shrinking 16 to 5.
- Reintroducing `maxStart=total-1` makes F15 fail on a second NEXT from 11–16.
- Replacing the LIVE engine update with audition-only audio makes WEL1 main-row PCM mismatch.
- Omitting the LIVE stop/revert makes WEL3 main-row PCM mismatch.
- Persisting LIVE edits without APPLY makes WEL1/WEL4 serialized-state comparison fail.
- Allowing stale live sessions to publish makes WEL4 fail.
- LIVE toggle is clicked on the native View, not asserted by paint-only proxy (WEL5).
- CMake mailbox test executes 50,000 atomic write/read snapshots including disabled state; concurrency under separate control/audio actors still requires specialist real-time tracing.

## Remaining release gates: **NOT RUN / NOT VERIFIED**

- Real Studio One 8.1.2: edit Start/End/Fade/Gain while playing a Pattern and a 64-position SONG; confirm next trigger changes audibly without new duplicate one-shots and with 44.1k/48k audio. Live OFF, CANCEL, APPLY, window close/reopen, project save/restore and multiple editor views must be repeated.
- Real Windows native dialogs and all four file drops; waveform pitch, tempo follow and WAV audition in the user's actual DAW.
- Host rejection / allocation fault **during LIVE stop and editor destruction**: two control-side retries exist, but permanent rejection can leave an editor's transient audio state uncertain until source change/reload. Requires explicit fault-injection follow-up before claiming unconditional rollback.
- Comprehensive GUI pixel inspection for this exact commit at 100, 120, 130, 150%, including longer status strings and rightmost popup; screenshots alone do not establish DAW mouse/hit behavior.
- Offline Windows timing (CI) is not hard-real-time ASIO certification; p95/p99 and max have to be measured under the user's actual project conditions.

**Disposition:** At this stage it is *not* a certified FULL AUDIT or V1.0.1 FINAL. The automated suite is substantial but the above unresolved DAW/fault gates must remain visible. A completely green CI result cannot override this section.

## 30 October 2026 — audit follow-up: constrained peer viewport regression

The comprehensive F15 boundary pass exposed a compatibility conflict under genuine VSTGUI peer HWND resize refusal. Run [#40996629622](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40996629622) compiled and passed 28/29 CTests, including F15's original matrix, LIVE actual drum PCM and the WAV/FLAC source-refresh lifecycle. The existing dual-editor host regression **Codex05 FAILED**: a refused host height increase with 5 instruments and a 4-row viewport must navigate from 1..4 to **2..5**, preserving 4 visible rows and the actual footer y-coordinate, rather than jumping to a 1-row final page and attempting an impossible resize.

Root correction: preserve **overlapping final page** only for host-constrained viewports (`limit<10`), while using canonical non-overlapping pages for the full ten-row window. Reverse navigation correctly returns to the previous actual start (e.g. for 16/6: 0→6→10→6→0). F15's 160 combination checks now compute the actual forward history and then reverse it; Codex05 continues to check real native peer View/resize denial. Neither user-facing audio nor LIVE exchange is changed by this follow-up. This run remains NOT VERIFIED until its own Windows QA and tester complete.
