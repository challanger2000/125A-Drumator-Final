# START HERE – 125A Drumator / verbindliche Projektübergabe
Stand: 02.11.2026. **Zuerst dieses Dokument und CURRENT.md lesen, bevor Dateien angefordert, Builds erstellt oder Änderungen vorgeschlagen werden.**

## 1. Verbindliche Verkaufsfassung – NIEMALS RÄTSELN
- **Original-Verkaufs-ZIP des Nutzers:** `125A_Drumator_v1.0.1_Gumroad_Final.zip`
- Bereits vorhandene inhaltlich **byteidentische** Archive in der ChatGPT-Dateibibliothek:
  `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`,
  `Drumator_1.0.1_Gumroad_NEU.zip`.
- **Alle drei ZIP-SHA256:** `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`.
- **Exakte geprüfte 1.0.1-Quellrevision:** `3748b1e9e1867357362304c51bec2cb99ac9657b`.
- `main` auf `f4c5caa4be82626ea89459d9835c12138d7c6a1b` enthält danach nur Dokumentationsänderungen.
- **VST3-Binary SHA256:** `06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278`.
- **Windows Release QA:** [Actions 41157069202](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41157069202): 29/29 CTests, 47/47 Steinberg Validator, 58/58 Plugin Tester.
- Die korrigierte 31.-Oktober-Verkaufs-ZIP enthält 26 Einträge inklusive VST3, DE/EN-Handbücher, START-HERE-DE.txt, Prüfsummen und BUILD-INFO.txt.
- Die ältere ZIP `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` ist **überholt**.
- **NICHT MEHR FRAGEN, OB DIE GUMROAD-ZIP FEHLT.** Bereits in der Datei-Bibliothek und vom Nutzer erneut hochgeladen. Falls sie im Container einer künftigen Sitzung nicht gemountet ist, mit `files.list` in der Library nach diesem exakten Namen suchen und `files.materialize` nutzen. Nicht raten und nicht sofort erneut hochladen lassen.
- Hauptquellen: [CURRENT.md](CURRENT.md) und [README.md](README.md) auf `main`. Die zuletzt geprüfte 1.0.1 ist die technische Basis; die veröffentlichte VST3 und das Gumroad-Paket bleiben unangetastet.

## 2. Vorgeschichte: V1.5 verwerfen
- Der frühere Branch `develop/v1.5.0` enthielt Beat Creator mit 8 Styles sowie Sample-Presets/Preview-Kit, dessen Klang der Nutzer ablehnte.
- Der Nutzer meldete außerdem: **WAV-Export läuft wesentlich langsamer als das Studio-One-Tempo**. Das ist ein offener V1.5-Bug, NICHT als gelöst behaupten.
- **Nicht** die komplette V1.5 als Basis nehmen, keine alten Samplekits übernehmen, nicht ungeprüft deren Export/GUI portieren.
- Nur klar isolierbare Verbesserungen wie die Velocity-Zeichengesten sollen gezielt übernommen und gegen 1.0.1 getestet werden.

## 3. Neues Projekt – aktiver Branch
- Branch `develop/beat-creator-10-styles-20261102`, von `main` (1.0.1) angelegt.
- **BC42 inzwischen implementiert:** Beat Creator mit STYLE-Dropdown für 10 Genres, 1/2-BAR-Umschaltung, Complexity mit direkt editierbarer Zahl, sichtbarer A/B/C-Schrittvorschau, GENERATE/APPLY/CONFIRM/CANCEL und unveränderter 8-Instrumente-Legende. Controller- und Processor-Anschluss übernehmen A/B/C gemeinsam per bereits in V1.5 bewährter, atomarer Compare-and-Swap-Transaktion; ein gemeinsames Undo ist vorhanden. **Keine Demo-Samples, keine Sample-Kits, kein Kit-Browser.** Alle Änderungen ausschließlich auf diesem Entwicklungsbranch, 1.0.1-Verkaufspaket unverändert. Vollständiger Windows-VST3-QA-Lauf noch gesondert zu prüfen.
- 10 elektronische Genres, per **einzelnem STYLE-Dropdown**: 80s Synthpop, Italo Disco, Synthwave, EBM, 90s Eurodance, House, Techno, Trance, Hip-Hop, Drum & Bass.
- **80 fest definierte Groove-Motive (8 pro Genre)**; A Hauptgroove, B passende Variation, C halbtaktiger genrebezogener Break; Complexity, Velocity, 1/2 Takte.
- **KEINE mitgelieferten Samples, KEIN Kit-Browser, KEIN Preview-Kit-Button.** Nutzer lädt seine WAV/FLAC separat. Der Creator schreibt nur Steps und Velocity.
- Feste Legende: 01 Kick, 02 Snare, 03 Closed HH, 04 Open HH, 05 Low Tom, 06 High Tom, 07 Crash, 08 Clap. Reihen 9–16 unangetastet.
- Velocity-Tools von der vorherigen Entwicklung (Paint, Shift-Line, Shift+Alt-Curve, Alt-Accent) als **isolierter Algorithmus plus Regressionstests** übernommen. Die GUI-Gestenintegration wurde in BC41 direkt aus dem bewährten V1.5-Code übernommen: `src/vst3/UiV101Interaction.h` ist **byteidentisch** zur V1.5-Implementation, in `src/vst3/DrumatorView.h/.cpp` wurde nur die Weitergabe von Alt/Shift ergänzt. **Windows-CI für BC41 ist noch zu prüfen, keine Studio-One-Praxisbestätigung.**
- **CI** [Run 41371936461](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41371936461) für Code-Commit `2e14f9af`: Windows und unabhängiger Tester **SUCCESS**. Danach nur Dokumentationsänderungen. Die ersten beiden Zwischenläufe scheiterten an Compile-Fehlern, die im Erfolgsstand korrigiert wurden.
- Den Detailvertrag enthält [docs/BEAT-CREATOR-BC40-10-ELECTRONIC-STYLES.md](docs/BEAT-CREATOR-BC40-10-ELECTRONIC-STYLES.md).

## 4. Nächste Arbeiten – ohne weitere Nutzer-Rückfragen zur Basis
1. **Nicht erneut Velocity, Groove-Kern, Style-Dropdown oder A/B/C-Transaktion entwickeln**: diese Funktionen sind bereits im isolierten Branch umgesetzt.
2. BC41 Velocity-Mausgesten: Windows-CI [41373979469](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41373979469) SUCCESS, Studio-One-Praxistest der neuen GUI noch ausstehend.
3. BC42 GUI/Engine: Windows-CI [ab Lauf 41376112322](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41376112322) noch prüfen. Eine frühere parallele Prüfung schlug in einem zeitkritischen 64-Stimmen-Lasttest fehl, nicht aufgrund eines bestätigten Compile-Problems; alte gleichzeitige Testläufe können sich gegenseitig beeinflussen.
4. **Vor Ausgabe einer Test-VST3**: MIDI-/WAV-Export (DAW-BPM, Patterns/Song, unterschiedliche BPM), Song, Recall, GUI, Velocity, unabhängiger Tester und Studio-One-Praxistest validieren. Keine unbewiesene Tempo-Korrektur behaupten.
5. Änderungen ausschließlich in diesem isolierten Branch; **keine** Release-Änderung, kein Gumroad-Upload, kein direktes Überschreiben von `main`.

## 5. Leitregel
**Nicht den Nutzer wiederholt fragen, was bereits in README/CURRENT/START-HERE oder Library belegt ist. Zuerst nachsehen.** Statusworte `SUCCESS` nur mit konkretem geprüften Build; isolierter Code ist noch kein fertiges Plugin.
