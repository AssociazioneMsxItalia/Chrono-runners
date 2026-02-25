#!/usr/bin/env python3

"""
Extracts individual screens from a Magellan (.mag) project file and exports
them as C header files.

Reads the MAP sections directly from the .mag file. MAP #1 is the collage
(skipped by default). MAP #2+ are individual screens.

Each MAP section has the structure:
    * MAP #N
    M+                  (map start marker)
    MS:<width>|<height> (map size in tiles)
    MB:0                (background tile index)
    MP:<t>|<t>|...      (tile data rows, one MP line per row)
    ...
    * SPRITE LOCATIONS
    ...
    M-                  (map end marker)

Usage:
    python3 extract_screens.py <input.mag> [output_dir] [--screen N]
                               [--full-extract [N ...]] [--cutscene-extract [N ...]]

Arguments:
    input.mag                  - Magellan project file
    output_dir                 - Output directory for .h files (default: current directory)
    --screen N                 - Export only screen N (1-based MAP number, e.g. 2 = MAP #2)
    --full-extract [N ...]     - Export all rows (no HUD skip).
                                 Optionally specify MAP numbers to apply this to.
                                 If no numbers given, applies to all exported screens.
    --cutscene-extract [N ...] - Export only rows 2-19 (zero-based, cutscene area).
                                 Optionally specify MAP numbers to apply this to.
                                 If no numbers given, applies to all exported screens.

Examples:
    python3 extract_screens.py "Chrono Runner.mag" ./output
    python3 extract_screens.py "Chrono Runner.mag" ./output --screen 5
    python3 extract_screens.py "Chrono Runner.mag" ./output --full-extract
    python3 extract_screens.py "Chrono Runner.mag" ./output --full-extract 2 3
    python3 extract_screens.py "Chrono Runner.mag" ./output --cutscene-extract
    python3 extract_screens.py "Chrono Runner.mag" ./output --cutscene-extract 4 5
"""

import sys
import os
import re
import argparse


def parse_screens_mag(input_file: str) -> dict[int, list[str]]:
    """
    Parses a Magellan .mag project file and returns a dictionary mapping
    MAP numbers to tile byte lists.
    MAP #1 is the collage and is skipped.
    """
    screens = {}
    current_map = None
    current_data = []
    in_map_data = True

    map_pattern = re.compile(r'^\* MAP #(\d+)$')

    with open(input_file, 'r') as f:
        for line in f:
            line_stripped = line.strip()

            map_match = map_pattern.match(line_stripped)
            if map_match:
                current_map = int(map_match.group(1))
                current_data = []
                in_map_data = True
                continue

            if line_stripped == 'M-':
                if current_map is not None and current_map > 1:
                    screens[current_map] = current_data
                current_map = None
                continue

            if line_stripped == '* SPRITE LOCATIONS':
                in_map_data = False
                continue

            if (current_map is not None and current_map > 1
                    and in_map_data and line_stripped.startswith('MP:')):
                values = line_stripped[3:].split('|')
                for v in values:
                    current_data.append(f'0x{int(v):02x}')

    # Handle last map if file doesn't end with M-
    if current_map is not None and current_map > 1:
        screens[current_map] = current_data

    return screens


def write_screen_header(map_num: int, data: list[str], output_dir: str,
                        extract_mode: str = 'default') -> tuple[str, int]:
    """
    Writes a single screen to a C header file.
    Output filename: screen_{map_num}.h (MAP #2 -> screen_2.h)

    extract_mode:
        'default'   - Skip the first 2 rows (HUD placeholders); export rows 2 onward.
        'full'      - Export all rows with no trimming.
        'cutscene'  - Export only rows 2-19 (zero-based, inclusive).
    """
    output_filename = f"screen_{map_num}.h"
    output_path = os.path.join(output_dir, output_filename)

    var_name = f"g_Screen{map_num}"

    if extract_mode == 'full':
        pass                        # keep all data
    elif extract_mode == 'cutscene':
        data = data[64:640]         # rows 2-19 (zero-based), 18 rows × 32 bytes
    else:                           # 'default'
        data = data[64:]            # skip first 2 rows (HUD placeholders)

    with open(output_path, 'w') as f:
        f.write(f'#pragma once\n\n')

        f.write(f"const unsigned char {var_name}[] = {{\n")

        bytes_per_line = 32
        for i in range(0, len(data), bytes_per_line):
            chunk = data[i:i + bytes_per_line]
            line = ', '.join(chunk)
            f.write(f"    {line},\n")

        f.write("};\n")

    return output_path, len(data)


def main():
    parser = argparse.ArgumentParser(
        description='Extract screens from a Magellan (.mag) file to C header files.'
    )
    parser.add_argument('input_file', help='Magellan project file (.mag)')
    parser.add_argument('output_dir', nargs='?', default='.',
                        help='Output directory (default: current directory)')
    parser.add_argument('--screen', type=int, default=None,
                        help='Export only this MAP number (e.g. 2 = MAP #2)')
    parser.add_argument('--full-extract', nargs='*', type=int, default=None,
                        metavar='N',
                        help='Export all rows with no trimming. '
                             'Optionally list MAP numbers to apply this to.')
    parser.add_argument('--cutscene-extract', nargs='*', type=int, default=None,
                        metavar='N',
                        help='Export only rows 2-19 (zero-based). '
                             'Optionally list MAP numbers to apply this to.')

    args = parser.parse_args()

    # Build sets for each non-default extract mode.
    # None  → flag not given (doesn't apply to any map)
    # set() → empty set after flag with no args → applies to all maps (sentinel: use None below)
    # set of ints → applies only to listed maps

    full_set = None
    if args.full_extract is not None:
        full_set = set(args.full_extract) if args.full_extract else None  # None = all maps

    cutscene_set = None
    if args.cutscene_extract is not None:
        cutscene_set = set(args.cutscene_extract) if args.cutscene_extract else None  # None = all maps

    if not os.path.isfile(args.input_file):
        print(f"Error: Input file '{args.input_file}' not found", file=sys.stderr)
        sys.exit(1)

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)

    print(f"Parsing {args.input_file}...")
    screens = parse_screens_mag(args.input_file)

    if not screens:
        print("No screens found", file=sys.stderr)
        sys.exit(1)

    print(f"Found {len(screens)} screens: MAP #{min(screens.keys())} to MAP #{max(screens.keys())}")

    def get_extract_mode(map_num: int) -> str:
        # --cutscene-extract takes priority over --full-extract
        if (args.cutscene_extract is not None
                and (cutscene_set is None or map_num in cutscene_set)):
            return 'cutscene'
        if (args.full_extract is not None
                and (full_set is None or map_num in full_set)):
            return 'full'
        return 'default'

    if args.screen is not None:
        if args.screen not in screens:
            print(f"Error: MAP #{args.screen} not found. Available: {sorted(screens.keys())}",
                  file=sys.stderr)
            sys.exit(1)

        mode = get_extract_mode(args.screen)
        output_path, byte_count = write_screen_header(
            args.screen, screens[args.screen], args.output_dir, extract_mode=mode)
        print(f"Exported MAP #{args.screen} -> {output_path} ({byte_count} bytes) [{mode}]")
    else:
        for map_num in sorted(screens.keys()):
            mode = get_extract_mode(map_num)
            output_path, byte_count = write_screen_header(
                map_num, screens[map_num], args.output_dir, extract_mode=mode)
            print(f"Exported MAP #{map_num} -> {output_path} ({byte_count} bytes) [{mode}]")

    print("Done!")


if __name__ == "__main__":
    main()
