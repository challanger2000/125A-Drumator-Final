# WAV-/MIDI-Export: Tempo und tatsächliche PCM-Zeitachse

Prüfdatum: 02.11.2026, UTC. Repository: `challanger2000/125A-Drumator-Final`.
Arbeitsbranch: `develop/beat-creator-10-styles-20261102`.

## Ergebnis und Beweisgrenze

**Ein Fehler im gemeinsamen Exportweg ist reproduziert:** Nach einer fehlgeschlagenen Tempoübertragung konnte der Controller mit einem älteren BPM-Wert weiter exportieren. Der Offline-Renderer berechnet dann folgerichtig eine falsche PCM-Zeitachse. Die Live-Wiedergabe des Processors kann gleichzeitig dem aktuellen Hosttempo folgen. WAV und MIDI waren beide betroffen.

**Das beweist noch nicht die eindeutige Ursache der gelöschten WAV des Benutzers.** Die Originaldatei ist nicht mehr vorhanden; `Plugin.flac`, `Export.flac` und das verwendete neue Factory-Kit waren im zugänglichen Workspace nicht verfügbar. Die beschriebenen FLACs sind spätere Studio-One-Songexporte und dürfen ohnehin nicht als ursprüngliche Drag-WAV behandelt werden. Keine aus diesen Dateien erfundenen Messwerte, kein Studio-One-/Winamp-PASS. Factory-Kit-spezifische PCM-Prüfung: NICHT DURCHGEFÜHRT; die automatischen Timingtests verwenden ausdrücklich synthetische, kontrollierbare Testsignale.

Bei erfolgreich übertragenem Tempo ergab bereits der unveränderte Produktcode im echten nativen Drag-Export dieselben PCM-Schlagpositionen wie der Live-Processor. Eine genrespezifische Clock, eine falsche Halbierung/Verdoppelung oder eine falsche 48-kHz-WAV-Zeitbasis ist dadurch für diese getesteten Fälle nicht nachgewiesen. Die Step-Formel und die Genre-/OHH-Vorlagen wurden deshalb nicht geändert.

## Exakter Quellstand und Release-Abgrenzung

