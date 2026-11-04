# START HERE – 125A Drumator V1.5.0

**OWNER-UPDATE 04.11.:** Plugin V1.5 wurde in der DAW bereits geöffnet; Gumroad-Upload erfolgt jetzt. Ein vollumfänglicher DAW-Funktionstest und die abgeschlossene Shop-Veröffentlichung sind noch NICHT VERIFIZIERT. Siehe [V1.5-Release-Übergabe](docs/V15-RELEASE-HANDOFF-20261105.md).

**AKTUELL:** Korrigierte **Gumroad-Kundenfassung 1.5.0**, V15-FTR1 (Versionsgravur im Footer neu positioniert) ist **gebaut, auditiert und in der ChatGPT-Dateibibliothek gespeichert**. Nicht beim alten RC4, RC3 oder V1.0.1 stehen bleiben. [CURRENT.md](CURRENT.md) ist verbindlich.

## Kunden-ZIP und Integritätswerte

- **Library:** `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip`.
- **Canonical Library-ZIP SHA256:** `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401` (17.434.576 Bytes, 24 Dateien, ZIP CRC und Inhalts-Hashes PASS).
- **Neue geprüfte VST3-DLL SHA256:** `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`.
- **Besitzerkit enthalten:** `125A Drumator Kit.vstpreset` mit 8 WAVs, SHA256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`. Zwei aktuelle Benutzerhandbücher DE/EN, Anleitung DE/EN, Changelog und Checksums sind drin.
- **Ältere Kundenfassung vor Footer-Fix:** Archiv `/125A-Drumator/Archiv-RC1-RC4/125A_Drumator_v1.5.0_Gumroad_Paket_VOR_FooterFix.zip`; **nicht verwenden**. RC1–RC4 sind historische Testarchive.

**Hinweis:** Die früher verlinkte Download-ZIP kann SHA256 a4423f857fc345ade35fa496fc241f61df22c81c07033a5c01e933966886ccc8 haben; gleiche 24 Mitglieder und Inhalte, aber abweichende ZIP-Zeitstempel. Der in Library abgelegte ZIP-SHA ist d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401. Nicht mit zwei verschiedenen Plugin-Versionen verwechseln.

## Verifizierter Quellcode und Tests

- **Repo:** https://github.com/challanger2000/125A-Drumator-Final
- **Branch:** `develop/beat-creator-10-styles-20261102`
- **Code:** `286be54c463fb09a488d68e0fff288b85d17b6aa`, V15-FTR1 ist ausschließlich präzise MAIN-/SONG-Footertextgeometrie plus Assertions. Spätere Commits ändern nur Docs.
- **Vollständiger [Windows-Testlauf #41932222861](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861): SUCCESS**: 33/33 CTests, Steinberg 47/47 und 125A Plugin Tester 58 PASS / 0 WARNING / 0 FAIL. Native Windows-Screenshots aus dem Lauf visuell geprüft.
- **Versionsgravur repariert:** `VERSION 1.5` auf gemeinsamer zweiter Zeile bei `footerTop+27..+37`; Status bleibt in erster Zeile `footerTop+9..+25`; Status und Gravur können sich aufgrund eigener Clipping-Bounds nicht übermalen. Buttons und Hitboxen unverändert. Details: [docs/V15-FTR1-GUI-RELEASE-20261105.md](docs/V15-FTR1-GUI-RELEASE-20261105.md).

## Unverrückbare Nutzeranforderungen

**Complexity +/−5** (Start 35; Direkteingabe 0–100), Swing 50% echter 2:1-Achtel, Crash-Fix BC59, ursprünglicher **MIDI-Ein-Datei-Export/Drag SMF Typ 1 mit 17 Spuren**, DRUMATOR-Logo 20% kleiner, Beat Creator 10 Styles/80 Motive. **RC3-Mehrdateien-MIDI und die ungefragten 10er-Schritte bleiben verworfen.**

## Was offen bleibt

- **Noch nicht auf Gumroad veröffentlicht** (nur Kunden-ZIP fertig und in Library).
- Weitergabe-Rechte der Kit-WAVs vor kommerziellem Verkauf klären.
- Native Windows-Actions-Testhost ist keine echte Studio-One-/ASIO-Sitzung.

**Neue Chats:** Zuerst `CURRENT.md`, diese Datei und die Library prüfen, nicht nach vorhandenen Paketen fragen. Audit darf nicht erneut funktionierende MIDI-/DSP-Prozesse aus eigenem Antrieb verändern. Das veröffentlichte `main` und die alte V1.0.1 bleiben unangetastet.
