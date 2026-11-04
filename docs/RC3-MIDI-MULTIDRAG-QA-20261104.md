# 125A Drumator 1.5 RC3 – Windows-Multi-MIDI-Drag, Paket und QA

**Datum:** 04.11.2026 · **Quellcode:** `e22e65517e3e61d5e55e29b0ee7e34ea9b856a3c` · **Branch:** `develop/beat-creator-10-styles-20261102`.

## Unabhängig überprüfte Ausführung

- [Windows Actions #41916990564](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41916990564) **SUCCESS**, 33/33 CTests, Steinberg Validator 47/47.
- [Unabhängiger 125A Plugin Tester v0.3.2](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41916990564) **58 PASS / 0 WARNING / 0 FAIL** an der exakt paketierten DLL-SHA256 `8a3f1571cbb497b04a54a0a15041574946aa18e7e4bae47584c51fd0634d8e28`.
- **F06 native OLE-Prüfung:** `testF06MultiFileMidiWindowsOle` im vorhandenen `drumator_integration_tests` ist im Windows-CTest enthalten und konstruiert **drei echte MIDI-Dateien** (Kick, Snare, Clap). Der Test ruft `MidiDragPackage::create` und `Win32DataObject::GetData(CF_HDROP)` auf und prüft `DragQueryFileW` für Anzahl (3), reelle Unicode-Dateipfade, Zeilenreihenfolge und bestehende Dateien. Negativtests: Fremddatei verhindert Annahme; Fehlerbereinigung zerstört keine fremden Dateien. Die Testinstrumentierung besitzt wieder nur einen gemeinsamen `doDrag`-Anker für MIDI und WAV.
- **Schutz gegen ungültige Dateien:** nur reale, lesbare MIDI-Dateien mit bekanntem Zeilennamen werden an Windows übergeben; `CF_HDROP` enthält keine Ordner.
- **Aufbewahrung:** Drag-Dateien werden für asynchronen DAW-Import vorgehalten; eigene alte Temp-Dateien können nach 14 Tagen vorsichtig bereinigt werden.

## Prüfumfang und Grenzen

| Funktion | Nachweis | Status |
| --- | --- | --- |
| MIDI Save As: getrennte Dateien je genutzter Zeile | F03 in `MidiExportTests`, nativer Controller-/Dateisystemtest | PASS |
| MIDI Drag: gemeinsame reale Windows-Mehrdateienübergabe | F06 über VSTGUI-`CDropSource` und `Win32DataObject(CF_HDROP)` | PASS |
| WAV Drag als Einzeldatei | alter Pfad beibehalten; vorhandener Testanker für den gemeinsamen OLE-Aufruf | PASS in Windows-QA |
| SW05 Swing = 2:1 bei 50 % | unabhängige Taktbruchteil-Orakel, MIDI/WAV/VST3-PCM | PASS |
| BC64 Complexity Schrittweite 10 | gemeinsame Funktion in GUI und Unit-Test, 0/100-Grenzen | PASS |
| Studio-One-Import von mehreren Dateipfaden in passende Clips/Spuren | Besitzer-DAW / tatsächliche GUI-Laufzeit | NOT RUN |
| Weitergaberechte aller eingebetteten Kitsamples | Rechteklärung durch Herausgeber | NOT VERIFIED |

## Private RC3-ZIP

Datei `125A_Drumator_v1.5.0_RC3_MIDI_MultiDrag_StudioOne_Test.zip` ist aus dem echten GitHub-Windows-Build-Artefakt `125A-Drumator-M11-Full-Kit-Stress-QA` mit ID `13365826473` hergestellt. 25 Archivdateien einschließlich kompletter VST3-Ressourcen, Besitzerpreset und korrigierten Handbüchern DE/EN. **SHA-256 der ZIP:** `879406474e68400052dc3c5e28357c71c1cf494caaa5928f62c1daa5a289a67d`. **Preset-SHA256:** `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`. Vollständige SHA256SUMS.txt liegt im Archiv; jedes Archivmitglied wurde erneut gelesen und geprüft.

**RC3 ist eine private Testfassung.** Kein FINAL oder Gumroad-Release ohne erfolgreiche echte Host-Abnahme, Rechteprüfung und Eigentümerfreigabe. `main` und die bestehende Verkaufsversion 1.0.1 sind unverändert.
