# Drumator 1.5 – standalone Drumkit Browser

The Beat Creator creates **rhythms only**. Its A/B/C PREVIEW plays exactly the loaded sounds; absent samples remain silent. APPLY changes only the generated patterns.

## User drumkit format

`.125akit` is a **standalone embedded-audio drumkit**: a valid `KitCodec` portable stream. The kit browser uses its **samples, display names, file names and sample trims only**. The loader does **not** replace project pattern A–H, Song Chain, current pattern, groove lengths, swing/humanize, rows, output buses, automation values, volume/pan/pitch/mute/solo or master settings.

The files are discovered without relying on any host's preset path:

```
125A Drumator.vst3/
  Contents/
    x86_64-win/125A Drumator.vst3
    Resources/
      Samples/
        START-HERE.txt
        01_Rock.125akit
        02_Techno.125akit
        ...
```

The package deliberately contains **no** inherited electronic, 808, acoustic or generated factory WAV kits. User-approved real recordings can be added later to `Samples`.

## Creating a kit from WAV files

Build the `drumator_kit_pack` executable and run:

```
drumator_kit_pack "C:\MyDrums\Rock" "Rock.125akit"
```

The folder may contain `01_Kick.wav`, `02_Snare.wav`, ... through `16_AnyLabel.wav` (one WAV per instrument). The row number selects the slot; unassigned slots stay silent. PCM WAV or supported float WAV is embedded and never fetched externally. Put the resulting kit into `Contents/Resources/Samples` and click **KITS** in the Drumator footer to browse, select and **LOAD**.

`*.vstpreset` is deliberately not required for internal selection; renaming a VST3 preset to `.125akit` does not convert it. A VST3 preset may include A–H patterns and project state, so blindly loading it as a kit could discard the current groove. Conversion from an exported preset should be a separate verified operation once a real example is available.

## Status/QA

BC33/BC34/BC35/BC36 are implementation milestones. Build/test results must be attached to the current head before this feature is called verified. The sold version 1.0.1 on `main` is not modified.
