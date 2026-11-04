# START HERE – 125A Drumator V1.5.0

**Einzige aktuelle Projektentscheidung und Dateiquelle:** [CURRENT.md](CURRENT.md). Alle älteren RC1/RC2/RC3-Stände und alte Chatbehauptungen sind historisch. **Bei einem neuen Chat bitte nicht wieder bei V1.0.1 anfangen oder nach der schon erstellten V1.5-Gumroad-ZIP fragen.**

## Sofort-Links und Datei

- **Aktuelles V1.5-Gumroad-Kundenpaket – FERTIG ERSTELLT, NICHT im Store hochgeladen:** `125A_Drumator_v1.5.0_Gumroad_Paket.zip` in der **ChatGPT Library** unter `/125A-Drumator/Aktuell-v1.5.0/`.
- Library-Index: `/125A-Drumator/Aktuell-v1.5.0/START-HIER.md`. Vollständiges ZIP-SHA256: `9adf31ea2095c209ba861a6399bbc4f9c03dbd8ccc2f4b0a83842fd3ae4cb61a`. 24 Dateien, ZIP geprüft.
- 125A-Kit ist **im ZIP**, SHA256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`; deutsche und englische Handbücher ebenfalls. Besitzer-Original in Library `/125A-Drumator/Originaldateien/`.
- **Windows-Produkt-DLL SHA256:** `67818e9d012a9151d44eeb69f73baf12d166fea96f71a33a812a3161f42d0acb`. Code entspricht Produktcommit `7736fe9`; [Windows-Actions #41923917904](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41923917904) vollständig **SUCCESS: 33/33 + 47/47 + 58 PASS/0 WARNING/0 FAIL**, inklusive neuer AUD-F07 Original-MIDI-Drag-Regression.
- Ältere Testpakete liegen unter `/125A-Drumator/Archiv-RC1-RC4/` und dürfen nicht als aktuelle V1.5-Kundenfassung verwendet werden. `main` / verkaufte V1.0.1 bleibt unangetastet.

## Freigegebene technische Anforderungen

- **Complexity: Start 35, +/- 5** (nicht 10), direkte Eingabe 0–100.
- **SWING:** 50 % echtes 2:1-Achtel.
- **Crash BC59:** seltene genreabhängige Akzente in A/B/C, Fix erhalten.
- **MIDI Save As und MIDI-Drag:** EINE Datei, **SMF Typ 1 mit 17 Spuren**, 960 PPQ. RC3-Mehrdateien-Drag ist verworfen.
- **GUI:** DRUMATOR-Schriftzug um 20 % verkleinert; „VERSION 1.5“ unten vorhanden, aber am 04.11. **vom Nutzer optisch beanstandet: zu tief an unterer Gehäusekante**.

## Was tatsächlich noch offen ist

1. **GUI-Position von `VERSION 1.5`** korrigieren, wenn der Benutzer das anweist: `src/vst3/UiV101NativePainter.h`, `footerPlate`, Label-Bounds aktuell `{599.,footerTop+34.,681.,footerTop+44.}`. Danach zwingend alle Windows-Gates und exakt passende Kunden-ZIP **neu** erstellen. Vorhandene ZIP nicht überschreiben, solange keine Version-/Hashänderung dokumentiert wird.
2. Ob/wann Nutzer das V1.5-Paket bei **Gumroad tatsächlich hochlädt**, ist offen. Das fertige ZIP in Library bedeutet nicht Shop-Veröffentlichung.
3. Kommerzielle Weitergabe der Beispielkit-Samples muss vom Rechteinhaber geklärt sein; technische QA stellt keine Lizenz dar.
4. Keine echte Studio-One-/ASIO-Ausführung innerhalb GitHub CI; der Assistent muss Fehler im Testhost selbst suchen, statt den Nutzer als Fehlersucher einzuplanen.

## Arbeitsregel

1. Zuerst `CURRENT.md`, diesen Einstieg, Audit [docs/RC4-RELEASE-AUDIT-20261104.md](docs/RC4-RELEASE-AUDIT-20261104.md) und die Library-Dateien lesen; bei Dateien `files.list` / `files.materialize` nutzen statt zu behaupten, sie seien nicht vorhanden.
2. Keine „Verbesserungen“ an MIDI, Complexity oder DSP ohne **konkret nachgewiesenen Fehler oder ausdrücklichen Nutzerauftrag**.
3. Releaseänderungen isoliert halten, Regressionstests schreiben, Windows-Build/Steinberg-Validator/Plugin Tester wiederholen, neuen ZIP-Hash dokumentieren. `main`, Tags und Shop nie selbständig ändern.
4. Frühere Übergabedokumente sind per Git-Historie erhalten: [historische START-HERE.md](https://github.com/challanger2000/125A-Drumator-Final/blob/6184718dda4a725fe4892a9e76533ec24dfa2f17/START-HERE.md).
