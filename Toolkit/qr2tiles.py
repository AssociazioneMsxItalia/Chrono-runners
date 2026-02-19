#!/usr/bin/env python3
"""
QR Code to MSX Tile Map converter.

Takes a QR code image (25x25 QR modules) and converts it to a 13x13 2D array
of tile indices, where each tile represents a 2x2 block of QR modules.

Tile mapping (binary encoding: TL*8 + TR*4 + BL*2 + BR*1):
  Pattern 0000 (all white) -> tile 100
  Pattern 0001 .. 1110     -> tiles 182 .. 195
  Pattern 1111 (all black) -> tile 47

Output: patches the QR region in-place inside a Magellan .mag file.
The QR region is hardcoded at col=10, row=4 (13x13 tiles) in the map's tile space.
"""

import argparse
from PIL import Image

TILE_ALL_WHITE = 100
TILE_ALL_BLACK = 47
TILE_QR_BASE = 182  # maps to pattern 0001; pattern N -> TILE_QR_BASE + N - 1

QR_MODULES = 25
QR_COL = 10   # top-left column of the QR region in the map
QR_ROW = 4    # top-left row of the QR region in the map


def pattern_to_tile(tl, tr, bl, br):
    """Map a 2x2 pixel pattern to a tile index.

    Each argument is 0 (white) or 1 (black).
    """
    pattern = tl * 8 + tr * 4 + bl * 2 + br
    if pattern == 0:
        return TILE_ALL_WHITE
    if pattern == 15:
        return TILE_ALL_BLACK
    return TILE_QR_BASE + pattern - 1


def load_qr_pixels(filepath):
    """Load a QR code image and return a 25x25 binary array (1=black, 0=white)."""
    img = Image.open(filepath).convert("L")
    w, h = img.size

    # Auto-detect sampling step
    step_x = w / QR_MODULES
    step_y = h / QR_MODULES

    pixels = [[0] * QR_MODULES for _ in range(QR_MODULES)]
    for y in range(QR_MODULES):
        for x in range(QR_MODULES):
            sx = min(int(x * step_x + step_x / 2), w - 1)
            sy = min(int(y * step_y + step_y / 2), h - 1)
            pixels[y][x] = 1 if img.getpixel((sx, sy)) < 128 else 0

    return pixels


def qr_to_tilemap(pixels):
    """Convert a 25x25 binary QR array into a 13x13 tile index map.

    The 25x25 grid is padded to 26x26 with white (0) on the right and bottom
    edges so it divides evenly into 2x2 blocks.
    """
    size = QR_MODULES + 1  # 26, so 2x2 blocks divide evenly into 13x13
    padded = [[0] * size for _ in range(size)]
    for y in range(QR_MODULES):
        for x in range(QR_MODULES):
            padded[y][x] = pixels[y][x]

    rows = size // 2  # 13
    tilemap = []
    for ty in range(rows):
        row = []
        for tx in range(rows):
            tl = padded[ty * 2    ][tx * 2    ]
            tr = padded[ty * 2    ][tx * 2 + 1]
            bl = padded[ty * 2 + 1][tx * 2    ]
            br = padded[ty * 2 + 1][tx * 2 + 1]
            row.append(pattern_to_tile(tl, tr, bl, br))
        tilemap.append(row)

    return tilemap


def patch_mag(filepath, map_number, tilemap):
    """Patch a Magellan .mag file by overwriting the QR region in the specified map.

    Finds '* MAP #N', collects its MP: lines, and replaces tile values at
    QR_COL/QR_ROW with the 13x13 tilemap.
    """
    with open(filepath, "r") as f:
        lines = f.readlines()

    # Find the target map section
    map_header = f"* MAP #{map_number}"
    map_start = next(
        (i for i, line in enumerate(lines) if line.strip() == map_header),
        None,
    )
    if map_start is None:
        raise ValueError(f"Map #{map_number} not found in {filepath}")

    # Collect the indices of MP: lines within this map
    mp_indices = []
    for i in range(map_start + 1, len(lines)):
        stripped = lines[i].strip()
        if stripped.startswith("* MAP #") or stripped == "M-":
            break
        if stripped.startswith("MP:"):
            mp_indices.append(i)

    # Patch each tilemap row into the corresponding MP: line
    for ty, tm_row in enumerate(tilemap):
        map_row = QR_ROW + ty
        if map_row >= len(mp_indices):
            break
        line_idx = mp_indices[map_row]
        values = lines[line_idx].rstrip("\n")[3:].split("|")  # strip "MP:" prefix
        for tx, tile in enumerate(tm_row):
            map_col = QR_COL + tx
            if map_col >= len(values):
                break
            values[map_col] = str(tile)
        lines[line_idx] = "MP:" + "|".join(values) + "\n"

    with open(filepath, "w") as f:
        f.writelines(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Convert a 25x25 QR code image and patch it into a Magellan .mag file."
    )
    parser.add_argument("image", help="Path to the QR code image")
    parser.add_argument("mag", help="Path to the Magellan .mag file to patch")
    parser.add_argument(
        "--map",
        type=int,
        required=True,
        dest="map_number",
        help="Map number to patch (e.g. 48)",
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="Print the detected 25x25 binary QR grid before patching",
    )
    args = parser.parse_args()

    pixels = load_qr_pixels(args.image)

    if args.debug:
        print("Detected QR modules (1=black, 0=white):")
        for y in range(QR_MODULES):
            print("".join("##" if pixels[y][x] else "  " for x in range(QR_MODULES)))
        print()

    tilemap = qr_to_tilemap(pixels)
    patch_mag(args.mag, args.map_number, tilemap)
    print(f"Patched map #{args.map_number} in {args.mag} "
          f"at col={QR_COL}, row={QR_ROW} ({len(tilemap)}x{len(tilemap[0])} tiles).")


if __name__ == "__main__":
    main()
