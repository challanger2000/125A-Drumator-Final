# START HERE – 125A Drumator V1.5.0

**NEU 06.11.2026: Die ursprünglichen, kantigen 17×34-Step-Tasten aus dem vom Nutzer gezeigten ERSTEN Screenshot sind wieder 1:1 im aktuellen V1.5-Code enthalten.** Dies ist ein neues geprüfte Kundenpaket, aber eine nachträgliche Aktualisierung des bereits bestehenden Gumroad-Angebots wurde **noch NICHT** vom Eigentümer bestätigt.

- **Aktuell:** `develop/beat-creator-10-styles-20261102`, Produktcode `92c2e412d78b4685f0527f8fb8e31571735b3bb3`; Test-HEAD `f95cc5d5d09bd142f1f55799791eb1c1bf5b427f`.
- **Quelle:** [CURRENT.md](CURRENT.md) → komplette Baseline, ZIP, SHA, bisherige Store-Version und offene Gates. [Original-Steps Release-QA](docs/V15-ORIGINAL-STEPS-20261106.md).
- **Windows QA:** [#42174832595](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42174832595) **SUCCESS**: 33/33 CTests (inkl. originaler Pixel-Parität), 47/47 Steinberg Validator, 58 PASS / 0 WARN / 0 FAIL Plugin Tester.
- **Neues geprüftes Gumroad-Kundenpaket:** `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip` in ChatGPT Library; SHA256 **`33d40c5125818b87d41b525aa8e277303722b619058ca631e7b6e305195403f2`**, 17.444.393 Bytes, 24 Dateien.
- **VST3 SHA256:** `04ad26caa0ee1abe3985b90860d7e23da85043d59d963c0b9097c9623bfff7e5`. Eigentümer-Kit SHA256 `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`, 8 WAVs. Handbücher DE/EN und restliche Assets unverändert.
- **Vorher von Eigentümer am 04./05.11. hochgeladene V1.5.0:** ohne originalen Step-Restore; alter Code `286be54c`, DLL SHA `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`. Alter ZIP-SHA aus Library `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401`. Archiv: `/125A-Drumator/Archiv-RC1-RC4/125A_Drumator_v1.5.0_Gumroad_VOR_OriginalSteps_2026-11-06.zip`.
- **Kein bestätigtes Gumroad-Update mit neuer 06.11-DLL.** Nach Upload durch den Eigentümer kann der Shop aktualisiert markiert werden. Bis dahin die aktuelle ZIP in der Library und die ältere Shop-Fassung unterscheiden.
- **`main`** bleibt alte 1.0.1, alter `develop/v1.5.0` ist divergiert. Nicht mergen oder Source zurücksetzen.
- **Unverändert:** Complexity +/-5 (Default 35); EINE SMF Typ-1 MIDI-Datei mit 17 Tracks und 960 PPQ; Swing 50% 2:1; Crash BC59; Beat Creator 10 Styles; Footer `VERSION 1.5` korrekt.
- **Keine versteckten Feature-Änderungen:** exakt zwei Produktquelldateien (Paint + passende Step-Hit-Geometrie) und zwei Regressionstestdateien. Vorherige GUI-Builds nicht als neuester Stand ausgeben.
- **DAW-Scope:** frühere V1.5 in DAW geöffnet, neues Pixel-Update echte DAW-Abnahme noch **NOT VERIFIED**. Windows-CI nicht als Studio-One-Test ausgeben.
