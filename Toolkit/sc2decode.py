#!/usr/bin/env python3
"""
SC2 Decoder - Converts MSX SCREEN 2 (SC2) files to PNG images or C headers

SC2 files are VRAM dumps from MSX SCREEN 2 mode (TMS9918 Graphic Mode 2).
They typically have a 7-byte BLOAD header followed by raw VRAM data.

VRAM Layout for SCREEN 2:
  - Pattern Table  @ 0x0000: 6144 bytes (768 tiles × 8 bytes)
  - Name Table     @ 0x1800: 768 bytes  (32×24 tile indices)
  - Color Table    @ 0x2000: 6144 bytes (2 colors per 8-pixel row)

Usage:
  python sc2decode.py input.sc2 [output.png]      # Export to PNG
  python sc2decode.py input.sc2 -f header         # Export to C header
  python sc2decode.py input.sc2 --info            # Show file info only
"""

import argparse
import struct
import sys
from pathlib import Path

# TMS9918 color palette (RGB values)
TMS9918_PALETTE = [
    (0x00, 0x00, 0x00),  # 0: Transparent (rendered as black)
    (0x00, 0x00, 0x00),  # 1: Black
    (0x21, 0xC8, 0x42),  # 2: Medium Green
    (0x5E, 0xDC, 0x78),  # 3: Light Green
    (0x54, 0x55, 0xED),  # 4: Dark Blue
    (0x7D, 0x76, 0xFC),  # 5: Light Blue
    (0xD4, 0x52, 0x4D),  # 6: Dark Red
    (0x42, 0xEB, 0xF5),  # 7: Cyan
    (0xFC, 0x55, 0x54),  # 8: Medium Red
    (0xFF, 0x79, 0x78),  # 9: Light Red
    (0xD4, 0xC1, 0x54),  # 10: Dark Yellow
    (0xE6, 0xCE, 0x80),  # 11: Light Yellow
    (0x21, 0xB0, 0x3B),  # 12: Dark Green
    (0xC9, 0x5B, 0xBA),  # 13: Magenta
    (0xCC, 0xCC, 0xCC),  # 14: Gray
    (0xFF, 0xFF, 0xFF),  # 15: White
]

# VRAM addresses for SCREEN 2
VRAM_PATTERN_TABLE = 0x0000
VRAM_NAME_TABLE = 0x1800
VRAM_COLOR_TABLE = 0x2000

# Sizes
PATTERN_TABLE_SIZE = 6144  # 768 patterns × 8 bytes
NAME_TABLE_SIZE = 768       # 32 × 24 tiles
COLOR_TABLE_SIZE = 6144     # Same as pattern table

SCREEN_WIDTH = 256
SCREEN_HEIGHT = 192
TILES_X = 32
TILES_Y = 24
TILE_SIZE = 8


def parse_bload_header(data):
    """Parse the 7-byte BLOAD header if present."""
    if len(data) < 7:
        return None, data

    if data[0] == 0xFE:
        # BLOAD header present
        start_addr = struct.unpack('<H', data[1:3])[0]
        end_addr = struct.unpack('<H', data[3:5])[0]
        exec_addr = struct.unpack('<H', data[5:7])[0]
        return {
            'start': start_addr,
            'end': end_addr,
            'exec': exec_addr,
            'size': end_addr - start_addr + 1
        }, data[7:]

    return None, data


def generate_sequential_name_table():
    """Generate a sequential name table (0-255 repeated 3 times)."""
    return bytes(list(range(256)) * 3)


def extract_vram_tables(vram_data, header_info):
    """Extract pattern, name, and color tables from VRAM data.

    Supports two layouts:
      1. Full VRAM layout (>=14336 bytes): Pattern@0x0000, Name@0x1800, Color@0x2000
      2. Packed format (12288 bytes): Pattern(6144) + Color(6144), sequential name table implied
    """
    data_len = len(vram_data)

    # Packed format: exactly 2x6144 bytes = Pattern + Color, no name table
    if not header_info and data_len == PATTERN_TABLE_SIZE + COLOR_TABLE_SIZE:
        pattern_table = vram_data[:PATTERN_TABLE_SIZE]
        color_table = vram_data[PATTERN_TABLE_SIZE:]
        name_table = generate_sequential_name_table()
        return pattern_table, name_table, color_table

    # Standard VRAM layout
    base_offset = 0
    if header_info:
        base_offset = header_info['start']

    # Calculate offsets into the data
    pt_offset = VRAM_PATTERN_TABLE - base_offset
    nt_offset = VRAM_NAME_TABLE - base_offset
    ct_offset = VRAM_COLOR_TABLE - base_offset

    # Extract tables
    pattern_table = None
    name_table = None
    color_table = None

    if pt_offset >= 0 and pt_offset + PATTERN_TABLE_SIZE <= data_len:
        pattern_table = vram_data[pt_offset:pt_offset + PATTERN_TABLE_SIZE]

    if nt_offset >= 0 and nt_offset + NAME_TABLE_SIZE <= data_len:
        name_table = vram_data[nt_offset:nt_offset + NAME_TABLE_SIZE]

    if ct_offset >= 0 and ct_offset + COLOR_TABLE_SIZE <= data_len:
        color_table = vram_data[ct_offset:ct_offset + COLOR_TABLE_SIZE]

    return pattern_table, name_table, color_table


