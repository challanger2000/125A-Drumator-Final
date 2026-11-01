# 125A Drumator 1.5 — Eight source-recorded kit candidates

**Status: sample-selection and sonic QA staging, NOT production VST3.**

This feature branch creates a separate reproducible archive of 8 × 8 original-recorded drum hits. It does not yet change any code in the shipped VST3, factory-kit integration, user samples, or version numbers.

## Source and rights

- Michael Fischer's original Roland TR-808 sample recordings, mirrored at [TidalCycles sounds-tr808-fischer](https://github.com/tidalcycles/sounds-tr808-fischer), **CC0 1.0**, pinned commit `85fbecf1bec32553395625ea659e2a56dfd7c0e1`.
- Karoryfer's *Big Rusty Drums*, [sfzinstruments repository](https://github.com/sfzinstruments/karoryfer.big-rusty-drums), **CC0 1.0**, pinned commit `f07ce00df34a46b6b08375be56fe116cf15782bc`.

Both repositories include an explicit CC0 license. See the original libraries for their recording documentation. Commercial redistribution is permitted by CC0; no license claim is made about trademarks or product endorsements. Every audio source path, originating immutable revision and the source/derived SHA256 are written to `SOURCE-MANIFEST.json`.

## Kit selection

| Style | Recorded foundation |
|---|---|
| Industrial / NDH | Heavily hit acoustic kick/snare/toms, mixed with electronic hats/clap |
| EBM | Real Roland TR-808 kit; short and mechanical |
| Techno | TR-808 with long kick/cymbal variants and restrained low EQ |
| House | TR-808 lighter tuned kick/snare with open hats |
| Rock | Acoustic Big Rusty kit, independently selected recorded hits |
| Metal | Acoustic Big Rusty kit, harder velocities/rimshots and moderate compression |
| Hip-Hop | TR-808 deep kick and dry snare, dark cymbal choices |
| Drum & Bass | Short TR-808 bass drum with acoustic snare, toms and crash |

All kits preserve the existing Beat Creator's eight-row role order: BD, Snare, CHH, OHH, Low Tom, High Tom, Crash, Clap. Some common CC0 hits are shared between related styles, but the kits differ by source selection, tuning/decay variants and mild processing. There is no claim of 64 unique original recordings.

## Verification/release boundary

The standalone workflow downloads only recordings at pinned upstream commits, derives trimmed PCM16 mono WAVs at 44.1 kHz and checks duration, peak, RMS, clip safety, file count and content diversity. No effect has been added to the VST3 and `main` remains the sold v1.0.1. A complete product integration requires the same samples to sound in PLAY A/B/C and after APPLY, preservation of user kits and undo/recall, correct installation, and a Windows/Studio One audio audition.

No direct copying from third-party commercial drum-library files. The archive is a development candidate only.
