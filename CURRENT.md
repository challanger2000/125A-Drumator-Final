# 125A Drumator — authoritative current status

**BC65 – verbindliche Nutzerentscheidung vom 04.11.2026:** Beat Creator Complexity verwendet die ursprüngliche Schrittweite **+/− 5 Prozentpunkte**, Startwert **35 %** (35 → 40 → 45); an 0/100 wird begrenzt. Direkteingabe 0–100 bleibt möglich. Frühere +/− 10-Angaben in RC2-/RC3-Berichten sind **historisch und überholt**. MIDI-Export/Drag bleibt auf dem originalen Funktionsstand ohne die unbeauftragte Änderung. Swing, DRUMATOR-Schriftzug und „VERSION 1.5“ bleiben unverändert.

## Verbindlicher Stand nach Rücknahme des unbeauftragten MIDI-Umbaus – 04.11.2026

**Release Candidate – keine Freigabe:** MIDI-Export und MIDI-Drag sind *wortgleich auf den letzten geprüften Stand* `c6556014c1ff1d7e9322e8ba386a8f4b9ac614cf` zurückgestellt. Der Drag verwendet wieder die bislang in Studio One funktionierende **eine Format-1-MIDI-Datei mit getrennten Instrumentenspuren**. Die nachfolgenden RC3-Aussagen über Mehrdateien-Drag und neue per-row-Save-As-Ausgabe sind **historisch und ungültig für den jetzigen Code**.

**Unverändert bewahrt:** SW05 echter 2:1-Achtel-Swing bei 50 %, BC64 Complexity +/- 10, Standard-GUI-Wortmarke bereits auf `kTitleReduction=0.64` (20 % kleiner als der vorige Faktor 0.80), und die kleine untere Beschriftung `VERSION 1.5` in der gemeinsamen MAIN/SONG-Fußleiste. Die GUI-Anforderungen waren also bereits umgesetzt; ein zweites Schrumpfen wäre falsch.

**Prüfpflicht:** Die Rücknahme wird erst bei erfolgreich abgeschlossenem Windows-Build, 33/33 CTests, Steinberg Validator 47/47 und 125A Plugin Tester 58/0/0 als technisch regressionsgeprüft betrachtet. Studio One/ASIO muss vor FINAL tatsächlich separat geprüft werden. `main` und die veröffentlichte 1.0.1 sind unangetastet. **RC3-Multi-Drag-Test-ZIP nicht verwenden.**

---
## RC3 MIDI-Multi-Drag – neuer privater V1.5-Prüfstand (04.11.2026)

