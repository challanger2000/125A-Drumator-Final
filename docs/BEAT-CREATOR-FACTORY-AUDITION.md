# 125A Drumator 1.5 - Stage 3: original factory audition kit

## Original sound generation
\`src/core/BeatCreatorFactoryKit.h\` generates **all eight voices from arithmetic** in the source code, no recorded PCM or external samples, no third-party sample licensing dependencies. For private listening tests only, no claims that the sounds replace a complete commercial drum library.

| Row | Voice | Construction |
|---|---|---|
| 1 | BD | pitch-swept bass oscillator + short noisy click |
| 2 | Snare | filtered synthetic noise + tuned body |
| 3 | CHH | short noisy/metallic high-frequency decay |
| 4 | OHH | longer noisy/metallic decay |
| 5 | Low Tom | lower pitched swept shell oscillator |
| 6 | High Tom | higher pitched swept shell oscillator |
| 7 | Crash | synthetic noise + metallic inharmonic overtones |
| 8 | Clap | clustered transient bursts + short noise tail |

Source files: **44.1 kHz PCM16 mono**. No sample is silently loaded into the live user's kit. Instrument rows 9-16 remain untouched and available.

## Rendering / listening pack
\`tools/BeatCreatorAudition.cpp\` builds a temporary \`KitState\` entirely separate from the user's open project, then routes each A / B / C 16-step draft into Drumator's **existing** \`OfflineWavExport::render\`. This reuses the real sample preparation, scheduling, voices and limiter; it is not an alternative drum/sequencer engine.

Output: 8 original source WAVs and **24 distinct musical audition loops** (8 styles × 3 roles). Loops are **48 kHz / 24-bit stereo**; 120 BPM is NOT assumed for every genre. Each style renders at its own fixed demo BPM:
Industrial/NDH 124; EBM 126; Techno 132; House 124; Rock 110; Metal 160; Hip-Hop 92; Drum & Bass 174.

GitHub workflow packages the audio separately as artifact \`125A-Drumator-15-Beat-Creator-Audition-WAVs\` (development branch only). Listen in Studio One or a WAV player; this is NOT a replacement for live GUI preview.

## Quality gates
- Factory sample byte determinism, WAV framing/format, existing WavPcm16Decoder acceptance, onset, levels, headroom, max source size.
- No project sample/step edits when constructing the audition-only kit or rendering a preview.
- Existing real OfflineWavExport used for 48k/24b stereo pattern audition; verify one-bar length and step-row mapping with binary tests.
- Full Windows x64 VST3 regression, Steinberg Validator, 125A Plugin Tester and 8+24 WAV artifact availability.

No third-party audio assets have been copied. This milestone does NOT install a factory kit, change the GUI or introduce live Preview into the VST3. Musical quality requires an actual listening test with these WAVs.
