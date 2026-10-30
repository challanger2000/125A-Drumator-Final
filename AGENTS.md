# 125A Drumator — contributor and coding-agent rules

**Current project:** Windows x64 VST3 `125A Drumator`, version 1.0.1. Consult [CURRENT.md](CURRENT.md) for up-to-date product SHA, CI and FINAL/RC distinction. Do not trust historical notes.

## Mandatory source of authority

1. Read [125A-Engineering/START-HERE.md](https://github.com/challanger2000/125A-Engineering/blob/main/START-HERE.md) and follow its complete prescribed reading order.
2. Read the [125A-Branding README](https://github.com/challanger2000/125A-Branding/blob/main/README.md) before editing any branded asset.
3. Inspect repository HEAD, branch, the relevant feature tests and GitHub Windows QA before any change.

## Engineering contract

- **Windows 11 x64 VST3 only.** No macOS, Linux or standalone variants.
- Pin Steinberg VST3 SDK to **3.8.1** (tag `v3.8.1_build_84`, commit `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`), with exact tagged submodules. Do not float SDK/VSTGUI revisions.
- Audio thread: no disk I/O, locks, heap allocation, sleeping, unbounded loops or other blocking work.
- Save/recall, fault handling, multiprocessor/editor lifecycle and geometry must be tested for **state transitions and rejected host operations**, not only happy-path button clicks.
- Native VSTGUI renderer and mouse hitboxes must derive from the same layout constants. Follow the dark modern hardware style and keep uniform button dimensions.
- VST3 is built and QA-checked through Windows Actions with CTest, official Steinberg Validator, native GUI/host tests and independent 125A Plugin Tester. A green compiler or validator alone is not a release gate.
- Avoid unnecessary Actions runs, branch proliferation, test-only fallbacks and speculative source modifications. Check the exact SHA before editing; use compare-and-swap ref updates.
- `main` is the current canonical 1.0.1 branch. `develop/v1.0.1-gui-redesign` is kept synchronized as the working branch. Keep both protected from unreviewed code changes; start future changes on a dedicated topic branch and require Windows QA before promotion.
- Product documentation should be accurate for the exact binary. Historical milestone claims are archived in [docs/history](docs/history/).
- **No Gumroad release, FINAL claim or release tag without explicit owner DAW approval, rights review, EULA/support terms and customer ZIP inspection.** Keep internal QA reports, review folders, debug artifacts and logs OUT of customer packages.
