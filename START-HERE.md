# START HERE – 125A Drumator / verbindliche Projektübergabe

**Update 04.11.2026 – RC3 MIDI-MULTI-DRAG (NEUER aktueller Windows-Prüfstand):** Produktcode `e22e65517e3e61d5e55e29b0ee7e34ea9b856a3c`, GitHub Actions [#41916990564](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41916990564) **SUCCESS (33/33 CTests, Steinberg 47/47, unabhängiger Tester 58/0/0)**. F06 liefert bei MIDI-**Drag** jetzt separate MIDI-Dateien über eine einzige Windows-`CF_HDROP`-Dateiliste. Windows-Integrationstest validiert echte OLE-Dateipfade, Tonspuren-Reihenfolge und Lebensdauer der Dateien. F03 Save As MIDI mit separaten Dateien, SW05 echter 2:1-Swing und BC64 Complexity +/- 10 sind enthalten. Privates Paket `125A_Drumator_v1.5.0_RC3_MIDI_MultiDrag_StudioOne_Test.zip` geprüft (SHA-256 `879406474e68400052dc3c5e28357c71c1cf494caaa5928f62c1daa5a289a67d`, 25 Dateien), mit exakt passender VST3, dem originalen Besitzerpreset und korrigierten DE/EN-PDF-Handbüchern. [Prüfmatrix](docs/RC3-MIDI-MULTIDRAG-QA-20261104.md). **Kein Studio-One-Praxistest und keine Verkaufsfreigabe; `main` bleibt veröffentlichte 1.0.1.**

**Historische Notiz 04.11.2026 – NACH RC2-AUDIT:** Die verifizierte V1.5-Korrekturbasis steht bei `a1d76cec42a9caa56a7cfd9d18756c2771daa3df`, Windows Actions [#41913685096](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41913685096) **SUCCESS: 33/33 CTests, Validator 47/47, Plugin Tester 58/0/0**. SW05 korrigiert 50-%-Swing zu 2:1; BC64 setzt Complexity-Schritte auf 10; F03 exportiert beim Speichern separate MIDI-Dateien in einen neuen Ordner, während MIDI-Drag weiterhin das bisherige einzelne Type-1-SMF nutzt. Genauer QA-Vertrag: [docs/RC2-POSTAUDIT-QA-20261104.md](docs/RC2-POSTAUDIT-QA-20261104.md). **Das zuvor erzeugte RC2-ZIP gehört zu `9d2eea4e` und enthält diese Reparaturen NICHT.** Keine neue Kunden-ZIP und noch keine Studio-One-Abnahme, kein Gumroad-Update, keine Freigabe. `main` bleibt die unveränderte 1.0.1-Verkaufsbasis.

**Historische Notiz 04.11.2026 – RC1-Vorbereitung:** Siehe den neuen obersten Abschnitt in [CURRENT.md](CURRENT.md). Vor öffentlich zugänglicher Freigabe: letztes vom Besitzer geliefertes `125A Drumator Kit.vstpreset` (3.1 MB), beide V1.5-Handbücher DE/EN, echte Studio-One-Abnahme und Sample-Weitergaberechte prüfen. Ältere Aussagen in diesem Dokument über „keine Kits“ betreffen den **Plugin-internen** Sample-Fallback und den ursprünglichen Stand; das optionale Preset wird als **separate Kundenbeilage** mitgeliefert. 1.0.1 bleibt unberührt.
Stand: 03.11.2026. **Zuerst dieses Dokument und CURRENT.md lesen, bevor Dateien angefordert, Builds erstellt oder Änderungen vorgeschlagen werden.**

## 1. Verbindliche Verkaufsfassung – NIEMALS RÄTSELN
- **Original-Verkaufs-ZIP des Nutzers:** `125A_Drumator_v1.0.1_Gumroad_Final.zip`
- Bereits vorhandene inhaltlich **byteidentische** Archive in der ChatGPT-Dateibibliothek:
  `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`,
  `Drumator_1.0.1_Gumroad_NEU.zip`.
- **Alle drei ZIP-SHA256:** `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`.
- **Exakte geprüfte 1.0.1-Quellrevision:** `3748b1e9e1867357362304c51bec2cb99ac9657b`.
- `main` auf `f4c5caa4be82626ea89459d9835c12138d7c6a1b` enthält danach nur Dokumentationsänderungen.
- **VST3-Binary SHA256:** `06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278`.
- **Windows Release QA:** [Actions 41157069202](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41157069202): 29/29 CTests, 47/47 Steinberg Validator, 58/58 Plugin Tester.
- Die korrigierte 31.-Oktober-Verkaufs-ZIP enthält 26 Einträge inklusive VST3, DE/EN-Handbücher, START-HERE-DE.txt, Prüfsummen und BUILD-INFO.txt.
- Die ältere ZIP `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` ist **überholt**.
- **NICHT MEHR FRAGEN, OB DIE GUMROAD-ZIP FEHLT.** Bereits in der Datei-Bibliothek und vom Nutzer erneut hochgeladen. Falls sie im Container einer künftigen Sitzung nicht gemountet ist, mit `files.list` in der Library nach diesem exakten Namen suchen und `files.materialize` nutzen. Nicht raten und nicht sofort erneut hochladen lassen.
- Hauptquellen: [CURRENT.md](CURRENT.md) und [README.md](README.md) auf `main`. Die zuletzt geprüfte 1.0.1 ist die technische Basis; die veröffentlichte VST3 und das Gumroad-Paket bleiben unangetastet.

## 2. Vorgeschichte: frühere V1.5-Basis verwerfen
- Der frühere Branch `develop/v1.5.0` enthielt Beat Creator mit 8 Styles sowie Sample-Presets/Preview-Kit, dessen Klang der Nutzer ablehnte.
- Der Nutzer meldete im verworfenen früheren V1.5-Branch einen **WAV-Export, der wesentlich langsamer als das Host-Tempo** lief. Im neuen Entwicklungsbranch wurde der Host-Tempo-Transfer separat repariert und mit nativen Export-/PCM-Regressionsmatrizen geprüft; der Fehler des alten Branches ist deshalb nicht ungeprüft auf den aktuellen Code übertragbar. Ein echter DAW-Import- und Hörtest ist weiterhin erforderlich.
- **Nicht** die komplette V1.5 als Basis nehmen, keine alten Samplekits übernehmen, nicht ungeprüft deren Export/GUI portieren.
- Nur klar isolierbare Verbesserungen wie die Velocity-Zeichengesten sollen gezielt übernommen und gegen 1.0.1 getestet werden.

## 3. Neues Projekt – aktiver Branch
- Branch `develop/beat-creator-10-styles-20261102`, von `main` (1.0.1) angelegt.
- **Aktueller Code (Stand 03.11.2026, geprüfter SHA `4f8c272c4a4cf739fe797ca26e4a68453623f8f2`):** Beat Creator mit STYLE-Dropdown für 10 Genres, 1/2-BAR-Umschaltung, Complexity mit direkt editierbarer Zahl, sichtbarer A/B/C-Schrittvorschau, GENERATE/APPLY/CONFIRM/CANCEL und unveränderter 8-Instrumente-Legende. Controller- und Processor-Anschluss übernehmen A/B/C gemeinsam per bereits in V1.5 bewährter, atomarer Compare-and-Swap-Transaktion; ein gemeinsames Undo ist vorhanden. **Keine Demo-Samples, keine Sample-Kits, kein Kit-Browser.** Alle Änderungen ausschließlich auf diesem Entwicklungsbranch, 1.0.1-Verkaufspaket unverändert. Vollständiger Windows-VST3-QA-Lauf [41730408421](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41730408421) **SUCCESS**: 32/32 CTests, Steinberg 47/47 und unabhängiger Plugin Tester v0.3.2 mit 58 PASS / 0 WARNING / 0 FAIL.
- 10 elektronische Genres, per **einzelnem STYLE-Dropdown**: 80s Synthpop, Italo Disco, Synthwave, EBM, 90s Eurodance, House, Techno, Trance, Hip-Hop, Drum & Bass.
- **80 fest definierte Groove-Motive (8 pro Genre)**; A Hauptgroove, B passende Variation, C halbtaktiger genrebezogener Break; Complexity, Velocity, 1/2 Takte.
- **KEINE mitgelieferten Samples, KEIN Kit-Browser, KEIN Preview-Kit-Button.** Nutzer lädt seine WAV/FLAC separat. Der Creator schreibt nur Steps und Velocity.
- Feste Legende: 01 Kick, 02 Snare, 03 Closed HH, 04 Open HH, 05 Low Tom, 06 High Tom, 07 Crash, 08 Clap. Reihen 9–16 unangetastet.
- Velocity-Tools von der vorherigen Entwicklung (Paint, Shift-Line, Shift+Alt-Curve, Alt-Accent) als **isolierter Algorithmus plus Regressionstests** übernommen. Die GUI-Gestenintegration wurde in BC41 direkt aus dem bewährten V1.5-Code übernommen: `src/vst3/UiV101Interaction.h` ist **byteidentisch** zur V1.5-Implementation, in `src/vst3/DrumatorView.h/.cpp` wurde nur die Weitergabe von Alt/Shift ergänzt. **BC41 und der darauf aufbauende aktuelle Code sind Windows-CI-geprüft; die Besitzer-DAW-Praxisbestätigung für V1.5 fehlt weiterhin.**
- **Aktueller bestätigter Code- und QA-Stand:** `4f8c272c4a4cf739fe797ca26e4a68453623f8f2`, [Actions 41730408421](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41730408421) SUCCESS, alle Jobs erfolgreich. Vorherige rote Zwischenläufe sind überholt. `main` wurde nicht verändert.
- Den Detailvertrag enthält [docs/BEAT-CREATOR-BC40-10-ELECTRONIC-STYLES.md](docs/BEAT-CREATOR-BC40-10-ELECTRONIC-STYLES.md).

## 4. Audit-Reparaturen und nächste Arbeiten
1. **Codex-Audit vom 03.11.2026:** Der SONG-Modus verwendet jetzt für A/B/C einen gesonderten, nur insgesamt aktualisierten Rückfall-Cache einschließlich Pattern-Längen. Der deterministische real-Processor-Test **BT01** hält einen A/B/C-Writer mit ungerader Versionsnummer an und kontrolliert die PCM-Ausgabe und Längen: **PASS**.
2. **Strengere QA:** RE09 misst echte zeitliche Callback-/Import-Überschneidungen und prüft alle Samples kontinuierlicher Blöcke; **RM30** beweist die Erkennung eines beschädigten PCM-Präfixes. Mutationen werden nur durch den erwarteten Fachfehler erkannt, nicht durch beliebige Ausnahmen. Der gezielte WAV-Cache-Mutationstest **RM16** besteht im aktuellen CI.
3. **Kleinere Reparaturen:** Deaktivierte SONG-Add/Remove-Buttons blitzen nicht als bestätigte Aktion auf (**REL09 PASS**); die SOLO-Fade-Berechnung weist übergroße endliche Sampleraten zurück.
4. **V1.5 vor Veröffentlichung:** Besitzer-Praxistest in einer echten Windows-DAW (44,1 kHz/24 Bit, A/B/C/Velocity, MIDI-Patternwahl, Song/REST, Multi-Out, Projekt-Recall, MIDI-/WAV-Export und Loop-/Transportverhalten). Die CI beweist keine vollständige Host-/ASIO-Freigabe. Danach Rechte-, Handbuch-, EULA-/Support- und Kunden-ZIP-Prüfung; Veröffentlichung nur nach ausdrücklicher Zustimmung.
5. **Nicht mehr erneut entwickeln:** Groove-Kern, Style-Dropdown, A/B/C-Transaktion, Velocity-Gesten und die genannten Audit-Reparaturen sind im geprüften Branch vorhanden. Keine unbegründeten neuen CI-Läufe. **`main`, 1.0.1-Gumroad-Paket und Release-Tags nicht ändern.**


## 5. Leitregel
**Nicht den Nutzer wiederholt fragen, was bereits in README/CURRENT/START-HERE oder Library belegt ist. Zuerst nachsehen.** Statusworte `SUCCESS` nur mit konkretem geprüften Build; isolierter Code ist noch kein fertiges Plugin.
