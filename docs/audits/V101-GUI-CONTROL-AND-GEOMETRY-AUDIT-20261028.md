# 125A Drumator V1.0.1 — vollständiger GUI-Bedien-/Geometrie-Audit

**Datum:** 28.10.2026  
**Repository:** `challanger2000/125A-Drumator-Final`  
**Branch:** `develop/v1.0.1-gui-redesign`  
**Status:** Technischer GUI-Abnahmekandidat — **keine V1.0.1-Veröffentlichung**. Die vollständige native Menü-/SONG-/Wave-Vollmatrix hat den ersten Windows-CI-Lauf bestanden; die danach entdeckten SONG-Popup-/Ctrl-Reset-Korrekturen werden separat im abschließenden Windows-CI-Lauf validiert.

## 1. Verbindliche Quelle und Grenzen

- Verbindliche Regeln: `125A-Engineering/START-HERE.md`, `STANDARDS/GUI-GUIDELINES.md`, `STANDARDS/PROFESSIONAL-PLUGIN-BEHAVIOR.md`, `QA/RELEASE-QA.md`; 125A-Branding als Quelle für die Logo-Geometrie.
- `AGENTS.md` und `CURRENT.md` des Final-Repositories weisen **V1.0.0** als bisher Windows-geprüften Produktkandidaten aus. Original-Produktstand `a14d9e67470ad0af94a8250baca13fa3a15fb70e`; Final-Paket-QA `#40432613982`. **Öffentliche Veröffentlichung/Gumroad bisher nicht freigegeben.**
- V1.0.1 ist ein **GUI-spezifischer Entwicklungszweig**; `main` und V1.0.0 wurden durch diesen Audit nicht geändert. Keine Änderungen an Audio-Engine, MIDI-Sequencer, Kit-Dateiformat, Parameter-IDs oder VST3-ABI.
- Es werden der **echte VST3-Editor** via `Controller::createView`, ein Windows-`IPlugFrame` und die tatsächlich genutzten Controller/Processor-Pfade getestet; dies ist **kein** Studio-One-Test und keine automatische Designfreigabe durch den Besitzer.
- Der Steinberg Validator prüft Host-Verträge, **nicht** die Vollständigkeit einzelner Menüs. Die unabhängigen 125A-Plugin-Tester-Fälle prüfen weitere Plugin-Protokolle, aber ersetzen keine per-Button-Evidenz.

## 2. Physikalische Geometrie: eine Quelle für Zeichnung und Mausklicks

