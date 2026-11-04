# 125A Drumator – Arbeits- und Release-Navigation für Coding-Agents

**Pflicht vor jeder Arbeit:** 125A-Engineering/START-HERE.md und alle relevanten Regeln lesen; danach CURRENT.md und START-HERE.md aus diesem Final-Repository. Release-Dateinamen und grüne CI allein sind keine Shop-Freigabe.

## Autoritativer Stand am 05.11.2026

- **Bisher bestätigte öffentliche FULL-Fassung:** V1.0.1 auf main bei Commit f4c5caa4be82626ea89459d9835c12138d7c6a1b. Dokumentierte aktualisierte 1.0.1-Kunden-ZIP: 125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip (SHA256 cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb), deren konkreter Shop-Upload nicht bestätigt ist.
- **Neueste zum Upload vorgesehene FULL V1.5.0:** Entwicklung auf develop/beat-creator-10-styles-20261102; letzter verifizierter Produktcode-Commit 286be54c463fb09a488d68e0fff288b85d17b6aa.
- **Kundenpaket:** 125A_Drumator_v1.5.0_Gumroad_Paket.zip, Library /125A-Drumator/Aktuell-v1.5.0/. Canonical Library-ZIP-SHA256 d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401. Separat verlinkte Download-ZIP-SHA256 a4423f857fc345ade35fa496fc241f61df22c81c07033a5c01e933966886ccc8. Die beiden ZIP-Container enthalten die exakt gleichen 24 Dateipfade und Inhalte; nur ZIP-Metadaten unterscheiden sich.
- **Verifizierte VST3 SHA256:** 9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b; Windows-CI https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41932222861 SUCCESS mit 33/33 CTests, 47/47 Validator, 58 PASS / 0 WARN / 0 FAIL 125A Tester.
- **Neuester Besitzerbericht:** V1.5 wurde in seiner DAW bereits geöffnet; Gumroad-Upload wird jetzt durchgeführt. Das ist KEIN bestätigter vollständiger DAW-Funktionstest und KEIN bestätigter abgeschlossener Shop-Upload.
- **Bisher nicht abgeschlossen:** Shop-Upload und tatsächlich ausgelieferter ZIP-SHA, vollständige DAW-interaktive Freigabe, Weitergaberechte am Beispielkit und Freigabe einer main/Tag/GitHub-Release-Umstellung. Deshalb main nicht ungefragt ändern.

## Zu bewahrende V1.5-Produktverträge

- Beat Creator Complexity Standard 35, Plus/Minus genau 5, direkte Eingabe 0–100.
- MIDI Save As/Drag genau EINE SMF Typ-1-Datei mit 17 Spuren/960 PPQ; kein verworfener RC3-Mehrdateienumbau.
- BC59 Crash-Frequenzkorrektur, SW05 Swing 50% = 2:1-Achtel und DRUMATOR-Wortmarke 20% kleiner bleiben erhalten.
- V15-FTR1: VERSION 1.5 im MAIN- und SONG-Footer in eigener Zeile, Statusfelder und Hitboxes kollisionsfrei; CI-geprüft.
- Windows x64 VST3 only. Kein Produktcodeumbau während einer read-only Releaseprüfung.

**Achtung:** develop/v1.5.0 ist gegenüber der oben genannten aktuellen V1.5-Quelle stark DIVERGIERT; niemals einfach mergen. Neue Chats lesen docs/V15-RELEASE-HANDOFF-20261105.md und CURRENT.md zuerst.

---

## Historische Contributor-Anweisungen (für stabile 1.0.1, weiterhin gültig soweit nicht durch neueren Stand präzisiert)
# 125A Drumator — contributor and coding-agent rules

**Current project:** Windows x64 VST3 `125A Drumator`, version 1.0.1. Consult [CURRENT.md](CURRENT.md) for up-to-date product SHA, CI and FINAL/RC distinction. Do not trust historical notes.

## Mandatory source of authority

1. Read [125A-Engineering/START-HERE.md](https://github.com/challanger2000/125A-Engineering/blob/main/START-HERE.md) and follow its complete prescribed reading order.
2. Read the [125A-Branding README](https://github.com/challanger2000/125A-Branding/blob/main/README.md) before editing any branded asset.
3. Inspect repository HEAD, branch, the relevant feature tests and GitHub Windows QA before any change.

## Engineering contract

- **Windows 11 x64 VST3 only.** No macOS, Linux or standalone variants.
- Pin Steinberg VST3 SDK to **3.8.1** (tag `v3.8.1_build_84`, commit `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`), with exact tagged submodules. Do not float SDK/VSTGUI revisions.
- Audio thread: no disk I/O, locks, heap allocation, sleeping, unbounded loops or other blocking work.
- Save/recall, fault handling, multiprocessor/editor lifecycle and geometry must be tested for **state transitions and rejected host operations**, not only happy-path button clicks.
- Native VSTGUI renderer and mouse hitboxes must derive from the same layout constants. Follow the dark modern hardware style and keep uniform button dimensions.
- VST3 is built and QA-checked through Windows Actions with CTest, official Steinberg Validator, native GUI/host tests and independent 125A Plugin Tester. A green compiler or validator alone is not a release gate.
- Avoid unnecessary Actions runs, branch proliferation, test-only fallbacks and speculative source modifications. Check the exact SHA before editing; use compare-and-swap ref updates.
- `main` is the current canonical 1.0.1 branch. `develop/v1.0.1-gui-redesign` is kept synchronized as the working branch. Keep both protected from unreviewed code changes; start future changes on a dedicated topic branch and require Windows QA before promotion.
- Product documentation should be accurate for the exact binary. Historical milestone claims are archived in [docs/history](docs/history/).
- **No Gumroad release, FINAL claim or release tag without explicit owner DAW approval, rights review, EULA/support terms and customer ZIP inspection.** Keep internal QA reports, review folders, debug artifacts and logs OUT of customer packages.