**Aktueller geprüfter Windows-VST3-Code:** `e22e65517e3e61d5e55e29b0ee7e34ea9b856a3c` auf `develop/beat-creator-10-styles-20261102`. **GitHub Actions [#41916990564](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41916990564): SUCCESS – 33/33 CTests, 47/47 Steinberg Validator, unabhängiger 125A Plugin Tester v0.3.2: 58 PASS / 0 WARNING / 0 FAIL.** Windows-Nativtest F06 überprüft drei physisch getrennte MIDI-Dateien bis zur echten VSTGUI-/OLE-`CF_HDROP`-Darstellung mit `Win32DataObject`, deren Reihenfolge, Aufbewahrung und Schutz vor fremden Dateien. Das ist mehr als ein MIDI-Encoder-Test, aber **keine Ausführung von Studio One**.

**Private RC3-Test-ZIP erstellt und integritätsgeprüft:** `125A_Drumator_v1.5.0_RC3_MIDI_MultiDrag_StudioOne_Test.zip` (25 Dateien, SHA-256 `879406474e68400052dc3c5e28357c71c1cf494caaa5928f62c1daa5a289a67d`). Enthält genau das von GitHub Actions #41916990564 geprüfte VST3 (DLL-SHA256 `8a3f1571cbb497b04a54a0a15041574946aa18e7e4bae47584c51fd0634d8e28`), das unveränderte Besitzerpreset `125A Drumator Kit.vstpreset` (SHA-256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`), korrigierte PDF-Handbücher DE/EN, Startanleitungen, Host-Kompatibilitätscheck und Dateiprüfsummen. GitHub Build-Artefakt: `125A-Drumator-M11-Full-Kit-Stress-QA`, Artefakt-ID `13365826473`. Die frühere RC2-ZIP gehört zum alten Quellstand und soll nicht mehr für V1.5-Abnahme verwendet werden.

**Funktionsstand:** SW05 Swing 50 % = 2:1-Achtel-Triolenverhältnis, BC64 Complexity +/- 10 Punkte mit Startwert 35, F03 Save As getrennte MIDI-Dateien je musikalisch belegter Instrumentenzeile und F06 **auch MIDI-Drag mit mehreren getrennten Dateien** als eine Windows-Mehrdateienübergabe (kein einzelnes 17-Spuren-MIDI, kein Directory-Drag). WAV-Drag bleibt Einzeldatei. Das Handbuch erläutert die Bedienung und das enthaltene Kit. [RC3-Detailnachweise](docs/RC3-MIDI-MULTIDRAG-QA-20261104.md).

**Freigabe weiterhin HOLD:** Der DAW-spezifische Effekt eines Mehrdateien-Drops auf Spuren/Clips und die Besitzerabnahme sind **NOT RUN**; Sample-Weitergaberechte noch ungeklärt. Kein FINAL, kein Release-Tag, keine Gumroad-Aktualisierung, keine Änderung an `main`/V1.0.1.

---

## Historischer Entwicklungskandidat nach RC2 – 04.11.2026 (KEINE Verkaufsfreigabe)

**Aktueller Quellcode:** `a1d76cec42a9caa56a7cfd9d18756c2771daa3df` auf `develop/beat-creator-10-styles-20261102`. **Windows Actions #41913685096: SUCCESS – 33/33 CTests, 47/47 Steinberg Validator, unabhängiger 125A Plugin Tester v0.3.2: 58 PASS / 0 WARNING / 0 FAIL.** [Beleglauf](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41913685096). Die vorgeschaltete Swing-/Complexity-Reparatur `c6556014` bestand dieselben drei Gates in [#41912269901](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41912269901).

**Behobene Auditbefunde:** SW05: Swing 50 % liefert ein echtes 2:1-Achtel-Triolenverhältnis (belegt durch mathematisch unabhängige 0/35/50-%-Timingorakel und tatsächliche VST3-Audioausgabe); BC64: Beat-Creator-Complexity +/- 10, ab Start 35, mit Grenzwertklemmung und direkter Zahleneingabe; F03: **Save As MIDI** schreibt einen neuen Ordner `<gewählter Name> - MIDI` mit je einer MIDI-Datei pro musikalisch belegter Instrumentenreihe, gültigem Tempo und eigener Instrumentenspur. Der bestehende **MIDI-Drag bleibt vorerst bewusst beim kompatiblen Typ-1-Ein-Datei-Format**; eine echte Studio-One-Abnahme für Mehrdateien-D&D existiert nicht. Details, Nachweis-/Testmatrix und offene Grenzen: [docs/RC2-POSTAUDIT-QA-20261104.md](docs/RC2-POSTAUDIT-QA-20261104.md).

**WICHTIG:** Die zuvor bereitgestellte `125A_Drumator_v1.5.0_RC2_Swing_Crash_StudioOne_Test.zip` ist noch aus Quellcommit `9d2eea4e` erstellt und enthält **NICHT** SW05, BC64 oder den getrennten Save-As-MIDI-Export. **Noch keine neue VST3-Test-ZIP erstellt oder an den Nutzer ausgehändigt.** Es fehlt außerdem die Anpassung der V1.5-PDF-Handbücher an diese Änderungen und der finale autorisierte Studio-One-Praxiseindruck (44,1 kHz/24 Bit). Die Sample-Weitergaberechte müssen vor jeglichem Verkauf geklärt werden. Kein Gumroad-Upload, kein FINAL-Tag und keine Änderung am veröffentlichten `main`/1.0.1 ohne Besitzerfreigabe.

---

## Historischer Stand: Drumator V1.5.0 RC1 – private Studio-One-Abnahme (04.11.2026)

Die V1.5-Entwicklung bleibt ausschließlich auf `develop/beat-creator-10-styles-20261102`. Der gepinnte VSTGUI-NV01-Fix wurde vom unabhängigen Codex-Nachaudit auf `a30d72c` akzeptiert. Die bislang letzte CI bestand **33/33 CTests, 47/47 Validator, 58/0/0 Plugin Tester**: https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41801233263.

**RC1 ist kein öffentliches Release.** Version `1.5.0` ist die Steinberg-VST3-Binary-/Factory-/moduleinfo-Version; die Kennzeichnung `RC1` gehört in die Paket- und Build-Info. Das laufende Windows-VST3-Releasepaket muss erneut auf genau dem RC-Commit geprüft werden. Keine Änderungen an `main`, V1.0.1, Release-Tags oder Gumroad ohne ausdrückliche Freigabe.

**Verbindliche RC1-Kundenbeilage:** das vom Besitzer bereitgestellte Preset `125A Drumator Kit.vstpreset` (letzter Upload `125A Drumator Kit(2).vstpreset`, SHA-256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`), sowie die aktualisierten Handbücher Deutsch und Englisch, beide mit explizitem Hinweis auf das Kit. Preset nicht gegen ältere 482-KB-Version austauschen. Private Praxistests mit den eingebetteten Samples sind möglich; die Weitergaberechte vor öffentlicher Freigabe klären.

**Abnahme in Studio One (Besitzer, Windows, 44,1 kHz/24 Bit):** Plugin als Instrument öffnen, Preset laden und alle acht Sounds tatsächlich hören; Beat Creator zehn Stile und A/B/C, Complexity/Undo; Velocity-Gesten; 16/32 Steps; SONG/REST/Loop und Patternanzeige; MIDI-/WAV-Export inkl. Tempotiming und Sample-Tails; Multi-Out; Projekt speichern, DAW neu starten, Recall und Preset; GUI öffnen/schließen, zwei Fenster; Undo, LIVE/WAV-Editor und PREVIEW STOP. Ergebnisse protokollieren. Nach bestandener Abnahme erst endgültige Freigabe anfragen.


> **AKTUELLER ENTWICKLUNGSSTAND – 03.11.2026:** V1.5-Entwicklung ausschließlich auf `develop/beat-creator-10-styles-20261102`; `main` und die veröffentlichte 1.0.1 bleiben unverändert. **Exakter letzter geprüfter Code-Commit: `4f8c272c4a4cf739fe797ca26e4a68453623f8f2`.** [Windows Actions #41730408421](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41730408421) **SUCCESS: 32/32 CTests, 47/47 Steinberg Validator, 58 PASS / 0 WARNING / 0 FAIL im unabhängigen 125A Plugin Tester v0.3.2.** Der Codex-Read-only-Audit vom 03.11. identifizierte einen SONG-A/B/C-Rückfallpfad, Lücken in RE09/Mutationsprüfungen sowie kleinere GUI- und Grenzwertprobleme. Diese wurden auf dem Entwicklungsbranch korrigiert; der deterministische Processor-Race-Test **BT01**, die vollständige PCM-Präfix-Mutation **RM30**, die SONG-Button-Rückmeldung **REL09** und die strengeren Mutationstests einschließlich **RM16** bestehen im aktuellen Lauf. Beat Creator: zehn elektronische Genres / 80 Motive, A/B/C-Gruppentransaktion mit Undo, 16/32 Schritte, direkt editierbare Complexity; Velocity-Zeichgesten vorhanden. **Keine mitgelieferten Samples und kein Kit-Browser.** Der aktuelle Code hat **noch keinen abschließenden Besitzer-Praxistest in einer echten DAW** und ist **nicht zur Gumroad-Veröffentlichung freigegeben**. Die jeweils aktuelle Übergabe steht in [START-HERE.md](START-HERE.md). Verifiziertes 1.0.1-Verkaufs-ZIP: `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip` mit SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`; dieses Produkt bleibt unverändert.

---

**Historischer Stand der veröffentlichten Version 1.0.1 (31. Oktober 2026):** F21-Footer-Korrektur und geprüftes Kunden-ZIP. Die folgenden Angaben betreffen ausschließlich diese unveränderte Verkaufsfassung; für den aktuellen V1.5-Entwicklungsbranch gilt der oben angegebene SHA und CI-Lauf.

## Authoritative source and build

- **Historische 1.0.1-Basis:** `main` bleibt die veröffentlichte 1.0.1. Der gesonderte V1.5-Branch hat inzwischen abweichenden Quellcode. Keine überholten F18/F19/F20-ZIPs für Kunden verwenden.
- **Exact product code verified:** `3748b1e9e1867357362304c51bec2cb99ac9657b`.
- **[Windows Actions #41157069202](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41157069202): SUCCESS.** 29/29 CTests, Steinberg VST3 Validator 47/47, independent 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- **Product DLL SHA256:** `06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278`.
- **Updated complete customer ZIP:** `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`; 26 files; SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`. Prepared outside the repository and validated for archive integrity and all per-file hashes. Both illustrated DE/EN PDFs use the actual new GUI screenshot.
- **Storefront status:** The previous 1.0.1 Gumroad package was confirmed uploaded by the owner on 30 October 2026. This new corrected ZIP **must replace that file in Gumroad**; upload by the owner has not been verified or performed here.

## Final GUI corrections within 1.0.1

- **F20 (verified in owner's Studio One screenshot):** Regular instrument pages are nonoverlapping: 1–10 then 11–16, or only row 11 for an 11-row kit. Normal 10-row viewport keeps its 686px logical height across NEXT/PREV. Unoccupied areas are inactive background. Constrained peer-window recovery remains intact.
- **F21:** Footer bezel shifted exactly 4 logical pixels left, from x16..1264 to **x12..1260**, matching the row chassis. Footer stays 1248px wide. All button rectangles, interaction hitboxes, songs, DSP and window size logic remain untouched.
- **Visual regression:** Pixel difference between prior F20 and F21 actual Windows renders is confined to the footer bezel region (130% main, 16-row second page, and 11-row second page); there are no other changed pixels.
- **Earlier 1.0.1 work retained:** PLAY/FILL/EDIT order, corrected headings and colors, aligned right-header controls, Logo aligned to PATTERN A, 66 stable automation parameters.
- **Version decision:** **1.0.1**. No 1.0.2, no new release tag.
- **QA scope:** Windows automated host/native render CI and independent plugin validation passed. The owner's previous Studio One check covers F20. A fresh manual DAW check of this F21 DLL was not performed; F21 only moves the decorative footer panel.

## Superseded items (historical only)

- F18 corrected a photo crop but did not solve DAW resizing.
- F19 used overlapping 2–11/7–16 pages and was rejected.
- Earlier `125A_Drumator_1.0.1_Next_Page_10_plus_6_Test.zip` and `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` are superseded for distribution.
- Older audit reports remain only as historical evidence; README and this file identify the current build.