| Bereich | Vertrag | Geometriequelle | Prüfung |
|---|---|---|---|
| Haupt-GUI | 1280 logische px; 158px Kopf; zehn 48px-Zeilen; 48px Fuß; normale Höhe 686px | `UiV101Layout.h` | C++-Layout-/Host-Resizetests |
| Pattern A–H | 8 × **35×32px**, 39px Pitch / 4px Spalt | `patternButton(i)` | alle acht Auswahlpfade |
| Große Kopftasten | MENU, BARS, SONG, SONG EDIT jeweils **83×32px**, 8px Zwischenraum | `patternMenu`, `barsButton`, `headerGroups` | einheitliche Klick-/Zeichen-Rechtecke |
| Zoom | 80×28px, 100/120/130/150%; Popup-Zellen mit eigener Geometrie | `zoomTrigger`, `zoomOption` | Klicks, Host-Akzeptanz und Ablehnung |
| Instrument | M/S/FILL, Sample-Navigation/Name/Menu, PLAY, EDIT; exakt getrennte X-Zonen | `UiV101Layout::rowHit` | native Mausklicks plus Bridge |
| Steps | 32 × 17px, 19px Pitch / 2px Leerspalte; 16 Instrumente | `stepRect`, `rowHit` | Klick/Raster/1-Bar-Grenze |
| Regler | VOL/PAN/PITCH plus SWING/HUMANIZE/MASTER, inklusive Ctrl-/Doppelklick-Reset | `knobRect`, `headerKnob` | Main-/SONG-Maustest, feine Drag-Gesten |
| VEL/SET | Nur eine geöffnete 48px-Erweiterung; sechs SET-Tasten 112×32px und Echtzeit-Zufallswert | `UiV101Interaction::settingsRect` | Copy/Paste/Clear/Random, Minus/Plus/Drag |
| Fuß | ADD/REMOVE ROW je 90×25px; PREV/NEXT PAGE je 95×25px | `footerAdd`, `footerRemove`, `footerPrev/Next` | Host-Resize/Seitenwechsel/Rollback |
| SONG-Navigation | PREVIOUS/NEXT jeweils **100×26px**, Abstand 8px | `UiSongLayout.h` | Seiten 1/2, Grenzen |
| SONG-Editor | ADD ENTRY/REMOVE LAST je **98×26px**, Abstand 8px | `UiSongLayout.h` | 1..64, automatische Selektion/Clamp |
| SONG-Pattern | A–H je 42×26px (46px Pitch), REST als eigene 84×26px-Aktion | `UiSongLayout::patternChoiceAt` | A–H/REST vollständig |
| SONG-Länge | 1/2 BAR, 1 BAR, 2 BARS je 100×26px, Abstand 6px | SONG-Painter und alter Controller-Hitvertrag | alle drei |
| SONG START | – / + / RESET je 26px hoch; BAR-Anzeige **nicht klickbar** | `UiSongLayout` und SONG-Painter | plus/minus/reset, Lesefeld inert |
| Wave-Kopf | acht identische Tasten **86×32px**, 8px Zwischenraum | `UiV101WaveLayout::toolbar` | alle acht UI-Befehle |
| Wave-Gruppen | vier blaue Rahmen: Zoom/Fit, Preview/Stop, Reset, Cancel/Apply | `toolbarGroups` | keine Überdeckung aktiver Tasten |
| Wave-Werte | fünf gleichgroße 140×46px LCD-Felder: START/END/FADE IN/FADE OUT/GAIN | `UiV101WaveLayout::readout` | Position/Abstände und Echtwerte |

**Wichtig:** Einheitliche Breite gilt innerhalb einer tatsächlichen Funktionsgruppe, nicht über funktionsfremde Bereiche. Haupt-/Wave-Bedientasten verwenden 32px Höhe, kompakte SONG-Tasten 26px und die Fußtasten 25px wegen der jeweils unterschiedlichen Einbauhöhen.

## 3. Vollständige Funktionspfad-Matrix

`tests/integration/V101FullGuiAudit.inl` testet ab jetzt im nativen VST3-Editor unter Windows, nicht nur in einem isolierten Layoutmodell:

