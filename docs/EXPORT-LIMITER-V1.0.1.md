# 125A Drumator V1.0.1 - Export & Output Limiter (development)

- EXPORT (compact lower chassis footer) offers PATTERN MIDI/WAV and SONG MIDI/WAV. The Save dialog supplies a destination. The current pattern/entire SONG chain is exported independently of DAW transport.
- MIDI: Standard MIDI File type 1, 960 PPQ, 1 conductor and 16 instrument tracks. Channel 10, notes 36..51, note lengths 1/64, deterministic StepScheduler swing/humanize and LIVE VEL. Empty steps have no notes. The imported MIDI uses the last host tempo received via MeterSnapshot, or defaults to 120 BPM before a host tempo is available. Muted rows are kept in the musical MIDI arrangement.
- WAV: stereo master PCM 24-bit at 48 kHz, using SampleVoiceEngine, loaded PCM WAV/FLAC-converted WAV, per-row pitch/gain/pan, trims/fades, mute/solo/choke, swing/humanize/live velocity and master gain. The WAV combines all 16 rows regardless of DAW stem routing and terminates at the exact Pattern/SONG grid boundary. One-shot tails are trimmed at this boundary with a 5 ms fade to avoid clicks. Uses the last host tempo, with 120 BPM fallback.
- LIMIT ON/OFF: stereo-linked -0.3 dBFS zero-latency sample-peak limiter on MAIN and EACH active stereo stem bus. Immediate attenuation and 80 ms gain-release. No lookahead delay and no additional VST3 PDC latency. OFF skips processing entirely: unchanged PCM and sample positions. WAV export retains beat-zero alignment. This is a sample-peak limiter, not a true-peak oversampling limiter.
- Controller stores a control-thread PCM mirror after successful imports/recalls, so offline export does not read or allocate on the audio thread. WAV rendering runs synchronously on the GUI/control thread; large songs can temporarily block the editor until complete. No truncated export is reported as successful. A staged file and backup/rollback scheme protect existing destinations.
- DM18 state adds persistent limiter ON/OFF; DM17 and earlier recall OFF. Sample edits are mirrored on successful commit.
- QA: MIDI parser/count/event tests, PCM header/tail/limiting tests, limiter bypass/peak/stereo tests, all existing CTests, Steinberg Validator and independent plugin tester required. Native GUI screenshots and actual Studio One MIDI/WAV imports and recalled limiter still need final user confirmation; no FINAL release yet.

## Drag & Drop export candidate — 30.10.2026

- In the four existing EXPORT popup choices, **click/release** still opens the usual Save As file picker.
- **Press and move at least 4 logical pixels** from PATTERN MIDI, PATTERN WAV, SONG MIDI or SONG WAV initiates VSTGUI CDropSource with a real UTF-8 kFilePath (Windows CF_HDROP). The popup presents DRAG affordances; no DSP, limiter, MIDI or saved state has changed.
- The existing controller MIDI/WAV exports write a unique temporary file only on actual drag, not when opening the menu. No GUI/DAW file picker is necessary for dragging.
- Every drag owns a separate exclusively created temporary directory. Failed exporter attempts discard only their own files. Successful exported files remain after the OLE drag session completes, because a host may read a path asynchronously. **No automatic temp cleanup yet**: Studio One's copy/reference behaviour must be verified before adding a safe retention policy.
- Drop onto a matching Studio One instrument track (MIDI) or audio track (WAV). The host decides whether to accept, import or reference a file; VSTGUI returning drag-started does **not** prove a DAW drop succeeded.
- Automated filesystem lifecycle and existing MIDI/WAV export validation are required. The four actual VSTGUI→Windows CF_HDROP→Studio One drops (and project save/reopen afterward), source package inspection in a native host and temporary file disposal after real import are **NOT RUN until independently measured**. No FINAL release.

## Studio One WAV synchronization correction (30.10.2026)

- User reported WAV export out of sync with the project already set to 120 BPM.
- Confirmed source-code problem: old exporter extended the clip until one-shot voices stopped, leaving a non-musical file length even when sequenced events used the correct tempo. Its previous tail test required this undesired extra duration.
- Both Pattern and Song WAV now contain exactly the calculated musical grid duration: 120 BPM / 16 steps = 96000 PCM frames (2.000 seconds), 32 steps = 192000 frames (4.000 seconds). At fractional BPM, the boundary is rounded to the nearest sample. End-of-grid tails are faded over the final 5ms.
- Automated tests require exact WAV sizes, last step onset at 90000 frames, no early hit, tail fade, 2-bar Pattern and SONG, and 100 BPM length. MIDI, live voices and existing limiter behavior remain unchanged.
- Independent Studio One reimport/loop playback is still NOT RUN for this patch. Exact onsets and duration assertions do not establish host import/timestretch or driver behaviour.
