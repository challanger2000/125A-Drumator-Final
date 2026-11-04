# 125A Drumator V1.5.0 – V15-FTR1 Footer-Layout-Fix / Releaseprüfung

**Stand:** 05.11.2026 · **Produktcommit:** [286be54c](https://github.com/challanger2000/125A-Drumator-Final/commit/286be54c463fb09a488d68e0fff288b85d17b6aa) · **Windows-CI:** [41932222861](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861) **SUCCESS**.

## Ausgangsfehler und Korrektur

Der Nutzer zeigte, dass die `VERSION 1.5`-Gravur unter MAIN und insbesondere SONG direkt am unteren Bezel-Rand gezeichnet war. Alte Bounding Box `{599.,footerTop+34.,681.,footerTop+44.}`; der gemeinsame Footer ist 48px hoch, der sichtbare Bezel endet bei `footerTop+42`. Dadurch ragte die Labelbox 2px unter den Bezel und kollidierte potenziell mit Status-Textboxen, die bis `+33/+35` reichen.

**Geprüfte neue gemeinsame Footer-Grenzen (100%-Koordinaten, proportionale Skalierung):**

- Bezel: `x 12..1260`, relative `y +4..+42`.
- Versionslabel: `x 599..681`, relative `y +27..+37`, horizontal exakt um `x=640` zentriert; 5px Abstand zur inneren unteren Begrenzung.
- MAIN-Zusammenfassung: `x 260..510`, relative `y +9..+25`.
- MAIN-Status: `x 520..900`, relative `y +9..+25`.
- SONG-Status: `x 32..1090`, relative `y +9..+25`.
- 2px Mindestabstand zwischen oberer Textbox und unterer Labelbox; Status- und Versionstext werden **in ihren eigenen Bounds geclippt**.
- Footer-Aktionsknöpfe `ADD ROW/REMOVE ROW`, `PREV/NEXT PAGE`, `EXPORT` und alle nativen Hitboxen bleiben unverändert.

**Beteiligte Dateien:** `src/core/UiV101Layout.h` (zentraler Geometrievertrag), `src/vst3/UiV101NativePainter.h` (gemeinsamer Footer), `src/vst3/UiV101SongPainter.h` (SONG-Status), `tests/UiV101LayoutTests.cpp` (Assertions für MAIN/SONG, 432-/686px-Ansichten, alle Footer-Schaltflächen und beide Statusbereiche). **Keine Änderungen am MIDI-/DSP-/Beat-Creator- oder Maushitcode.**

## Testnachweis

- [Actions 41932222861](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861): **33/33 CTests PASS, 47/47 Steinberg Validator PASS, unabhängiger 125A Plugin Tester 58 PASS / 0 WARNING / 0 FAIL**. Auch AUD-F07 mit originalem Ein-Datei-MIDI-Drag PASS.
- **Echte native Rasterbilder** aus [V101-Renderings-Artefakt 13371524848](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861/artifacts/13371524848): `v101-layout-native-main-120.bmp` (120%), `v101-song-board-hardware-150.bmp` (150%). Beide wurden nach CI über den Windows-Painter visuell geprüft: Gravur lesbar, mittig innerhalb der unteren Leiste; Status darüber und EXPORT/Knöpfe frei.
- **Gumroad-Kunden-ZIP**, 24 Dateien, ZIP-CRC und SHA-Liste PASS: `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip`, SHA-256 `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401`, Größe 17.434.576 Bytes. Geprüfte enthaltene Windows-VST3-DLL: SHA-256 `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`.
- Kit unverändert SHA `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`; **16/16 restliche VST3-Dateien**, DE/EN-PDF-Handbücher und Kundentexte bytegleich mit vorheriger Kundenfassung. Nur DLL und SHA-Liste geändert.
- Vorherige ZIP als `...Gumroad_Paket_VOR_FooterFix.zip` in `/125A-Drumator/Archiv-RC1-RC4/`, SHA-256 `9adf31ea2095c209ba861a6399bbc4f9c03dbd8ccc2f4b0a83842fd3ae4cb61a`.

**Status:** V15-FTR1 technisch abgeschlossen; Kundenpaket ist auf neuestem Stand in der Library. **Kein bestätigter Gumroad-Shop-Upload.** Keine Garantie einer eigenständig ausgeführten Studio-One-/ASIO-Sitzung durch GitHub CI. Rechte der Beispielkit-Samples vor kommerziellem Vertrieb klären.
