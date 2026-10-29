# 125A Drumator V1.0.1 — Gesamt-Audit nach dem GUI-Redesign

**Prüfdatum:** 29.10.2026  
**Geprüfter PRODUKT-HEAD:** `e59023351a61e01cbfe80bfe3d01d0aacfaefc06`  
**Repository / Branch:** `challanger2000/125A-Drumator-Final` / `develop/v1.0.1-gui-redesign`  
**Umfang:** Quell- und Testpfade für Host/Audio/MIDI/State, GUI-MAIN/SONG/WAVE, Dateiverwaltung, Ressourcen, Zoom/Hitboxen und finale Windows-VST3-Paketierung. Bestehende historische Auditberichte wurden gegen den aktuellen Quellstand und aktuelle Belege kontrolliert.  
**Audit-Methode:** Read-only-Quellprüfung, vorhandene Regressionen/Orakel kritisch überprüft, aktuelle GitHub-Joblogs nachverfolgt, Release-ZIP programmgesteuert auf Inhalt/Datei-Integrität/Version/Binär-Hash geprüft, reale VSTGUI-Renderings verglichen. **Kein lokaler Windows-/ASIO-Host oder Studio-One-Praxistest**. Keine Produktcodeänderung durch diesen Audit. Kein Anspruch, dass jede mögliche Zeile oder Hostsequenz dynamisch ausgeführt wurde.

## 1. Ergebnis und Freigabegrenze

**Windows-automatisierte technische QA: PASS für den exakten geprüften Produkt-HEAD.**  
**Drumator V1.0.1 FINAL / Studio-One-Praxistauglichkeit: NICHT FREIGEGEBEN; wichtige reale Hostprüfungen NOT RUN.**

