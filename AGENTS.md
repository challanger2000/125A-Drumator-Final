# 125A Drumator – verbindlicher Einstieg für Coding-Agents

**Pflicht:** Zuerst [125A-Engineering/START-HERE.md](https://github.com/challanger2000/125A-Engineering/blob/main/START-HERE.md) und anwendbare Standards/Guardrails lesen, dann [CURRENT.md](CURRENT.md), [START-HERE.md](START-HERE.md) und [Original-Step-Restore-Nachweis](docs/V15-ORIGINAL-STEPS-20261106.md). Jede Datei, jeder Commit und jedes Paket vor weiteren Schreiboperationen prüfen.

## Autoritative Versions- und Gumroad-Zuordnung (06.11.2026)

- **Aktuell vom Eigentümer als hochgeladen bestätigtes Windows-x64-VST3 FULL:** **125A Drumator 1.5.0**, exakter Gumroad-ZIP-Dateiname `125A_Drumator_v1.5.0_Gumroad_Paket.zip` (unabhängig tatsächlich ausgelieferter Shop-ZIP-Hash UNBEKANNT). Die am 04./05.11. hochgeladene Fassung entspricht dem **vorherigen** geprüften Produktcode `286be54c463fb09a488d68e0fff288b85d17b6aa`, DLL `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b` und vorbereiteten Library-ZIP SHA `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401`.
- **Neueste technisch geprüfte FULL-Updatefassung (noch NICHT als neu hochgeladen bestätigt):** V1.5.0 mit exakt wiederhergestellten Original-Steps, Code `92c2e412d78b4685f0527f8fb8e31571735b3bb3`, CI-/Test-Source `f95cc5d5d09bd142f1f55799791eb1c1bf5b427f`. Windows QA [#42174832595](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42174832595) **33/33, 47/47, 58 PASS / 0 WARN / 0 FAIL**. DLL SHA `04ad26caa0ee1abe3985b90860d7e23da85043d59d963c0b9097c9623bfff7e5`.
- **NEUESTES Kunden-Gumroad-ZIP für das Update:** `125A_Drumator_v1.5.0_Gumroad_Paket.zip` in ChatGPT Library `/125A-Drumator/Aktuell-v1.5.0/` SHA `33d40c5125818b87d41b525aa8e277303722b619058ca631e7b6e305195403f2`, 24 Mitglieder. Der vorige ZIP ist als `125A_Drumator_v1.5.0_Gumroad_VOR_OriginalSteps_2026-11-06.zip` unter `/125A-Drumator/Archiv-RC1-RC4/` gesichert. **Bei Gumroad selbst NOCH NICHT als ersetzt bestätigt.**
- **Historisches stabiles `main`:** 1.0.1 bei `f4c5caa4be82626ea89459d9835c12138d7c6a1b`. Die alte `develop/v1.5.0`-Branch ist stark divergiert, **nicht als heutige V1.5-Quelle verwenden**.

## Zu bewahrende Verträge

- Original-Step-Zeichnung: **exakt** die vorherigen alten 17×34-Hardwaretasten aus `DrumatorView.cpp::drawHardwareStep`, nur vertikale Verschiebung auf `row+7..41` in 48px-Zeilen. 19px Pitch, passende 17×34-Hitflächen, keine anderen GUI-Elemente blockieren.
- Complexity startet 35 und ändert sich +/- **5** (nicht 10).
- MIDI Save As/Drag: **EINE** Standard-MIDI-Datei SMF-1, 17 Tracks, 960 PPQ. Kein RC3-Mehrdateien-Umbau.
- Crash BC59, Swing 50% 2:1, Logo-Größe und Footer `VERSION 1.5` unverändert.
- Nicht ungefragt DSP/MIDI/Beat Creator/Velocity/State ändern. Bei allen Eingriffen Requirements→UI→Controller→Engine→Audio/MIDI→Recall prüfen.
- Kein Release-Tag, `main`-Merge oder Shop-Upload durch den Agent ohne ausdrücklich passende Owner-Freigabe. Ein grün automatisierter Windows-Build ersetzt keinen realen DAW-Test.

**Verbindlicher detaillierter Status:** [CURRENT.md](CURRENT.md). Frühere Statusdokumente liegen in der Git-Historie.
