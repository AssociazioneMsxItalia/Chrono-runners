#!/usr/bin/env python3
"""
Extract level data from a Magellan (.mag) project file.

Reads sprite locations from each MAP section and generates _lvl.h files
for the Chrono Runner game. Only processes MAPs that have at least one
sprite placed on the map (in pixel coordinates).

Usage:
    python extract_levels.py <magellan_file> [output_dir]

Sprite ID to game entity mapping (derived from cross-referencing
existing level files with Magellan sprite placements):
    28       -> Enemy type 0
    34, 37   -> Enemy type 1
    41, 42   -> Enemy type 2
    46       -> Enemy type 3
    63       -> Mine
    27, 35   -> Platform
"""

import sys
import os
import re

# ---------------------------------------------------------------------------
# Sprite ID -> entity mapping
# ---------------------------------------------------------------------------

ENEMY_SPRITES = {
    28: 0,   # Enemy type 0
    30: 0,   # Enemy type 0
    34: 1,   # Enemy type 1
    35: 1,   # Enemy type 1
    37: 1,   # Enemy type 1 (variant)
    40: 2,   # Enemy type 2
    41: 2,   # Enemy type 2
    42: 2,   # Enemy type 2 (variant)
    46: 3,   # Enemy type 3
    47: 3,
    49: 3,
}

MINE_SPRITES = {63}

PLATFORM_SPRITES = {26, 27, 35, 75}

# Tiles that indicate platform movement direction
PLATFORM_VERTICAL_TILES = {80, 81, 82, 83}
PLATFORM_HORIZONTAL_TILES = {73, 74, 75}

# Start/End position tiles (2x2 grids)
START_TILE = 56
END_TILE = 57

# Key tile pattern: 94|95 on one row, 102|103 on the next
KEY_TOP_LEFT = 94
KEY_TOP_RIGHT = 95
KEY_BOT_LEFT = 102
KEY_BOT_RIGHT = 103

# ---------------------------------------------------------------------------
# Parsing
# ---------------------------------------------------------------------------

def parse_magellan(filepath):
    """Parse a Magellan .mag file and return a list of map dicts."""
    with open(filepath, 'r') as f:
        lines = f.readlines()

    maps = []
    current_map = None
    in_sprites = False

    for line in lines:
        line = line.strip()

        # Detect map header
        m = re.match(r'^\* MAP #(\d+)$', line)
        if m:
            map_num = int(m.group(1))
            current_map = {
                'number': map_num,
                'width': 0,
                'height': 0,
                'sprites': [],
                'tiles': [],
            }
            in_sprites = False
            continue

        if current_map is None:
            continue

        if line == 'M+':
            continue

        if line.startswith('MS:'):
            parts = line[3:].split('|')
            current_map['width'] = int(parts[0])
            current_map['height'] = int(parts[1])
            continue

        if line.startswith('MB:'):
            continue

        if line.startswith('MP:'):
            tiles = [int(x) for x in line[3:].split('|')]
            current_map['tiles'].append(tiles)
            continue

        if line == '* SPRITE LOCATIONS':
            in_sprites = True
            continue

        if line.startswith('SL:') and in_sprites:
            parts = line[3:].split('|')
            x, y, sprite_id = int(parts[0]), int(parts[1]), int(parts[2])
            current_map['sprites'].append((x, y, sprite_id))
            continue

        if line == 'M-':
            if current_map:
                maps.append(current_map)
            current_map = None
            in_sprites = False
            continue

    return maps


def is_on_map(x, y, width, height):
    """Check if pixel (x, y) is within the map bounds (width/height in tiles)."""
    return x < width * 8 and y < height * 8


# ---------------------------------------------------------------------------
# Entity extraction
# ---------------------------------------------------------------------------

def find_2x2_tile(tiles, tile_id):
    """Find the position of a 2x2 grid of the given tile ID.

    Returns {'x': col, 'y': row} or None if not found.
    """
    num_rows = len(tiles)
    if num_rows < 2:
        return None

    for row_idx in range(num_rows - 1):
        row = tiles[row_idx]
        next_row = tiles[row_idx + 1]

        for col_idx in range(len(row) - 1):
            if (row[col_idx] == tile_id and
                row[col_idx + 1] == tile_id and
                next_row[col_idx] == tile_id and
                next_row[col_idx + 1] == tile_id):
                return {'x': col_idx, 'y': row_idx}
    return None