- [Windows-Workflow #40811357723](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40811357723): Windows-Job SUCCESS; nachgelagerter unabhängiger Plugin-Tester SUCCESS.
- **24/24 CTests**, einschließlich nativer CView-/Controller-/Mouse-/Menü-/SONG-/WAVE-Matrix.
- **47/47** offizieller Steinberg-Validator, keine fehlgeschlagenen Tests.
- **58 PASS / 0 WARNING / 0 FAIL**, 125A Plugin Tester v0.3.2 am selben aktuellen Build.
- Tatsächliche VST3-Archiv-ID **12870279249**, GUI-Renderings **12870413418**, Testerreport **12870661795**.
- Eigenständig geprüfte Windows-Bundle-ZIP: **16 Einträge, ZIP-Integrität fehlerfrei**, Windows-x64-DLL, vollständige Ressourcen-/Metadatenstruktur.
- Exakte VST3-DLL `Contents/x86_64-win/125A Drumator.vst3`: **SHA-256 `1dad3825950eeeed02e017ca76dca5fce55a6eaec64e893a2c04f0fe83a676e8`**.
- `moduleinfo.json`: Plugin und Controller jeweils **1.0.1**; `CMakeLists.txt` und `PluginIDs.h` ebenfalls **1.0.1**. **Kein aktueller Versionskonflikt in der Binary.**
- Aktuelle GUI-Renderings MAIN/SONG/WAVE bei 100/120/130/150 %, aktueller 6-Zeilen-Footer, Velocity-/SET-Lane, Popup und Langnamen-Prüfung vorhanden. Renderings sind native **Offscreen-/Windows-Hostfixture-Aufnahmen**, **keine Studio-One-Screenshots**.
- Gemessener Offline-F14-Stresstest 64 Stimmen/16×, 48 kHz, 256 Samples: **p95 1,1516 ms, p99 1,2860 ms, max 1,3925 ms, 0/180 Offline-Deadline-Überschreitungen** bei 5,3333 ms Sollzeit auf GitHub-Runner. Nicht als Echtzeit-Zertifizierung unter ASIO interpretieren.

## 2. Produkt- und Kontrollpfade

| Bereich / Vertrag | Belegte Implementierung und Prüfungen | Auditstatus |
|---|---|---|
| MAIN Pattern A–H, 1/2-Bar-Länge, MENU mit 7 Aktionen | `UiV101Layout`, `UiV101Interaction`, `UiV101Popup`, `DrumatorView`, `Controller`, `Processor`; native vollständige Menü- und Hitbox-Matrix sowie echte Pattern-Daten | **PASS Windows** |
| 16 Instrumente; M/S/FILL, Sample-Feld/Navi, PLAY/EDIT; 32 Steps | `UiV101Layout::rowHit`, `UiV101ControllerBridge`, `testV101AllRowMouseIsolationAndSampleBrowse`; Negativ-/Nachbarzeilen-/2.-Seite-Kontrollen | **PASS Windows** |
| VOL/PAN/PITCH, SWING/HUMANIZE/MASTER; 101-Frame-Titanblau-Knob | Gemeinsame Painter-/Hit-Geometrie; Reset-/Drag-/Werteprüfungen, PNG-Ressourcendimension 125×12625; unveränderter Zeiger/Außenring laut Filmstrip-Farbprüfung | **PASS Windows**, **DAW-Haptik NOT RUN** |
| Velocity-Lane und SET: Output/Choke, Copy/Paste/Clear/Random Velocity/Realtime +/-/Drag | `UiV101Interaction::settingsAt`, Controller/PCM/Row-Operationen und native Matrix | **PASS Windows** |
| SONG: 64 Plätze, 16/Seite, A–H/REST, Längen, Start +/-/RESET, ADD/REMOVE, PREV/NEXT | `UiSongLayout`, `UiV101SongPainter`; Song-/Popup-Clickthrough und Prozessor-Timing/Recall-Orakel | **PASS Windows**; Studio-One-Synchronisation NOT RUN |
| WAVE: Zoom+/−, FIT, PREVIEW/STOP, RESET, CANCEL/APPLY; Marker, Fade, Gain | `UiV101WaveLayout`, `DrumatorView::drawV101Wave/onMouseDown`, SampleEditor IPC und native PCM-/Klicktests | **PASS Windows**, reale Dialog-/DAW-Abnahme NOT RUN |
| WAV/FLAC laden, beschädigte Datei, PCM, Sample-Swap, Song/Kit speichern | `WavPcm16Decoder`, `FlacImport`, `KitCodec`, `SampleExchange`, Processor/Controller-State; gepackter Host-Smoke und frische VST3-Instanz | **PASS Windows** für Fixture-Szenarien |
| MIDI/Event-Timing, Mute/Solo/Choke, Stimmen, 16 Stereo-Outs | `TriggerBatch`/4096-MIDI-Inspektionsgrenze, `SampleVoiceEngine`, Sample-offset/Transport/Cycle/Stem-PCM-Tests | **PASS Windows** für geprüfte Lastfälle |
| Realtime-Sicherheit des Normal-Audio-Pfads | `Processor::process` und `SampleVoiceEngine::render`: keine direkt sichtbare Mutex-Sperre, Datei-I/O oder Heap-Allokation im untersuchten Normalblock; bounded 64-Voice/2048-Trigger-Konzept | **Quelle plausibel + Windows-Offline-Tests PASS**, reales ASIO NOT RUN |
| VST3-Editor-Lifecycle, Zoom, Hostresize, zwei Views, IPC, Meter/Clip | Nativer Windows-`CView`/Hostfixture-Test, abgewiesener Resize, Wave-IPC-Proxy-/Recall- und Meter-Tests | **PASS Windows**, echtes Studio One/isolierte Hostprozesse NOT RUN |
| Binärpaket und Pinned Toolchain | SDK 3.8.1, 47/47 Validator, gepinnter `dr_flac`-Hash, bundle Resource-/SHA-Kontrollen | **PASS Windows** |

**Wichtig:** `PASS Windows` bedeutet, dass die konkret genannten automatisierten Prüforakel bestanden sind; nicht, dass jeder natürliche Benutzerablauf in einer DAW erwiesen ist. Der Testkatalog enthält weit mehr als 24 Teilfälle: viele native Integrationsfälle laufen innerhalb eines einzelnen CTests.

## 3. Befunde (nach Priorität)

### DR-A01 — HIGH / RELEASE BLOCKER — realer Studio-One-/ASIO-Abnahmetest fehlt

Reales **Fender Studio Pro / Studio One 8**: Plug-in laden, echte WAV-/FLAC-Dateien per OS-Picker und Drag-and-drop, Abbruch und ungültige Quelle, MIDI, Stop/Play/Cycle, SONG-Start ab BAR 2/4, mehrere echte OUT-Busse, Hot-Swap während Wiedergabe, wiederholt speichern/schließen/neuladen, 100/120/130/150%-Hostzoom und längerer ASIO-Betrieb auf dem Zielrechner sind von den GitHub-Runnern **nicht** nachgewiesen. Auch das reale Verhalten bei getrennten Host-/Plugin-Prozessen bleibt außerhalb der Simulationsnachweise.

**Status NOT RUN; keine FINAL-Freigabe.** Die dafür bereits existierende Checkliste `docs/STUDIO-ONE-REGRESSION-2026-10-22.md` kann als Anfang dienen, muss aber um die zuletzt geänderten GUI-Ressourcen ergänzt werden.

### DR-A02 — MEDIUM / BESTÄTIGTER QA-MANGEL — alte helle Prototypbilder im aktuellen Screenshot-Artefakt

In `tests/integration/WindowsIntegration.cpp:1657–1730` ruft `captureV101Preview` den Painter **ohne** `faceplatePng` und **ohne** `ButtonSprites` auf. Der Painter fällt auf seine prozedurale helle Oberfläche zurück. Derselbe aktuelle Workflow archiviert diese Bilder als `v101-layout-native-prototype-100/130.bmp` neben der dunklen produktiven MAIN-/SONG-/WAVE-GUI; zusätzlich trägt der produktive Header-Screenshot noch den irreführenden Namen `packaged-vst3-resources-silver-100.bmp`.

**Reproduziert** durch Quelle und Öffnen der Artefakte aus Run #40811357723. Ein grüner Bildtest kann eine **nicht aktuelle Skin-Variante** bestätigen. Dies hat in der Sichtkontrolle mehrfach zu falschen Aussagen geführt.

**Abhilfe:** Prototypbilder als **LEGACY/FALLBACK** eindeutig separieren/umbenennen oder dieselben produktiven Bitmap- und Button-Ressourcen an den Test-Painter übergeben. Primärer GUI-Designnachweis nur aus echtem aktuellem View/Bundle.

### DR-A03 — MEDIUM / TESTABDECKUNG — WAVE-Faceplate hat keinen konkreten Pixel-Regressionstest

Seit `DrumatorView.cpp:1192–1230` wird der mittlere Ausschnitt der produktiven Metalloberfläche in das WAVE-Modal gezeichnet; zuletzt wurde die zusätzliche Abdunklung entfernt. `testV101LiveEditor` prüft WAVE-Öffnung, Buttons/Zoom und die Verdunklung **hinter** dem Dialog, aber **nicht**, ob die tatsächliche WAVE-Innenfläche die richtige Originaltextur bei der beabsichtigten Helligkeit enthält.

**Status CODE/SCREENSHOT SICHTBAR, gezielter Negativtest NOT RUN.** Der Test könnte trotz Rückfall auf eine glatte Fläche weiterhin PASS melden. Für den nächsten Änderungszyklus eine feste Pixelregion im Modal gegen die unabhängig gerenderte entsprechende Originaltextur prüfen, mit Negativkontrolle (Bitmap absichtlich fehlend/gestrichen).

### DR-A04 — HIGH-DENSITY-STRESSTEST BESTANDEN (16.384 Punkte); allgemeine O(N)-Grenze bleibt — Automationspunktlisten

`Processor::process` begrenzt die Prüfung von **Host-MIDI-Events auf 4096** und meldet nicht inspizierte Elemente nachvollziehbar. Für `inputParameterChanges`-Queues laufen dagegen Schleifen über `getParameterCount()` und `getPointCount()` ohne entsprechendes explizites Arbeitsbudget, einschließlich Restore- und Outputless-Pfad (`DrumatorProcessor.cpp`, etwa Zeilen 850, 893 und 1143). Der Pakettest belegt einen kombinierten Fall mit **250** Automationspunkten und 8 MIDI-Ereignissen, nicht extreme Parameterqueue-Größen.

**Status QUELL-RISIKO BESTÄTIGT, Host-Stress-Test NOT RUN.** Bei fehlerhaftem oder extrem dichtem Hostinput ist die maximale Workload nicht explizit beschränkt. Erst einen reproduzierbaren großen Automations-Stresstest mit Zeit-/State-Orakel erstellen; dann nötigenfalls bounded Verhalten definieren, **ohne** gültige Automation still zu verschlucken.

### DR-A05 — LOW/MEDIUM / USABILITY — WAVE-END-Auswahl ohne nahe Markerposition nicht offensichtlich

`DrumatorView::onMouseDown` wählt bei leerem Wellenformbereich gewöhnlich **START**, und nur **Shift+Klick** zielt auf **END**, sofern man nicht in die Nähe des vorhandenen END-Markers klickt. Das ist implementiert und automatisiert testbar, aber ein nicht offensichtlicher Workflow für reinen Mausbetrieb. Die Bedienungsanleitung erklärt den Shift-Fall nicht.

**Status UX-Befund, keine nachgewiesene Audiofehlfunktion.** Als Dokumentationsfix oder mit ausdrücklich freigegebenem UX-Design beheben, nicht als spontanen Redesign-Auftrag.

### DR-A06 — LOW / GUI-POLITUR — SONG-Leerfelder sind absichtlich grau statt dunkelblau

`UiV101SongPainter.h:82–89` zeichnet unbesetzte SONG-Kacheln mit `Color{78,81,83,255}` und heller Umrandung; der restliche SONG-Modus verwendet Navy/Graphit. Das ist technisch nachvollziehbar, weicht aber sichtbar vom nun konsistenten dunklen Design ab.

**Status DESIGNENTSCHEIDUNG, kein Funktionsfehler.** Nur nach Benutzerfreigabe angleichen.

### DR-A07 — LOW / DOKUMENTATION — aktuell gebaute Version und Dokument-Einstieg nicht synchron erläutert

Der aktive Entwicklungsstand liefert **1.0.1** in `CMakeLists.txt`, `PluginIDs.h` und `moduleinfo.json` (gleichlautend, korrekt). Dagegen beginnen `CURRENT.md`, `README.md` und `AGENTS.md` noch mit dem **V1.0.0-Releasekandidaten** und enthalten zahlreiche historische „aktueller“-Abschnitte. **Das ist kein Binärversionsfehler**, da sich diese Dateien auf die bis dahin owner-approved Baseline beziehen; ein knapper expliziter Hinweis auf den separat Windows-geprüften **V1.0.1-GUI-Branch**, ohne V1.0.1 als öffentlich released auszugeben, würde Fehlgriffe verhindern.

### DR-A08 — PRODUKTGRENZEN / DOKUMENTATION — explizite V1-Scope-Entscheidungen kommunizieren

`docs/V1-SCOPE.md` begrenzt Song-Taktberechnung auf **4/4** und DAW-Automation auf den **Pattern-A–H-Parameter**. Das ist bewusst, kein neu gefundener Bug. `KitState.h` setzt derzeit **16 MiB je WAV** und **64 MiB gesamte eingebettete WAV-Daten** als Grenzen. Diese Limits gehören klar in die Anwenderanleitung und den Freigabetest; andere Taktarten, vollständige DAW-Automation aller Regler und zeitgestreckte Loops dürfen nicht suggeriert werden.

## 4. Was NICHT gefunden wurde

Bei den geprüften Produktionspfaden und der aktuellen automatisierten Matrix ist **kein reproduzierter neuer DSP-, MIDI-, Absturz-, State-Recall- oder VST3-Vertragsfehler** nachgewiesen. Die große Mehrheit früherer A01/A03-/SONG-/GUI-Matrix-Befunde besitzt konkrete, erfolgreiche Reparaturnachweise. Die obenstehenden QA-Lücken dürfen nicht stillschweigend als Produktdefekte gewertet werden – umgekehrt macht ein grüner Validator sie nicht zu bestandenen Tests.

Aktuelle Ressourcenliste im gebauten Bundle: 8 dunkle Button-Zustände/Auflösungen, ursprüngliche Legacy-Faceplate und -Schraube, 101-Frame-Titanblau-Knob, modernes transparentes DRUMATOR-Wortlogo, aktuelle industrielle Faceplate, UIDESC und moduleinfo. Legacy-Faceplate/Schraube sind weiterhin an den Legacy-Fallback gebunden; nicht unbesehen löschen.

## 5. Nächste Prioritäten für V1.0.1-Freigabe

| Priorität | Aufgabe | Erwarteter Nachweis |
|---|---|---|
| **P0** | Echter Studio-One-/ASIO-Test mit **diesem** VST3 und User-Samples, komplette Save/Reopen-/Audio-/SONG-/Multi-Out-/Zoom-Matrix | Besitzer-Testprotokoll mit PASS/FAIL/NOT RUN je Anforderung; exakte DLL/Version |
| **P1** | Screenshot-Artefakte auf aktuelle Produktion trennen, WAVE-Textur-Oracle hinzufügen | vorab Rotfall, danach grün; korrekte MAIN/SONG/WAVE-Pixelbilder |
| **P1** | Maximalbelastung der Host-Parameterqueues realistisch und adversarial messen | reproduzierbare Callback-Zeit, korrekte letzte Automation, Host-Tie-Priorität |
| **P2** | WAVE-END-Bedienung erklären / ggf. bewusstes UX-Update; SONG-Graukacheln nach Besitzerurteil | Manual-/UI-Abnahme und Maustest |
| **P2** | README/CURRENT für getrennte V1.0.0-Approved-Baseline und V1.0.1-Branch präzisieren; 4/4 und Limits im Handbuch | eindeutige Release-/Scope-Dokumentation |

## 6. Verbindliche Audit-Grenze

- Kein eigenständiger Windows-Prozess oder Studio One / ASIO in dieser Prüfsitzung ausgeführt: **NOT RUN**.
- Keine freigegebene **V1.0.1 FINAL**, keine Behauptung öffentlicher Veröffentlichung, keine Änderung an `main`.
- Dieser Bericht prüft den **exakten** Source-HEAD `e5902335...`; ein späterer Dokumentationscommit verändert diese geprüfte Binärbasis **nicht**. Nach künftigen Produktcode-Änderungen sind neue Nachweise nötig.
- Bei reproduzierten Problemen: erst Quellursache + negatives Regressionstest-Orakel, danach gebündelte Reparatur; keine kosmetischen oder funktionsfremden Änderungen im Audit.

**Auditentscheidung:** **Windows-CI technisch PASS / reale Releasefreigabe BLOCKIERT durch DR-A01; relevante QA-/Realtime-Abdeckungsdefizite DR-A02 bis DR-A04 sind gezielt zu bearbeiten.**

## 7. Nachbesserung und tatsächliche Windows-QA — 29.10.2026

**Korrekturcommit:** [`66d07ba`](https://github.com/challanger2000/125A-Drumator-Final/commit/66d07ba8bdb69d0cf7b286fb6400ca5c432720c1) (Tests, CI-Screenshot-Auswahl, Anleitung, `CURRENT.md`; keine Änderung an DSP/MIDI/GUI-Produktcode).  
**Exakter Prüflauf:** [GitHub Actions #40814316169](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40814316169): **Windows SUCCESS und unabhängiger Plugin-Tester SUCCESS; 24/24 CTests, 47/47 Steinberg-Validator-Tests, 58 PASS / 0 WARNING / 0 FAIL**.

- **DR-A02: gezielte V1.0.1-Prototypverwechslung geschlossen.** Native Fallback-/Prototyp-Bilder werden weiterhin für die Testorakel erzeugt, aber vor der Veröffentlichung der Screenshot-ZIP aus `build/gui-shots/` entfernt. Das neue, selbst heruntergeladene Bildartefakt **12871788006** enthält **kein** `v101-layout-native-prototype-*`, `v101-layout-native-zoom-menu-*` oder `v101-controller-*-lane-*`. Die produktiven MAIN-, SONG- und WAVE-Renderings sind vorhanden; ZIP-Integrität bestätigt. **Rest:** 60 Dateien insgesamt, darunter andere ältere `gui39`/`v2`/SONG-Diagnostikbilder; nicht mit V1.0.1-Design-Screenshots verwechseln.
- **DR-A03: konkrete WAVE-Faceplate-Regression bei 100% geschlossen.** `testV101LiveEditor` prüft jetzt fünf frei sichtbare Metall-Pixel im WAVE-Modal **gegen ein separat gerendertes Original-PNG-Referenzbild** mit unveränderter Helligkeit (RGB-Differenzsumme ≤15). Ein glatter Ersatz oder die zuvor entfernte zusätzliche 40%-Abdunklung würde an mehreren Punkten scheitern. Die volle 24-CTest-Matrix einschließlich dieses neuen Orakels ist bestanden; 120/130/150% haben native Renderings, aber kein identisches 5-Punkt-Orakel.
- **DR-A05/DR-A08: Bedienungsgrenzen dokumentiert.** END-Klick mit Shift, 4/4, Pattern-only-DAW-Automation, 16/64-MiB eingebettete WAV-Grenzen stehen in `docs/BEDIENUNGSANLEITUNG-DE.md`. Keine Änderung an Bedienfunktionen.
- **DR-A04 OPEN:** Für extrem große VST3-Hostautomations-Queues ist weiterhin erst ein valider Grenzfall-/Callback-Zeit-Test erforderlich. Ein willkürliches Abschneiden wäre eine mögliche Automations-Regression und wurde bewusst nicht implementiert.
- **DR-A01 NOT RUN / RELEASE BLOCKER:** echte Studio-One-/ASIO-Interaktion mit genau dieser VST3, Native-Dateidialog, realer Save/Close/Reopen-Recall, 16 DAW-Ausgänge und Zoomfreigabe.

**Ehrliche Fehlchronik:** Der vorangegangene Workflow [#40814030401](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40814030401) endete mit **BUILD FAIL** wegen einer neuen lokalen Testvariable `original` (MSVC C4456, /WX); CTests/Validator/Tester wurden dort **NICHT AUSGEFÜHRT**. Die reine Testvariablen-Umbenennung im Commit `66d07ba` ist mit dem vollständigen SUCCESS-Run #40814316169 gegengeprüft.

**Schluss:** Die zwei konkreten GUI-Test-/Screenshot-Befunde sind jetzt für die beschriebenen automatisierten Windows-Szenarien geschlossen. **Produktcode bleibt unverändert**, **V1.0.1 FINAL weiterhin nicht freigegeben**.

## 8. Abschluss Extremtest Host-Automation – 29.10.2026

**Endgültiger Testcommit:** [`14eee24ae432e8c4b4d39f63ed3f0c88a6aa8bbf`](https://github.com/challanger2000/125A-Drumator-Final/commit/14eee24ae432e8c4b4d39f63ed3f0c88a6aa8bbf) – **nur `tests/PackagedAudioHostSmoke.cpp`, keine Produkt-/DSP-/State-/GUI-Änderung.**

**Exakter Gesamtprüflauf:** [Windows GitHub Actions #40816232717](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40816232717) **SUCCESS (Windows + unabhängiger Tester)**.
- **24/24 CTests** PASS
- **47/47 Steinberg Validator** PASS
- **58 PASS / 0 WARNING / 0 FAIL** im unabhängigen 125A Plugin Tester V0.3.2 gegen die tatsächlich gebaute DLL.
- Verifizierte Windows-x64-VST3-DLL SHA-256: `d18543c50031fccf8933c106896ae076a235be7c94e06c1c8adf76315e3b693c`.

### P16 – tatsächlich ausgeführte Extremfälle

- Die echte **paketierte VST3-DLL** verarbeitet **16.384 Host-Automationspunkte innerhalb eines 256-Sample-Blocks**, **12** Durchgänge; ein unabhängiger Host-Testzähler belegt **196.608 tatsächlich gelesene `getPoint()`-Einträge**.
- Alle Punkte liegen auf Sampleoffset 64 und alternieren Pattern A/B; der **letzte Host-Punkt (B)** muss Vorrang vor einer **gleichzeitigen MIDI-Pattern-A-Note** haben. Die tatsächlich ausgegebene PCM-Audioantwort im Bereich Samples 180–239 bestätigt B; alle geprüften L/R-Werte endlich, kein unerwünschter Host/MIDI-Prioritätsverlust.
- **Ohne Audioausgänge (`numOutputs=0`)** bleiben die 16.384 Parameterpunkte wirksam; anschließend ist Pattern B im wieder aktivierten Audiosignal nachgewiesen.
- **Zero-Frame-Flush (`numSamples=0`)** verarbeitet separat 16.384 Offset-0-Parameterpunkte; Pattern B bleibt nachfolgend im realen Audio erhalten.
- Timing ausschließlich um `IAudioProcessor::process`, **nicht** um Fixture-Erstellung: **Median 0,3708 ms, gemessener Maximalwert und 12-Messwerte-p99 0,3742 ms** (Windows-2022-Runner, 48 kHz/256 Samples, rechnerische Deadline 5,333 ms). **Offline-Diagnostik, kein echter ASIO-Realtime-Nachweis**.
- Die SDK-Helferklasse `ParameterValueQueue` führt bei identischen Sampleoffsets intern eine Zusammenfassung aus. Der erste Probecommit [`d3302a2`](https://github.com/challanger2000/125A-Drumator-Final/commit/d3302a2a929eeb726311ca430a1cbb98a858ec64) scheiterte **ausschließlich an der Test-Assertion**, dass alle 16.384 Punkte vorliegen – **kein nachgewiesener Pluginfehler**. Erst die zweite Probe mit selbst implementierter VST3-Interface-Testqueue lieferte tatsächlich alle 16.384 Punkte. Der erste Job [#40815728141](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40815728141) war **FAIL** und wird **nicht** als bestandener Test umetikettiert.

### Auditentscheidung zu DR-A04

**DR-A04: CLOSED für den konkret festgelegten Extremtest (16.384 Punkte, normal, outputless und zero-frame).** In diesen Belastungen **kein reproduzierter Fehler** und kein Bedarf für einen DSP-Code-Fix.

**Abgrenzung:** Die aktuelle Quellimplementierung iteriert weiterhin über `getPointCount()`; eine mathematisch konstante O(1)-Obergrenze für beliebig fehlerhafte Hostqueues ist damit **nicht** bewiesen. Das ist ein allgemeines Robustheitslimit, jedoch nach positivem 16.384-Punkt-Stress **kein separat reproduzierter V1.0.1-Releaseblocker**. Eine willkürliche Limitierung würde gültige Automation ändern und wird ohne Fehlernachweis nicht eingeführt.

**Einziger verbleibender V1.0.1-Releaseblocker des Audits: DR-A01 – echte DAW-/ASIO-/Save-Recall-Bedienabnahme auf dem Nutzerrechner (NOT RUN).** Es wurde keine V1.0.1-FINAL-Freigabe durch den Audit erteilt.
