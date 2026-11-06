# 125A Drumator – verbindlicher Einstieg für Coding-Agents

**Pflicht:** Zuerst [125A-Engineering/START-HERE.md](https://github.com/challanger2000/125A-Engineering/blob/main/START-HERE.md) und anwendbare Standards/Guardrails lesen, dann [CURRENT.md](CURRENT.md), [START-HERE.md](START-HERE.md) und [Original-Step-Restore-Nachweis](docs/V15-ORIGINAL-STEPS-20261106.md). Jede Datei, jeder Commit und jedes Paket vor weiteren Schreiboperationen prüfen.

## AKTUELL: zusätzlich geprüfter GUI-Feinschliff (06.11.2026)

- **Source/Branch:** `develop/beat-creator-10-styles-20261102`; GUI-Produktänderung `e1106fa321d6ca75de6baf2fc0b5ea55a2074527`; vollständig geprüfter Quell-/Teststand `3f1e82c99e716071486fa73b81c3e4f062400276`.
- **Windows QA:** [Actions #42179212802](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42179212802) **SUCCESS**: 33/33 CTests inklusive nativer Original-Steps-Pixelparität, 47/47 Steinberg Validator, unabhängiger 125A Plugin Tester v0.3.2 mit **58 PASS / 0 WARNING / 0 FAIL**.
- **VST3-Binärdatei:** Windows x64, SHA256 `33bcf9e5d270192ba55cf211e63f1efa004f6e5230d33bc4090ad6b300e189be`. Byteidentisch mit Windows-CI-Artefakt.
- **Neueste komplette Kunden-ZIP:** ChatGPT Library `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_GUI_Update_2026-11-06.zip`; ZIP SHA256 `d308ed84700cfed3392d0cc3bd6a9c31451145fde13711c481737b07d707b217`; 17.444.371 Bytes, 24 Mitglieder. ZIP CRC PASS, sämtliche 23 Dateihashes in SHA256SUMS.txt kontrolliert.
- **Nur GUI:** DRUMATOR-Wortmarke 90% des ursprünglichen Fitted-Renders plus schwacher 10%-Offset-Reliefschatten; VOL/PAN/PITCH 40px zentriert mit 40x40 passenden Maushitflächen; Preview-Kopfhörersymbole 20% kleiner und dezenter, unveränderte 37x32 Buttonflächen. **Originale 17x34 Steps und MIDI/DSP/Swing/Export/Beat-Creator/State unverändert.**
- **Paketinhalt:** Die 16 VST3-Ressourcen, Kit-Preset mit 8 WAV, DE/EN-Handbücher und Installationstexte sind bytegleich mit dem vorigen Original-Steps-Paket. Nur VST3-DLL und SHA256SUMS.txt unterscheiden sich.
- **Release-Grenze:** Dieses neue GUI-Paket ist in der Library, aber **ein Upload/Ersetzen auf Gumroad ist NICHT bestätigt**. Echter Studio-One-Praxistest mit der neuen DLL ist **NOT VERIFIED**; Windows-CI ersetzt nicht die Eigentümer-DAW-Abnahme. Altes V1.5.0-Kundenpaket nicht überschreiben oder als neuen GUI-Stand ausgeben.

## Vorherige Versions- und Gumroad-Zuordnung (Original-Steps vor GUI-Feinschliff)

- **Aktuell vom Eigentümer als hochgeladen bestätigtes Windows-x64-VST3 FULL:** **125A Drumator 1.5.0**, exakter Gumroad-ZIP-Dateiname `125A_Drumator_v1.5.0_Gumroad_Paket.zip` (unabhängig tatsächlich ausgelieferter Shop-ZIP-Hash UNBEKANNT). Die am 04./05.11. hochgeladene Fassung entspricht dem **vorherigen** geprüften Produktcode `286be54c463fb09a488d68e0fff288b85d17b6aa`, DLL `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b` und vorbereiteten Library-ZIP SHA `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401`.
- **Vorherige technisch geprüfte FULL-Updatefassung (noch NICHT als neu hochgeladen bestätigt):** V1.5.0 mit exakt wiederhergestellten Original-Steps, Code `92c2e412d78b4685f0527f8fb8e31571735b3bb3`, CI-/Test-Source `f95cc5d5d09bd142f1f55799791eb1c1bf5b427f`. Windows QA [#42174832595](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42174832595) **33/33, 47/47, 58 PASS / 0 WARN / 0 FAIL**. DLL SHA `04ad26caa0ee1abe3985b90860d7e23da85043d59d963c0b9097c9623bfff7e5`.
- **VORHERIGES Kunden-Gumroad-ZIP für das Update:** `125A_Drumator_v1.5.0_Gumroad_Paket.zip` in ChatGPT Library `/125A-Drumator/Aktuell-v1.5.0/` SHA `33d40c5125818b87d41b525aa8e277303722b619058ca631e7b6e305195403f2`, 24 Mitglieder. Der vorige ZIP ist als `125A_Drumator_v1.5.0_Gumroad_VOR_OriginalSteps_2026-11-06.zip` unter `/125A-Drumator/Archiv-RC1-RC4/` gesichert. **Bei Gumroad selbst NOCH NICHT als ersetzt bestätigt.**
- **Historisches stabiles `main`:** 1.0.1 bei `f4c5caa4be82626ea89459d9835c12138d7c6a1b`. Die alte `develop/v1.5.0`-Branch ist stark divergiert, **nicht als heutige V1.5-Quelle verwenden**.

## Zu bewahrende Verträge

- Original-Step-Zeichnung: **exakt** die vorherigen alten 17×34-Hardwaretasten aus `DrumatorView.cpp::drawHardwareStep`, nur vertikale Verschiebung auf `row+7..41` in 48px-Zeilen. 19px Pitch, passende 17×34-Hitflächen, keine anderen GUI-Elemente blockieren.
- Complexity startet 35 und ändert sich +/- **5** (nicht 10).
- MIDI Save As/Drag: **EINE** Standard-MIDI-Datei SMF-1, 17 Tracks, 960 PPQ. Kein RC3-Mehrdateien-Umbau.
- Crash BC59, Swing 50% 2:1, Logo-Größe und Footer `VERSION 1.5` unverändert.
- Nicht ungefragt DSP/MIDI/Beat Creator/Velocity/State ändern. Bei allen Eingriffen Requirements→UI→Controller→Engine→Audio/MIDI→Recall prüfen.
- Kein Release-Tag, `main`-Merge oder Shop-Upload durch den Agent ohne ausdrücklich passende Owner-Freigabe. Ein grün automatisierter Windows-Build ersetzt keinen realen DAW-Test.

**Verbindlicher detaillierter Status:** [CURRENT.md](CURRENT.md). Frühere Statusdokumente liegen in der Git-Historie.
