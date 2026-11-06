# V15-ORIGINAL-STEPS – Exakte Wiederherstellung der ursprünglichen Step-Tasten

**Datum 06.11.2026**. **Produktcode:** `92c2e412d78b4685f0527f8fb8e31571735b3bb3`. **Windows Regressionstest SHA:** `f95cc5d5d09bd142f1f55799791eb1c1bf5b427f`. **CI:** [#42174832595](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/42174832595), COMPLETED SUCCESS.

## Expliziter optischer Nutzervertrag

Die Step-Tasten müssen **1:1 aussehen wie im ersten vom Eigentümer gezeigten 1.0.1-Screenshot**, nicht stilistisch nur ähnlich. Die aktuell größere V1.5 mit blau-schwarzem Stahlgehäuse darf ansonsten unverändert bleiben.

Wiederverwendete Referenz: unverändertes `src/vst3/DrumatorView.cpp::drawHardwareStep`, früher ursprüngliche 130px-Zeilen. Exakt dieselben 17×34-Logikpixel, die selben CColor-RGBA-Werte, dieselbe Reihenfolge von Außenfläche, Downbeat-Rahmen, Off-/On-Innenkörper, oberer Reflexkante und Orange-Linse. `src/vst3/UiV101NativePainter.h::stepCap` ersetzt dafür die alten V1.5-Rundungen und Satin-Gradienten. **Einzige geometrische Translation:** altes Zeilen-Y +69..+103 -> kompaktes V1.5 Zeilen-Y +7..+41, *ohne Skalierung*. `src/core/UiV101Layout.h::stepRect` und `rowHit` decken jetzt dieselben real gezeichneten 17×34-Tasten ab, mit 19px Step-Pitch und 2px nichtanklickbarer Lücke. Die 48px-Zeilen und deren übrige Controls unverändert.

Die einzige produktive Dateidifferenz gegenüber dem zuvor von Eigentümer veröffentlichten Stand:
- `src/vst3/UiV101NativePainter.h` – originale Zeichenbefehle statt V1.5-Satin-Keycaps
- `src/core/UiV101Layout.h` – sichtbarer Tastenbereich mit Hitfläche synchronisiert
- Testcode `tests/UiV101LayoutTests.cpp` und `tests/integration/WindowsIntegration.cpp` erweitert; **keine anderen Änderungen an Produktcode**.

## Nachweis und Grenzen

| Prüfung | Resultat |
| --- | --- |
| Echtes Windows x64 VST3 gebaut | PASS |
| CTests | **33/33 PASS** |
| Windows VSTGUI tatsächliche Rasterpixel On/Off/Downbeat/Disabled gegen separat kopierte Original-Zeichenroutine geprüft | **PASS** |
| Step-Paint/Hitbox-Kollisionen, 17×34 und 2px Gutter | **PASS** |
| Steinberg VST3 Validator | **47/47 PASS** |
| Unabhängiger 125A Plugin Tester V0.3.2 | **58 PASS / 0 WARNING / 0 FAIL** |
| Echte Windows MAIN-120% / SONG-150% Renderings | Als Actions-Artefakt vorhanden, visuell geprüft |
| Vollständige Studio-One-Abnahme mit der **neuen** DLL | **NOT RUN** |
| Aktueller Gumroad-Shop mit **neuem** ZIP aktualisiert | **NOT CONFIRMED** |

## Neue Kunden-ZIP, Integrität und alte Verkaufs-ZIP

- **Aktueller Library-Pfad:** `/125A-Drumator/Aktuell-v1.5.0/125A_Drumator_v1.5.0_Gumroad_Paket.zip`, **SHA-256 `33d40c5125818b87d41b525aa8e277303722b619058ca631e7b6e305195403f2`**, 17.444.393 Bytes, 24 Dateien.
- **Neue VST3-DLL:** SHA `04ad26caa0ee1abe3985b90860d7e23da85043d59d963c0b9097c9623bfff7e5`, bytegleich mit dem im Workflow #42174832595 gebauten Paket.
- **Verändert in der Kunden-ZIP:** nur kompilierte DLL + `SHA256SUMS.txt`. Die anderen 22 Einträge (einschl. 16 übriger VST3-Ressourcen, deutsch/englischer PDF-Handbücher, Beispielkit und Installationshinweisen) sind bytegleich.
- **Kit:** `125A Drumator Kit.vstpreset` (8 WAV), SHA `4e76b529b18fa248aaa1a48fe44328f82f10490026f42daf8f85f000fe27c8e6`.
- **Voriges Kunden-ZIP (Eigentümer-Upload 04./05.11.)** als Sicherung: `/125A-Drumator/Archiv-RC1-RC4/125A_Drumator_v1.5.0_Gumroad_VOR_OriginalSteps_2026-11-06.zip` (ZIP SHA `d03532acb58b74d9d2b5c1093cf3413921997a2313509111766e672a509de401`, DLL SHA `9899f9793077ff93db74180175ef75d911223439050ddcaaa3c6a70f56478c6b`).

**Unverändert:** MIDI SMF-1 EINE Datei 17 Spuren/960 PPQ, Velocity 1–127, Complexity 35/+/-5, 10 Beat Creator Styles, SWING/Crash, State, Audio, Song, Export, Branding-Logo, Footer-Version. `main` weiterhin stabile alte 1.0.1. **Neue ZIP muss durch den Eigentümer bei Gumroad noch ersetzt werden**; nicht behaupten, das sei bereits geschehen.