def find_key(tiles):
    """Find the key position from tile data (94|95 top, 102|103 bottom)."""
    num_rows = len(tiles)
    for row_idx in range(num_rows - 1):
        row = tiles[row_idx]
        next_row = tiles[row_idx + 1]
        for col_idx in range(len(row) - 1):
            if (row[col_idx] == KEY_TOP_LEFT and
                row[col_idx + 1] == KEY_TOP_RIGHT and
                next_row[col_idx] == KEY_BOT_LEFT and
                next_row[col_idx + 1] == KEY_BOT_RIGHT):
                return {'x': col_idx, 'y': row_idx}
    return None


def calculate_patrol_range(px, py, tiles, width, height):
    """Calculate patrol range for an enemy based on the tile below its feet.

    Receives pixel coordinates (px, py). Enemies are 2x2 tiles; the tile
    "below their feet" is at tile row (py // 8 + 2). We scan left and right
    while the tile index differs by at most 1 from the base tile, then
    reduce max by 1 to keep the sprite's right edge on the platform.
    Returns pixel-space bounds.
    """
    # Convert pixel coords to tile coords for tile lookups
    tx = px // 8
    ty = py // 8

    # The bottom row of the 2x2 sprite
    feet_row = ty + 2

    # Get the tile at the bottom of the enemy sprite
    row = tiles[feet_row]
    if tx >= len(row):
        patrol_min = max(8, px - 40)
        patrol_max = min((width - 2) * 8, px + 40)
        return patrol_min, patrol_max

    base_tile = row[tx]

    # Scan left while tile index differs by at most 1 from base
    patrol_min = tx
    while patrol_min > 0 and abs(row[patrol_min - 1] - base_tile) <= 1:
        patrol_min -= 1

    # Scan right while tile index differs by at most 1 from base
    patrol_max = tx
    while patrol_max < len(row) - 1 and abs(row[patrol_max + 1] - base_tile) <= 1:
        patrol_max += 1

    # Reduce max by 1 to keep the sprite's right edge (which extends 1 tile right) on platform
    patrol_max = max(patrol_max - 1, patrol_min)

    # Clamp to safe bounds (at least 1 tile from edges)
    patrol_min = max(1, patrol_min)
    patrol_max = min(width - 2, patrol_max)

    # Convert back to pixel space
    return patrol_min * 8, patrol_max * 8


def detect_platform_direction_and_range(px, py, tiles):
    """Detect platform movement direction and range from the track tiles.

    Checks the 4 tiles under the 2x2 sprite to find a track tile, then
    scans along that axis to find the full extent of the track.
    Returns (dir_x, dir_y, min_x, min_y, max_x, max_y) in pixel space.
    """
    tx, ty = px // 8, py // 8

    for row in range(ty, ty + 2):
        if row >= len(tiles):
            continue
        for col in range(tx, tx + 2):
            if col >= len(tiles[row]):
                continue
            tile = tiles[row][col]

            if tile in PLATFORM_VERTICAL_TILES:
                # Scan up
                min_row = row
                while min_row > 0 and tiles[min_row - 1][col] in PLATFORM_VERTICAL_TILES:
                    min_row -= 1
                # Scan down
                max_row = row
                while max_row < len(tiles) - 1 and tiles[max_row + 1][col] in PLATFORM_VERTICAL_TILES:
                    max_row += 1
                return 0, 1, px, min_row * 8, px, max_row * 8

            if tile in PLATFORM_HORIZONTAL_TILES:
                # Scan left
                min_col = col
                while min_col > 0 and tiles[row][min_col - 1] in PLATFORM_HORIZONTAL_TILES:
                    min_col -= 1
                # Scan right
                max_col = col
                while max_col < len(tiles[row]) - 1 and tiles[row][max_col + 1] in PLATFORM_HORIZONTAL_TILES:
                    max_col += 1
                return 1, 0, min_col * 8, py, max_col * 8, py

    return 0, 0, px, py, px, py


