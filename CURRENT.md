# 125A Drumator — authoritative current status

## Drumator V1.5.0 RC1 – private Studio-One-Abnahme (04.11.2026)

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