| Funktion/Unterfunktionen | Ablauf/positive Evidenz | Negativ-/Randfall und vorhandene Tests | Freigabestatus |
|---|---|---|---|
| PATTERN A–H (8) | jede Taste verändert `currentPattern`, Automationshost empfängt `performEdit` | tote Gaps, nicht aktive Steps nach 1 Bar | **CI ausstehend für neue Vollmatrix** |
| PATTERN MENU (7) | COPY/PASTE, CLEAR/UNDO, RANDOM VELOCITY, VELOCITY 100 und DUPLICATE prüfen alle tatsächliche Notes/Werte | Undo-/Clipboard-Zustand und Blockierung bei offenem Menü | **CI ausstehend** |
| FILL (8) | sämtliche Menüindizes mit der verbindlichen mathematischen \`filledRow(RowFill)\`-32-Step-Referenz verglichen, inklusive Ghost-Notenwerten | nur Zielreihe; 1-Bar-Inaktivität separat geprüft | **CI ausstehend** |
| Output (16) | sämtliche Werte MAIN, OUT 1..15 durch echte Menüauswahl und `rowOutput` | Rechtecke in beiden Paginierungshöhen | **CI ausstehend** |
| Choke (5) | OFF und 1–4 über echte Menüauswahl und `rowChoke` | korrekte Zielzeile | **CI ausstehend** |
| SAMPLE MENU (3) | REMOVE auf korrekter Zeile, RENAME öffnet echtes `CTextEdit`; LOAD ruft den VSTGUI-Dateidialog | OS-Dateidialog wird nicht blind automatisiert; vorhandene direkte WAV/FLAC-Importtests | **LOAD-Dialog: NOT RUN**, andere Optionen **CI ausstehend** |
| SET (6) | OUTPUT/CHOKE, COPY/PASTE/CLEAR/Random Velocity; Realtime Minus/Plus/Drag | fehlende Copy-Vorlage, Grenzwerte 0/100 und reale Realtime-Steuerung | **CI ausstehend** |
| Instrument M/S, Sample PREV/NEXT, PLAY, EDIT | VST3-Controller/Processor, PLAY-Queue bit, Drum-WAV-Editor; getrennte Schaltflächen | ungültige/fehlende Sampledatei und Browsing-Wrap bestehende Tests | **CI ausstehend für neue PLAY-Queue** |
| Steps und Velocity | 32 Steps, Velocity-Lane, 16 Reihen, 1-Bar-Sperre, Edit-/Undo-/Host-Rollback | Leerlücken, ungültige Positionen, Host-Resize-Ablehnung | **bestehende Windows-CI PASS** |
| Mixer/Global | VOL/PAN/PITCH, SWING/HUMANIZE/MASTER, Clip-Reset; Ctrl/double-click Defaults und Shift-Feineinstellung | Ctrl-Reset im SONG-Modus zuvor abweichend, **im Audit behoben** | **CI ausstehend für SONG-Reset** |
| FOOTER & ZOOM | ADD/REMOVE 1..16, Seiten-Navigation, Zoom-Menü-Host-Resize | bei verweigertem Resize alter Zustand wiederherstellen | **bestehende Windows-CI PASS** |
| SONG (9 + 3 + Editieren) | A–H/REST, 8/16/32 Steps, Start +/–/RESET, 16/Seite, ADD/REMOVE, Seiten-PREV/NEXT | 1..64, Auswahl/Seite, kein automatisches +Entry-Tile, Bar-Anzeige inert | **CI ausstehend für neue Matrix** |
| Wave (8) | ZOOM+/–, FIT, PREVIEW, STOP, RESET, CANCEL, APPLY mit echter Wave-/Edit-State und Recall | Handle-Fade, Marker-Drag, Source-PCM-Fade, Zoom, Cancel-Rollback und Stop-vor-Apply | **CI ausstehend für neue Matrix** |
| File drop/import | normale WAV/FLAC-Decoder, PCM-Peaks, Namen, Fehler- und Datei-Swap-Fälle | defekte/trunkierte FLAC, fehlende Dateien, Portabilität/State | **bestehende Windows-CI PASS** |

**Anbindungskette:** VSTGUI-CView / native Mausklickkoordinate → `UiV101Layout/UiSongLayout/UiV101WaveLayout` → `UiV101Interaction` bzw. Modalkontext → `DrumatorView::v101MouseDown/applyV101Menu` → `Controller` → `Processor` (VST3 ConnectionPoint/Messages) → MIDI/Audio/Projektzustand. Die neue Menü-Testmatrix prüft konkrete Zielzustände und exakte Steps statt nur zurückgegebene Erfolgswerte.

## 4. Fehlerfund und Korrekturen während dieses Audits

1. **Falsch beschriftete Sample-Vorschau:** Über dem Kopfhörersymbol stand `PREV`, als wäre es „vorheriges Sample“. Korrigiert zu **PLAY**, passend zur tatsächlichen Audition-Funktion; jetzt zusätzlich überprüfbare Queue im realen Processor.
2. **Ctrl-/Doppelklick-Reset im SONG-Modus:** Die globalen Kopfregler waren sichtbar, aber der SONG-spezifische MouseDown-Pfad umging den Default-Reset. Korrigiert; neue Regression für alle drei Knöpfe.
3. **Scheinbarer SONG-START-Button:** Die Taktanzeige war wie eine aktive Silber-Taste gezeichnet, obwohl ihre Fläche nicht klickbar ist. Als dunkelblaue passive Zahlenanzeige gekennzeichnet; neue Negativkontrolle.
4. **Verdeckte Header-Popups im SONG-Editor:** Die SONG-Faceplate übermalte die bereits gezeichneten Pattern-/Zoom-Popups, insbesondere die letzte Zoom-Option; außerdem wurden Menüauswahl-Klicks bei y≥158 an die darunterliegenden SONG-Bedienelemente geleitet. Die Popups werden jetzt nach der SONG-Faceplate **zuoberst neu gezeichnet**, und ein aktiver Popup-Zustand bekommt beim Maus-Dispatch **Vorrang vor dem SONG-Editor**. Ein echter Windows-VSTGUI-Pixeltest und ein Negativtest auf Klickdurchgriff wurden ergänzt.\n5. **Fehler im neuen Pixel-Testorakel:** Der erste vollständige Sonderfall-Lauf #40613295263 brach bei `SONG board visually hides Pattern popup option` ab. **Ursache des Tests:** Er prüfte den exakten Mittelpunkt der silbernen Menü-Taste, an dem der bewusst dunkel gezeichnete Text `COPY` liegt; der Test verwechselte die schwarze Schrift mit einer verdeckten Fläche. Korrigiert wird das durch Abtasten der unbelasteten Satinfläche links vom Glyphen; zusätzlich wird die tatsächliche Menüart per `IntegrationAccess::v101Popup()` abgefragt. Ein neuer Lauf ist zum Nachweis erforderlich.\n6. **Bisher unzureichende Menü-Tiefe:** Vor der erweiterten Matrix wurden insbesondere viele Pattern-, Fill-, Output- und Choke-Optionen nicht einzeln im neuen CView getestet. Regressionen ergänzt. Ergebnisse nur nach CI als PASS bewerten.

## 5. Ausstehende externe Freigabeprüfungen

| Prüfung | Ort | Status |
|---|---|---|
| Komplette GUI-Menümatrix am tatsächlich gebauten Windows-VST3 | GitHub Actions Windows | **PASS #40612433257**, erweiterte SONG-Popup-Sonderfälle **PENDING #40613295263** |
| Unabhängiger 125A Plugin Tester und Steinberg Validator des gleichen Kommits | GitHub Actions Windows | **PASS #40612433257**, abschließender Commit **PENDING #40613295263** |
| Echte VSTGUI-Pixelbilder bei 100/130% mit anschließendem Eigentümer-Designreview | Windows-Offscreen und menschliche Sichtprüfung | **PENDING** |
| Studio One 8: laden, schließen, wieder öffnen, 100/120/130/150% Zoom, echte Dialoge, Maus-D&D und Projekt speichern/laden | Eigentümer-PC / DAW | **NOT RUN in CI** |
| Native Windows-Dateiauswahl: Datei wählen und Dialog abbrechen | Echtbetrieb | **NOT RUN in automatisierter Menümatrix** |
| macOS GUI/Host-Test | Externe Mac-Hardware | **NOT RUN** (kein Freigabeanspruch) |

Die letzte Studio-One-Abnahme und ein owner-reviewed Screenshot sind **keine** automatisierten CTests und dürfen nicht stillschweigend als bestanden gelten.

## 6. Durchlauf- und Commit-Evidenz

- Vorheriger geprüfter GUI-Stand: [Windows Actions #40610557293](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40610557293), **24/24 CTests, 47/47 Validator, 58 PASS / 0 WARN / 0 FAIL**. Dieser Run enthält **nicht** die nachfolgenden Audit-Fixes.
- Vollmatrix-Test: [Windows Actions #40612433257](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40612433257), **SUCCESS** — 24/24 CTests, 47/47 Steinberg Validator, unabhängiger 125A Plugin Tester **58 PASS / 0 WARNING / 0 FAIL**. Die zwei neuen nativen Fälle \`V101 total menu matrix and per-key geometry PASS\` und \`V101 SONG and Wave full native button matrix PASS\` sind in den echten Windows-Integrationslogs bestätigt. Dieser Run enthält noch nicht die spätere SONG-Popup-Korrektur.\n- Finaler Korrektur-Audit: [Windows Actions #40613295263](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40613295263), Trigger-Commit \`500d8fef98\`, **PENDING** bis sämtliche Jobs abgeschlossen sind. Produktseitige Folge-Commits nach dem Trigger: keine; Workflow wurde anschließend nur auf manuelles Auslösen zurückgesetzt.
- Quell-/Testdateien: `src/vst3/UiV101NativePainter.h`, `src/vst3/UiV101SongPainter.h`, `src/vst3/DrumatorView.cpp`, `tests/integration/V101FullGuiAudit.inl`, `tests/integration/WindowsIntegration.cpp`.
- Version bleibt **V1.0.1 Development**. **Keine** Freigabe, kein Merge, keine Änderung von V1.0.0.
