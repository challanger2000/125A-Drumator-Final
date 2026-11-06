> **Historische Release-Übergabe vom 05.11.2026:** Beschreibt die zuvor veröffentlichte V1.5.0 **vor** dem Original-Steps-Update. Die neueste am 06.11. vollständig geprüfte Quell-/ZIP-Fassung steht in [V15-ORIGINAL-STEPS](V15-ORIGINAL-STEPS-20261106.md). Das neue ZIP wurde erstellt, aber dessen Austausch im Gumroad-Shop ist noch **NICHT bestätigt**.\n\n# 125A Drumator V1.5.0 – Release-Übergabe und offener Gate-Status

**Stand 05.11.2026:** Windows-FULL-Kundenpaket erstellt und technisch getestet. Besitzer bestätigte das Öffnen in seiner DAW und bestätigte anschließend ausdrücklich, dass das ZIP **bei Gumroad hochgeladen ist**. Besitzerbestätigter Upload: ABGESCHLOSSEN. Unabhängiger Shop-Dateiabruf und vollständige Host-Interaktion: noch NICHT VERIFIZIERT.

## FULL versus Demo / Quelle

| Kriterium | Bestehendes FULL V1.0.1 | Neuer FULL-Kandidat V1.5.0 | Demo |
| --- | --- | --- | --- |
| Source | main f4c5caa4be82626ea89459d9835c12138d7c6a1b | develop/beat-creator-10-styles-20261102, letzter Produktcode 286be54c463fb09a488d68e0fff288b85d17b6aa | keine verifizierte V1.5 Demo |
| Build/QA | Windows #41157069202 dokumentiert, 29/47/58 | Windows #41932222861: 33/33 CTests, 47/47 Steinberg, 58/0/0 125A Tester, inklusive AUD-F07 | NOT RUN |
| Package | aktualisierte 1.0.1-ZIP vorbereitet; Originalangebot am 30.10. vom Besitzer als veröffentlicht bestätigt | 125A_Drumator_v1.5.0_Gumroad_Paket.zip in ChatGPT Library /125A-Drumator/Aktuell-v1.5.0/ | unbekannt |
| Vollständiger ZIP SHA256 | vorbereitete aktualisierte 1.0.1 ZIP cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb | Library d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401; Chat-Download a4423f857fc345ade35fa496fc241f61df22c81c07033a5c01e933966886ccc8. Gleiche 24 ZIP-Einträge byteidentisch, nur Metadaten unterschiedlich. | — |
| VST3 SHA256 | 06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278 | 9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b | — |
| DAW und Shop | F20 Studio-One-Praxistest früher bestätigt; Update-Auslieferung unklar | DAW **geöffnet** (owner-reported); Upload **abgeschlossen** (owner-confirmed); tatsächlichen Shop-ZIP-Hash noch nicht unabhängig geprüft | — |

Die ältere Branch develop/v1.5.0 ist stark vom neueren Beat-Creator-Code divergiert, daher NICHT als Release-Quelle übernehmen, mergen oder löschen.

## Kritische Anforderungen und End-to-End-Kennzeichnung

| Vertrag / Gate | Umgebung und Evidence | Status |
| --- | --- | --- |
| Windows VST3, Validator, unabhängiger 125A Tester | GitHub Actions #41932222861, echte gepackte VST3-Binärdatei | PASS – Windows CI |
| Beat Creator Complexity 35 und +/-5, Crash BC59, SW05 Swing | Quell- und separate UI/MIDI/WAV/PCM-Regressionsprüfungen | PASS – Windows CI |
| MIDI Export/Drag eine SMF-Typ-1-Datei, 17 Spuren | nativer Windows-CF_HDROP-Test AUD-F07 + Export | PASS – Windows CI |
| Versionsgravur MAIN/SONG, keine Kollision mit Controls/Status | native View-Renderings und Layout-Assertions | PASS – Windows CI |
| Kundenpaket mit 8 WAVs im Besitzer-Kit, DE/EN PDFs, 24 ZIP-Mitgliedern | ZIP-CRC, Member SHA-256 und exakte DLL SHA | PASS – Kundenpaketdateien |
| V1.5-Plugin in realer DAW öffnen | Besitzerbericht am 04.11.2026 | PASS – DAW-Start vom Besitzer bestätigt |
| DAW: Abspielen, Beat Creator A/B/C, MIDI-/WAV-Drag, Save/Reopen, Automationsrückruf, Zoom, sämtliche Buttons, Busse | tatsächliche vollständige DAW-Sitzung; CI-Host allein nicht ausreichend | NOT VERIFIED – nicht automatisch FAIL |
| Gumroad-Upload V1.5.0 | ausdrückliche Aussage des Eigentümers nach Upload | **PASS – owner-confirmed upload completed** |
| Exakte ausgelieferte Shop-ZIP-Bytes/SHA | tatsächlicher externer Shop-Download/Hash | **NOT VERIFIED** (getrennt vom bestätigten Upload) |
| Freigabe der eingebetteten 8 Beispiel-WAVs und finaler EULA/Support-Texte | Owner/Rechteinhaber | NOT VERIFIED |
| Neue GitHub-main-Fassung / offizielles Release-Tag | Owner-Release-Sync aller Dokumente nach Freigabe | NOT DONE; main bleibt 1.0.1 |

## Optionaler GitHub-Abgleich nach bestätigtem Shop-Upload

Der Uploadabschluss **ist vom Eigentümer bestätigt**, nicht erneut danach fragen. Für einen unabhängigen technischen Shop-Abgleich müssten die tatsächlich ausgelieferten ZIP-Bytes geprüft werden; Owner-Hostabnahme und Rechte/Support getrennt behandeln. Erst danach ein kontrollierter GitHub-Merge mit genau diesem Source/Binary-Baseline-Nachweis und synchronen Updates in AGENTS.md/CURRENT.md, 125A-Engineering RELEASE-REGISTER.md/RELEASE-STATUS.md sowie dem GitHub Release. Keine eigenmächtigen Änderungen am MIDI/DSP/Beat Creator oder an der divergierten develop/v1.5.0-Branch. Ein weiterer GitHub Actions Build wird nicht allein wegen dieser Dokumentationsaktualisierung ausgelöst.
