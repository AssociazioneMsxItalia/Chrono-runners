#!/usr/bin/env python3

"""
Parses a combined screen file and exports individual screens as C header files.

Supported input formats:
  - ASM export: MC0/MD0 is the collage (skipped), MC1/MD1+ are individual screens
  - Magellan .mag project: MAP #1 is the collage (skipped), MAP #2+ are individual screens

Usage:
    python3 split_screens.py <input> [output_dir] [--screen N] [--no-skip-hud [N ...]]

Arguments:
    input                  - Input file (.asm or .mag)
    output_dir             - Output directory for .h files (default: current directory)
    --screen N             - Export only screen N (1-based, matching MD number)
    --no-skip-hud [N ...]  - Keep the first 2 rows (normally skipped as HUD placeholders).
                             Optionally specify screen numbers (1-based) to apply this to.
                             If no numbers given, applies to all exported screens.

Examples:
    python3 split_screens.py all_screens.asm ./output
    python3 split_screens.py project.mag ./output
    python3 split_screens.py all_screens.asm ./output --screen 5
    python3 split_screens.py all_screens.asm ./output --no-skip-hud
    python3 split_screens.py all_screens.asm ./output --no-skip-hud 3 7
"""

import sys
import os
import re
import argparse


def parse_data_line(line: str) -> list[str]:
    """
    Parses a DATA line and returns a list of hex byte strings.
    Input:  'MD1    DATA >2C2C,>2C2C,>2C2C,>2C2C'
    or:     '       DATA >2C2C,>2C2C,>2C2C,>2C2C'
    Output: ['0x2c', '0x2c', '0x2c', '0x2c', ...]
    """
    # Find the DATA portion
    if 'DATA' not in line:
        return []

    data_part = line.split('DATA')[1].strip()
    hex_words = data_part.split(',')

    bytes_list = []
    for word in hex_words:
        # Clean the value (remove > and spaces)
        hex_val = word.strip().lstrip('>')

        # Process 16-bit values (4 hex chars)
        if len(hex_val) == 4:
            high_byte = hex_val[0:2].lower()
            low_byte = hex_val[2:4].lower()
            bytes_list.append(f'0x{high_byte}')
            bytes_list.append(f'0x{low_byte}')
        elif hex_val:
            print(f"Warning: malformed value ignored: '{word}'", file=sys.stderr)

    return bytes_list


def parse_screens(input_file: str) -> dict[int, list[str]]:
    """
    Parses the ASM file and returns a dictionary mapping screen numbers to byte lists.
    Skips MC0/MD0 (the collage).
    """
    screens = {}
    current_screen = None
    current_data = []

    # Pattern to match MD lines (e.g., MD1, MD10, MD34)
    md_pattern = re.compile(r'^MD(\d+)\s+DATA')
    # Pattern to match MC lines (marks start of a new screen section)
    mc_pattern = re.compile(r'^MC(\d+)\s+DATA')

    with open(input_file, 'r') as f:
        for line in f:
            line_stripped = line.strip()

            # Check for MC marker (start of new screen section)
            mc_match = mc_pattern.match(line_stripped)
            if mc_match:
                # Save previous screen if any (and not MC0)
                if current_screen is not None and current_screen > 0:
                    screens[current_screen] = current_data

                screen_num = int(mc_match.group(1))
                if screen_num > 0:
                    current_screen = screen_num
                    current_data = []
                else:
                    current_screen = None
                    current_data = []
                continue

            # Check for MD line (data start for a screen)
            md_match = md_pattern.match(line_stripped)
            if md_match:
                screen_num = int(md_match.group(1))
                if screen_num > 0:
                    current_screen = screen_num
                    current_data = parse_data_line(line_stripped)
                continue

            # Check for continuation DATA lines (within a screen)
            if current_screen is not None and current_screen > 0:
                if 'DATA' in line_stripped and line_stripped.startswith('DATA'):
                    # Pure DATA continuation line
                    current_data.extend(parse_data_line(line_stripped))
                elif line_stripped.startswith('DATA'):
                    current_data.extend(parse_data_line(line_stripped))

    # Don't forget the last screen
    if current_screen is not None and current_screen > 0:
        screens[current_screen] = current_data

    return screens


def parse_screens_mag(input_file: str) -> dict[int, list[str]]:
    """
    Parses a Magellan .mag project file and returns a dictionary mapping
    screen numbers to byte lists (same format as parse_screens).
    MAP #1 is the collage (skipped). MAP #2+ are individual screens.
    MAP #N maps to screen_num = N-1 (so MAP #2 -> 1, like MD1 in ASM).
    """
    screens = {}
    current_map = None
    current_data = []
    in_map_data = True  # tracks whether we're still in MP lines vs SPRITE LOCATIONS

    map_pattern = re.compile(r'^\* MAP #(\d+)$')

    with open(input_file, 'r') as f:
        for line in f:
            line_stripped = line.strip()

            # Check for MAP marker
            map_match = map_pattern.match(line_stripped)
            if map_match:
                current_map = int(map_match.group(1))
                current_data = []
                in_map_data = True
                continue

            # M- marks end of a map section
            if line_stripped == 'M-':
                if current_map is not None and current_map > 1:
                    screens[current_map - 1] = current_data
                current_map = None
                continue

            # Stop collecting MP lines once we hit SPRITE LOCATIONS
            if line_stripped == '* SPRITE LOCATIONS':
                in_map_data = False
                continue

            # Parse MP lines (tile data rows)
            if (current_map is not None and current_map > 1
                    and in_map_data and line_stripped.startswith('MP:')):
                values = line_stripped[3:].split('|')
                for v in values:
                    current_data.append(f'0x{int(v):02x}')

    # Handle last map if file doesn't end with M-
    if current_map is not None and current_map > 1:
        screens[current_map - 1] = current_data

    return screens


