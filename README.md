# Chrono Runner

**By Associazione MSX Italia**

> Placed **10th out of 50 entries** in the [MSXDev'25](https://www.msxdev.org/2026/05/03/msxdev25-results/) competition. **Playable right now** in your browser on the [contest page](https://www.msxdev.org/2026/03/14/msxdev25-38-chrono-runner/) through WebMSX.

---

## About the Game

Chrono Runner is a platformer for MSX1 computers in which you control a time-travelling runner racing through four worlds of increasingly dangerous levels. Your goal is simple: reach the exit of each stage. What makes it unique is your ability to **rewind time**: when you die or get stuck, you can reverse your steps and try a different path, giving the game its signature puzzle-platformer twist.

The game features moving platforms, hazardous mines, and four types of enemies with distinct behaviours, from patrolling guards to enemies that project energy fields. Between worlds, story cutscenes advance a light narrative that ties the time-travel theme together, and a final boss level caps the adventure.

Chrono Runner is built on the **[MSXgl](https://github.com/aoineko-fr/MSXgl)** game library and targets MSX1 hardware with a ROM mapper, spread across multiple 16K segments to fit four worlds (29 regular levels plus secret stages), music, sound effects, and full-screen graphics within the constraints of the platform.

## Repository Structure

| Path | Contents |
|------|----------|
| `chronorunner.c` | Main game loop, state machine, player physics |
| `chronorunner_s*.c` | Per-segment level data (one file per world) |
| `cutscene*.c` | Cutscene scripts and inter-world story screens |
| `content/` | Compiled binary assets (tiles, sprites, screens, music) |
| `assets/` | Source Magellan (`.mag`) sprite/level design files |
| `sound/` | PT3 music and AY-FX sound effect sources |
| `Toolkit/` | Python scripts for asset conversion |

## Building

Prerequisites: [MSXgl](https://github.com/aoineko-fr/MSXgl) (includes SDCC and MSXtk), Python 3 if you want to update assets with the `Toolkit/` utilities.

1. Clone or download MSXgl locally.
2. Clone this repository into MSXgl's `projects/` directory:
   ```console
   cd MSXgl/projects
   git clone <this-repo-url> Chrono-runners
   ```
3. Run the build script from inside the project directory:
   ```console
   # macOS / Linux
   cd Chrono-runners
   ./build.sh

   # Windows
   build.bat
   ```

The build produces a ROM image in `out/` that can be run in any MSX emulator (e.g. openMSX) or on real hardware via a FlashROM cartridge.

### Asset pipeline

Sprites are authored in **Magellan** and exported as Assembly Data, then converted to MSXgl header format with the toolkit script:

```console
python3 Toolkit/spr2msx.py <exported_assembly_file>
```

This produces `data_sprt_layer.h`; move it to `content/sprite/` before rebuilding.

Screen maps and level definitions are extracted from Magellan `.mag` files with:

```console
python3 Toolkit/extract_screens.sh <mag_file>
python3 Toolkit/extract_levels.sh <mag_file>
```

## License

This project uses two separate licenses depending on the type of content:

| Content | License |
|---------|---------|
| Source code (`.c`, `.h`, scripts) | [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html) |
| Assets (graphics, music, sound effects, level data) | [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International](https://creativecommons.org/licenses/by-nc-sa/4.0/) |

**Code — GPLv3:** You are free to use, modify, and distribute the source code under the terms of the GPLv3. Any derivative work that distributes the code must also be released under the GPLv3.

**Assets — CC BY-NC-SA 4.0:** The game's graphics, music, sound effects, and level designs may be shared and adapted for non-commercial purposes, provided you give appropriate credit to Associazione MSX Italia and distribute any derivative assets under the same license.
