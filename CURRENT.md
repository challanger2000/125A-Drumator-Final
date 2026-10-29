# 125A Drumator — Current

## Aktueller V1.0.1-GUI-Entwicklungszweig (29.10.2026)

- Branch: `develop/v1.0.1-gui-redesign`; Version in CMake, PluginIDs und Windows-Binary: **1.0.1**.
- Geprüfter Produktstand vor dem Audit-Nachbesserungszyklus: `e59023351a61e01cbfe80bfe3d01d0aacfaefc06`; [Windows Actions #40811357723](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40811357723): **24/24 CTests, 47/47 Validator, 58 PASS / 0 WARNING / 0 FAIL** im unabhängigen Tester.
- [Aktueller umfangreicher Audit](docs/audits/DRUMATOR-V101-GESAMT-AUDIT-20261029.md). Nachfolgeänderungen an Tests/Workflows sind **nur** mit einem neuen erfolgreichen Workflow für ihren exakten Commit als geprüft zu werten.
- **NICHT V1.0.1 FINAL**: Studio One/ASIO/echter Datei-Picker/DAW-Projekt-Recall und User-Designfreigabe sind für die aktuelle Binary **NOT RUN**.
- Die folgende V1.0.0-Sektion bezeichnet den zuvor bestätigten Releasekandidaten und wird durch den noch nicht veröffentlichten GUI-Entwicklungszweig nicht ersetzt.

- Target Windows VST3 version: **1.0.0**
- Status: **final-repository release candidate**; publication/package not yet verified
- Source repo: `challanger2000/125A-Drumator`
- Audited product commit: `a14d9e67470ad0af94a8250baca13fa3a15fb70e`
- Source export (docs-only additional commit): `91afadfd643fd32fa49e3d0fdb4ec9e39c9e106b`
- Windows original source QA: [run 40425541707](https://github.com/challanger2000/125A-Drumator/actions/runs/40425541707), 23/23 CTests, 47/47 Validator, 58/0/0 Tester
- Final repository Windows QA: **PASS** at [Actions #40430043280](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40430043280), commit `69d2e24ff0311516d20e274da95ad69f50e75692`: **23/23 CTests; 47/47 Steinberg Validator; packaged VST3 audio/host PASS; 125A Plugin Tester v0.3.2: 58 PASS / 0 WARNING / 0 FAIL**; exact candidate DLL SHA-256 `3366e1a0bdff3553cad4cabec3b24780b0414ddf0e16299e4d7c86e616dcbcb4`
- Public V1-FINAL/Gumroad publication and customer ZIP: **NOT VERIFIED** (the v1.0.0 Windows VST3 build itself passed technical QA)
- Git submodules: **none** in the copied source tree; third-party VST3 SDK is fetched at build time
