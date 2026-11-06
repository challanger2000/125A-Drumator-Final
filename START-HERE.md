# START HERE – 125A Drumator V1.5.0

## AKTUELL: GUI-Feinschliff 06.11.2026 – neuer geprüfter V1.5.0-Gumroad-Paketstand

- **Source/Branch:** `develop/beat-creator-10-styles-20261102`; GUI-Produktänderung `e1106fa321d6ca75de6baf2fc0b5ea55a2074527`; vollständig geprüfter Quell-/Teststand `3f1e82c99e716071486fa73b81c3e4f062400276`.
- **Windows QA:** [Actions #42179212802](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42179212802) **SUCCESS**: 33/33 CTests inklusive nativer Original-Steps-Pixelparität, 47/47 Steinberg Validator, unabhängiger 125A Plugin Tester v0.3.2 mit **58 PASS / 0 WARNING / 0 FAIL**.
- **VST3-Binärdatei:** Windows x64, SHA256 `33bcf9e5d270192ba55cf211e63f1efa004f6e5230d33bc4090ad6b300e189be`. Byteidentisch mit Windows-CI-Artefakt.
- **Neueste komplette Kunden-ZIP:** ChatGPT Library `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_GUI_Update_2026-11-06.zip`; ZIP SHA256 `d308ed84700cfed3392d0cc3bd6a9c31451145fde13711c481737b07d707b217`; 17.444.371 Bytes, 24 Mitglieder. ZIP CRC PASS, sämtliche 23 Dateihashes in SHA256SUMS.txt kontrolliert.
- **Nur GUI:** DRUMATOR-Wortmarke 90% des ursprünglichen Fitted-Renders plus schwacher 10%-Offset-Reliefschatten; VOL/PAN/PITCH 40px zentriert mit 40x40 passenden Maushitflächen; Preview-Kopfhörersymbole 20% kleiner und dezenter, unveränderte 37x32 Buttonflächen. **Originale 17x34 Steps und MIDI/DSP/Swing/Export/Beat-Creator/State unverändert.**
- **Paketinhalt:** Die 16 VST3-Ressourcen, Kit-Preset mit 8 WAV, DE/EN-Handbücher und Installationstexte sind bytegleich mit dem vorigen Original-Steps-Paket. Nur VST3-DLL und SHA256SUMS.txt unterscheiden sich.
- **Release-Grenze:** Dieses neue GUI-Paket ist in der Library, aber **ein Upload/Ersetzen auf Gumroad ist NICHT bestätigt**. Echter Studio-One-Praxistest mit der neuen DLL ist **NOT VERIFIED**; Windows-CI ersetzt nicht die Eigentümer-DAW-Abnahme. Altes V1.5.0-Kundenpaket nicht überschreiben oder als neuen GUI-Stand ausgeben.

**Vorheriger Ausgangsstand 06.11.2026: Die ursprünglichen, kantigen 17×34-Step-Tasten aus dem vom Nutzer gezeigten ERSTEN Screenshot sind wieder 1:1 im aktuellen V1.5-Code enthalten.** Dies ist ein neues geprüfte Kundenpaket, aber eine nachträgliche Aktualisierung des bereits bestehenden Gumroad-Angebots wurde **noch NICHT** vom Eigentümer bestätigt.

- **Vorheriger Original-Steps-Stand:** `develop/beat-creator-10-styles-20261102`, Produktcode `92c2e412d78b4685f0527f8fb8e31571735b3bb3`; Test-HEAD `f95cc5d5d09bd142f1f55799791eb1c1bf5b427f`.
- **Quelle:** [CURRENT.md](CURRENT.md) → komplette Baseline, ZIP, SHA, bisherige Store-Version und offene Gates. [Original-Steps Release-QA](docs/V15-ORIGINAL-STEPS-20261106.md).
- **Windows QA:** [#42174832595](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42174832595) **SUCCESS**: 33/33 CTests (inkl. originaler Pixel-Parität), 47/47 Steinberg Validator, 58 PASS / 0 WARN / 0 FAIL Plugin Tester.
- **Vorheriges geprüftes Gumroad-Kundenpaket:** `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip` in ChatGPT Library; SHA256 **`33d40c5125818b87d41b525aa8e277303722b619058ca631e7b6e305195403f2`**, 17.444.393 Bytes, 24 Dateien.
- **VST3 SHA256:** `04ad26caa0ee1abe3985b90860d7e23da85043d59d963c0b9097c9623bfff7e5`. Eigentümer-Kit SHA256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`, 8 WAVs. Handbücher DE/EN und restliche Assets unverändert.
- **Vorher von Eigentümer am 04./05.11. hochgeladene V1.5.0:** ohne originalen Step-Restore; alter Code `286be54c`, DLL SHA `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`. Alter ZIP-SHA aus Library `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401`. Archiv: `/125A-Drumator/Archiv-RC1-RC4/125A_Drumator_v1.5.0_Gumroad_VOR_OriginalSteps_2026-11-06.zip`.
- **Kein bestätigtes Gumroad-Update mit neuer 06.11-DLL.** Nach Upload durch den Eigentümer kann der Shop aktualisiert markiert werden. Bis dahin die aktuelle ZIP in der Library und die ältere Shop-Fassung unterscheiden.
- **`main`** bleibt alte 1.0.1, alter `develop/v1.5.0` ist divergiert. Nicht mergen oder Source zurücksetzen.
- **Unverändert:** Complexity +/-5 (Default 35); EINE SMF Typ-1 MIDI-Datei mit 17 Tracks und 960 PPQ; Swing 50% 2:1; Crash BC59; Beat Creator 10 Styles; Footer `VERSION 1.5` korrekt.
- **Keine versteckten Feature-Änderungen:** exakt zwei Produktquelldateien (Paint + passende Step-Hit-Geometrie) und zwei Regressionstestdateien. Vorherige GUI-Builds nicht als neuester Stand ausgeben.
- **DAW-Scope:** frühere V1.5 in DAW geöffnet, neues Pixel-Update echte DAW-Abnahme noch **NOT VERIFIED**. Windows-CI nicht als Studio-One-Test ausgeben.
