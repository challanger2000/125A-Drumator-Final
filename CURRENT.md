# 125A Drumator V1.5.0 – AKTUELLER PROJEKTSTAND

**Stand: 06.11.2026.** Die vom Eigentümer gewünschte **exakte Wiederherstellung der ursprünglichen 17×34-Step-Tasten aus dem ersten Screenshot** wurde implementiert und durch Windows-Pixelvergleich, alle CTests, Steinberg Validator sowie 125A Plugin Tester geprüft. Die **aktualisierte Kunden-ZIP wurde erstellt und in ChatGPT Library gesichert, aber NOCH NICHT als bei Gumroad hochgeladen bestätigt**. Das bisherige Gumroad-V1.5.0-Angebot enthält nach letzter Eigentümerbestätigung die frühere V1.5.0 ohne diesen neuen Step-Fix.

## SOFORT: Neueste geprüfte V1.5.0 – Original-Steps Update

- **Repository:** https://github.com/challanger2000/125A-Drumator-Final
- **Aktuelle Entwicklungsbranch:** `develop/beat-creator-10-styles-20261102`.
- **Produktcode-Fix:** `92c2e412d78b4685f0527f8fb8e31571735b3bb3`; letzter geprüfter Source-/Test-Commit `f95cc5d5d09bd142f1f55799791eb1c1bf5b427f`.
- **Windows QA:** [Actions #42174832595](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42174832595) **SUCCESS**: **33/33 CTests**, inklusive nativer originaler Hardware-Step-Pixel-Parität für On/Off/Downbeat/Disabled; **47/47 Steinberg Validator**; **58 PASS / 0 WARNING / 0 FAIL unabhängiger 125A Plugin Tester v0.3.2**. Echte Windows-MAIN- und SONG-Ansichten im CI archiviert.
- **Neueste installierbare Windows-x64-VST3-DLL SHA256:** `04ad26caa0ee1abe3985b90860d7e23da85043d59d963c0b9097c9623bfff7e5` (bytegleich mit erfolgreichem CI-Artefakt).
- **Neueste aktualisierte Kunden-ZIP (24 Dateien):** `125A_Drumator_v1.5.0_Gumroad_Paket.zip` in **ChatGPT Library** `/125A-Drumator/Aktuell-v1.5.0/`; **ZIP SHA-256 `33d40c5125818b87d41b525aa8e277303722b619058ca631e7b6e305195403f2`**, Größe **17.444.393 Bytes**, ZIP CRC und die 23 Datei-Hashes in `SHA256SUMS.txt` PASS.
- **Optional eindeutiger Nutzer-Downloadname derselben Bytes:** `125A_Drumator_v1.5.0_Gumroad_OriginalSteps_Update_2026-11-06.zip`. Der Name dient zur Unterscheidung von der bereits hochgeladenen früheren V1.5.0.
- **Unverändert:** `125A Drumator Kit.vstpreset` mit 8 WAVs, SHA256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`; DE/EN-PDF-Handbücher, Installationsinfos, sämtliche VST3-Ressourcen byteidentisch zur bisherigen V1.5.0. Nur die DLL und aktualisierte Prüfsummenliste unterscheiden sich.

## Exakte optische Wiederherstellung (nicht nur ähnliches Design)

- Original der alten Steps: `src/vst3/DrumatorView.cpp::drawHardwareStep` aus der bestätigten V1.0.1 in `main`: 17px breit, 34px hoch, dunkle rechteckige Außenfassung, kräftiger hellblauer Beat-Rahmen, originales helles Orange/Reflex.
- V1.5-Produktrenderer: `src/vst3/UiV101NativePainter.h::stepCap` zeichnet **dieselben Formen, Pixelmaße, Farbwerte und Zeichenbefehle ohne Skalierung**; einzig eine vertikale Translation von ehemals `y+69..103` auf `y+7..41` innerhalb der 48px-Instrumentenzeile.
- Die Touch-/Klickflächen entsprechen den tatsächlichen 17×34-Pixel-Tasten; der 19px-Step-Abstand mit 2px Lücke bleibt. Keine Berührung mit benachbarten Zeilen, Instrumentenreglern oder Velocity-Lane.
- Betroffene Produktdateien **nur** `src/vst3/UiV101NativePainter.h`, `src/core/UiV101Layout.h`; Tests **nur** `tests/UiV101LayoutTests.cpp`, `tests/integration/WindowsIntegration.cpp`.
- **MIDI, DSP, Voice-Engine, Velocity, Beat Creator, SWING, Crash, Song/Export und State/Automation unverändert.** Git-Diff gegen vorherige Version hat nur diese vier Dateien.
- Einzelheiten: [docs/V15-ORIGINAL-STEPS-20261106.md](docs/V15-ORIGINAL-STEPS-20261106.md).

## Release-Trennung – SEHR WICHTIG für neue Chats und Codex

- **Frühere V1.5.0 laut Eigentümer auf Gumroad hochgeladen (04./05.11.2026):** Quellcode `286be54c463fb09a488d68e0fff288b85d17b6aa`, erfolgreiche [Windows #41932222861](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861), DLL SHA `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`. Bisherige Library-ZIP SHA `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401` (eine extern verlinkte ZIP-Verpackung mit identischen 24 Mitgliedern hatte SHA `a4423f857fc345ade35fa496fc241f61df22c81c07033a5c01e933966886ccc8`). Welche tatsächlichen Zip-Bytes der Shop liefert, wurde nie unabhängig geprüft.
- **NEUES Original-Steps-Update 06.11.:** Code f95cc5d, Kunden-ZIP SHA **33d40c5**, DLL **04ad26c**. **Der Nutzer hat dessen neuen Gumroad-Upload NOCH NICHT bestätigt.** Neue Chats dürfen nicht behaupten, das Update sei schon öffentlich hochgeladen, und dürfen nicht die alte ZIP als aktuell bereitstellen.
- Die bisherige ZIP wurde in ChatGPT Library archiviert: `/125A-Drumator/Archiv-RC1-RC4/125A_Drumator_v1.5.0_Gumroad_VOR_OriginalSteps_2026-11-06.zip`. Die aktuellen 24 Dateien sind jetzt **nur** im Ordner `Aktuell-v1.5.0`.
- `main` und `develop/v1.0.1-gui-redesign` bleiben für die frühere 1.0.1 unverändert. Die alte separate `develop/v1.5.0` ist stark divergiert und **NICHT** aktueller Quellstand – nicht ungeprüft mergen.
- Nach der Eigentümerfreigabe die `125A-Engineering`-Release-Synchronisierung durchführen; keine eigenmächtige Branch-Zusammenführung oder Tag-Erstellung.

## Unverrückbare V1.5-Funktionen

- Beat Creator **10 Styles / 80 Grooves, A/B/C**, Complexity default 35 %, +/- **5 Prozentpunkte**, direkte Eingabe 0–100.
- MIDI Save As und Drag **EINE SMF Format-1-Datei mit 17 Tracks / 960 PPQ**, kein RC3-Mehrdateien-Experiment.
- SWING 50 % echtes 2:1-Achtel, seltene genregewichtete Crash-Akzente (BC59), DRUMATOR-Wortmarke 20% verkleinert.
- V15-FTR1 `VERSION 1.5` in eigener Footer-Zeile, MAIN/SONG kollisionsfrei.
- Windows-x64 VST3 only. Keine eigenmächtigen Funktionsänderungen.

**Verifikationsgrenze:** Windows-CI ist nicht die tatsächliche DAW. Der Nutzer hat die vorherige V1.5.0 bereits in seiner DAW geöffnet. Die **neue Original-Steps-DLL** ist in einer realen DAW noch **NOT VERIFIED**; Änderungen an Maushit-Geometrie wurden in nativen Host-/Layouttests geprüft, nicht in Studio One bestätigt. Rechte an den 8 Kit-WAVs bleiben unabhängig vom GUI-Fix ungeklärt.
