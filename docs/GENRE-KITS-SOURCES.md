# 125A Drumator 1.5 — Eight source-recorded kit candidates

**Status: BC29 recorded-kit VST3 development integration, Windows QA PASS; not a released product.**

The branch contains eight sets of source-recorded 8-piece kits and a Windows VST3 integration. The 64 WAVs ship inside the development `.vst3` bundle at `Contents/Resources/BeatCreator/`; PLAY A/B/C and APPLY use the same selected kit. The sold 1.0.1 binary and `main` remain unchanged. No Studio One owner listening acceptance has been asserted.

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

The sample generation workflow downloads only recordings at pinned upstream commits, derives trimmed PCM16 mono WAVs at 44.1 kHz and checks duration, peak, RMS, clip safety, file count and content diversity. The real VST3 loader uses the same WAV files for PLAY A/B/C and for the empty-row-only APPLY transaction; 32/32 CTests, 47/47 Steinberg Validator and 58/58 independent plugin tests passed on GitHub Actions #41289563236. Subjective listening and owner Studio One testing remain separate pre-release checks.

No direct copying from third-party commercial drum-library files. The archive is a development candidate only.
