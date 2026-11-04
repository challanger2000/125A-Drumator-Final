# Drumator V1.5 – unabhängiger, produktcode-schonender Release-Audit

**Datum:** 04.11.2026 · **Status:** Release HOLD (noch keine Verkaufsfreigabe).  
**Verifiziertes Produkt:** `7736fe91090283e43c89b97b627bb872212b86b9` – [vollständig erfolgreicher Windows-Lauf #41921721850](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41921721850).  
**Audit-Zusatz:** Tests-only `a1f50c7` + `d1b297e`, neue OLE-Gegenprobe AUD-F07, [Lauf #41923917904](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41923917904), **SUCCESS: 33/33 CTests einschließlich AUD-F07, Steinberg 47/47, unabhängiger Plugin Tester 58 PASS / 0 WARNING / 0 FAIL**. `src/` gegenüber erfolgreichem Produktstand **unverändert**.

## Verifizierte Gates

1. **33/33 CTests PASS**, inklusive Core-Swing, MIDI-Export, WAV-Export, Beat Creator, Zustandssicherung, Kit, Undo, Velocity, SongChain und Windows/VSTGUI-Integration.
2. **Steinberg VST3 Validator 47/47 PASS**, reale paketierte Windows-x64-DLL.
3. **125A Plugin Tester 0.3.2: 58 PASS / 0 WARNING / 0 FAIL**. Echte VST3-Instanziierung und zahlreiche 32-/64-Bit-Prozessprofile; unterstützte Raten 44,1 bis 192 kHz und Blöcke 1–8192 werden soweit möglich geprüft.
4. Der GitHub-Job prüft **echtes paketiertes VST3-PCM**, Stereo-Multi-Out-Busse und native GUI-Ressourcen. Host-Simulation und Offline-Lastmessung sind **nicht** mit einer echten ASIO-Studio-One-Sitzung gleichzusetzen.

## Detaillierte Quell- und Testprüfung

| Bereich | Quell-/Testnachweis | Ergebnis / Einschränkung |
| --- | --- | --- |
| **Swing SW05** | `SequencerCore.h`, `CoreTests.cpp`, `MidiExportTests.cpp`, `OfflineWavExportTests.cpp`, `ExportTempoAudit.inl` | 50 % = 2:1-Achtel: bei 120 BPM/48 kHz Offbeat bei Frame **16000** statt gerade **12000**. Viertel bleiben gerade. Auch am VST3-Ausgang geprüft. |
| **Complexity BC65** | `UiBeatCreatorDialog.h` und `UiV101LayoutTests.cpp` | Explizite Nutzerentscheidung: 35 % Start, **+/−5**, freier 0–100-Wert und Grenzbegrenzung. Keine 10er-Schritte. |
| **Crash BC59** | `BeatCreator.h` seit Commit `9d2eea4e` byteidentisch; `BeatCreatorTests.cpp` | 10 Styles, 8 Motive, 16/32 Steps; seltene Crash-Akzente in A/B/C; Frequenz in Tests begrenzt. |
| **MIDI-Export & Drag** | `MidiExport.h`, `DrumatorController.cpp`, `DrumatorView.cpp` | ursprüngliche Architektur: **eine SMF-Typ-1-Datei**, 960 PPQ, Tempo-Track plus 16 Instrumentenspuren; sieben Dateien exakt auf den geprüften Vor-Umbau-Stand zurückgesetzt. |
| **AUD-F07 Original-Drag** | `tests/integration/WindowsIntegration.cpp` | Neuer **nur testseitiger** Native-View-Test löst Pattern-/Song-Drag bei echtem Host-Tempo aus, prüft OLE-`CF_HDROP`, einzelne Datei, SMF-Header, Dateilebensdauer. **AUD-F07 PASSED** im echten Windows-VSTGUI-Test: Original-Drag mit jeweils einer 17-spurigen MIDI-Datei per `CF_HDROP`. |
| **WAV/Song/REST/Tempo** | `OfflineWavExport.h`, `SongChain.h`, `ExportTempoAudit.inl` | 48 kHz/24-Bit PCM, korrekte Taktlängen, **44** Drag-vs.-Live-VST3-Vergleiche, Stale-BPM-Negativkontrolle und transaktionale Dateiausgabe. |
| **Transport, MIDI, Recall** | `PackagedAudioHostSmoke.cpp`, `WindowsIntegration.cpp` | Song A/B/A, Cycle-Wraps, Locate/Stop, wechselndes Tempo und Blockgrößen, Automations-/MIDI-Arbitration, Replay, zwei Fenster, Restore/OOM. |
| **GUI** | `UiV101NativePainter.h`; native Windows-Pixeltests | Wortmarke auf `kTitleReduction=0.64` (20 % unter vorherigen 0,80); gemeinsame MAIN/SONG-Footer-Gravur „VERSION 1.5“. |
| **Kundenbeilage** | geprüfte frühere Preset-/Paketstruktur | Besitzerpreset `125A Drumator Kit.vstpreset` mit 8 WAVs, SHA-256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`. **RC3-ZIP ist ungültig** und darf nicht als aktueller Release-Kandidat verteilt werden. |

## Eigene Befunde

- **Kein weiterer nachgewiesener DSP-/Produktcode-Releaseblocker** bei diesen Quell- und Testabgleichen. Das ist kein Beweis der Fehlerfreiheit und keine vollständige Zeile-für-Zeile-Inspektion sämtlicher Dateien.
- **Bestätigter Prozess-/Dokumentationsmangel:** Die alte `CURRENT.md` enthielt ungültige „aktuelle“ RC3-Multi-Drag- und 10er-Complexity-Aussagen; die verbindlichen ersten Absätze/Übergabe wurden konkret berichtigt, ohne eine Funktion zu verändern.
- **Offene Beweisgrenzen:** Kein echtes Studio-One-/ASIO-Projekt in GitHub Actions, kein genereller Hardware-/Treiber-Echtzeitbeweis; Distribution des eingebetteten Beispielkits setzt entsprechend geklärte Weitergaberechte voraus.
- **Keine eigenmächtige Produktänderung aus einem Auditbefund ableiten.** User muss keine technischen Fehler in der DAW suchen. Freigabe nur auf exakt geprüftem Build/ZIP und nach expliziter Besitzerentscheidung.

**Releaseentscheidung:** **HOLD ausschließlich für noch fehlendes aktuelles Paket/Vertriebsfreigabe**; AUD-F07 ist PASS. Für die finale Distribution sind ein konsistentes Paket mit korrekten DE-/EN-Handbüchern und Besitzerkit nachgewiesen sind; dann kann der technisch geprüfte private V1.5-Kandidat bereitgestellt werden, ohne den Nutzer als Fehlersucher einzuspannen.
