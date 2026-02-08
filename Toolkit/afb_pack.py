#!/usr/bin/env python3
"""
Pack multiple binary .afx files into an .afb bank.

AFB format:
  [0]          : Effect count (u8, 0 means 256)
  [1..2*N]     : N x 16-bit LE relative increments (offset table)
  [header_end..]: Effect data + null-terminated name strings, concatenated

Each offset table entry is a relative increment: the player computes
  effect_addr = &table_entry_high_byte + increment_value
which the Z80 does efficiently with pointer arithmetic.

Between effects, a null-terminated ASCII name (the source filename without
extension) is embedded as metadata. It sits after the D0 20 end-of-stream
marker and is never read by the player.
"""

import argparse
import os
import sys


def build_afb(effects):
    """Build AFB bank bytes from a list of (name, data) tuples.

    Args:
        effects: list of (name: str, data: bytes) tuples

    Returns:
        bytes: the complete AFB bank
    """
    count = len(effects)
    if count == 0:
        raise ValueError("No effects provided")
    if count > 256:
        raise ValueError(f"Too many effects: {count} (max 256)")

    header_size = 1 + count * 2

    # Compute where each effect's data will start
    effect_starts = []
    pos = header_size
    for name, data in effects:
        effect_starts.append(pos)
        pos += len(data) + len(name.encode('ascii')) + 1  # +1 for null terminator

    # Compute relative increments for the offset table
    # From the Z80 code: effect_addr = (1 + id*2 + 1) + value
    # So: value = effect_addr - 2 - id*2
    offsets = []
    for i, start in enumerate(effect_starts):
        value = start - (2 + i * 2)
        if value < 0 or value > 0xFFFF:
            raise ValueError(
                f"Offset overflow for effect {i} ({effects[i][0]}): "
                f"value {value} does not fit in 16 bits"
            )
        offsets.append(value)

    # Assemble the bank
    bank = bytearray()

    # Byte 0: count (0 encodes as 256 in the player)
    bank.append(count & 0xFF)

    # Offset table: N x 16-bit LE
    for val in offsets:
        bank.append(val & 0xFF)
        bank.append((val >> 8) & 0xFF)

    # Effect data + name strings
    for name, data in effects:
        bank.extend(data)
        bank.extend(name.encode('ascii'))
        bank.append(0x00)

    return bytes(bank)


def format_as_header(data, var_name):
    """Format binary data as a C header array, matching MSXbin style.

    Args:
        data: bytes to encode
        var_name: C variable name

    Returns:
        str: the complete .h file content
    """
    lines = []
    lines.append(f'#include "msxgl.h"\n')
    lines.append(f'const u8 {var_name}[] = {{')

    for i in range(0, len(data), 16):
        chunk = data[i:i + 16]
        hex_str = ', '.join(f'0x{b:02X}' for b in chunk)
        lines.append(f'\t{hex_str}, ')

    lines.append('};')
    lines.append(f'// Total bytes: {len(data)}')

    return '\n'.join(lines) + '\n'


def main():
    parser = argparse.ArgumentParser(
        description='Pack binary .afx files into an .afb bank.'
    )
    parser.add_argument(
        'afx_files', nargs='*', metavar='FILE',
        help='Input .afx files (binary format)'
    )
    parser.add_argument(
        '-f', '--filelist', metavar='PATH',
        help='Text file listing .afx paths, one per line'
    )
    parser.add_argument(
        '-o', '--output', required=True, metavar='PATH',
        help='Output file path'
    )
    parser.add_argument(
        '--header', action='store_true',
        help='Output as C header (.h) instead of raw binary'
    )
    parser.add_argument(
        '--var', metavar='NAME',
        help='C variable name for header output (default: g_<output_basename>)'
    )

    args = parser.parse_args()

    # Collect .afx paths from command line and/or filelist
    afx_paths = list(args.afx_files)
    if args.filelist:
        list_dir = os.path.dirname(os.path.abspath(args.filelist))
        with open(args.filelist) as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                # Resolve relative paths against the filelist's directory
                if not os.path.isabs(line):
                    line = os.path.join(list_dir, line)
                afx_paths.append(line)
    if not afx_paths:
        parser.error('No .afx files specified (use positional args or --filelist)')

    # Read all input .afx files
    effects = []
    for path in afx_paths:
        name = os.path.splitext(os.path.basename(path))[0]
        with open(path, 'rb') as f:
            data = f.read()
        if len(data) < 2:
            print(f'Warning: {path} is very short ({len(data)} bytes)', file=sys.stderr)
        effects.append((name, data))
        print(f'  [{len(effects) - 1}] {name} ({len(data)} bytes)')

    # Build the AFB bank
    bank = build_afb(effects)
    print(f'Bank: {len(effects)} effects, {len(bank)} bytes')

    # Write output
    if args.header:
        if args.var:
            var_name = args.var
        else:
            base = os.path.splitext(os.path.basename(args.output))[0]
            var_name = 'g_' + base
        content = format_as_header(bank, var_name)
        with open(args.output, 'w') as f:
            f.write(content)
    else:
        with open(args.output, 'wb') as f:
            f.write(bank)

    print(f'Written: {args.output}')


if __name__ == '__main__':
    main()