def extract_entities(map_data):
    """Extract game entities from a map's sprite and tile data."""
    width = map_data['width']
    height = map_data['height']
    tiles = map_data['tiles']

    enemies = []
    mines = []
    platforms = []
    unknown = []

    for x, y, sprite_id in map_data['sprites']:
        if not is_on_map(x, y, width, height):
            continue

        if sprite_id in ENEMY_SPRITES:
            # Calculate patrol range based on tile below enemy's feet
            patrol_min, patrol_max = calculate_patrol_range(x, y, tiles, width, height)
            enemies.append({
                'x': x,
                'y': y,
                'type': ENEMY_SPRITES[sprite_id],
                'sprite_id': sprite_id,
                'min_x': patrol_min,
                'max_x': patrol_max,
            })
        elif sprite_id in MINE_SPRITES:
            mines.append({'x': x, 'y': y})
        elif sprite_id in PLATFORM_SPRITES:
            dir_x, dir_y, min_x, min_y, max_x, max_y = \
                detect_platform_direction_and_range(x, y, tiles)
            platforms.append({
                'x': x,
                'y': y,
                'sprite_id': sprite_id,
                'dir_x': dir_x,
                'dir_y': dir_y,
                'min_x': min_x,
                'min_y': min_y,
                'max_x': max_x,
                'max_y': max_y,
            })
        else:
            unknown.append({'x': x, 'y': y, 'sprite_id': sprite_id})

    start = find_2x2_tile(map_data['tiles'], START_TILE)
    end = find_2x2_tile(map_data['tiles'], END_TILE)
    key = find_key(map_data['tiles'])

    return {
        'enemies': enemies,
        'mines': mines,
        'platforms': platforms,
        'start': start,
        'end': end,
        'key': key,
        'unknown': unknown,
    }


# ---------------------------------------------------------------------------
# Code generation
# ---------------------------------------------------------------------------

