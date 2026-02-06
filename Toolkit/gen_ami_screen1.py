#!/usr/bin/env python3
"""
Generate MSX Graphics Mode 1 (Screen 1) data for "Associazione MSX Italia" logo.

Produces:
- ami_screen1.h: C header file with PGT, PNT, CT arrays for MSXgl
- ami_pgt.bin:   Pattern Generator Table (2048 bytes) -> VRAM 0x0000
- ami_pnt.bin:   Pattern Name Table (768 bytes)       -> VRAM 0x1800
- ami_ct.bin:    Color Table (32 bytes)                -> VRAM 0x2000
- ami_preview.ppm: Visual preview of the screen (PPM format, no dependencies)

MSX Graphics Mode 1 constraints:
- 32x24 character grid (256x192 pixels)
- 256 possible 8x8 character patterns
- Color table: 32 entries, one per group of 8 character codes
  (chars 0-7 share colors, 8-15 share colors, etc.)
- Each color byte: high nibble = foreground, low nibble = background
"""

import os
import struct

# Output directory (same as this script)
OUT_DIR = os.path.dirname(os.path.abspath(__file__))

# ─── TMS9918A Color Palette ───────────────────────────────────────────
TRANSPARENT   = 0
BLACK         = 1
MEDIUM_GREEN  = 2
LIGHT_GREEN   = 3
DARK_BLUE     = 4
LIGHT_BLUE    = 5
DARK_RED      = 6
CYAN          = 7
MEDIUM_RED    = 8
LIGHT_RED     = 9
DARK_YELLOW   = 10
LIGHT_YELLOW  = 11
DARK_GREEN    = 12
MAGENTA       = 13
GRAY          = 14
WHITE         = 15

# RGB approximations of TMS9918A colors (for preview)
TMS_RGB = {
    0:  (0,   0,   0),     # Transparent (render as black)
    1:  (0,   0,   0),     # Black
    2:  (33,  200, 66),    # Medium Green
    3:  (94,  220, 120),   # Light Green
    4:  (84,  85,  237),   # Dark Blue
    5:  (125, 118, 252),   # Light Blue
    6:  (212, 82,  77),    # Dark Red
    7:  (66,  235, 245),   # Cyan
    8:  (252, 85,  84),    # Medium Red
    9:  (255, 121, 120),   # Light Red
    10: (212, 193, 84),    # Dark Yellow
    11: (230, 206, 128),   # Light Yellow
    12: (33,  176, 59),    # Dark Green
    13: (201, 91,  186),   # Magenta
    14: (204, 204, 204),   # Gray
    15: (255, 255, 255),   # White
}

# ─── 8x8 Font Patterns (MSX-style) ───────────────────────────────────
# Each character is 8 bytes, each byte is one row (MSB = leftmost pixel)
FONT = {
    ' ': [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00],
    'A': [0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00],
    'C': [0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00],
    'E': [0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00],
    'I': [0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00],
    'L': [0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00],
    'M': [0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00],
    'N': [0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00],
    'O': [0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00],
    'S': [0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00],
    'T': [0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00],
    'X': [0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00],
    'Z': [0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00],
}

# ─── Character Code Assignments ──────────────────────────────────────
# Each group of 8 character codes shares one foreground/background color pair.
# Codes are packed contiguously to minimize output size.
#
# Group 0 (chars 0-7):   WHITE on DARK_BLUE   — "ASSOCIAZIONE" letters (A S O C I Z N) + blue space
# Group 1 (chars 8-15):  WHITE on BLACK       — MSX letters + black space
# Group 2 (chars 16-23): BLACK on MEDIUM_GREEN — IT section
# Group 3 (chars 24-31): BLACK on WHITE        — AL section
# Group 4 (chars 32-39): BLACK on MEDIUM_RED   — IA section
# Group 5 (chars 40-47): WHITE on DARK_BLUE   — E (for ASSOCIAZIONE)
#
# Note: BLUE_SP is char 0, so VRAM init (all zeros) gives a blue screen.

CHAR_DEFS = {
    # Group 0 (chars 0-7): WHITE on DARK_BLUE — ASSOCIAZIONE letters + blue space
    0: (' ', WHITE, DARK_BLUE),  # blue space (char 0 = default VRAM fill)
    1: ('A', WHITE, DARK_BLUE),
    2: ('S', WHITE, DARK_BLUE),
    3: ('O', WHITE, DARK_BLUE),
    4: ('C', WHITE, DARK_BLUE),
    5: ('I', WHITE, DARK_BLUE),
    6: ('Z', WHITE, DARK_BLUE),
    7: ('N', WHITE, DARK_BLUE),
    # Group 1 (chars 8-15): WHITE on BLACK
    8: (' ', WHITE, BLACK),      # black space
    9: ('M', WHITE, BLACK),
    10: ('S', WHITE, BLACK),
    11: ('X', WHITE, BLACK),
    # Group 2 (chars 16-23): BLACK on MEDIUM_GREEN
    16: (' ', BLACK, MEDIUM_GREEN),
    17: ('I', BLACK, MEDIUM_GREEN),
    18: ('T', BLACK, MEDIUM_GREEN),
    # Group 3 (chars 24-31): BLACK on WHITE
    24: (' ', BLACK, WHITE),
    25: ('A', BLACK, WHITE),
    26: ('L', BLACK, WHITE),
    # Group 4 (chars 32-39): BLACK on MEDIUM_RED
    32: (' ', BLACK, MEDIUM_RED),
    33: ('I', BLACK, MEDIUM_RED),
    34: ('A', BLACK, MEDIUM_RED),
    # Group 5 (chars 40-47): WHITE on DARK_BLUE — E
    40: ('E', WHITE, DARK_BLUE),
}

