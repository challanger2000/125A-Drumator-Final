# 125A Drumator V1.5.0 – AKTUELLER VERBINDLICHER STAND

**Aktualisiert:** 05.11.2026 (Europe/Berlin). **Aktueller Kunden-Build:** V1.5.0 einschließlich **korrigierter Versionsgravur (V15-FTR1)**, vollständig im Windows-Test geprüft und als **Gumroad-Kunden-ZIP** bereitgestellt. **Die tatsächlich öffentliche Gumroad-Seite wurde nicht aktualisiert**; `main` / bisherige V1.0.1 unverändert. Keine neue GUI-Korrektur offen.

## 1. Aktuelles Gumroad-Paket – eindeutig finden

- **Dateiname:** `125A_Drumator_v1.5.0_Gumroad_Paket.zip`.
- **ChatGPT Library:** `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip`. Diese Library-Datei wurde nach erfolgreicher GUI-Korrektur **in-place überschrieben** und ist die einzig aktuelle Kundenfassung.
- **Library-Index:** `/125A-Drumator/Aktuell-v1.5.0/START-HIER.md`.
- **SHA-256 ZIP:** `a4423f857fc345ade35fa496fc241f61df22c81c07033a5c01e933966886ccc8`.
- **Größe:** 17.434.576 Bytes; **24 geprüfte Dateien**, ZIP-CRC und jedes enthaltene SHA256SUMS-Element erfolgreich geprüft.
- **Plugin-DLL SHA-256:** `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`, byteidentisch mit dem Windows-GitHub-CI-Artefakt des Quellcode-Commits `286be54c463fb09a488d68e0fff288b85d17b6aa`.
- **Beispielkit SHA-256:** `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`, `125A Drumator Kit.vstpreset`, acht eingebettete WAVs, unverändert. Das Original liegt unter `/125A-Drumator/Originaldateien/125A Drumator Kit(2).vstpreset`.
- **Handbücher DE/EN**, Installation DE/EN, Changelog und sämtliche anderen 16 VST3-Ressourcen sind gegenüber der vorigen Verkaufs-ZIP **byteidentisch**. Nur die geprüfte DLL wurde ersetzt und die SHA-Liste aktualisiert.
- **Vorherige Kunden-ZIP:** `/125A-Drumator/Archiv-RC1-RC4/125A_Drumator_v1.5.0_Gumroad_Paket_VOR_FooterFix.zip` (SHA-256 `9adf31ea2095c209ba861a6399bbc4f9c03dbd8ccc2f4b0a83842fd3ae4cb61a`), **veraltet**. Alle RC1–RC4-Testpakete dort ebenfalls nur Archiv. Vor allem RC3-Multi-MIDI ist **verworfen**.

## 2. Verifizierter Quellstand, Korrektur und Gate

- **Repo:** https://github.com/challanger2000/125A-Drumator-Final ; **Entwicklungsbranch:** `develop/beat-creator-10-styles-20261102`.
- **Aktueller Produktcode-Commit:** [`286be54c`](https://github.com/challanger2000/125A-Drumator-Final/commit/286be54c463fb09a488d68e0fff288b85d17b6aa); danach nur GitHub-Dokumentation.
- **Windows-Actions:** [#41932222861](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861) **SUCCESS – 33/33 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL unabhängiger 125A Plugin Tester**. AUD-F07 Original-MIDI-Drag weiter bestanden.
- **V15-FTR1 Versionsgravur**: In MAIN und SONG zentral bei `x=599..681, y=footerTop+27..+37`, Abstand zum unteren Gehäuserahmen. Hauptstatus und SONG-Status beide auf eigene obere `y=+9..+25`-Zeile geclippt, Version in eigene untere Zeile. Keine Überlappung mit Status, ADD/REMOVE, PREV/NEXT, EXPORT oder Klickbereichen. Native Windows-Renderings bei 120% MAIN und 150% SONG **visuell geprüft**. Details: [V15-FTR1 Release-QA](docs/V15-FTR1-GUI-RELEASE-20261105.md).

## 3. Verbindliche Funktionsverträge – KEINE eigenmächtigen Umdeutungen

- **Complexity** Start 35 %, Plus/Minus **5 Prozentpunkte**, Direkteingabe 0–100. Niemals wieder 10er-Schritte.
- **Beat Creator** 10 elektronische Genres / 80 Motive, A/B/C; **Crash BC59** mit seltenen genregewichteten A/B/C-Akzenten erhalten.
- **Swing SW05** bei 50 % echtes 2:1-Achtel, zusätzlich in VST3/WAV/MIDI getestet.
- **MIDI Save As + Drag:** ursprünglicher **Ein-Datei-Export** als Standard MIDI File Format 1, 960 PPQ, 17 Tracks (Conductor + 16 Drumspuren). **Kein Mehrdateien-Drag oder -Save-As!**
- **GUI:** DRUMATOR-Wordmark um 20 % verkleinert (`kTitleReduction=0.64`), `VERSION 1.5` jetzt korrekt in MAIN- und SONG-Fußleiste, ohne Status-/Knopf-Kollision.
- Windows-x64 VST3 only. Kein Eingriff in DSP/MIDI/Timing/Beat-Creator bei V15-FTR1.

## 4. Was tatsächlich noch offen ist

- **Kein nachgewiesener Gumroad-Store-Upload** der V1.5: Kunden-ZIP in Library ≠ Shop-Veröffentlichung.
- **Rechteklärung** für kommerzielle Weitergabe der acht WAV-Samples im Besitzerpreset.
- GitHub Actions simulieren einen VST3-Host und rendern echte Windows-GUI, aber keine echte Studio-One-/ASIO-Sitzung. Der Nutzer darf nicht als Software-Fehlersucher behandelt werden.

**Zuerst lesen:** Dieses Dokument, [START-HERE.md](START-HERE.md), [V15-FTR1 Release-QA](docs/V15-FTR1-GUI-RELEASE-20261105.md). Frühere widersprüchliche Git-Übergaben bleiben nur historisch abrufbar, etwa [CURRENT.md vor der Bereinigung](https://github.com/challanger2000/125A-Drumator-Final/blob/6184718dda4a725fe4892a9e76533ec24dfa2f17/CURRENT.md). Keine Produktänderungen ohne konkreten belegten Fehler oder Nutzerauftrag.
