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
    python3 extract_screens.py <input.mag> [output_dir] [--screen N] [--no-skip-hud [N ...]]

Arguments:
    input.mag              - Magellan project file
    output_dir             - Output directory for .h files (default: current directory)
    --screen N             - Export only screen N (1-based MAP number, e.g. 2 = MAP #2)
    --no-skip-hud [N ...]  - Keep the first 2 rows (normally skipped as HUD placeholders).
                             Optionally specify MAP numbers to apply this to.
                             If no numbers given, applies to all exported screens.

Examples:
    python3 extract_screens.py "Chrono Runner.mag" ./output
    python3 extract_screens.py "Chrono Runner.mag" ./output --screen 5
    python3 extract_screens.py "Chrono Runner.mag" ./output --no-skip-hud
    python3 extract_screens.py "Chrono Runner.mag" ./output --no-skip-hud 2 3
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
                        skip_hud: bool = True) -> tuple[str, int]:
    """
    Writes a single screen to a C header file.
    Output filename: screen_{map_num}.h (MAP #2 -> screen_2.h)
    By default skips the first 64 bytes (2 rows of 32 tiles) which are HUD placeholders.
    """
    output_filename = f"screen_{map_num}.h"
    output_path = os.path.join(output_dir, output_filename)

    var_name = f"g_Screen{map_num}"

    if skip_hud:
        data = data[64:]

    with open(output_path, 'w') as f:
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
    parser.add_argument('--no-skip-hud', nargs='*', type=int, default=None,
                        help='Keep the first 2 rows (normally skipped as HUD placeholders). '
                             'Optionally list MAP numbers to apply this to.')

    args = parser.parse_args()

    # Build the set of screens that should NOT skip HUD rows.
    if args.no_skip_hud is None:
        no_skip_hud_set = set()          # skip HUD on all (default)
    elif len(args.no_skip_hud) == 0:
        no_skip_hud_set = None           # keep HUD on all
    else:
        no_skip_hud_set = set(args.no_skip_hud)

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

    if args.screen is not None:
        if args.screen not in screens:
            print(f"Error: MAP #{args.screen} not found. Available: {sorted(screens.keys())}",
                  file=sys.stderr)
            sys.exit(1)

        skip_hud = not (no_skip_hud_set is None or args.screen in no_skip_hud_set)
        output_path, byte_count = write_screen_header(
            args.screen, screens[args.screen], args.output_dir, skip_hud=skip_hud)
        print(f"Exported MAP #{args.screen} -> {output_path} ({byte_count} bytes)")
    else:
        for map_num in sorted(screens.keys()):
            skip_hud = not (no_skip_hud_set is None or map_num in no_skip_hud_set)
            output_path, byte_count = write_screen_header(
                map_num, screens[map_num], args.output_dir, skip_hud=skip_hud)
            print(f"Exported MAP #{map_num} -> {output_path} ({byte_count} bytes)")

    print("Done!")


if __name__ == "__main__":
    main()