def generate_level_h(map_num, entities):
    """Generate a _lvl.h C header string from extracted entities."""
    enemies = entities['enemies']
    mines = entities['mines']
    platforms = entities['platforms']
    start = entities['start']
    end = entities['end']
    key = entities['key']
    unknown = entities['unknown']

    lines = []
    lines.append(f'extern unsigned char g_Screen{map_num}[];')
    lines.append('')

    suffix = f'map{map_num}'

    # --- Platforms ---
    if platforms:
        lines.append(f'struct Platform platforms_{suffix}[] = {{')
        for i, p in enumerate(platforms):
            comma = ',' if i < len(platforms) - 1 else ''
            px, py = p["x"] // 8, p["y"] // 8
            lines.append(f'   {{{px}*8, {py}*8,  // pos_x pos_y')
            lines.append(f'       {p["dir_x"]},    {p["dir_y"]},  // dir_x dir_y')
            lines.append(f'    {p["min_x"] // 8}*8, {p["min_y"] // 8}*8,  // min_x min_y')
            lines.append(f'    {p["max_x"] // 8}*8, {p["max_y"] // 8}*8}}{comma} // max_x max_y')
        lines.append('};')
        lines.append('')

    # --- Mines ---
    if mines:
        lines.append(f'struct Mine mines_{suffix}[] = {{')
        for i, m in enumerate(mines):
            comma = ',' if i < len(mines) - 1 else ''
            mx, my = m["x"] // 8, m["y"] // 8
            lines.append(f'\t{{{mx}*8, {my}*8, TRUE}}{comma} // pos_x pos_y')
        lines.append('};')
        lines.append('')

    # --- Enemies ---
    if enemies:
        lines.append(f'struct Enemy enemies_{suffix}[] = {{')
        for i, e in enumerate(enemies):
            comma = ',' if i < len(enemies) - 1 else ''
            ex, ey = e["x"] // 8, e["y"] // 8
            emin, emax = e["min_x"] // 8, e["max_x"] // 8
            lines.append(f'  {{{ex}*8, {ey}*8,  // pos_x pos_y')
            lines.append(f'\t        1,  // dir_x  TODO: set direction (-1 or 1)')
            lines.append(f'\t{emin}*8, {emax}*8,  // min_x max_x  TODO: adjust patrol range')
            lines.append(f'\t        {e["type"]},  // type (0-3)')
            lines.append(f'\t        0,  // mDX (initialized to 0)')
            lines.append(f'\t        0,  // stunned_timer (initialized to 0)')
            lines.append(f'\t\t\t0,  // field_state')
            lines.append(f'\t\t\t0,  // field_timer')
            lines.append(f'\t\t\t0,  // field_x')
            lines.append(f'\t\t\t0,  // field_y')
            lines.append(f'\t\t\t0}}{comma} // field_mDX')
        lines.append('};')
        lines.append('')

    # --- Start / End / Key positions ---
    start_x, start_y = (start['x'], start['y']) if start else (0, 0)
    end_x, end_y = (end['x'], end['y']) if end else (0, 0)
    key_x, key_y = (key['x'], key['y']) if key else (0, 0)

    # --- Level struct ---
    lines.append(f'struct Level level_{suffix} = {{')
    lines.append(f'\t{start_x}, {start_y},       // start_x start_y')
    lines.append(f'\t{end_x}, {end_y},       // end_x end_y')
    lines.append(f'\t{key_x}, {key_y},      // key_x key_y')
    lines.append(f'\t0, 0,        // crystal_x crystal_y')

    if platforms:
        lines.append(f'\tnumberof(platforms_{suffix}),  // num_platforms')
        lines.append(f'\tplatforms_{suffix},       // platforms')
    else:
        lines.append(f'\t0,           // num_platforms')
        lines.append(f'\tNULL,       // platforms')

    if mines:
        lines.append(f'\tnumberof(mines_{suffix}),  // num_mines')
        lines.append(f'\tmines_{suffix},        // mines')
    else:
        lines.append(f'\t0,           // num_mines')
        lines.append(f'\tNULL,        // mines')

    if enemies:
        lines.append(f'\tnumberof(enemies_{suffix}),  // num_enemies')
        lines.append(f'\tenemies_{suffix},  // enemies')
    else:
        lines.append(f'\t0,           // num_enemies')
        lines.append(f'\tNULL,        // enemies')

    lines.append(f'\tg_Screen{map_num},   // layout')
    lines.append(f'\t"TODO: LEVEL NAME",  // name')
    lines.append('};')
    lines.append('')

    # --- Unknown sprites (as comments) ---
    if unknown:
        lines.append('// Unknown sprite placements:')
        for u in unknown:
            lines.append(f'//   sprite {u["sprite_id"]} at pixel ({u["x"]}, {u["y"]})')
        lines.append('')

    return '\n'.join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) < 3:
        print(f'Usage: {sys.argv[0]} <magellan_file> <output_dir>')
        print()
        print('Extracts level entity data (enemies, mines, platforms) from a')
        print('Magellan .mag project file and generates _lvl.h template files.')
        print()
        print('Only MAPs with at least one sprite in pixel coordinates are processed.')
        print('MAP #1 is always skipped (it is the master collage).')
        sys.exit(1)

    mag_file = sys.argv[1]
    output_dir = sys.argv[2]

    if not os.path.isfile(mag_file):
        print(f'Error: file not found: {mag_file}')
        sys.exit(1)

    os.makedirs(output_dir, exist_ok=True)

    print(f'Parsing {mag_file} ...')
    maps = parse_magellan(mag_file)
    print(f'Found {len(maps)} MAPs total.')

    generated = 0
    for map_data in maps:
        map_num = map_data['number']

        # Skip MAP #1 (collage / overview)
        if map_num == 1:
            continue

        entities = extract_entities(map_data)

        # Only process maps that have at least one sprite entity
        total_entities = (len(entities['enemies']) +
                          len(entities['mines']) +
                          len(entities['platforms']) +
                          len(entities['unknown']))
        if total_entities == 0:
            continue

        content = generate_level_h(map_num, entities)
        out_file = os.path.join(output_dir, f'map{map_num:02d}_lvl.h')

        with open(out_file, 'w') as f:
            f.write(content)

        # Summary line
        parts = []
        if entities['enemies']:
            parts.append(f"{len(entities['enemies'])} enemies")
        if entities['mines']:
            parts.append(f"{len(entities['mines'])} mines")
        if entities['platforms']:
            parts.append(f"{len(entities['platforms'])} platforms")
        if entities['unknown']:
            parts.append(f"{len(entities['unknown'])} unknown")
        start_info = ", start found" if entities['start'] else ""
        end_info = ", end found" if entities['end'] else ""
        key_info = ", key found" if entities['key'] else ""

        print(f'  MAP #{map_num:2d} -> {out_file}  ({", ".join(parts)}{start_info}{end_info}{key_info})')
        generated += 1

    print(f'\nGenerated {generated} level files in {os.path.abspath(output_dir)}')
    if generated > 0:
        print('NOTE: Generated files contain TODO markers for values that need')
        print('      manual adjustment (dir_x, min/max ranges, start/end, names).')


if __name__ == '__main__':
    main()
