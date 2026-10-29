# 125A Drumator V1.0.1 — unabhängiger kommerzieller GUI-Gesamt-Audit

**Prüfdatum:** 29. Oktober 2026  
**Ergebnis:** **GUI AUDIT NOT READY / kommerzielle Freigabe BLOCKIERT**  
**Neuester geprüfter Produkt-/GUI-Stand:** `17f2169cd8e3cf0e6e251054055e57a92813a2bf`; Windows [#40946548467](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40946548467) **SUCCESS**: 27/27 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL im unabhängigen Tester. Die früheren Audits und ihre damaligen Befunde bleiben als historische Basis dokumentiert; aktualisierter Abschluss in Abschnitt **10**. **Kein kommerzielles Release, Studio One weiterhin NOT RUN**.  
**Exakt geprüfter Produkt-HEAD:** `a7377e6ababe50df724e29ba791b85ea8f183f74`  
**Repository:** `challanger2000/125A-Drumator-Final` · **Branch:** `develop/v1.0.1-gui-redesign`  
**Produkt:** 125A Drumator, **Windows x64 VST3 1.0.1**, Steinberg SDK **3.8.1**  
**Arbeitsweise:** **READ-ONLY**: kein Produktcode, keine Ressourcen und keine Tests verändert; dieser Bericht ist die einzige Repository-Änderung. Das Design ist grundsätzlich owner-abgenommen; die folgende Prüfung ist keine eigenmächtige Designfreigabe.

## 1. Verbindliche Grundlagen und Provenienz

Gelesen/abgeglichen: `125A-Engineering/START-HERE.md`, `README.md`, `ENGINEERING-GUARDRAILS.md`, `STANDARDS/GUI-GUIDELINES.md`, `STANDARDS/BRANDING.md`, `STANDARDS/PROFESSIONAL-PLUGIN-BEHAVIOR.md`, `STANDARDS/REPOSITORY-WORKFLOW.md`, `STANDARDS/SOURCE-BASELINE-GATE.md`, `QA/RELEASE-QA.md`, `QA/REALTIME-QA.md`, einschlägige Recherche-/Roadmap-Übersichten; Final-Repository `AGENTS.md`, `CURRENT.md`, Implementierung und Testquellen; `125A-Branding/README.md` und die bestehende Master-SVG-Geometrie.

**Aktuelle, vom Eigentümer vorläufig abgenommene Gestaltung:** vollständiges proportioniertes 125A-Originalsymbol **mit** „125A SYSTEMS“ und „AUDIO SOFTWARE“, **ohne** silbernes Metallschild/Schrauben, auf der dunklen Kopfplatte. Der frühere im übergebenen Auditauftrag enthaltene Prüfpunkt „125A Systems-Metallschild“ ist historisch überholt. Auf keinen Fall daraus eine Rückbauanforderung ableiten.

**Aktueller CI- und Bildbeleg für exakt `a7377e6ababe50df724e29ba791b85ea8f183f74`:**
- [Windows GitHub Actions #40934339692](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40934339692), Windows-Job **SUCCESS**, unabhängiger Plugin-Tester **SUCCESS**.
- **27/27** CTests, darunter die native Win32-VSTGUI-/Controller-/Processor-Integration.
- Offizieller Steinberg Validator **47/47**, **0** fehlgeschlagen.
- 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- VSTGUI-Bildarchiv: `125A-Drumator-VSTGUI-Native-Renderings`, Artifact-ID **12924730931**; ZIP geprüft, **63 BMP-Einträge**, davon nur **15 mit Präfix `v101-`**; die übrigen 48 sind ältere/andere Testfamilien und **nicht** pauschal aktuelle Produktions-GUI.
- Gebautes VST3: `125A-Drumator-M11-Full-Kit-Stress-QA`, Artifact-ID **12923957635**; ZIP-CRC-Test bestanden, **17 Dateieinträge**.
- Exakte aktuelle VST3-Binärdatei: `125A Drumator.vst3/Contents/x86_64-win/125A Drumator.vst3`, **SHA-256 `734fac40e902d4a656190e0d55fcb269d7e5e8b03ba1c4beae975b2607103a71`**.
- `moduleinfo.json` der tatsächlich gebauten Datei: Instrument-Klasse „125A Drumator“, Controller-Klasse „125A Drumator Controller“, beide Version **1.0.1**, SDK **3.8.1**.
- Zusätzliche aktuelle Host-View-/Logo-Renderings: `125A-Drumator-V101-Master-Signet-View-Renderings`, Artifact-ID **12923902610**.

**Grenze des Nachweises:** Die Bilder stammen aus `VSTGUI::renderBitmapOffscreen` über einen **tatsächlich an einen nativen Windows-Testhost angeschlossenen VST3-Editor** und verwenden die aktuelle V1.0.1-Painter-Implementierung. Sie sind **keine** Aufnahmen aus Studio One und beweisen weder dessen FilePicker noch dessen ASIO-, Resize- oder Projekt-Recall-Verhalten. Ein Dateiname allein ist kein Beleg für einen geöffneten Menü-/VEL-/SET-Zustand.

**Release-Trennung:** `main` bleibt V1.0.0-Releasekandidat; `AGENTS.md` und `CURRENT.md` vermerken, dass Eigentümer-Gumroad-Veröffentlichung/Kundenpaket nicht verifiziert sind. Keine GitHub Releases für das Final-Repository gefunden. V1.0.1 ist ein **unveröffentlichter Entwicklungsstand**, nicht FINAL. Eine freigegebene kommerzielle FULL-Paketzuordnung oder separate Demo-/FULL-Provenienz ist hierfür **NOT VERIFIED**.

## 2. Visueller Gesamtbefund und nachgewiesene Layoutgeometrie

**Manuell angesehen:** tatsächliche V1.0.1-Screenshots `v101-layout-native-main-150.bmp`, `v101-song-board-hardware-150.bmp`, `v101-main-export-menu-100.bmp`, `v101-song-export-menu-100.bmp`, `v101-main-limiter-on-100.bmp`, `v101-main-long-label-and-status-100.bmp`, `packaged-vst3-resources-silver-100.bmp` (historischer irreführender *Dateiname*, aber aktuelle dunkle View); zusätzlich WAVE-Renderings 100/150 und 100/120/130/150 im Überblick sowie die veralteten `velocity-design-100/130` und `v2-choke-menu` zur Provenienzabgrenzung. Kontaktübersicht der produktiven 100/120/130/150-Snapshots mit Quellgeometrie und Pixel-/Hitbox-Assertions abgeglichen. **Nicht behauptet wird die manuelle Pixel-für-Pixel-Prüfung jedes der 63 Bilder**.

| Bereich / Koordinaten bei logischen 100 % | Tatsächliche Feststellung | Status |
|---|---|---|
| Header Pattern A–H, `y=99..131`, `x=33+39*i..+35` | Gleiche Höhen und 4px Gruppentakt; in nativen Bildern gleichmäßig; Auswahl orange | PASS (sichtbare Beispiele + Geometrie/Hosttests) |
| MENU `x=364..447`, BARS `455..538`, SONG `546..629`, SONG EDIT `637..720` | Je **83×32** mit 8px Abständen und gleich hohem Ober-/Unterrand | PASS |
| SWING/HUMANIZE/MASTER | Drei gemeinsame echte Drehregler inkl. Werte; keine quadratischen Pseudoknöpfe | PASS für gezeigte Zustände; DAW-Haptik NOT RUN |
| Step-Ruler 1/5/9/13/17/21/25/29 | `UiV101Layout::stepNumberRect(i)` entspricht dem *tatsächlich gemalten* Step-Cap-Zentrum `stepX+1+i*19`; Tests kontrollieren alle Viertel | PASS geometrisch + sichtbare 150%-Stichprobe |
| Reihen und Step-Grids | 32 Caps, 17px aktiv nutzbare Breite bei 19px Pitch, 2px passive Zwischenräume, native 150%-Bildprüfung; ROW-Editor hat weitere 48px Höhe | PASS für belegte Ansichten, nicht alle Zustände bei jedem Zoom |
| SET | Fünf **112×32** mit 10px Abstand (`x=336/458/580/702/824`), zentriert; aktuelles MAIN-Bild zeigt SET bei 150 % | PASS Geometrie + reale Maustests |
| VEL | RANDOM VEL, RESET VEL, LIVE VEL sowie 32 Balken im aktuell gezeichneten Code; native Host-Maustests bestehen | **NOT VISUALLY VERIFIED** für aktuelle dunkle V1.0.1 auf allen Zoomstufen |
| MAIN Footer | `x=8..1272` Panel, letzte Reihe sichtbar, Eckschrauben nach oben gezeichnet und erhalten, korrekte Gesamtbreite | PASS im aktuellen nativen 150%-Bild |
| Footer-Buttons | ADD/REMOVE **90×25**, EXPORT/PREV/NEXT **95×25**; horizontal gleiche Höhe, unterschiedliche Breite nach Gruppe | Geometrisch PASS; gestalterisch LOW-Befund |
| LIMIT | `x=1069..1152,y=54..86`, **83×32**, 10px frei vor Zoom-Popup ab x1162; ON orange, OFF dunkel; kein Zoom-Clickthrough | PASS native 100%-Bild + Zoom-/Hosttests |
| EXPORT | `x=915..1010` relativ zum 48px-Footer, **95×25**; in MAIN und SONG sichtbar; Menü mit vier 210px breiten Optionen | PASS native 100%-Bilder und Mouse-/Menu-Tests |
| 125A-Logo und DRUMATOR | Vorläufig vom Eigentümer abgenommenes vollständiges Masterlogo auf dunkler Faceplate; keine sichtbare Plakette; DRUMATOR-Schriftzug unbeschnitten im untersuchten 150%-Bild | PASS für zuvor akzeptierten Designstand |
| SONG-Board | Kopfzeile/Bediengruppe, Status, SONG START und Navigation sichtbar; deutliche **helle Leerzellen** im sonst dunklen Bedienfeld | Funktionstests PASS; optischer Befund **G02** |
| WAVE-Modal | Metalloberfläche, Wellenform, Regler, Toolbar und Scrim in 100/120/130/150 gerendert; Fünfpunkt-Metall-Pixelorakel vorhanden | PASS für geprüfte Fixture-Szenarien; echte Mono-/Stereo-Dateien in DAW NOT RUN |

**Wichtige Trennung:** Die neue Beschriftung `PTNR` über MENU ist **kein technischer Defekt**, aber als kommerziell sichtbare Abkürzung unklar. Die grauen SONG-Leerfelder sind **bewusst so programmiert**, nicht ein zufälliger Renderingfehler; damit bleibt die Designentscheidung beim Eigentümer.

## 3. Aktuelle Bildschirm-/Zoom-Belegmatrix

Legende: **BILD** = aktuelles natives V1.0.1-VST3-Rendering vorhanden; **FUNK** = zusätzlich konkrete native Funktions-/Hostprüfung; **LÜCKE** = kein spezifischer aktueller visueller Beleg im Archiv.

| Ansicht / Zustand | 100 % | 120 % | 130 % | 150 % |
|---|---|---|---|---|
| MAIN normal | BILD (gepackte View / Langnamen), FUNK | BILD, FUNK | **LÜCKE** für eindeutig benannte aktuelle MAIN-Bildaufnahme, Zoom FUNK | BILD, FUNK |
| MAIN SET | Quell-/Maustests, **LÜCKE BILD** | LÜCKE BILD | LÜCKE BILD | **BILD** (aufgeklappte erste Instrumentenzeile) |
| MAIN VEL | **LÜCKE BILD** | LÜCKE BILD | LÜCKE BILD | LÜCKE BILD |
| MAIN Instrumente 11–16 (zweite Seite) | Funktionsprüfung, **LÜCKE AKTUELLES V101-BILD** | LÜCKE BILD | LÜCKE BILD | LÜCKE BILD |
| SONG BOARD | **BILD, FUNK** | **BILD, FUNK** | **BILD, FUNK** | **BILD, FUNK** |
| SONG mit EXPORT-Menü | **BILD, FUNK** | LÜCKE BILD | LÜCKE BILD | LÜCKE BILD |
| WAVE-Editor | **BILD, FUNK** | **BILD, FUNK** | **BILD, FUNK** | **BILD, FUNK** |
| MAIN mit EXPORT/LIMIT-Zustand | **BILD, FUNK** | LÜCKE BILD | LÜCKE BILD | LÜCKE BILD |
| Geöffnete Pattern-/Fill-/BUS-/Choke-/Sample-Menüs der **aktuellen** Skin | native FUNK + Dunkel-Sprite-Pixeltest; **LÜCKE systematische aktuelle Einzelbilder** | LÜCKE BILD | LÜCKE BILD | LÜCKE BILD |
| Zoom-Popup, erster/letzter Punkt | Host-FUNK und Popup-Pixeltests | Host-FUNK | Host-FUNK | Host-FUNK; komplette 4er-Screenshot-Matrix **LÜCKE** |

**Achtung: falsche Belegzuordnung verhindern.** Die Dateien `velocity-design-100.bmp`, `velocity-design-130.bmp`, `v2-choke-menu.bmp`, `v2-fill-menu.bmp`, `v2-output-menu.bmp`, `v2-pattern-actions-menu.bmp`, `v2-sample-actions-menu.bmp`, `v2-zoom-menu.bmp` usw. zeigen die **ältere blaue Oberfläche**. Sie dürfen weder als Bilder der dunkel-metallischen V1.0.1 noch als Beleg ihrer aktuellen Buttonausrichtung verwendet werden. Das Archiv gehört vor kommerzieller Designfreigabe in eindeutig markierte `CURRENT-V101` vs. `LEGACY-DIAGNOSTIC`-Gruppen.

## 4. Tatsächliche GUI-Funktionskette — Traceability

| Bereich / Elemente | Eingabe → Controller/Engine → sichtbarer/gespeicherter Zustand | Aktueller Befund |
|---|---|---|
| A–H/1 oder 2 Bars | native Maus/Host-Automation; Pattern- und Längenstate; 32-Step-Grenzen | Windows PASS |
| Pattern-Menü: COPY, PASTE, CLEAR, UNDO, RANDOM VELOCITY, VELOCITY 100, DUPLICATE | alle **7/7** nativen Menüaktionen mit konkreten Pattern-Daten/Undo | Windows PASS |
| FILL | **8/8** Varianten, unabhängig definierte rhythmische 32-Step-Masken/Velocities | Windows PASS |
| BUS | **16/16** Ziele, Controller-/Processor-Zustand, State-Serialisierung und Rücksetzen; DAW muss zusätzliche Busse aktivieren | Windows PASS; DAW-Stem-Routing NOT RUN |
| CHOKE | **5/5** Gruppen, native Auswahl und Controller-State | Windows PASS; musikalisches Abhören in DAW NOT RUN |
| Sample-Menü LOAD/RENAME/REMOVE, Sample-Vor/Zurück, PLAY/EDIT | native Datei-/PCM-Fixtures, Namens-Edit und Vorschau-Queue; echte OS-Dialogbedienung nicht vollständig | Windows teilweise PASS; nativer DAW-FilePicker NOT RUN |
| 16 Instrumente, MUTE/SOLO, Step, Page | Host-Test über beide Seiten; Row-Isolation, Set/Toggle, Step-Gutter, Nachbarzeile | Windows PASS |
| VEL: RANDOM/RESET, LIVE VEL, Wert/Undo/Leerschritte | native Controller-Daten- und Undo-Tests, dazu musikalische MIDI-/PCM-Einzeltests | Windows PASS für Fälle; aktuelle visuelle und komplette Grenzmatrix noch offen |
| SET: BUS/CHOKE/COPY/PASTE/CLEAR | echte Maustests, Zeilen-/Pattern-Änderungen | Windows PASS |
| SONG 1–64, A–H/REST, 1/2/1/2 Bars, ADD/REMOVE, START ±/RESET, PREV/NEXT | `testV101SongAndWaveFullControlMatrix`, `testV101LiveEditor`; Sichtauswahl und Grenzen 16/17 usw. | Windows PASS für explizite Fixture-Fälle |
| WAVE START/END, FADE IN/OUT, GAIN, ZOOM±/FIT, PLAY/STOP/RESET/APPLY/CANCEL | native Maustests, echte SampleEditor-IPC, Commit und Cancel-Rollback, Popup-/Renderer-Tests | Windows PASS für Fixture-Fälle, DAW NOT RUN |
| ZOOM 100/120/130/150, Rückweg SONG→100 | echte `setZoomChecked`-/`zoomMatches`-Prüfung im Windows-Host, LIMIT-Clickthrough-Negativtest | Windows PASS, Studio One NOT RUN |
| MIDI-EXPORT Pattern/Song | `DrumatorView::exportSelection` → `Controller::exportMidi` → `MidiExport` → transaktionale `ExportFile`; Parser-Test Typ 1/17 Tracks/960 PPQ, Events/Song-Offsets; verbundene Fixture schreibt echte `.mid`-Dateien | Windows PASS; OS-Save-Dialog, DAW-Import, Nutzerdateien NOT RUN |
| WAV-EXPORT Pattern/Song | `Controller::exportWav` → `OfflineWavExport::render`, tatsächliche Stereodatei 48 kHz/24 Bit, Länge/Tail, PCM-Samples; verbundener Dateitest | Windows PASS für Fixtures; lange echte Songs/DAW-Import NOT RUN |
| LIMIT ON/OFF | echter Controller→Processor-Nachrichtenpfad; `StereoPeakLimiter`-Unit: 0 PDC, Peak-Ceiling, L/R-Link, bittransparenter OFF; WAV-Test begrenzt PCM | Windows PASS für Einzel- und Teilketten, aber **G06** vollständiger ON/OFF-Audiovergleich der verbundenen Exportkette nicht belegt |
| SAVE/CLOSE/REOPEN Projekt | viele State-/Recall-/Hostfixture-Tests; echte Studio-One-Projektdatei nicht geöffnet | Windows PASS für Fixtures, DAW-Projekt-Recall **NOT RUN** |

**Prüfmethodischer Hinweis:** Ein Controller-`true` ist nicht dasselbe wie der wirklich gehörte Audio-/MIDI-Effekt. Wo ein End-to-End-Test tatsächliche Bytes/PCM kontrolliert, ist das ausdrücklich genannt. Wo lediglich Hostfixture und Komponentenzustand geprüft sind, verbleibt **NOT RUN / PARTIAL** für die DAW und Nutzungsrealität.

## 5. Befunde nach Schweregrad, Ursache und Reproduktion

### G01 — GESCHLOSSEN IM NACHPRÜF-LAUF / ursprünglich HIGH — aktuelle VEL-/zweite Seiten-/Popup-Ansichten

**Beleg:** Artefakt **12924730931** enthält 63 Bilder, davon 15 explizit `v101-*`. Kein `v101-velocity-*` und kein gezieltes aktuelles `v101-page2-*` bei 100/120/130/150. Die vorhandenen `velocity-design-*` und `v2-*` stellen sichtbar die alte blaue Oberfläche dar. Keine komplette aktuelle Screenshotmatrix aller sieben Menüarten bei vier Zoomstufen.

**Risiko:** Echte Überschneidungen, Schriftpositionen, Zustandsgrafik und abgeschnittene letzte Zeile im aktuellen dunklen VEL/SET und auf zweiter Seite könnten trotz Grün in CTest und Validator durchrutschen. Genau das soll dieser unabhängige Audit verhindern.

**Reproduktion:** Artefakt herunterladen; Dateiliste und Inhalte `velocity-design-100.bmp` versus `v101-layout-native-main-150.bmp` vergleichen. Alle 63 als „aktuelles V1.0.1“ zu zählen wäre falsch.

**Betroffen:** `tests/integration/WindowsIntegration.cpp` (Snapshot-Erzeugung), `tests/integration/V101FullGuiAudit.inl` (GUI-Fälle), `.github/workflows/core-ci.yml` (Archiv-Auswahl). **Empfehlung nach separater Freigabe:** durch echte Editor-Klicks und `zoomMatches` belegte CURRENT-V101-Ansichten für VEL, SET, Page 2/letzte Reihe und jede Popup-Art bei allen vier Zooms erfassen; relevante Zustände **vor** Capture explizit assertieren; Bild-/Hitbox-Orakel ergänzen. **Kein Produktfehler bewiesen, aber Audit unvollständig.**

### G02 — HISTORISCHER MEDIUM-BEFUND; in Abschnitt 10 geschlossen — hellgraue SONG-Leerfelder

**Beleg:** `v101-song-board-hardware-150.bmp` und `v101-song-board-hardware-100.bmp`, zweite Kachelreihe; `src/vst3/UiV101SongPainter.h:75–86`: leere Slots `Color{78,81,83,255}` mit Rand `{120,118,114,255}` versus befüllte dunkle `Skin::navyWell()`.

**Reproduktion:** SONG EDIT mit acht Einträgen öffnen. Einträge 01–08 dunkel; 09–16 auffällig hellgrau. Unterschied ist absichtlich implementiert, nicht kaputt. **Empfehlung:** Eigentümerentscheid über dunkle deaktivierte Navy-Wells mit erhaltenem Kontrast statt pauschaler Farbersetzung. **Noch keine Codeänderung.**

### G03 — HISTORISCHER MEDIUM-BEFUND; in Abschnitt 10 geschlossen — unklare Header-Beschriftung „PTNR“

**Beleg:** in aktuellen MAIN-/SONG-100/150-Bildern steht über der MENU-Taste „PTNR“, daneben bereits „PATTERN“ über A–H. `src/vst3/UiV101NativePainter.h:543`: `label(c,"PTNR",{364.,80.,447.,96.},...)`; MENU selbst `x364..447,y99..131`.

**Reproduktion:** Plugin 100 % öffnen, Kopfzeile zwischen A–H und 1/2 BARS prüfen. Beschriftung ist nicht selbsterklärend und doppelt den Pattern-Kontext. **Empfehlung:** UX-Label konkret abstimmen (z. B. „PATTERN MENU“/Entfall des Kürzels), danach auf gleiche Höhe und optische Mitte prüfen.

### G04 — BLOCKER / RELEASE-NACHWEIS — Studio One / echter Save-Picker / User-Projekt-Recall nicht getestet

**Beleg:** `docs/EXPORT-LIMITER-V1.0.1.md` dokumentiert selbst ausstehende DAW-/Save-Dialog-Prüfungen. `src/vst3/DrumatorView.cpp:2203–2230` nutzt `VSTGUI::CNewFileSelector::kSelectSaveFile`, der in Headless-/CI-Fixtures nicht wie in Studio One bedient wird. Aktuelle Windows-Suite gibt explizit „Studio One NOT RUN“ aus.

**Reproduktion:** Nicht in dieser Umgebung – echte DAW und tatsächliche Windows-Save-Dialoge nötig. **Pflichtprüfung:** vier Dateitypen im echten Picker, vorgeschlagene Erweiterung, Verzeichnisse mit Unicode/Leerzeichen, Abbruch, Schreibschutz, existierende Datei mit Überschreiben/Rollback, lange SONG-Datei, Datei in DAW importieren, WAV anhören, LIMIT ON/OFF vergleichen, Projekt speichern/schließen/neu öffnen und sämtliche GUI-/Prozessorstates wiederfinden. **Kein behaupteter Produktfehler, aber zwingende Freigabes​​perre.**

### G05 — HISTORISCHER LOW-BEFUND; in Abschnitt 10 geschlossen — zwei unterschiedliche Footer-Buttonbreiten

**Beleg:** `src/core/UiV101Layout.h:43–46,83`: ADD/REMOVE jeweils **90×25**, PREV/NEXT/EXPORT **95×25**. In aktuellen MAIN-100/150-Bildern sichtbar. Einheitliche Höhe und Buttonfamilie, aber nicht alle fünf identisch breit.

**Empfehlung:** Nicht ungefragt angleichen; die funktionalen Gruppen können absichtlich verschieden sein. Vor kommerziellem Design-Sign-off entscheiden, ob konsequent eine Breite oder Gruppenbreiten gelten sollen; Symmetrie und Statusbereich dabei berücksichtigen.

### G06 — GESCHLOSSEN IM NACHPRÜF-LAUF / ursprünglich HIGH — LIMIT-End-to-End-Ausgangstest

**Beleg:** `tests/integration/WindowsIntegration.cpp` neue `testV101ConnectedPatternSongExportsAndLimiter`: `setLimiterEnabled(true)`, Export `limited.wav`, Prüfung der Datei-Header und Rücksetzen; **kein** sampleweiser Vergleich `LIMIT ON` vs. `LIMIT OFF` und keine integrierte Ceiling-/Bypass-PCM-Assertion in genau diesem verbundenen Exportpfad. Die separat bestehenden `tests/StereoPeakLimiterTests.cpp` und `tests/OfflineWavExportTests.cpp` prüfen dagegen sinnvoll Peak-/PCM-Verhalten.

**Risiko:** Eine fehlerhafte Zuordnung des Limiter-Zustands zwischen tatsächlicher Controller-Exportspiegelung und Audioausgabe könnte durch den neuen verbundenen Test unentdeckt bleiben. **Kein nachgewiesener DSP-Defekt.**

**Empfehlung:** Später unabhängiger Fixture-Verbundtest mit absichtlich übersteuernden Stimmen, sampleweisem 24-bit PCM-Referenzvergleich ON/OFF, Peak-Ceiling, Transienten, Stereo-Verhältnis, tatsächlichem Processor-Bus und Save/Recall bei deaktiviertem/enabled Limiter. Bestehende gute Unit-Tests beibehalten.

### G07 — LOW / BUNDLE-HYGIENE — historisches 125A-Metallschild trotz neuer Faceplate im Paket

**Beleg:** Der aktuelle, CRC-geprüfte Windows-VST3-Bundle enthält `Contents/Resources/125A Systems Metall-Logo Badge.png`. Das aktuelle V1.0.1-Rendering zeigt korrekt **kein** Schild; es existiert keine im jetzigen Painter notwendige Schild-Komposition. Die alte Ressource ist ein potenziell verwirrender/unnötiger Packagerest, kein sichtbarer Rendering-Bug.

**Empfehlung:** In einer gesondert freigegebenen Bereinigung prüfen, ob irgendein Legacy-Fallback die Datei lädt; nur nach belegter Nichtbenutzung aus `CMakeLists.txt`/Paket entfernen. Original-Branding-Assets nicht manipulieren.

### G08 — MEDIUM / RELEASE-DOKUMENTATION — aktuelle Baseline im CURRENT nicht mit Produkt-HEAD synchron

**Beleg:** `CURRENT.md` beschreibt noch ältere V1.0.1-Commits/Runs und führt nachfolgend V1.0.0 auf; tatsächlicher auditierter Stand ist `a7377e6ababe50df724e29ba791b85ea8f183f74`, Windows #40934339692. `AGENTS.md` verortet weiterhin V1.0.0 als aktuellen Releasekandidaten; das ist **nicht** automatisch falsch, weil 1.0.1 noch nicht veröffentlicht wurde, aber die Navigationsangabe für einen neuen Reviewer ist unvollständig.

**Empfehlung:** Im späteren Release-Vorbereitungszyklus aktuelle geprüfte 1.0.1-Entwicklungsbasis mit CLEARLY NOT FINAL aktualisieren; nur bei echter Owner-FULL-Freigabe AGENTS/CURRENT/Gumroad-Synchronisierung auslösen. Produktcode unverändert.

### G09 — LOW / METADATEN-CHECK — moduleinfo root „Name“ leer

**Beleg:** Geprüftes `Contents/Resources/moduleinfo.json` enthält oberste Zeile `"Name": ""`, während die tatsächliche VST3-Instrument-Klasse korrekt „125A Drumator“ heißt. Validator und unabhängiger Tester sind grün.

**Einordnung:** Kein belegter Host-Anzeigefehler. Bei kommerziellem Packaging gezielt DAW-Browser/Plugin-Metadaten prüfen und nur dann ändern, wenn ein wirklicher Mangel vorliegt.

## 6. Explizite Pass-Belege und verbleibende Negativfälle

**Belegt PASS (genau dieser Produkt-Commit):** Windows x64 VST3-Build; 27/27 CTests; 47/47 Steinberg Validator; 58/0/0 unabhängiger Tester; native VST3-Hostfixture-Pointer/Zoom; SONG-Zoom 150→100 ohne LIMIT-Umschalten; MAIN-/SONG-EXPORT-Menü und Sichtzustand; 32-Step-Geometrie; MENU/FILL/OUTPUT/CHOKE/SONG/WAVE-Funktionsmatrix der tatsächlich getesteten Fälle; MIDI- und WAV-Unit-Dateiparser, WAV-Stereosamples und End-Tail, Limiter-Einzeltest, verbundene Controller-/Processor-Dateierzeugung.

**Nicht als PASS behaupten:** vollständige manuelle Einzel-Pixelabnahme *jedes* Buttons in *jedem* möglichen Zustand, tatsächliche Studio-One-Dialoge, fremde MIDI-Importsoftware, reale User-Samples und verschiedene Schreibpfade; Langzeit-ASIO-Stabilität; echtes DAW-Hostresize unter Windows-DPI; Spotify/Gumroad/Kundenveröffentlichung (hier nicht relevant); unabhängiges kommerzielles Design-Sign-off nach offenen G02/G03/G05/G10; systematischer DAW-ON/OFF-Signalvergleich (Windows Processor-/WAV-PCM-Tests jetzt PASS); Apple/macOS (aus Scope ausgeschlossen).

**UI-Zustandsprüfungen, die noch als Nachweis fehlen:** jeweils geöffnetes/geschlossenes Popup, erster/letzter Eintrag und click-through-Negativkontrolle, VEL mit Schritt 1/127/leeren Steps/Undo, SET mit BUS 1/16 und CHOKE 0/4, beide Instrumentenseiten inkl. letzte Zeile/gesperrter PAGE, SONG 1/64 und BAR-Extrema, WAVE Mono und Stereo mit erkennbaren START/END/Fades, LONG LABELS und aktuelle echte Statusmeldungen, alle vier Zoomstufen *am realen Hosteditor* plus DAW-Verifikation.

## 7. Abnahmeentscheidung und Reihenfolge

**Entscheidung: GUI AUDIT NOT READY; kommerzielle Veröffentlichung NICHT freigeben.** Das technische Windows-Paket selbst ist nach seinen automatisierten Prüfungen **grün**. G01 und G06 sind in der Nachprüfung technisch geschlossen; die noch offenen Gates betreffen die echte Studio-One-/Dateipicker-/Recall-Abnahme, die dokumentierten Gestaltungsbefunde G02/G03/G05/G10 und die manuelle Vollabnahme der Bildqualität. Kein reproduzierter Produktabsturz wurde gefunden.

**Vorgeschlagene spätere Freigabe-Reihenfolge (noch NICHT implementiert):**
1. **G04:** echte Studio-One-Prüfliste einschließlich OS-Dialog, Export-/Import, Limiter, Zoom und Projekt-Recall ausführen. Die neuen G01/G06-Windows-Testbelege ersetzen das nicht.
2. **G02/G03/G05/G10:** Gestaltung erst nach konkreter Owner-Entscheidung gebündelt korrigieren, wenn gewünscht. Logo bleibt abgenommen und unangetastet.
3. **G07/G08/G09:** Verpackungsressourcen und Navigations-/Pluginmetadaten im späteren Release-Zyklus kontrollieren.
4. Danach die Gesamtmatrix auf einem **einzigen exakten neuen Produkt-Commit** erneut prüfen, aktuelle Bilder vollständig visuell kontrollieren, tatsächlichen Benutzer-/DAW-Abnahmebericht einholen. Erst dann ein finales kommerzielles Release-Gate evaluieren.

## 8. Prüfgeltung

Dieser Bericht ist ein **unabhängiger, read-only Quell-, Test-, Package- und Bild-Audit** des Produkt-HEADs `a7377e6ababe50df724e29ba791b85ea8f183f74`. Die Prüfung lief über GitHub-Quell-/CI-Belege und die aus dem erfolgreichen Windows-Run heruntergeladenen und inspizierten VST3- und Bilddateien. **Es wurde kein eigener Studio-One-Prozess und kein ASIO-Treiber gestartet.** Er sagt weder, dass alle visuellen Zustände bereits betrachtet wurden, noch dass sämtliche theoretisch möglichen Mouse-, FilePicker- und DAW-Sequenzen dynamisch gelaufen wären.

**Produktdateien bleiben unverändert.** Nur der ausdrücklich angeforderte Bericht wird auf dem bestehenden Entwicklungsbranch abgelegt. Kein Codex, keine neue Branch, kein Release und keine eigenmächtige Korrektur von G02/G03/G05/G06.


## 9. Nachprüfung 29.10.2026 — neue tatsächliche Windows-Belege

**Geprüfter Produkt-/Teststand:** `ccc6e1200380adde9ae326538b4f7b451f7e3814`. Für diese Nachprüfung wurde ausschließlich `tests/integration/WindowsIntegration.cpp` geändert; **keine** Produkt-DSP-Dateien, Faceplate, Fonts, Logos oder UI-Geometrie. Keine eigenmächtige Designkorrektur.

- [Windows-Lauf #40942211432](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40942211432): **SUCCESS**.
- **27/27 CTests PASS**, **47/47 Steinberg Validator PASS**, **58 PASS / 0 WARNING / 0 FAIL** im Plugin Tester v0.3.2.
- Tatsächliche V1.0.1-Bilder: Actions-Artifact **`125A-Drumator-VSTGUI-Native-Renderings`, ID 12926979107**; Archiv um **44 neue `v101-current-*`-BMPs** erweitert.
- Jeder neue Snapshot wird erst gespeichert, nachdem `view.zoomMatches(expected)` und `hostEditor.getZoomFactor()` den echten Hostzoom bestätigen. Menüzustände werden mit `IntegrationAccess::v101Popup`, der Zoomzustand separat mit `v101ZoomPopupOpen` verifiziert. Mit VEL/SET und Page-2 werden vorher die aktiven Zustände und die echte Hostgröße geprüft.
- **Aktuelle echte Screenshotmatrix an allen vier Zooms 100/120/130/150:** MAIN, VEL, SET, PAGE 2 mit sechs Instrumenten; die sieben Menügruppen Pattern, FILL, BUS, CHOKE, SAMPLE, EXPORT und ZOOM. Das sind **44 aktuelle Bilder**, strikt von den ebenfalls im Artefakt enthaltenen historischen `v2-*`- und `velocity-design-*`-Bildern getrennt. SONG- und WAVE-Editor lagen bereits mit echter 100/120/130/150-Capture- und Eingabekontrolle vor.
- **G01 STATUS: GESCHLOSSEN, WINDOWS VISUAL EVIDENCE PASS** für die benannten Zustände und vier Zoomstufen. Die manuelle Betrachtung der zwei Kontaktbogen-Gesamtüberblicke sowie der hochaufgelösten VEL/SET/PAGE2-/Menübeispiele zeigte keine angeschnittenen Buttons oder verschwundene Footer-Elemente. Das ersetzt **nicht** eine Freigabe aller Pixel-/Textvarianten durch den Eigentümer oder den DAW-Test.
- **G06 STATUS: GESCHLOSSEN, WINDOWS AUDIO PCM PASS.** Eine Fixture mit **zwei echten simultanen Stereo-Float-WAV-Samples** `L=+0.90/R=-0.72` je Instrument führt zu absichtlicher Summenübersteuerung. Der über `Controller::setLimiterEnabled` erreichte Processor-State wird per `getState` geprüft. Die tatsächlich geschriebenen **24-bit-WAV-Dateien** für OFF/ON/OFF werden frameweise auf -0.3-dBFS-Ceiling, Stereo-Quellverhältnis, wirksame Pegelreduktion und **bitidentischen** letzten OFF-Export geprüft. Zusätzlich erzeugen **zwei echte VST3-Processor-Instanzen** mit identischer Kit-MIDI-Clock separat die Audioausgangsblöcke für OFF und ON und prüfen **256 Frames** auf nachweisbare Dämpfung, unverändertes Stereo-Verhältnis und Ceiling. Die Audioverarbeitung selbst wurde dabei nicht modifiziert.
- **G04 STATUS UNVERÄNDERT: BLOCKER.** Weder Studio One mit realen Datei-Pickern noch ASIO-Last-/Projekt-Recall wurde in GitHub Actions ausgeführt.

### G10 — HISTORISCHER MEDIUM-BEFUND; in Abschnitt 10 geschlossen — Hintergrundleiste schimmert zwischen Instrumentenzeilen durch

**Echte Bilder:** `v101-current-settings-150.bmp` und `v101-current-velocity-150.bmp`, zwischen „Clap 01“ und „Rim 01“, etwa bei **Bild-y ≈ 870–885 bei 150 %**; ähnlicher Spalt in `v101-current-main-150.bmp`. In einer dünnen ansonsten dunklen horizontalen Trennfuge sind weiße/silbrige diagonale Fragmente und helle Abschnitte sichtbar.

**Nachvollziehbare Ursache:** `src/vst3/UiV101NativePainter.h::drawVintageFaceplate` zeichnet `resource/Futuristische industrielle Metalloberfläche.png` **hinter** den separat gerenderten Instrumentenzeilen als einzelne Fotografie mit fixer 686px-Referenzgröße. Das untere innere Metallbevel der Fotoressource verläuft bei der tatsächlichen vertikalen Skalierung ungefähr in dieser Bildzone und bleibt durch kleine nicht überdeckte Zeilenabstände sichtbar. Kein Funktions-/Hitboxfehler, aber die gleichmäßige Optik der Trennfugen wird unterbrochen.

**Vorgeschlagene Designkorrektur, noch NICHT ausgeführt:** Bei der späteren autorisierten GUI-Korrekturrunde die dynamischen Zeilenabstände mit einem durchgängigen passenden dunklen Untergrund hinterlegen oder die sichtbare Foto-Bevel-Zone davon trennen. **Nur die Instrumenten-Schnittstelle behandeln, nicht den vorläufig abgenommenen 125A-Signet-, Titel- oder Kopfplattenbereich ändern.** Danach 100/120/130/150 und 9-/10-/6-Zeilen-Zustände erneut mit echten Pixelbelegen prüfen.

### Weiterhin offene Entscheidungspunkte

- **G02:** helle inaktive SONG-Eintragskacheln; gestalterische Owner-Entscheidung.
- **G03:** doppeldeutig abgekürztes `PTNR`; gestalterische Owner-Entscheidung.
- **G05:** 90px- gegenüber 95px-Footer-Breiten; nach Gruppen differenziert, kein belegter Funktionsfehler.
- **G07:** ungenutztes historisches Badge im Package. Keine Ressourcen ohne überprüfte Nichtbenutzung entfernen.
- **G08:** ältere Navigations-/Release-Historie in `CURRENT.md` und `AGENTS.md`; Development-Hinweis unter diesem Report aktualisiert, noch kein neuer FULL-Release.
- **G09:** oberstes `moduleinfo.json.Name` leer trotz korrekter Controller-/Processor-Klassennamen; DAW-Browser-Abnahme offen.
- **G04:** manuelle Studio-One-OS-Picker-, Zoom-, Export-/Import-, Limiter-, ASIO- und Projekt-Recall-Abnahme **NOT RUN**.

**Gesamtergebnis bleibt: GUI AUDIT NOT READY / KEINE KOMMERZIELLE FREIGABE.** Die zwei High-Testlücken sind in Windows behoben, doch G04 und die Gestaltungskorrekturen/Freigaben sind noch offen.


## 10. Freigegebene, gebündelte GUI-Korrekturrunde — 29.10.2026

**Freigabegrundlage:** Auf den Vorschlag, G02/G03/G05/G10 als eine gebündelte GUI-Korrekturrunde auszuführen, erteilte der Eigentümer die Anweisung „los“. Die bis dahin vorläufig abgenommene 125A-Branding-Komposition wurde nicht verändert.

**Produktstand:** `17f2169cd8e3cf0e6e251054055e57a92813a2bf` (V1.0.1-Entwicklungsbranch); Änderungen gegenüber `ccc6e12` beschränken sich auf `src/core/UiV101Layout.h`, `src/vst3/UiV101NativePainter.h`, `src/vst3/UiV101SongPainter.h` und ergänzende Tests. Kein DSP-/Export-/Branding-Quellcode geändert.

**Abschließende überprüfte CI-Provenienz:** Windows [#40946548467](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40946548467) **SUCCESS** mit **27/27** CTests, **47/47** Steinberg Validator und **58 PASS / 0 WARNING / 0 FAIL** unabhängigen 125A Plugin Tester v0.3.2. Aktuelle native GUI-Aufnahmen im Artifact **`125A-Drumator-VSTGUI-Native-Renderings` / ID 12928724052** (einschließlich 44 `v101-current-*`-Bilder bei vier realen Hostzoomstufen).

| Auditpunkt | Konkrete Korrektur | Nachweis / Status |
|---|---|---|
| **G02**: helle SONG-Leerfelder | In `UiV101SongPainter.h` statt grau (`78/81/83`) dunkel-navy (`45/57/69`), matte dunkelblaue Kontur `63/80/95`; weiterhin heller als belegte Kacheln, ohne neue Hitflächen | Echte native `v101-song-board-hardware-100/120/130/150.bmp`; RGB-Test: Leerzelle ist dunkler als 75/75/80, trotzdem je >25 Stufen heller (R/G) als belegte Zelle. **CLOSED / PASS** |
| **G03**: „PTNR“ | Vollständige Beschriftung **`PATTERN EDIT`** in exakt derselben 83px-Headerfläche, Fontgröße 10; MENU darunter unverändert | Native `v101-current-main-100/150.bmp`, `v101-song-board-hardware-150.bmp`, visuelle Prüfung: vollständig lesbar, kein Clip, Abstände erhalten. **CLOSED / PASS** |
| **G05**: abweichende Footer-Breiten | ADD ROW und REMOVE ROW von 90 auf **95×25** angeglichen; präzise neue x-Bounds ADD `45..140`, REMOVE `151..246`; EXPORT/PREV/NEXT bleiben 95×25. 11px Zwischenraum, 14px bis Summary-Text | `UiV101LayoutTests.cpp` prüft alle fünf Maße, Zwischenräume, unveränderte vertikale Flucht. Echter Windows-VSTGUI-Maustest ADD/REMOVE und Page-Navigation bestand; native 100/150-Screenshots kontrolliert. **CLOSED / PASS** |
| **G10**: Fotokante in Instrumentenfugen | Ein einziges `Skin::navyWell()`-Fundament von x=12 bis 1260 zwischen `header` und `footer` **hinter** allen gezeichneten Instrumentenzeilen. Gesonderter Kopfplatten-/Logo-/Footerbereich und deren Foto bleiben unangetastet. Instrumenten-Hitflächen unverändert | Echter Hosteditor-Rastertest prüft alle neun Nahtstellen an x=650: rote/grüne/blaue Kanäle je unter 45/50/65. Alte Stelle bei GUI x=650,y=590 ursprünglich ca. `(62,64,66)`, nach Korrektur `(15,27,40)` bei 150%-Rendering. **CLOSED / PASS** |

**Gesonderte Fehlerdiagnose:** Der erste QA-Lauf für `481d2db` meldete genau einen visuellen Testfehler: Die neue SONG-Leerzelle `40/52/65` hatte gegenüber dem belegten Pixel `15/27/40` nur exakt 25 statt verlangten **mehr als 25** RGB-Stufen Kontrast. Die Endfassung `45/57/69` behebt den realen Grenzfall ohne Absenkung des Testmaßstabs. Der vollständige finale Build `17f2169` besteht sämtliche Prüfungen. Kein Test wurde deaktiviert.

**Visuell tatsächlich kontrolliert:** echte MAIN-150%-Aufnahme mit korrigierter Headerzeile, einheitlichen Footer-Tasten und dunklen 10 Zeilen; SONG-150%-Aufnahme mit deutlich zurückgenommenen Leerzellen; Trennfugen per pixelgenauem Vorher-/Nachher-RGB-Vergleich; 44 aktuelle Zoom-/Menü-Snapshots werden weiterhin im CI reproduzierbar erzeugt. **Nicht** behaupten, dass jedes potenzielle GUI-Rendering in Studio One manuell bestätigt ist.

### Verbleibende offene kommerzielle Freigabepunkte

- **G04 ist weiterhin BLOCKER:** Studio One auf dem Eigentümer-Rechner: tatsächliche Windows-Save-Dateiauswahldialoge, alle vier MIDI-/WAV-Exporte einschließlich Reimport, LIMIT ON/OFF-Abhören, ASIO-Dauerstabilität, Zoom/Windows-DPI, Projekt-SAVE/CLOSE/REOPEN und vollständiger State-Recall. **NOT RUN**.
- **G07/G09:** historisches, ungenutztes Badge im Package und oberstes leeres `moduleinfo.json.Name`; vor Veröffentlichung separat auf Verwendungszweck bzw. tatsächliche DAW-Anzeige prüfen, nicht blind ändern.
- **G08:** aktuelle QA-basierte Orientierung in `CURRENT.md` ergänzt; V1.0.0-Releasekandidat und nicht veröffentlichte V1.0.1 sind weiterhin getrennt.
- Owner-Sichtabnahme des aktuellen 150%-Renderings aus dem neuen Lauf steht aus. Das bereits akzeptierte 125A-Logo bleibt unangetastet.

**Endentscheidung dieses Software-/GUI-Korrekturdurchgangs:** **G02/G03/G05/G10 geschlossen; V1.0.1 Windows QA PASS.** Weiterhin **KEINE V1.0.1 FINAL- oder kommerzielle Releasefreigabe** ohne G04 und Eigentümer-Abnahme.
