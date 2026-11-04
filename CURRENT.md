# 125A Drumator V1.5.0 – AKTUELLER VERBINDLICHER STAND

**Zuletzt aktualisiert:** 04.11.2026. **Neuester Kunden-Build:** V1.5.0, technisch vollständig geprüft, **Gumroad-Kunden-ZIP erstellt und in der ChatGPT-Dateibibliothek archiviert**. Ein Upload in den tatsächlichen Gumroad-Shop ist **NICHT** bestätigt. **Offen:** Versionsbeschriftung sitzt optisch zu tief im Footer (neu am 04.11. gemeldet); deshalb vor einem späteren Shop-Update auf Layoutabnahme und ggf. neuen Build achten.

## 1. SOFORT FINDEN: Aktuelles Gumroad-Paket

- **Exakter Name:** `125A_Drumator_v1.5.0_Gumroad_Paket.zip`
- **ChatGPT Library:** `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip`
- **Library-Einstieg:** `/125A-Drumator/Aktuell-v1.5.0/START-HIER.md`
- **ZIP SHA-256:** `9adf31ea2095c209ba861a6399bbc4f9c03dbd8ccc2f4b0a83842fd3ae4cb61a`
- **24 Kunden-Dateien, 17.434.560 Bytes, ZIP-CRC erfolgreich geprüft.** Enthält eine vollständige Windows-x64-VST3, deutsches/englisches PDF-Handbuch, Installationsanleitungen, Changelog und Prüfsummen, sowie das Eigentümer-Kit `125A Drumator Kit.vstpreset` mit acht eingebetteten WAVs.
- **VST3-DLL SHA-256:** `67818e9d012a9151d44eeb69f73baf12d166fea96f71a33a812a3161f42d0acb`; **byteidentisch** mit dem geprüften Windows-CI-Build.
- **Kit SHA-256:** `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`. Besitzer-Original liegt separat unter `/125A-Drumator/Originaldateien/125A Drumator Kit(2).vstpreset`.
- Alte RC1–RC4-Testarchive sind in `/125A-Drumator/Archiv-RC1-RC4/`, **nicht** das aktuelle Kundenpaket. Besonders RC3 mit mehreren MIDI-Dateien ist **verworfen**.

## 2. Verifizierter Code und Qualitätsnachweise

- **Repository:** https://github.com/challanger2000/125A-Drumator-Final
- **V1.5-Branch:** `develop/beat-creator-10-styles-20261102`. Produktcode entspricht `7736fe91090283e43c89b97b627bb872212b86b9`.
- **Prüflauf mit zusätzlichem echten Windows-OLE-Ein-Datei-MIDI-Dragtest AUD-F07:** [Actions #41923917904](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41923917904), SHA `d1b297e51a142d0ac94e1f95ad25243cef892118`, **SUCCESS**.
- **Gates:** **33/33 CTests, 47/47 Steinberg VST3 Validator, 58 PASS / 0 WARNING / 0 FAIL** unabhängiger Plugin Tester.
- Spätere Commits änderten nur **Dokumentation**, nicht `src/` und keine Test-Funktionen. Nicht erneut ungefragt an produktiven MIDI-/DSP-/GUI-Funktionen arbeiten.

## 3. Verbindliche Funktionen – niemals uminterpretieren

- **Beat Creator:** 10 elektronische Genres/80 Grooves; A/B/C (Generate/Variate/Break), 16/32 Steps.
- **Complexity:** Startwert **35**, Plus/Minus **5er-Schritte**; Werte 0–100 direkt editierbar. Die früher behaupteten 10er-Schritte sind falsch und zurückgenommen.
- **Swing:** 50 % = 2:1-Achtel-Triolenfeel, via Audio/Export geprüft.
- **Crash:** sparsame genregewichtete A/B/C-Akzente, Korrektur `9d2eea4e` unverändert.
- **MIDI-Export und -Drag:** unveränderter ursprünglicher **Ein-Datei-Export, SMF Format 1, 960 PPQ, 17 Spuren** (Tempo + 16 Instrumente); DAW kann die enthaltenen Spuren getrennt importieren. **KEIN** Mehrdateien-Drag, kein neuer Save-As-Ordner pro Instrument!
- **GUI:** DRUMATOR-Wordmark gegenüber vormals 0,80 um 20 % kleiner (`kTitleReduction=0.64`), kleine Beschriftung `VERSION 1.5` unten, aber **deren Position ist beanstandet**.

## 4. Offene Sache nach der Paketerstellung

Am 04.11. nach Fertigstellung der Kunden-ZIP wurde die optisch misslungene Position von **`VERSION 1.5`** gemeldet. Aktuell zeichnet `src/vst3/UiV101NativePainter.h` in `footerPlate` das Label bei `{599.,footerTop+34.,681.,footerTop+44.}` direkt nahe am unteren Rand. **Nicht als behoben melden.** Das vorhandene Gumroad-ZIP ist der letzte vollständig getestete Build; falls die Position geändert wird, muss die Änderung minimal sein, anschließend Windows-CI vollständig bestehen und ein **neues** eindeutig benanntes Gumroad-ZIP gebaut werden. Keine heimliche Modifikation der bestehenden ZIP.

**Vertrieb:** Bisher keine Bestätigung eines tatsächlichen V1.5-Gumroad-Uploads. Rechte an den eingebetteten Kit-Samples vor öffentlichem Verkauf klären. Das veröffentlichte `main` und die bisherige V1.0.1 bleiben unverändert.

## 5. Übergabe / alte Dokumente

**Jeder neue Chat liest zuerst `CURRENT.md`, `START-HERE.md` und den Library-Index.** Vorherige, widersprüchliche Übergaben sind lediglich historische Git-Revisionen, siehe [CURRENT.md vor der Bereinigung](https://github.com/challanger2000/125A-Drumator-Final/blob/6184718dda4a725fe4892a9e76533ec24dfa2f17/CURRENT.md) und [START-HERE.md vor der Bereinigung](https://github.com/challanger2000/125A-Drumator-Final/blob/6184718dda4a725fe4892a9e76533ec24dfa2f17/START-HERE.md). Details: [RC4-Release-Audit](docs/RC4-RELEASE-AUDIT-20261104.md). **Keine alte RC3-Aussage hat Vorrang vor diesem Dokument.**