# Named references for readability
BLUE_SP = 0  # char 0 = blue space, so cleared VRAM shows blue screen
BLUE_A, BLUE_S, BLUE_O, BLUE_C = 1, 2, 3, 4
BLUE_I, BLUE_Z, BLUE_N = 5, 6, 7
BLACK_SP, BLACK_M, BLACK_S, BLACK_X = 8, 9, 10, 11
GREEN_SP, GREEN_I, GREEN_T = 16, 17, 18
WHITE_SP, WHITE_A, WHITE_L = 24, 25, 26
RED_SP, RED_I, RED_A = 32, 33, 34
BLUE_E = 40


def build_pgt():
    """Build Pattern Generator Table sized to the highest used char code."""
    max_code = max(CHAR_DEFS)
    pgt = bytearray((max_code + 1) * 8)
    for code, (letter, _, _) in CHAR_DEFS.items():
        pattern = FONT[letter]
        for row in range(8):
            pgt[code * 8 + row] = pattern[row]
    return pgt


def build_ct():
    """Build Color Table sized to the highest used color group."""
    max_group = max(code // 8 for code in CHAR_DEFS)
    ct = bytearray(max_group + 1)
    for code, (_, fg, bg) in CHAR_DEFS.items():
        ct[code // 8] = (fg << 4) | bg
    return ct


def build_pnt():
    """Build Pattern Name Table (768 bytes = 32x24)."""
    COLS, ROWS = 32, 24
    pnt = bytearray(COLS * ROWS)

    # Fill entire screen with black space
    for i in range(COLS * ROWS):
        pnt[i] = BLUE_SP

    # ── Row 8: "ASSOCIAZIONE" on full-width blue bar ──
    row = 8
    # Fill entire row with blue space
    for col in range(COLS):
        pnt[row * COLS + col] = BLUE_SP

    # Spell out "ASSOCIAZIONE" centered (12 chars → offset 10)
    assoc = "ASSOCIAZIONE"
    assoc_map = {
        'A': BLUE_A, 'S': BLUE_S, 'O': BLUE_O, 'C': BLUE_C,
        'I': BLUE_I, 'Z': BLUE_Z, 'N': BLUE_N, 'E': BLUE_E,
    }
    offset = (COLS - len(assoc)) // 2  # 10
    for i, ch in enumerate(assoc):
        pnt[row * COLS + offset + i] = assoc_map[ch]

    # ── Row 9: "MSX" centered ──
    row = 9
    msx = "MSX"
    msx_map = {'M': BLACK_M, 'S': BLACK_S, 'X': BLACK_X}
    for i in range(10, 22):
        pnt[row * COLS + i] = BLACK_SP
    offset = (COLS - len(msx)) // 2  # 14
    for i, ch in enumerate(msx):
        pnt[row * COLS + offset + i] = msx_map[ch]

    # ── Row 10: "ITALIA" tricolor bar (full-width) ──
    row = 10
    # Italian flag: green | white | red — split 32 columns into 3 bands
    # 11 green + 10 white + 11 red = 32
    green_start = 10
    green_end = 14
    white_end = 18
    red_end = 22

    for col in range(COLS):
        if col < green_start:
            continue
        if col < green_end:
            pnt[row * COLS + col] = GREEN_SP
        elif col < white_end:
            pnt[row * COLS + col] = WHITE_SP
        elif col < red_end:
            pnt[row * COLS + col] = RED_SP

    # "IT" centered in green band (cols 0-10, width 11)
    it_offset = 11
    pnt[row * COLS + it_offset]     = GREEN_I
    pnt[row * COLS + it_offset + 1] = GREEN_T

    # "AL" centered in white band (cols 11-20, width 10)
    al_offset = green_end + (white_end - green_end - 2) // 2  # 11 + 4 = 15
    pnt[row * COLS + al_offset]     = WHITE_A
    pnt[row * COLS + al_offset + 1] = WHITE_L

    # "IA" centered in red band (cols 21-31, width 11)
    ia_offset = 19
    pnt[row * COLS + ia_offset]     = RED_I
    pnt[row * COLS + ia_offset + 1] = RED_A

    return pnt


def generate_preview(pgt, pnt, ct, filename):
    """Generate a PPM preview image (256x192, no external dependencies)."""
    WIDTH, HEIGHT = 256, 192
    COLS, ROWS = 32, 24

    pixels = bytearray(WIDTH * HEIGHT * 3)

    for char_row in range(ROWS):
        for char_col in range(COLS):
            char_code = pnt[char_row * COLS + char_col]
            group = char_code // 8
            color_byte = ct[group]
            fg_color = (color_byte >> 4) & 0x0F
            bg_color = color_byte & 0x0F

            fg_rgb = TMS_RGB[fg_color]
            bg_rgb = TMS_RGB[bg_color]

            for py in range(8):
                pattern_byte = pgt[char_code * 8 + py]
                for px in range(8):
                    bit = (pattern_byte >> (7 - px)) & 1
                    rgb = fg_rgb if bit else bg_rgb

                    x = char_col * 8 + px
                    y = char_row * 8 + py
                    offset = (y * WIDTH + x) * 3
                    pixels[offset]     = rgb[0]
                    pixels[offset + 1] = rgb[1]
                    pixels[offset + 2] = rgb[2]

    with open(filename, 'wb') as f:
        f.write(f"P6\n{WIDTH} {HEIGHT}\n255\n".encode())
        f.write(pixels)


def generate_c_header(pgt, pnt, ct, filename):
    """Generate C header file with data arrays for MSXgl."""
    with open(filename, 'w') as f:
        f.write("// Associazione MSX Italia - Screen 1 (Graphics Mode 1) data\n")
        f.write("// Auto-generated by gen_ami_screen1.py\n")
        f.write("//\n")
        f.write("// Usage with MSXgl:\n")
        f.write("//   VDP_SetMode(VDP_MODE_SCREEN1);\n")
        f.write("//   VDP_WriteVRAM_16K(g_AMI_PatternTable, 0x0000, sizeof(g_AMI_PatternTable));\n")
        f.write("//   VDP_WriteVRAM_16K(g_AMI_ColorTable,   0x2000, sizeof(g_AMI_ColorTable));\n")
        f.write("//   VDP_WriteVRAM_16K(g_AMI_NameTable,    0x1800, sizeof(g_AMI_NameTable));\n")
        f.write("//\n\n")
        f.write("#pragma once\n\n")
        f.write("#include \"msxgl.h\"\n\n")

        # Pattern Generator Table
        f.write(f"// Pattern Generator Table ({len(pgt)} bytes) -> VRAM 0x0000\n")
        f.write("const u8 g_AMI_PatternTable[] = {\n")
        for i in range(0, len(pgt), 16):
            f.write("\t")
            f.write(", ".join(f"0x{b:02X}" for b in pgt[i:i+16]))
            f.write(",\n")
        f.write("};\n\n")

        # Color Table
        f.write(f"// Color Table ({len(ct)} bytes) -> VRAM 0x2000\n")
        f.write("// Format: high nibble = foreground, low nibble = background\n")
        f.write("const u8 g_AMI_ColorTable[] = {\n")
        f.write("\t")
        f.write(", ".join(f"0x{b:02X}" for b in ct))
        f.write("\n};\n\n")

        # Pattern Name Table
        f.write("// Pattern Name Table (128 bytes) -> VRAM 0x1800\n")
        f.write("// 32 columns x 4 rows, row-major order\n")
        f.write("const u8 g_AMI_NameTable[] = {\n")
        for row in range(8, 12):
            row_data = pnt[row * 32:(row + 1) * 32]
            f.write("\t")
            f.write(", ".join(f"0x{b:02X}" for b in row_data))
            f.write(",\n")
        f.write("};\n")


def main():
    pgt = build_pgt()
    ct = build_ct()
    pnt = build_pnt()

    # Write binary files
    bin_pgt = os.path.join(OUT_DIR, "ami_pgt.bin")
    bin_pnt = os.path.join(OUT_DIR, "ami_pnt.bin")
    bin_ct  = os.path.join(OUT_DIR, "ami_ct.bin")

    with open(bin_pgt, 'wb') as f:
        f.write(pgt)
    with open(bin_pnt, 'wb') as f:
        f.write(pnt)
    with open(bin_ct, 'wb') as f:
        f.write(ct)

    print(f"Written: {bin_pgt} ({len(pgt)} bytes)")
    print(f"Written: {bin_pnt} ({len(pnt)} bytes)")
    print(f"Written: {bin_ct} ({len(ct)} bytes)")

    # Write C header
    header = os.path.join(OUT_DIR, "ami_screen1.h")
    generate_c_header(pgt, pnt, ct, header)
    print(f"Written: {header}")

    # Write PPM preview
    preview = os.path.join(OUT_DIR, "ami_preview.ppm")
    generate_preview(pgt, pnt, ct, preview)
    print(f"Written: {preview}")

    # Print summary
    print("\n--- Screen 1 Layout Summary ---")
    print("Row  8: [BLUE BAR] ASSOCIAZIONE (white on dark blue)")
    print("Row 11: MSX (white on black, centered)")
    print("Row 14: [TRICOLOR] IT(green) AL(white) IA(red) (black text)")
    print(f"\nCharacter groups used: 6 of 32")
    print(f"Unique patterns defined: {len(CHAR_DEFS)}")


if __name__ == "__main__":
    main()
