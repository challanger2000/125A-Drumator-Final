# Official VST3 SDK baseline — 125A Drumator

Verified on **2026-10-16** against the official Steinberg developer portal and GitHub repository.

- Repository: https://github.com/steinbergmedia/vst3sdk
- Official release: **VST3 SDK 3.8.1** (Steinberg change log dated 2026-08-11)
- Immutable tag: `v3.8.1_build_84`
- Pinned top-level commit: `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`
- Documentation: https://steinbergmedia.github.io/vst3_dev_portal/pages/Versions/Version+3.8.1.html
- Target: Windows x64 VST3 / MSVC. VSTGUI comes from the SDK's tagged `vstgui4` submodule.

## Required integration discipline
Use the exact tagged source **recursively**, including `base`, `cmake`, `pluginterfaces`, `public.sdk`, and `vstgui4` submodules. Do not substitute a moving branch or automatically upgrade individual submodules.

Reproducible source retrieval, if a local/CI checkout is needed:

```bash
git clone --recursive --branch v3.8.1_build_84 https://github.com/steinbergmedia/vst3sdk.git
cd vst3sdk
git rev-parse HEAD
git submodule status --recursive
```

Expected top-level HEAD: `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`.
Validate each submodule commit from the tagged SDK checkout before compiling. Fix this dependency baseline in the future build configuration; no silently floating `HEAD`.

## Aktueller Integrationsstand (30.10.2026)
Der gepinnte Steinberg SDK 3.8.1 samt VSTGUI-Submodul wird im vollständigen Drumator 1.0.1 eingesetzt: native GUI, WAV-/FLAC-Import, MIDI und Audioprozessor. Aktueller geprüfter Binärquellstand: [`1acfe3698bcbe6eb88e1faee6ff8e538edbc390d`](https://github.com/challanger2000/125A-Drumator-Final/commit/1acfe3698bcbe6eb88e1faee6ff8e538edbc390d). [Windows QA #41125370883](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41125370883), Versuch 2, **29/29 CTests, 47/47 Validator, 58/0/0 unabhängiger Plugin Tester**. Die ursprüngliche Aussage von Oktober 16, die GUI sei noch nicht implementiert, galt für einen früheren Meilenstein. Maßgeblich für den Produktstand ist [CURRENT.md](../CURRENT.md).

Official primary sources:
- https://github.com/steinbergmedia/vst3sdk/tree/v3.8.1_build_84
- https://steinbergmedia.github.io/vst3_dev_portal/