| Gegenstand | Geprüfte Herkunft |
|---|---|
| Ausgangspunkt | `47479afdc91ae72d7a9b65ec3c53c6b50604a91f` (PERF03); vorhandener Windows-Lauf [41529747916](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41529747916) SUCCESS |
| Reproduktion, nur neue Tests | `2b1135a9207145b4651d993e2fb19aae2dc11209`; [Windows 41536888178](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41536888178), erwarteter Regressionstest FAIL |
| Reparatur und erweiterte QA (Lauf wegen Testharness-OOM rot) | `302edd9cec8e0c958d91949d71b896052e3e82af`; [Windows 41538729451](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41538729451); siehe tatsächliche Ergebnisse unten |
| Korrigierter Testharness, gleicher Produktcode | `3508b6ac3c6a86c46698f39f77bb1c4e8529f366`; [Windows 41539808439](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41539808439); siehe tatsächliche Ergebnisse unten |
| Verbindliche Engineering-Regeln | `125A-Engineering` `2e88454b895ca97d7c8c416e849d68a08a3a6c14`, `START-HERE.md`, relevante Standards/QA/Guardrails; lokale Projektregeln und `CURRENT.md` gelesen |
| Bestehendes freigegebenes FULL | Version 1.0.1, Produktcode `3748b1e9e1867357362304c51bec2cb99ac9657b`, Main-Dokumentation `f4c5caa4be82626ea89459d9835c12138d7c6a1b`, Windows 41157069202; unverändert |
| Existierendes FULL-Kundenpaket | `125A_Drumator_v1.0.1_Gumroad_Final.zip`, SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`; unverändert |
| Demo-Abgrenzung | Dieser Auftrag verwendet den vom FULL abstammenden Beat-Creator-Branch, keinen Demo-Branch und keinen Demo-Build. Keine Lizenzänderung, keine Versionsänderung, kein Release-/Gumroad-Upload. Eine neue Demo-Paketprüfung ist nicht Gegenstand dieses Auftrags. |

## Vollständiger untersuchter Drag-Pfad

1. `src/vst3/DrumatorView.cpp:3058` ff. und `:3671` ff.: EXPORT-Popup, Mausdruck, Drag-Schwelle, Auswahl Pattern/Song und MIDI/WAV.
2. `src/vst3/DrumatorView.cpp:2789`: `startExportDrag()`. Temporären Zielpfad erzeugen, Controllerexport aufrufen, fertige Datei prüfen, UTF-8-/NUL-terminierten `kFilePath` in `CDropSource` verpacken, anschließend `doDrag()`.
3. `src/core/DragExportFile.h:82`: eigener exklusiver temporärer Ordner je Drag; kein anderer Audioencoder. Fertige Dateien bleiben für verzögerten Hostimport erhalten. Bestehende Aufbewahrungslogik unverändert.
4. `src/vst3/DrumatorController.cpp:1919` und `:1939`: `exportMidi()` / `exportWav()`. Beide verwenden den durch dieselbe Hostabfrage bestätigten BPM-Wert. WAV übernimmt die vorhandenen PCM-/Edit-/Pattern-/Mixdaten; MIDI übernimmt dasselbe Pattern/Song mit musikalischen Ticks.
5. `src/vst3/DrumatorProcessor.cpp:151`: `MeterRequest` → `MeterSnapshot`, einschließlich BPM und Beobachtungs-/Anfragekennung. Tatsächlich gelieferter `ProcessContext::tempo` wird in `process()` bei `:1080` erfasst.
6. `src/core/OfflineWavExport.h:70`: `samplesPerStep = 15 × 48000 / bpm`; `:142` ff. PPQ/Clock/StepScheduler und tatsächliche SampleVoiceEngine; `:31` ff. 48-kHz-/24-Bit-/Stereo-Header; `:164` ff. PCM-Packing und bestehender finaler 5-ms-Fade.
7. `src/core/MidiExport.h:121`: Die interne 120-BPM-Clock dient der Sample-/Tick-Umrechnung auf 960 PPQ. Sie ist keine zweite Exporttempoquelle. `:134` schreibt `round(60000000 / hostBpm)` als MIDI-Tempometadatum.
8. Gepinntes VSTGUI: `vstgui/lib/platform/win32/win32dragging.cpp:424` ff. erzeugt `CF_HDROP`/`DROPFILES` aus Dateipfaden. Dieser SDK-Pfad übergibt den Dateinamen; er rendert oder timestretcht keine PCM-Daten.

Alle Positionen beziehen sich auf Reparaturcommit `302edd9`; die maßgeblichen Funktionen sind zusätzlich genannt, damit spätere Zeilenverschiebungen die Nachprüfung nicht verhindern.

## Nachgewiesene Ursache und Korrektur

**HIGH – veraltetes Tempo wurde als verwendbarer Exportzustand behandelt.** Im Ausgangsstand hielt der Controller `exportTempoBpm_{120.0}`. `requestMeterSnapshot()` ignorierte das Ergebnis von `sendMessage()`; fehlende/ungültige Tempoattribute und eine ausbleibende Antwort beließen den bisherigen Wert. `exportWav()` und `exportMidi()` verwendeten diesen Cache ohne aktuelle Abfrage oder Gültigkeitsprüfung. Im Processor wurde Exporttempo erst hinter Zero-Frame-/Outputless-Rückgaben aktualisiert. Recall und Reconfiguration machten die vorhandene Tempoannahme nicht ungültig.

Reproduktion am echten Controller/Processor: Zunächst 90 BPM verarbeiten und bis zum Controller übertragen. Anschließend tatsächliche Audioverarbeitung auf 120 BPM umstellen, aber Controller→Processor-Nachrichtenverbindung gezielt unterbrechen. Eine erneute Meterabfrage scheitert. Der alte Produktcode akzeptiert dennoch den WAV-Export und überschreibt eine vorhandene Testdatei. Der Windows-Regressionstest scheitert genau an dieser Akzeptanz, nicht an einem Compilerproblem.

Die Reparatur:

- Kein angenommener 120-BPM-Startwert mehr in Controller/Processor. Vor Kenntnis des Hosttempos wird kein Export erzeugt.
- Gültiges, tatsächlich vom Host geliefertes Tempo wird vor den Zero-Frame-/Outputless-Rückgaben erfasst, auch bei Stop. Die Live-Clock-Fortschreibung bleibt unverändert und wird nicht als neue Hostbeobachtung ausgegeben.
- Processor publiziert BPM plus Beobachtungsgeneration kohärent über atomare Felder. Audio-/Restore-Schreiber werden durch den bestehenden Restore-Gate-Vertrag getrennt; der UI-Leser versucht maximal drei Momentaufnahmen. Keine neue Audioallokation, Dateioperation, blockierende Sperre, UI-Nachricht oder Audio-Warteschleife.
- Anfragekennungen verhindern die Übernahme überholter oder bereits vor einem Recall ausgegebener Antworten. Fehlgeschlagene Abfragen und ungültige Tempoangaben machen den Export unbenutzbar.
- WAV/MIDI fragen vor dem Export erneut ab. Bereits bestätigte Antworten können bei asynchroner Hostvermittlung verwendet werden; auf einen synchronen Callback allein wird nicht bestanden.
- Nur eine neue tatsächliche Processor-Tempobeobachtung erneuert eine **3-Sekunden-Gültigkeitsfrist** auf dem Controllerthread. Gleichbleibende Meterantworten allein halten einen alten Hostkontext nicht unbegrenzt gültig. Diese Frist ist eine explizite Fehlerbehandlungspolitik, kein Nachweis einer garantierten Host-Latenz.
- Erfolgreicher Recall / Setup invalidieren den alten Hosttempobezug; abgelehnter Recall lässt den bisherigen gültigen Zustand unverändert.
- Ohne bestätigtes Tempo werden WAV/MIDI vor dem Öffnen/Ersetzen der Zieldatei abgelehnt. Der reale Drag-Pfad meldet: `EXPORT: HOST TEMPO UNAVAILABLE - PLAY HOST AND RETRY`.

**Praktische Auswirkung:** Liefert ein Host nach Stop länger als drei Sekunden keinen neuen gültigen Tempokontext, muss die Wiedergabe kurz gestartet werden. Das ist eine bewusst sichtbare Ablehnung statt einer möglicherweise falsch getakteten WAV. Eine kurzfristige Tempoänderung bei asynchroner Nachrichtenvermittlung kann weiterhin nur mit der neuesten tatsächlich zugestellten Hostbeobachtung bekannt sein; kein Plugin kann einen vom Host noch nicht gelieferten Wert messen. Große Songs werden weiterhin synchron auf dem Controllerthread gerendert; dieser Auftrag macht daraus keinen asynchronen Export.

## Tests, unabhängige Erwartungen und Negativkontrollen

- `tests/ExportTimingOracle.h`: unabhängiger RIFF-/PCM24-/MIDI-Parser. Erwartungen aus BPM und Stepnummer, ohne StepScheduler im Erwartungspfad. Prüft RIFF-Größe, PCM-Format, 48000 Hz, ByteRate 288000, BlockAlign 6, exakte Datenlänge sowie jeden Stereo-PCM-Frame, einschließlich erwarteter Nullintervalle. PCM-/Float-Vergleiche erlauben einen absoluten Fehler unter `2e-6` pro Kanal für Quantisierung/Float-Rechnung; dies ist keine Behauptung bitidentischer Float- und PCM24-Daten.
- `tests/ExportTimingTests.cpp`: **200 tatsächliche WAV/MIDI-Fälle**: 10 Styles × 16/32 Steps × 90/100/120/128/140 BPM × Pattern/Song. Echte Beat-Creator-A/B-Patterns (Complexity35, Seed42 je Style; keine Behauptung einer vollständigen 80-Motiv-/Seed-Matrix) mit unterscheidbaren synthetischen PCM-Impulsen. MIDI-Notenanzahl, Zeile, Velocity, Tickposition und Tempometadatum werden separat geprüft. Zusätzlich wird eine tatsächlich bei halbem BPM gerenderte Datei als Negativkontrolle vermessen.
- `tests/integration/ExportTempoAudit.inl`: **44 echte native Drag-/Live-Vergleiche**. 20 Fälle mit allen 10 Styles bei 120 BPM/16+32 Steps; 20 Viertelrasterfälle mit Pattern/Song und fünf Tempi; vier weitere Pattern-/Songfälle mit Swing, Humanize, Pitch +12/−12, Panorama, Gain, Choke, Limiter und stummer Zeile. Jede PCM-Position der über den tatsächlichen View erzeugten temporären WAV wird gegen den echten VST3-Processor verglichen.
- Der native Drag-Test öffnet den wirklichen VSTGUI-Editor am Windows-HWND, betätigt das reale Export-Popup und überschreitet per Mausbewegung die Drag-Schwelle. Nur die letzte OS-/OLE-Übergabe wird in der generierten QA-Quellkopie beobachtet. Der Test liest den tatsächlich erzeugten Dateipfad aus dem realen `CDropSource`-Paket und misst diese Datei **vor** einem DAW-Import. Das ist ausdrücklich kein Studio-One-Drop-/Time-Stretch-Test.
- Derselbe Fehlerassert wie vor der Reparatur bleibt erhalten. Eine QA-only-Mutation stellt die alte fehlende Invalidierung und die fehlende Exportprüfung wieder her. Sie muss tatsächlich die falsche WAV mit verschobenen Schlägen erzeugen und am unveränderten Akzeptanzassert scheitern. Die Mutation befindet sich ausschließlich in generierten Testquellen, niemals im ausgelieferten Plugin.
- Weitere reale Pfade: anfänglich unbekanntes Tempo, Zero-Frame-Stop, Stop/Play, deaktivierte Ausgänge, ablaufende Hostbeobachtung trotz Meterantworten, vertauschte/delayed Replies, ungültige Tempodaten, Projektwechsel mit verspäteter alter Antwort, abgelehnter Recall, Nachrichtenallokationsfehler und tatsächliche native Drag-Fehleranzeige.
- Die älteren Export-/Limiter-/Legacy-Recalltests erhalten jetzt ausdrücklich einen echten Host-Tempokontext über `process()`; ihre bisherigen Audio-/Dateiasserts wurden nicht abgeschwächt. Tests dürfen nicht selbst von einem impliziten 120-BPM-Fallback abhängen.

Die bisherigen Kern-Exporttests übergaben dem Renderer direkt 120 BPM; der ältere verbundene Exporttest verwendete den identischen Controller-Standardwert. Damit prüften sie ein korrektes Rendering bei richtigem Argument, aber keine fehlerhafte BPM-Übertragung, keine Ablaufzeit und keine bereits erzeugte Drag-Datei. Ein grüner solcher Test konnte den Fehler deshalb nicht erkennen.

## Tatsächlich ausgeführte Windows-QA

**Vorher:** [41536888178](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41536888178), Commit `2b1135a`. Build erfolgreich. Nativer Test `EXPTEMPO connected host BPM and every live/export PCM frame` PASS. `EXPTEMPO failed transfer must preserve destination` FAIL mit `failed tempo IPC accepted a stale-BPM WAV and overwrote destination`. Validator/Tester dieses roten Laufs wurden nicht als bestanden gezählt.

**Erster Reparaturlauf:** [41538729451](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41538729451), Commit `302edd9`. Kompilation erfolgreich, alle 44 tatsächlichen Drag-/Live-Vergleiche PASS, failed-transfer PASS, Mutation PASS. Der zusätzliche Lifecycle-Test FAIL mit `bad allocation`: `path.string()` wurde im Testaufruf nach Aktivierung der Allokationsfehlerinjektion ausgeführt und warf schon vor Eintritt in die Produktfunktion. Dieser Fehler des neuen Testharness wurde in `3508b6a` korrigiert: Argument vorher materialisieren; derselbe Produktpfad, dieselbe Fehlerinjektion und dieselben Assertions bleiben erhalten. Kein weiterer Produktcode wurde dafür geändert. Der Lauf ist insgesamt FAIL und wird nicht als bestandene Voll-QA gezählt.

**Abschließender Lauf: SUCCESS.** [41539808439](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41539808439) für exakt `3508b6ac3c6a86c46698f39f77bb1c4e8529f366`. Beide Jobs und alle erforderlichen Schritte sind laut GitHub-API erfolgreich abgeschlossen:

| Prüfung | Tatsächlicher Nachweis | Ergebnis |
|---|---|---|
| Windows x64 Kompilation | Job125600782143, Build-Schritt success; keine Linux-Builds | PASS |
| Gesamter CTest-Lauf | Ungefilterter CTest-Aufruf, Exit0; 32 projektspezifische Tests registriert, einschließlich neuer 200-Fälle-Matrix und nativer44-Matrix, Negativkontrolle, Lifecycle-/OOM-Test | PASS |
| Steinberg Validator | Checks-Annotation: `Result: 47 tests passed, 0 tests failed` | PASS |
| Paketierte Editor-/Audiohosttests | Erfolgreicher Assemble/Validate-Schritt; echte DLL-HWND-/Zoomtests und Audiohost-Smoke-PASS in Checks-API | PASS |
| Projektzustand/Routing/Restore | Paketierte T01/T02/T04/T28/T29/T30-PASS-Annotationen; T02 mit AUX-/L-R-Negativkontrollen | PASS für die automatisiert geprüften Fälle |
| Unabhängiger 125A Plugin Tester v0.3.2 | Job125604060944: Provenienzprüfung, Download des aktuellen Artefakts, gepinnter Tester, Ausführung und Reportprüfung success. Workflow erzwingt Exit0, JSON-PASS, 0WARNING/0FAIL | PASS |
| Studio One 8.1.2 / ASIO / Winamp | Keine tatsächliche Sitzung hier ausgeführt | NICHT DURCHGEFÜHRT |
| Verwendetes neues Factory-Kit | Nicht im zugänglichen Material vorhanden | NICHT DURCHGEFÜHRT |

Die Tester-TXT/JSON-Datei und der individuelle DLL-SHA konnten aufgrund des blockierten Artefaktdownloads hier nicht zusätzlich gelesen werden. Deshalb wird keine unbestätigte Tester-PASS-Anzahl oder ein erfundener DLL-Hash angegeben; die erfolgreiche automatisierte Reportprüfung und die Artefaktprovenienz sind verifiziert.

Automatischer Performance-Nebenbefund aus demselben Lauf: F14, 64Stimmen/16x, 48kHz/256, 180Blöcke: p95 `1.2445ms`, p99 `1.3188ms`, Max `1.7801ms`, 0/180 Deadlineüberschreitungen gegenüber `5.33333ms`. Dies ist ein nativer CI-/Durchsatzbefund und keine Echtzeitgarantie oder Studio-One-Aussetzerfreiheitsbescheinigung.

Insgesamt wurden drei begründete neue Windows-Läufe ausgelöst: ursprüngliche rote Reproduktion; Reparatur mit anschließend erkanntem Testharnessfehler; korrigierter Testharness und vollständige grüne QA. Keine blinden Wiederholungs-/Triggercommits, keine Versions-/Releaseänderung.

Die vollständigen offiziellen Logdownloads werden in dieser Cloud vom Proxy mit HTTP 403 abgelehnt. Tatsächliche Resultate werden deshalb zusätzlich aus der GitHub-Checks-API und den gesondert archivierten Messdateien verifiziert. Eine nicht zugängliche Logdatei wird nicht als gelesen dargestellt.

## PCM-Sollwerte und gesicherte Messergebnisse

Der native Windows-Lauf `41538729451` meldet bereits die tatsächlich erzeugten 120-BPM-Dateien mit `96000`, `192000` und `384000` PCM-Frames sowie identische Live-/Export-PCM-Werte an jeder Position. Die 44-Fälle-Matrix einschließlich fünf Tempi ist PASS. Die folgende Tabelle nennt die unabhängigen Sollwerte, die in diesen tatsächlichen Dateitests strikt geprüft werden. Sie stammt nicht aus einer Untersuchung der gelöschten Benutzer-WAV.

Gemessene Negativkontrolle: Host120 / absichtlich wiederhergestellter Controller-Cache90 → tatsächlich `128000` Frames (`2.666667 s`) für 16 Steps, zweiter Viertelschlag bei Frame `32000` statt `24000`. Der alte Fehler wird damit vor dem DAW-Import hörbar in die PCM-Datei geschrieben. Nach Reparatur wird genau dieser Fehlerpfad abgelehnt und das bestehende Ziel bleibt unverändert.

Das zusätzliche Timing-Artefakt des ersten Reparaturlaufs ist [hier verfügbar](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41538729451/artifacts/13185189066). Der Cloud-Proxy blockierte auch dessen lokalen Download (HTTP403). Die Dateien wurden daher nicht zusätzlich hier unter Linux analysiert; die Windows-Testparser, die Checks-API und die archivierten Messwerte sind die Nachweise. Die absichtlich falsche Datei ist dort ausdrücklich `negative-control-stale90-actual120.wav` benannt.

| BPM | 16 Steps: Frames / Sekunden | 32 Steps: Frames / Sekunden | Viertelabstand bei 48 kHz |
|---:|---:|---:|---:|
| 90 | 128000 / 2.666667 | 256000 / 5.333333 | 32000 Frames |
| 100 | 115200 / 2.400000 | 230400 / 4.800000 | 28800 Frames |
| 120 | 96000 / 2.000000 | 192000 / 4.000000 | 24000 Frames |
| 128 | 90000 / 1.875000 | 180000 / 3.750000 | 22500 Frames |
| 140 | 82286 / 1.714292 | 164571 / 3.428563 | 20571.428571 Frames; Onsets samplegerundet |

Bei 120 BPM: 16 Steps → Viertel-Onsets `0, 24000, 48000, 72000`; 32 Steps zusätzlich `96000, 120000, 144000, 168000`. Vier Songtakte → `384000` Frames / `8.000000` Sekunden. Enddauer wird auf den nächsten PCM-Frame gerundet; Onsets werden entsprechend der vorhandenen Schedulerkonvention auf die erste nicht frühere Sampleposition gelegt. Diese beiden Rundungsaufgaben sind zu unterscheiden.

## Installierbares Artefakt und vollständige Messdateien

Alle folgenden Artefakte gehören laut GitHub-API exakt zu Commit `3508b6ac3c6a86c46698f39f77bb1c4e8529f366` und dem abgeschlossenen Windows-Lauf `41539808439`:

- [Installierbares Windows-x64-VST3-ZIP](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41539808439/artifacts/13187250555): Artefakt `125A-Drumator-M11-Full-Kit-Stress-QA`, `10072913`Bytes. GitHub-Artefakt-Digest `sha256:000e525a73bcd090666040d9ea87bba98e37a61b16bba642827c6bc3eec69f87`. Dies ist der Archiv-Digest, kein DLL-Hash. Enthält den regulären Bundlebaum `125A Drumator.vst3/Contents/x86_64-win/125A Drumator.vst3` plus Resources. Das komplette äußere `.vst3`-Verzeichnis installieren, nicht nur die innere Binärdatei. Interner Testbuild, kein Ersatz des freigegebenen Gumroad-Pakets.
- [Tatsächliche Pre-Import-WAVs und CSV](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41539808439/artifacts/13187101063): `125A-Drumator-Export-Timing-Evidence`, `7811`Bytes, Digest `sha256:e28958af8e460f23daf9c9af1aa5a92f3af2945bf0c00a9881901a08f42bbf14`. Vier positive120-BPM-Pattern-/Song-WAVs und `live-versus-temp-wav.csv` mit44 Fällen. **Die zusätzlich enthaltene `negative-control-stale90-actual120.wav` ist absichtlich falsch** und demonstriert ausschließlich den erkannten alten Defekt.
- [Unabhängiger Testerbericht und candidate-provenance.json](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41539808439/artifacts/13185264920): `125A-Drumator-Independent-Plugin-Tester-v0.3.2`, `9892`Bytes; Digest `sha256:837caf1d67b38f99fd2add8c50107ac2631277ba7d720339614ea2db18a437be`.

Die Downloadlinks benötigen gegebenenfalls die GitHub-Anmeldung. Die Cloud konnte die externen Artefakt-Blobs nicht selbst herunterladen; Archivgröße, Digest und Source-SHA wurden über die offizielle GitHub-API gelesen. Nicht mit einer hier ausgeführten lokalen ZIP-/DLL-Prüfung verwechseln.

## Zweiter Fehlerbereich: DAW-Import / Time-Stretch

**NICHT NACHGEWIESEN:** Welche File-Tempo-Angabe, Stretch-Einstellung oder Erkennung Studio One 8.1.2 beim tatsächlichen Einfügen verwendet. Hier steht keine echte Studio-One-/ASIO-Sitzung zur Verfügung. Der spätere Songexport ist nicht die primäre Fehlerstelle und wurde nicht als Ersatz für die ursprüngliche Drag-WAV analysiert.

Der Export schreibt einen regulären PCM-RIFF-Header mit `fmt ` und `data`, keine `acid`-/`smpl`-/BWF-Looptempoinformation. Metadaten ändern von sich aus keine PCM-Abstände. Das bloße Fehlen dieser Metadaten ist kein Nachweis dafür, dass Winamp oder Studio One die konkrete Datei verlangsamt. Eine unbewiesene Loop-Metadatenvariante wurde deshalb nicht hinzugefügt. Ein zukünftiger solcher Zusatz benötigt eine verifizierte Formatspezifikation und echte Import-Negativkontrollen, um nicht selbst eine falsche Streckung auszulösen.

Noch notwendiger Studio-One-Test am identischen Windows-Artefakt: Live bei konstant 120 BPM laufen lassen; Pattern16, Pattern32 und Song4Takte per realem Drag einfügen; den unveränderten temporären Quelldateipfad sichern und seine PCM-Onsets mit dem importierten Event vergleichen. Importiertes File-Tempo, Stretch-Modus und Stretch-Faktor separat erfassen; denselben Quellclip einmal ohne automatische Streckung vergleichen. Danach Projekt schließen/öffnen und Medienabhängigkeit/Loopgrenzen prüfen. Keine erneute Benutzervermessung ist Voraussetzung für die hier ausgeführte automatische Reproduktion; dieser verbleibende Hosttest ist eine gesonderte Freigabegrenze.

## Guardrail für künftige Exportänderungen

Ein direkt mit120 aufgerufener Encoder-Test beweist keine korrekte Hosttempoübertragung. Jede Änderung am Exportpfad muss zusätzlich eine echte Controller-/Processor-Abfrage mit aktuellem Hostkontext, verzögerte/fehlgeschlagene Übertragung und die bereits erzeugte Drag-Datei prüfen. Keine 120-BPM-Voreinstellung, Genre-Tempoanpassung oder Loop-Metadaten dürfen einen ungültigen BPM-Bezug kaschieren. Test-Argumente vor OOM-Injektion erzeugen, damit der Fehler nachweislich innerhalb der zu prüfenden Produktgrenze liegt.

## Entscheidung

Der kontrolliert reproduzierte Fehler ist im Windows-geprüften Stand technisch korrigiert und durch einen echten vorher roten Test sowie eine relevante Mutation abgesichert. **Die ursprüngliche Benutzerdatei ist nicht eindeutig diagnostiziert; der reale Studio-One-Import bleibt OFFEN. Keine FINAL- oder Releasefreigabe.**
