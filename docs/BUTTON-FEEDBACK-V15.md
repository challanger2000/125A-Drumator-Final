# Drumator V1.5 — Button feedback verification matrix (BF01–BF03)

## Binding UI rule

The first indication must be the actual state/action, not a generic mouse-down overlay.

* **Persistent toggles** (ON/OFF, M/S, LIVE, SONG, LIMIT): a latched face and/or
  explicit ON/OFF label, **no** transient amber success overlay.
* **Exclusive selections** (Pattern A–H, 1/2 BAR, SONG Pattern/REST/Length,
  chain entry, Beat Creator style/bar, zoom preset, output/choke choice):
  show the selected value **only**; no generic amber overlay.
* **Mode switches and popup openers** (VEL, SET, SONG EDIT, Beat Creator,
  Sample Edit, MENU, FILL, LOAD, Export, zoom chooser): show the new
  window/menu/expanded state. No success indication before a valid action.
* **Value controls** (SWING/HUMANIZE/MASTER and channel knobs,
  Complexity +/- and direct input, Song Start +/-/RESET, LIVE VEL +/-,
  Sample Gain/markers/scroll, page arrows): show the actual changed value
  or page; no second flash.
* **One-shot actions** may show one short confirmation *only after success*:
  main meter reset, sample audition, VEL random/reset, Beat Creator GENERATE,
  SONG ADD/REMOVE. SET COPY/PASTE/CLEAR use **their own** single pressed sprite
  after success, not the generic overlay.
* **WAVE toolbar (8 keys)** and **popup option cells (43 mapped cells)**:
  physical pressed sprite while held, released on MouseUp/Cancel; no generic
  amber overlay. WAVE LIVE is latched and must never inherit action feedback.
* **Disabled/rejected/no-op controls**: never display success feedback. A
  physical depressed key on mouse-down is not a completion acknowledgement.
  Grey out disabled footer ADD/REMOVE, SONG START -/+/RESET, and
  row EDIT/preview for an empty sample. The associated hit geometry remains
  unchanged; the controller still enforces the actual operation gate.

## Complete surface inventory

| Surface | Control families | Feedback contract |
|---|---|---|
| Main header | Pattern A–H, Pattern Menu, Bars, SONG, SONG EDIT, Beat Creator, LIMIT, Zoom | State/selection/modal only |
| Main global | SWING, HUMANIZE, MASTER and meter reset | Value/meters; reset may acknowledge success |
| Main rows (1–16) | Row select, MUTE, SOLO, Load/Sample name, PREV/NEXT sample, Sample menu, PLAY, FILL, EDIT, 32 step cells, VOL/PAN/PITCH knobs, VEL/SET | State/visible action; PLAY may acknowledge |
| VEL expansion | 32 velocity bars, Random VEL, Reset VEL, LIVE VEL -/value/+, drag shapes | Values/gesture; random/reset acknowledged only on success |
| SET expansion | OUTPUT, CHOKE, COPY, PASTE, CLEAR | Select popup; COPY/PASTE/CLEAR have one success-only pressed sprite |
| Main footer | ADD/REMOVE ROW, PREV/NEXT PAGE, Export | Changed count/page or popup |
| SONG header | Shared main header + SONG EDIT latched | No flash |
| SONG editor | Entry tiles (64), Pattern A–H and REST, 1/2/2-bar selection, Start -/+ /RESET, previous/next page, ADD ENTRY, REMOVE LAST | Selection/readout; successful ADD/REMOVE may acknowledge |
| Beat Creator modal | Style dropdown (10 styles), 1/2 bar, Complexity -/+ and direct edit, GENERATE, APPLY/CONFIRM, CANCEL | Selected values/modal; GENERATE acknowledges after valid draft only |
| WAVE modal | LIVE, Zoom+/Zoom-/FIT, PREVIEW, STOP, RESET, CANCEL, APPLY, Gain and waveform controls | LIVE latched; eight actions native pressed sprite only |
| Popups | Pattern (7 actions), Fill, Output, Choke, Sample, Export; zoom preset choices | Selected state or native press until release, no extra amber overlay |
| Legacy/compact host surface | Main toggles, pattern selection, rows, SONG, sample editor, popup menus | Same semantic state; legacy sample editor retains only its native click outline |

## Implementation changes

1. No call to `recordButtonFlash` before dispatching `onMouseDown`.
2. The remaining flash whitelist is deliberately narrow and invoked only
   *after* the relevant controller action reports success.
3. SONG ADD/REMOVE acknowledges only if the actual chain count changed.
4. SET OUTPUT/CHOKE are not completed edits; only accepted COPY/PASTE/CLEAR
   use the existing `settingsFeedbackUntil_` pressed sprite.
5. WAVE buttons already have a pressed bitmap. Remove the duplicate drawn
   outline in the current GUI; clear the pressed state when closing WAVE.
6. Preserve all existing geometries, renderer textures, on/off labels,
   menu pressed states and musical/audio behavior.

## Automated evidence

* `BC63`: A–H and all header latches never flash; all 43 popup options
  and seven Pattern menu commands use their native pressed sprite.
* `BF01–BF03`: real Main/row/VEL/SET/Beat Creator/WAVE interactions;
  successful action acknowledgement and no duplicates for state changes.
* `REL09`: real SONG ADD/REMOVE success versus disabled count bounds.
* `REL05/06`: disabled paging, LIVE VEL default/drag logic.
* Prior GUI and audio regressions remain mandatory.

The Windows CI result is authoritative for this exact commit, not for earlier
green commits. Any visual manual DAW review remains a separate release check.
