# 125A Drumator — Current

## Aktueller V1.0.1-GUI-Entwicklungszweig (29.10.2026)

- Branch: `develop/v1.0.1-gui-redesign`; Version in CMake, PluginIDs und Windows-Binary: **1.0.1**.
- Geprüfter Produktstand vor dem Audit-Nachbesserungszyklus: `e59023351a61e01cbfe80bfe3d01d0aacfaefc06`; [Windows Actions #40811357723](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40811357723): **24/24 CTests, 47/47 Validator, 58 PASS / 0 WARNING / 0 FAIL** im unabhängigen Tester.
- [Aktueller umfangreicher Audit](docs/audits/DRUMATOR-V101-GESAMT-AUDIT-20261029.md), mit überprüftem Nachtrag: [Korrekturcommit `66d07ba`](https://github.com/challanger2000/125A-Drumator-Final/commit/66d07ba8bdb69d0cf7b286fb6400ca5c432720c1) — Windows [#40814316169](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40814316169) **SUCCESS, 24/24 CTests, 47/47 Validator, 58/58 Plugin Tester**. Produktcode seit `e590233` unverändert.
- **Automations-Extremtest abgeschlossen:** [Testcommit `14eee24`](https://github.com/challanger2000/125A-Drumator-Final/commit/14eee24ae432e8c4b4d39f63ed3f0c88a6aa8bbf), [Windows/Tester #40816232717](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40816232717) **SUCCESS, 24/24 + 47/47 + 58/58**. Tatsächlich 16.384 Host-Automationspunkte x12, normale Audio- sowie outputless/zero-frame-Pfade PASS; Maximaldauer 0,3742 ms (offline CI).
- **Neuester getesteter V1.0.1-GUI-Korrekturstand:** Produktänderung [`a336dc9`](https://github.com/challanger2000/125A-Drumator-Final/commit/a336dc919daafc531b895d826e4d2b3a83fec9d7) plus Testreihenfolge-Fix [`3a933c4`](https://github.com/challanger2000/125A-Drumator-Final/commit/3a933c4f4a72868729b7833d9b6ece7cfd6cce92). [Windows CI #40894599335](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40894599335) **SUCCESS: 24/24 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL im unabhängigen Plugin-Tester**. Getestete DLL SHA-256: `d351b2b0472a723f42a8423b242a1b4207908dca4ba895a17fa64f333ac58af0`. UI: dunkle Pattern/Fill/BUS/Choke/Sample-Menüs, neutrale Header-Ringe, BUS-Routing-Anzeige, Velocity-Einzelzeilenfunktionen RANDOM VEL / RESET VEL und LIVE VEL im VEL-Bereich. Die DAW-Aktivierung zusätzlicher Busse ist Host-seitig und weiterhin in Studio One praktisch zu prüfen.
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
