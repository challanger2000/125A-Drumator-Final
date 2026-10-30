# 125A Drumator – Benutzeranleitung V1.0.1

**Version 1.0.1.** Die aktuelle deutsch- und englischsprachige bebilderte PDF-Kundendokumentation liegt im separat vorbereiteten Verkaufspaket. Diese Quelle beschreibt die Funktionen host-neutral; die sichtbaren Regler sind englisch beschriftet.

## Was ist Drumator?

Ein VST3-Drum-Sampler für Windows mit Step-Sequencer. Er ist auf **One-Shot-Samples** ausgelegt: Kick, Snare, Hi-Hat, Clap, Percussion und kurze Effektklänge.

**One-Shots: ja. Synchronisierte Drumloops: nein.** Auch längere WAV- und FLAC-Dateien lassen sich importieren und abspielen. Drumator besitzt aktuell jedoch **keine automatische BPM-Erkennung, kein Loop-Tempo-Matching und kein Time-Stretching**. Ein importierter Drumloop passt sich daher nicht automatisch dem Tempo der DAW an.

## Sample laden und bearbeiten

1. VST3-Instrument in der DAW öffnen.
2. WAV/FLAC auf die Instrumentenzeile ziehen oder auf das Sample-Feld klicken, um eine Datei auszuwählen.
3. Mit dem **EDIT**-Button direkt neben dem Sample-Feld den Sample-Editor öffnen. EDIT ist nur bei tatsächlich vorhandenem Sample aktiv.
4. START und END durch Ziehen setzen, FADE IN / FADE OUT mit den Griffen bearbeiten; die angezeigte Stereo-Wellenform zeigt den aktuellen Fade direkt an.
5. GAIN verändert den Sample-Pegel (100 % = Originalpegel). Doppelklick oder **Strg + Linksklick** setzt GAIN auf 100 % zurück.
6. **PREVIEW** hört die Bearbeitung vor; **STOP** stoppt ausschließlich diese Vorschau, nicht den DAW-Transport.
7. **APPLY** übernimmt die Änderung, **CANCEL** verwirft sie. Die Bearbeitung verändert während der Vorschau nicht die gespeicherte WAV-Datei auf der Festplatte.

**Knackser vermeiden:** START manuell setzen und bei Bedarf einen kurzen FADE IN verwenden. Es gibt weder AUTO START noch ZERO SNAP.

**WAVE-Marker:** Ein Klick nahe der vorhandenen END-Markierung greift END; auf freier Wellenformfläche setzt ein einfacher Klick standardmäßig START. **Shift + Linksklick** auf freier Fläche setzt END. Dies ist die bestehende Bedienweise, keine neue Funktion.

## Instrumente: Velocity und BUS-Zuordnung (V1.0.1)

Über **VEL** unter der jeweiligen Instrumentenzeile lassen sich die Velocity-Balken der Steps direkt bearbeiten. **RANDOM VEL** verändert einmalig die Velocity vorhandener Steps **nur dieses Instruments** im ausgewählten Pattern; **RESET VEL** stellt die vorhandenen Steps derselben Instrumentenzeile auf **100** zurück. Leere Steps bleiben leer. In 1-BAR-Ansicht bleibt der nicht sichtbare zweite Takt unberührt. Beide Eingriffe sind über PATTERN → UNDO rückgängig zu machen.

**LIVE VEL** steuert dagegen eine nichtdestruktive Anschlagstärkenvariation während der Wiedergabe (0–100 %). Gespeicherte Step-Velocities bleiben dabei unverändert. Das Feld lässt sich ziehen; die Tasten −/+ verändern den Wert.

Im Bereich **SET** befindet sich die BUS-Zuordnung, die als **BUS 1** (MAIN) oder **BUS 2–16** angezeigt wird. Eine BUS-Auswahl legt das Audiosignal einer Instrumentenzeile auf den gewählten Stereo-VST3-Bus; sie **aktiviert im Host nicht automatisch** die zugehörige Ausgabespur. Für separate Drum-Kanäle die entsprechenden Instrumentenausgänge (DRUM 02 bis DRUM 16) **in der jeweiligen Hostanwendung aktivieren**. Solange ein zusätzlicher Bus nicht aktiv ist, kann Drumator das Signal auf MAIN ausgeben. Die BUS-Auswahl wird mit dem Projekt gespeichert.

**Wichtig:** BUS-Menü und 16-Bus-Audio-Routing wurden automatisiert geprüft. Die Aktivierung zusätzlicher Stereo-Ausgänge hängt von der jeweiligen Hostanwendung ab.

## Pattern und Tempo

Die Step-Schritte werden zum musikalischen Transport der DAW synchronisiert. In der Hauptansicht **1 BAR** oder **2 BARS** wählen. Die Buttons haben identische Abmessungen und verändern nur die Länge des aktuellen Patterns. Achtung: Dies ist **keine Tempoanpassung importierter Audiodateien**.

Die Pattern A–H lassen sich separat auswählen und bearbeiten. Das Pattern-Menü bietet COPY, PASTE, CLEAR, UNDO, RANDOM VELOCITY, VELOCITY 100 und DUPLICATE. Die Song-Ansicht ist ein eigener Modus; Pattern-Längen und Song-Einträge sind getrennt.

## Mischen und Ausgänge

Die Instrumentenzeilen bieten VOL, PAN, PITCH, Mute, Solo, Choke, Fill und Output-Zuordnung. Zusätzliche VST3-Ausgänge müssen in der DAW aktiviert werden, sofern sie dort für das Instrument angeboten werden. Die Sample-Preview-Wiedergabe ist nicht mit dem DAW-Transport identisch.

## Verbindliche Funktionsgrenzen

- SONG START und BAR-Zuordnung sind in V1 für **4/4** (16 Sechzehntel pro Takt) ausgelegt; andere Taktarten/Taktwechsel sind nicht validiert.
- Drumator stellt **66 automatisierbare VST3-Parameter** bereit: Pattern A–H, je 16× Volume (0–200 %), Pan (−100 bis +100), Mute (OFF/ON) und Pitch (−24 bis +24 Halbtöne) sowie den Master-Pegel (0–200 %). Die IDs bleiben unabhängig von Pattern und Ansichtsseite stabil.
- Für den eingebetteten Projekt-Samplezustand gelten technische Obergrenzen von **16 MiB WAV-Daten je Instrument** und **64 MiB WAV-Daten insgesamt**. Dateien vor der endgültigen Speicherung entsprechend testen.

## Projekt-Recall und Bedienhinweise

Gespeichert werden Samples, Steps/Patterns, SONG-Kette, Mix-/Ausgangsdaten, Limiter sowie mit APPLY bestätigte Sample-Edits. Auch der Pitch-Zustand ist Teil des Recalls. LIVE-Entwürfe ohne APPLY bleiben temporär. Nach einem Update ein vorhandenes Projekt schließen und erneut öffnen sowie Wiedergabe und Automation kontrollieren.

Automatische Tests der aktuellen Version einschließlich der 66 Parameter und fünf unabhängigen Fehlergegenproben sind bestanden; eine Verkaufsfreigabe hängt zusätzlich von der Kundenpaket- und Rechteprüfung ab. Es gibt keinen Rechtsklick-Zwang für grundlegende Funktionen.

Technische GUI-Vorgaben: [VSTGUI UX Blueprint](VSTGUI-UX-BLUEPRINT-20261023.md).
