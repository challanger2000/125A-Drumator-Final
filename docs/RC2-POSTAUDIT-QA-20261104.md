# 125A Drumator 1.5 – RC2-Postaudit, Reparaturen und Testadäquanz

**Stand:** 04.11.2026 · **Entwicklungsbranch:** `develop/beat-creator-10-styles-20261102`
**Quellstand:** `a1d76cec42a9caa56a7cfd9d18756c2771daa3df` · **Öffentliche Version 1.0.1 unverändert**.

## Nachweisgrundlage

- Vorheriges RC2: `9d2eea4e`, GitHub Actions [41905718729](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41905718729) grün, aber falsches Swing-Testorakel (`12.000 → 14.000` Frames bei 120 BPM / 48 kHz).
- SW05/BC64: Commit [c6556014](https://github.com/challanger2000/125A-Drumator-Final/commit/c6556014c1ff1d7e9322e8ba386a8f4b9ac614cf); [Actions 41912269901](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41912269901) SUCCESS: **33/33 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL**.
- F03-Dateiexport: Commit [a1d76cec](https://github.com/challanger2000/125A-Drumator-Final/commit/a1d76cec42a9caa56a7cfd9d18756c2771daa3df); [Actions 41913685096](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41913685096) SUCCESS: **33/33 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL**.

## Anforderungs- und Prüfmatrix

| Vertrag | Änderung | Test / unabhängiges Soll | Status |
| --- | --- | --- | --- |
| SW05: Swing 0 % gerade, 50 % 2:1-Triolen-Achtel | `SequencerCore.h` korrigiert gemeinsame Core-/Audio-/SMF-/WAV-Zeitbasis; Sechzehntel bleiben geordnet | `CoreTests.cpp`: Beat-Bruchteil statt kopierter Implementation; 0/35/50 %, 90/120/150 BPM, 44,1/48/96 kHz; `SW01` Blockgrenzen und negative PPQ | **PASS** im geprüften Windows-Lauf |
| SW05: echte Audio- und Export-Ausgabe | Neue Sollwerte für separate `SW02`-MIDI, `SW03`-24-Bit-WAV und `SW04`-VST3-PCM | Bei 120 BPM / 48 kHz muss Schritt 2 mit 50 % Swing bei **16.000 statt 14.000 Frames** und MIDI-Tick **640 statt 560** auftreten; Viertel bleiben stabil | **PASS** im geprüften Windows-Lauf |
| BC64: Complexity-Bedienung, Startwert 35, +/- 10 und 0/100 | Gleicher `UiBeatCreatorDialog::steppedComplexity()` in View und Test, direkte Eingabe bleibt 0–100 | `UiV101LayoutTests.cpp`: 35→45→35, 95→100, 5→0, Endpunkt-Klemmung | **PASS** automatisiert; echter Studio-One-Klick **NOT RUN** |
| F03: getrennte MIDI-Dateien statt 16 Spuren in einer Save-As-Datei | `MidiExport::encodeSeparate` schneidet aus dem bestehenden 17-Spuren-SMF einen Tempospur+Instrument-Dateisatz; `Controller::exportMidiRows` veröffentlicht einen neuen Ordner atomar | `MidiExportTests.cpp`: Zeilenisolation, Noten, gültige 2-Track-SMFs, Rollback bei ungültiger Eingabe; `WindowsIntegration.cpp`: verbundener Controller, echter Dateiexport, Zielordner, Schutz vor Überschreiben | **PASS** im geprüften Windows-Lauf |
| Altverhalten: D&D-Format-1 bleibt kompatibel | `Controller::exportMidi` bleibt für aktuellen OLE-Drag unverändert | Bestehende Multi-Spur-MIDI-/Drag-/Tempo-Regressionsprüfungen müssen weiterhin bestehen | **PASS** automatisiert; Studio-One-Multi-Datei-D&D **NICHT implementiert** |
| F04: Projektübergabe zeigt gültigen Stand | `CURRENT.md`, `START-HERE.md` und dieses QA-Dokument auf Code/CI gepinnt | Refs und exakte Windows-Gates genannt, ältere RC1-Einträge als Historie markiert | **DOKUMENTIERT** |

## Grenzen und aktuelle Freigabeentscheidung

**STATUS: HOLD – nicht auf Gumroad veröffentlichen.** Die Tests prüfen den tatsächlich gebauten Windows-VST3-Code, nicht eine Benutzeraufnahme oder das tatsächliche Studio-One-Pluginfenster. Die echte Studio-One-/ASIO-Interaktion, das Importverhalten mehrerer MIDI-Dateien und die optischen Endzustände sind **NOT RUN**. Die vorhandene RC2-ZIP aus `9d2eea4e` ist **nicht** die neue Binärdatei; eine neue verifizierte Test-ZIP wurde noch nicht hergestellt. Die PDF-Handbücher müssen den nun abweichenden Complexity-Schritt und die getrennten MIDI-Dateien erläutern. Das beigefügte `125A Drumator Kit.vstpreset` ist weiterhin mit exakt der freigegebenen Benutzerdatei zu vergleichen; die kommerziellen Rechte der enthaltenen Audiosamples müssen vor Vertrieb geklärt werden.

**Wichtig:** Die alte Swing-QA war trotz grüner Zahlen fachlich falsch, weil mehrere Tests den fehlerhaften 14.000-Frame-Wert zur Referenz erhoben. Der neue SW05-Sollwert wurde ausdrücklich aus dem musikalischen 2:1-Verhältnis unabhängig hergeleitet; neue künftige Tests müssen ebenfalls zuerst die fachliche Wirkung und erst dann die Implementierung absichern. Kein „vollständig fehlerfrei“ und kein „FINAL“ aus grüner CI ableiten.
