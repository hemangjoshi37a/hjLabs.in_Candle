# CLAUDE.md — hjLabs.in_Candle

> Guidance for Claude Code (and humans) working in this repository.

## What this is

`hjLabs.in_Candle` is a **fork of [Denvi/Candle](https://github.com/Denvi/Candle)** (v11.2),
the Qt/C++ GRBL controller + G-code visualizer for 3-axis CNC machines. This fork,
maintained under the **hjLabs.in** brand, extends Candle from a pure *machine controller*
into an integrated **CAD/CAM workbench** for a hobby/desktop 3-axis machine.

- **Fork (origin):** `https://github.com/hemangjoshi37a/hjLabs.in_Candle`
- **Upstream:** `https://github.com/Denvi/Candle` (remote `upstream`, branch `master`)
- **Local path:** `/home/hemang/Documents/GitHub/hjLabs.in_Candle`

Keep upstream syncable — do not gratuitously rewrite existing controller code. New
functionality should live in **new modules/tabs** beside the existing UI wherever
practical, so we can keep merging upstream fixes.

## Project goals (what we are building)

Two major new capabilities, each as its own **mode / tab / screen** alongside the
existing "Controller" view:

### 1. Wood-carving CAD/CAM mode
A lightweight in-app CAD + CAM workspace for 2.5D wood carving / engraving:
- **DXF import** (entities: lines, polylines, arcs, circles, splines→polyline, text).
- **Basic CAD primitives** drawable in-app: line, rectangle/square, circle, arc,
  polyline, and **text** (reuse/extend the single-stroke font approach — see the
  `hjLabs.nc` pen-plot generator already prototyped on this machine).
- **CAM operations**: profile (on/inside/outside), pocket/area clearing, engrave
  (follow path), with tool diameter, step-over, step-down, feeds/speeds, safe-Z.
- **Toolpath → G-code** generation feeding directly into Candle's existing
  visualizer + sender.

### 2. PCB-making mode (isolation routing / drilling)
A wizard-driven workflow to fabricate single/double-sided PCBs on the same 3-axis machine:
- **Gerber import** (RS-274X) for copper layers + **Excellon** for drill files.
- **Processing**: isolation-routing toolpath generation (offset copper boundaries),
  pad/track handling, board cutout, and drill-hit ordering.
- **Wizard / step sequencer** guiding the operator through ordered stages, e.g.:
  1. Set up / align board & zero
  2. Isolation mill the copper (signal) layer
  3. (optional) clear remaining copper / pads layer
  4. Drill holes (Excellon)
  5. Board outline cutout
  Each step produces its own NC program and prompts for tool change / re-zero between stages.

> The two modes share a common internal **geometry model** (2D entities + toolpaths)
> and a common **G-code emitter**. Design that shared core first; build mode-specific
> importers/UI on top.

## Existing architecture (upstream Candle — orient here before changing anything)

Qt 5 (works with 5.15) / C++ app, **CMake** build (`project(Candle VERSION 11.2)`).

```
CMakeLists.txt              # top-level; adds the subdirs below
src/candle/                 # main application
  main.cpp                  # entry point
  frmmain.{h,cpp}           # MAIN WINDOW — controller UI, sender, tabs live here
  frmsettings.{h,cpp}       # settings dialog
  frmscript.{h,cpp}         # JS scripting console
  frmhelp / frmabout / frmlog / frmchecklist
  parser/                   # G-code parsing + view model
    gcodeparser.{h,cpp}
    gcodepreprocessorutils.{h,cpp}
    gcodeviewparse.{h,cpp}  # builds drawable line segments
    linesegment.{h,cpp}, arcproperties.{h,cpp}
  drawers/                  # OpenGL visualizers (gcodedrawer, machinebounds, heightmap*)
  widgets/                  # custom UI widgets
  connections/              # serial/transport layer to GRBL
  settings/  styles/  fonts/  shaders/  tables/  utils/  logging/  translations/
src/customwidgets/          # shared Qt designer widgets (built first)
src/candleplugins/          # runtime JS plugins (each: config.ini, script.js, widget.ui, images/)
  camera/ coordinatesystem/ emergencybutton/ usercommands/
src/designerplugins/        # Qt Designer plugins (cameraplugin, customwidgetsplugin)
src/scriptbindings/         # Qt Script (JS) bindings, incl. generated_cpp/
deploy/  help/  wiki/  screenshots/
```

Key facts:
- **G-code visualization** flows: text → `gcodeparser` → `gcodeviewparse` →
  `linesegment`s → `drawers/gcodedrawer` (OpenGL). New CAM output should produce
  G-code that feeds this same pipeline (or build a parallel preview drawer).
- **UI** is Qt Widgets with `.ui` files and `CMAKE_AUTOUIC`/`AUTOMOC` on. The main
  window `frmmain` is the natural place to add a top-level tab/mode switcher.
- **Plugins** are JS + a `widget.ui`, loaded from a plugins dir. Decide per feature
  whether a new capability belongs as native C++ (CAD/CAM engines → yes) or a JS
  plugin (light UI helpers → maybe).
- `vcpkg.json` / `CMakePresets.json` exist for dependency management (vcpkg).

## Build & run

```bash
# from repo root
cmake --preset <preset>          # see CMakePresets.json for available presets
cmake --build --preset <preset>  # or: cmake -S . -B build && cmake --build build
```
The system already has Qt 5.15 dev packages (`qtbase5-dev`, `qt5-qmake`,
`libqt5serialport5-dev` may be needed) and the released v11.2 `.deb` is installed
system-wide for reference. Confirm the exact preset/deps before first build.

## Suggested implementation order (for the next session)

1. **Don't break the controller.** Get a clean build of the unmodified fork first.
2. Add a **mode/tab scaffold** in `frmmain` (e.g. "Controller" | "CAD/CAM" | "PCB")
   with empty placeholder widgets — verify it builds and runs.
3. Build the **shared 2D geometry model** + **G-code emitter** (unit-tested,
   UI-independent).
4. **Wood CAM**: DXF importer → geometry model → CAM ops → emitter → visualizer.
5. **PCB**: Gerber/Excellon importer → geometry model → isolation/drill toolpaths →
   wizard sequencer → emitter.
6. Reuse the existing visualizer/drawers for preview; reuse the existing sender for
   running jobs.

## Conventions & guardrails

- **Match upstream style**: follow the surrounding C++/Qt conventions in `src/candle`
  (naming, indentation, `frm*` for windows, `.ui` for layouts).
- **Keep upstream mergeable**: prefer adding files/classes over editing core ones;
  isolate new code under clear subdirs (e.g. `src/candle/cad/`, `src/candle/cam/`,
  `src/candle/pcb/`).
- **External formats** (DXF, Gerber RS-274X, Excellon) are fiddly — prefer a vetted
  parsing library (via vcpkg) over hand-rolling, and write tests against sample files.
- **Safety**: generated G-code must always include safe-Z retracts, explicit units
  (G21), absolute mode (G90), and conservative default feeds. This machine is also
  used as a pen-plotter (Z = pen up/down) — keep Z semantics configurable.
- Update this file as the architecture solidifies.

## Shared CAD/CAM core (`src/cadcam/`) — implemented

UI-independent static library `cadcam` (Qt Core/Gui only), linked into `candle`
and standalone-buildable for tests:

```
src/cadcam/
  geometry.{h,cpp}      # Point/BBox/Polyline, arc & bulge flattening, predicates
  entity.{h,cpp}        # Entity (Line/Arc/Circle/Polyline) + Drawing document
  toolpath.{h,cpp}      # Tool, ToolpathMove (Rapid/Feed/Plunge), Toolpath
  gcodeemitter.{h,cpp}  # safe G-code: G21/G90/G94/G17, safe-Z, spindle vs Pen Z mode
  dxfimporter.{h,cpp}   # ASCII DXF: LINE/CIRCLE/ARC/LWPOLYLINE/POLYLINE (+bulges)
  offset.{h,cpp}        # miter polygon offset + concentric inset rings (v1; TODO Clipper2)
  camoperations.{h,cpp} # engrave / profile (on/in/out) / pocket, multi-depth
  tests/tst_cadcam.cpp  # 23 QtTest cases (all passing)
```

Build & test the core standalone:
```bash
/usr/bin/cmake -S src/cadcam -B build-cadcam && /usr/bin/cmake --build build-cadcam -j8
./build-cadcam/tests/tst_cadcam
```

UI integration: `src/candle/cam/cadcamwidget.{h,cpp}` is a dock added in
`frmMain`'s constructor; its `programGenerated()` signal feeds the existing
`loadFile(QList<QString>)` so generated toolpaths render in the stock visualizer.
(Env hook `HJLABS_CADCAM_AUTODEMO=1` / `HJLABS_CADCAM_OP=0..4` drives it for
headless screenshot testing.)

## Status

- [x] Forked from Denvi/Candle and cloned locally.
- [x] CLAUDE.md created (this file).
- [x] Clean baseline build of the controller (Qt5 deps incl. SerialPort/Script/
      WebSockets/Help/Multimedia installed; build with `/usr/bin/cmake -S . -B build`).
- [x] Shared 2D geometry model + safe G-code emitter (unit-tested, UI-independent).
- [x] DXF importer + wood CAM ops (engrave/profile/pocket) → toolpaths → emitter.
- [x] CAD/CAM dock in `frmMain`; generated toolpaths render in the visualizer
      (verified visually: engrave, profile in/out, pocket all correct).
- [x] Auto-soldering mode: table-driven X/Y/Z points, per-point feed type
      (pre-solder vs touch-down) + feed time; spindle output repurposed as the
      solder-wire feeder (M3/G4/M5). Core `src/cadcam/soldering.{h,cpp}` (tested),
      UI `src/candle/cam/solderingwidget.{h,cpp}`, tab beside CAD/CAM.
- [x] UI/UX: min/max/restore window buttons, panels made scrollable so the
      window minimum stays below screen width (fixes missing maximize button on
      small screens) and resizes small; secondary modes shown as dock tabs
      (Modification | CAD/CAM | Soldering), gated by UI_LAYOUT_VERSION.
- [x] Writing mode: type text -> single-stroke font (`src/cadcam/strokefont.*`,
      built-in + JSON custom fonts) -> pen G-code. UI `cam/writingwidget.*`.
- [x] PCB mode (v1): Gerber RS-274X + Excellon importers, isolation routes /
      drilling / board cutout (`src/cadcam/{gerberimporter,excellonimporter,pcbcam}.*`),
      UI `cam/pcbwidget.*`. TODO: Clipper2 union for dense copper, wizard sequencer.
- [x] Motion/Limits panel: read/write GRBL `$`-settings (limit switches, homing,
      per-axis steps/max-rate/accel). `cam/motionwidget.*`. (GRBL = linear accel only.)
- [x] Soldering "Record position" button captures live work-coords into a row.
- [x] Handwriting-font pipeline (python3.13): printable template + photo ->
      centerline-vectorized custom font JSON (`tools/handwriting/`), loaded by Writing mode.
- [x] Plugins fixed in dev build: candleplugins + Qt Script binding extensions
      (`script/*.so`) copied/built next to the binary so importExtension works.
- All new modes are dock tabs (Modification | CAD/CAM | Soldering | Writing | PCB |
      Motion), gated by UID_LAYOUT_VERSION. Core has 32 passing QtTest cases.
- [ ] Plugin-ization of native modes: deferred (user choice).
- [ ] Wood CAM polish: in-app CAD drawing tools, robust offsetting (Clipper2 via
      vcpkg), tabs/bridges, ramped plunges. Known UX: 6 dock tabs overflow the
      narrow default left dock (tab strip scrolls); consider a wider default or a
      top-level mode bar. Plugin custom-widget styling (StyledToolButton etc.) being
      fixed separately.