def decode_screen2(pattern_table, name_table, color_table):
    """Decode SCREEN 2 data into a pixel array (RGB)."""
    pixels = [[(0, 0, 0) for _ in range(SCREEN_WIDTH)] for _ in range(SCREEN_HEIGHT)]

    for tile_y in range(TILES_Y):
        for tile_x in range(TILES_X):
            # Get the tile index from name table
            name_idx = tile_y * TILES_X + tile_x
            tile_num = name_table[name_idx]

            # In SCREEN 2, the screen is divided into 3 banks of 256 patterns each
            # Bank 0: rows 0-7 (tile_y 0-7)
            # Bank 1: rows 8-15 (tile_y 8-15)
            # Bank 2: rows 16-23 (tile_y 16-23)
            bank = tile_y // 8
            pattern_idx = bank * 256 + tile_num

            # Get pattern and color data for this tile
            pattern_offset = pattern_idx * 8
            color_offset = pattern_idx * 8

            for row in range(TILE_SIZE):
                pattern_byte = pattern_table[pattern_offset + row]
                color_byte = color_table[color_offset + row]

                # Extract foreground and background colors
                fg_color = (color_byte >> 4) & 0x0F
                bg_color = color_byte & 0x0F

                for col in range(TILE_SIZE):
                    # Check if pixel is set (1 = foreground, 0 = background)
                    bit = (pattern_byte >> (7 - col)) & 1
                    color_idx = fg_color if bit else bg_color

                    # Handle transparent color (0) - render as black or background
                    if color_idx == 0:
                        color_idx = 1  # Use black for transparent

                    pixel_x = tile_x * TILE_SIZE + col
                    pixel_y = tile_y * TILE_SIZE + row
                    pixels[pixel_y][pixel_x] = TMS9918_PALETTE[color_idx]

    return pixels


def save_png(pixels, output_path):
    """Save pixel array as PNG using pure Python (no external dependencies)."""
    import zlib

    width = len(pixels[0])
    height = len(pixels)

    def write_chunk(chunk_type, data):
        chunk = chunk_type + data
        crc = zlib.crc32(chunk) & 0xffffffff
        return struct.pack('>I', len(data)) + chunk + struct.pack('>I', crc)

    # PNG signature
    png_data = b'\x89PNG\r\n\x1a\n'

    # IHDR chunk
    ihdr_data = struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)
    png_data += write_chunk(b'IHDR', ihdr_data)

    # IDAT chunk (image data)
    raw_data = b''
    for row in pixels:
        raw_data += b'\x00'  # Filter type: None
        for r, g, b in row:
            raw_data += bytes([r, g, b])

    compressed = zlib.compress(raw_data, 9)
    png_data += write_chunk(b'IDAT', compressed)

    # IEND chunk
    png_data += write_chunk(b'IEND', b'')

    with open(output_path, 'wb') as f:
        f.write(png_data)


def format_byte_array(data, bytes_per_line=8):
    """Format a byte array as C-style hex values, 8 bytes per line."""
    lines = []
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        hex_values = ','.join(f'0x{b:02X}' for b in chunk)
        lines.append(f'    {hex_values},')
    return '\n'.join(lines)


def save_c_header(pattern_table, name_table, color_table, output_path, prefix):
    """Save tables as a C header file in MSXgl format."""
    # Generate array name prefix from output filename if not specified
    if not prefix:
        prefix = Path(output_path).stem
        # Convert to valid C identifier (replace non-alphanumeric with underscore)
        prefix = ''.join(c if c.isalnum() else '_' for c in prefix)
        prefix = f'g_{prefix}'

    content = []

    # Header comment
    content.append(f'// Generated from SC2 file by sc2decode.py')
    content.append(f'// Pattern table: {len(pattern_table)} bytes (768 patterns × 8 bytes)')
    content.append(f'// Color table:   {len(color_table)} bytes (768 patterns × 8 bytes)')
    content.append(f'// Name table:    {len(name_table)} bytes (32 × 24 tiles)')
    content.append('')

    # Color table (output first to match the reference file order)
    content.append(f'const unsigned char {prefix}_Colors[] =')
    content.append('{')
    content.append(format_byte_array(color_table))
    content.append('};')
    content.append('')

    # Pattern table
    content.append(f'const unsigned char {prefix}_Patterns[] =')
    content.append('{')
    content.append(format_byte_array(pattern_table))
    content.append('};')
    content.append('')

    # Name table
    content.append(f'const unsigned char {prefix}_Names[] =')
    content.append('{')
    content.append(format_byte_array(name_table))
    content.append('};')
    content.append('')

    with open(output_path, 'w') as f:
        f.write('\n'.join(content))