def write_screen_header(screen_num: int, data: list[str], output_dir: str,
                        skip_hud: bool = True):
    """
    Writes a single screen to a C header file.
    Output filename: screen_{n+1}.h (so MD1 becomes screen_2.h)
    By default skips the first 64 bytes (2 rows of 32 tiles) which are HUD placeholders.
    """
    output_filename = f"screen_{screen_num + 1}.h"
    output_path = os.path.join(output_dir, output_filename)

    # Variable name based on screen number
    var_name = f"g_Screen{screen_num + 1}"

    # Skip first 64 bytes (2 rows of HUD placeholder) unless told not to
    if skip_hud:
        data = data[64:]

    with open(output_path, 'w') as f:
        f.write(f"const unsigned char {var_name}[] = {{\n")

        # Write 32 bytes per line (matching the intermission.h format)
        bytes_per_line = 32
        for i in range(0, len(data), bytes_per_line):
            chunk = data[i:i + bytes_per_line]
            line = ', '.join(chunk)
            if i + bytes_per_line < len(data):
                f.write(f"    {line},\n")
            else:
                # Last line - still add comma for consistency
                f.write(f"    {line},\n")

        f.write("};\n")

    return output_path, len(data)


def main():
    parser = argparse.ArgumentParser(
        description='Convert combined ASM screen file to individual C header files.'
    )
    parser.add_argument('input_file', help='Input file (.asm or .mag)')
    parser.add_argument('output_dir', nargs='?', default='.',
                        help='Output directory (default: current directory)')
    parser.add_argument('--screen', type=int, default=None,
                        help='Export only this screen number (1-based, matching MD number)')
    parser.add_argument('--no-skip-hud', nargs='*', type=int, default=None,
                        help='Keep the first 2 rows (normally skipped as HUD placeholders). '
                             'Optionally list screen numbers (1-based) to apply this to.')

    args = parser.parse_args()

    # Build the set of screens that should NOT skip HUD rows.
    # None  = flag not given    -> skip HUD on all screens (default)
    # []    = flag given, no args -> keep HUD on all exported screens
    # [3,7] = flag given with args -> keep HUD only on screens 3 and 7
    if args.no_skip_hud is None:
        no_skip_hud_set = set()
    elif len(args.no_skip_hud) == 0:
        no_skip_hud_set = None  # means "all"
    else:
        no_skip_hud_set = set(args.no_skip_hud)

    # Validate input file
    if not os.path.isfile(args.input_file):
        print(f"Error: Input file '{args.input_file}' not found", file=sys.stderr)
        sys.exit(1)

    # Create output directory if needed
    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)

    # Parse all screens (detect format by extension)
    ext = os.path.splitext(args.input_file)[1].lower()
    print(f"Parsing {args.input_file} ({ext} format)...")

    if ext == '.mag':
        screens = parse_screens_mag(args.input_file)
    else:
        screens = parse_screens(args.input_file)

    if not screens:
        print("No screens found", file=sys.stderr)
        sys.exit(1)

    print(f"Found {len(screens)} screens: #{min(screens.keys())} to #{max(screens.keys())}")

    # Export screens
    if args.screen is not None:
        screen = args.screen - 1

        # Export single screen
        if screen not in screens:
            print(f"Error: Screen {screen} not found. Available: {sorted(screens.keys())}",
                  file=sys.stderr)
            sys.exit(1)

        skip_hud = not (no_skip_hud_set is None or args.screen in no_skip_hud_set)
        output_path, byte_count = write_screen_header(screen, screens[screen], args.output_dir,
                                                      skip_hud=skip_hud)
        print(f"Exported screen {args.screen} -> {output_path} ({byte_count} bytes)")
    else:
        # Export all screens
        for screen_num in sorted(screens.keys()):
            # screen_num is 0-based internally; no_skip_hud_set uses 1-based screen numbers
            screen_1based = screen_num + 1
            skip_hud = not (no_skip_hud_set is None or screen_1based in no_skip_hud_set)
            data = screens[screen_num]
            output_path, byte_count = write_screen_header(screen_num, data, args.output_dir,
                                                          skip_hud=skip_hud)
            print(f"Exported screen {screen_num} -> {output_path} ({byte_count} bytes)")

    print("Done!")


if __name__ == "__main__":
    main()