def print_file_info(filepath, header_info, vram_data, pattern_table, name_table, color_table):
    """Print detailed information about the SC2 file."""
    print(f"File: {filepath}")
    print(f"Total file size: {Path(filepath).stat().st_size} bytes")
    print()

    if header_info:
        print("BLOAD Header:")
        print(f"  Start address: 0x{header_info['start']:04X}")
        print(f"  End address:   0x{header_info['end']:04X}")
        print(f"  Exec address:  0x{header_info['exec']:04X}")
        print(f"  Data size:     {header_info['size']} bytes")
    else:
        print("No BLOAD header detected (raw VRAM data)")

    print()
    print(f"VRAM data size: {len(vram_data)} bytes")
    print()
    print("Tables extracted:")
    print(f"  Pattern Table: {'Yes' if pattern_table else 'No'} ({len(pattern_table) if pattern_table else 0} bytes)")
    print(f"  Name Table:    {'Yes' if name_table else 'No'} ({len(name_table) if name_table else 0} bytes)")
    print(f"  Color Table:   {'Yes' if color_table else 'No'} ({len(color_table) if color_table else 0} bytes)")

    # Analyze name table if present
    if name_table:
        print()
        print("Name table analysis:")
        # Check if it's sequential (0,1,2,...,255 repeated 3 times)
        is_sequential = True
        for bank in range(3):
            for i in range(256):
                idx = bank * 256 + i
                if name_table[idx] != i:
                    is_sequential = False
                    break
            if not is_sequential:
                break

        if is_sequential:
            print("  Pattern: Sequential (0-255 repeated 3 times)")
        else:
            # Show unique values
            unique = set(name_table)
            print(f"  Unique tile indices: {len(unique)}")
            if len(unique) <= 16:
                print(f"  Values: {sorted(unique)}")


def main():
    parser = argparse.ArgumentParser(
        description='Decode MSX SCREEN 2 (SC2) files to PNG images or C headers',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s image.sc2                    # Output to image.png
  %(prog)s image.sc2 output.png         # Specify output filename
  %(prog)s image.sc2 -f header          # Output to image.h (C header)
  %(prog)s image.sc2 -f header -o out.h # Specify header filename
  %(prog)s image.sc2 --info             # Show file information only
  %(prog)s image.sc2 --no-header        # Treat file as raw VRAM (no BLOAD header)
"""
    )
    parser.add_argument('input', help='Input SC2 file')
    parser.add_argument('output', nargs='?', help='Output file (default: input with new extension)')
    parser.add_argument('-f', '--format', choices=['png', 'header'], default='png',
                        help='Output format: png (image) or header (C header file)')
    parser.add_argument('-p', '--prefix', help='Array name prefix for C header (default: g_<filename>)')
    parser.add_argument('--info', action='store_true', help='Show file information only')
    parser.add_argument('--no-header', action='store_true',
                        help='Treat input as raw VRAM data (no BLOAD header)')
    parser.add_argument('--transparent', choices=['black', 'keep'], default='black',
                        help='How to handle transparent color 0 (default: black)')

    args = parser.parse_args()

    # Read input file
    input_path = Path(args.input)
    if not input_path.exists():
        print(f"Error: File not found: {args.input}", file=sys.stderr)
        return 1

    with open(input_path, 'rb') as f:
        data = f.read()

    # Parse header
    if args.no_header:
        header_info = None
        vram_data = data
    else:
        header_info, vram_data = parse_bload_header(data)

    # Extract tables
    pattern_table, name_table, color_table = extract_vram_tables(vram_data, header_info)

    # Info mode
    if args.info:
        print_file_info(args.input, header_info, vram_data, pattern_table, name_table, color_table)
        return 0

    # Validate that we have all required tables
    if not all([pattern_table, name_table, color_table]):
        print("Error: Could not extract all required tables from file", file=sys.stderr)
        print("  Pattern Table:", "OK" if pattern_table else "MISSING")
        print("  Name Table:", "OK" if name_table else "MISSING")
        print("  Color Table:", "OK" if color_table else "MISSING")
        print("\nTry using --info to see file details, or --no-header if the file lacks a BLOAD header")
        return 1

    # Determine output path and format
    if args.output:
        output_path = Path(args.output)
    else:
        ext = '.h' if args.format == 'header' else '.png'
        output_path = input_path.with_suffix(ext)

    # Export based on format
    if args.format == 'header':
        save_c_header(pattern_table, name_table, color_table, output_path, args.prefix)
        print(f"Saved C header: {output_path}")
        print(f"  Arrays: {args.prefix or 'g_' + output_path.stem}_Patterns, _Colors, _Names")
    else:
        # Decode and save PNG
        pixels = decode_screen2(pattern_table, name_table, color_table)
        save_png(pixels, output_path)
        print(f"Saved PNG: {output_path}")

    return 0


if __name__ == '__main__':
    sys.exit(main())
