#!/usr/bin/env python3

"""
Converte un file di definizione dati testuale in un header C.

Uso: python3 convert.py <file_input.txt> <file_output.h>
"""

import sys

def parse_data_line(line: str) -> str | None:
    """
    Converte una singola riga di dati (es. DATA >XXXX,>XXXX)
    in una stringa di byte formattati C (es. 0xHH,0xLL,0xHH,0xLL).
    """
    # 1. Isola la parte contenente i dati, dopo 'DATA '
    data_part = line.split('DATA ')[1].strip()

    # 2. Separa i valori 16-bit (es. [">6060", ">6060", ...])
    hex_words = data_part.split(',')

    bytes_list = []
    for word in hex_words:
        # 3. Pulisci il valore (rimuovi > e spazi)
        hex_val = word.strip().lstrip('>')

        # 4. Assicurati che sia un valore 16-bit (4 caratteri hex)
        if len(hex_val) == 4:
            # 5. Dividi in High-byte e Low-byte
            high_byte = hex_val[0:2]
            low_byte = hex_val[2:4]

            # 6. Aggiungi alla lista nel formato C
            bytes_list.append(f'0x{high_byte}')
            bytes_list.append(f'0x{low_byte}')
        elif hex_val:
            # Segnala se un valore non è formattato correttamente
            print(f"Attenzione: valore mal formattato ignorato: '{word}'", file=sys.stderr)

    # 7. Unisci tutti gli 8 byte risultanti con una virgola
    return ','.join(bytes_list)

def convert_file(input_file: str, output_file: str):
    """
    Legge il file input, processa le righe e scrive il file output.
    """
    colors_data = []
    patterns_data = []

    # --- Lettura e Processamento ---
    with open(input_file, 'r') as infile:
        for line in infile:
            line_stripped = line.strip()

            if not line_stripped or 'DATA' not in line_stripped:
                continue # Salta righe vuote o non contenenti dati

            c_line = parse_data_line(line_stripped)
            if not c_line:
                continue # Salta righe mal formattate

            if line_stripped.startswith('COL'):
                colors_data.append(c_line)
            elif line_stripped.startswith('PAT'):
                patterns_data.append(c_line)

    # --- Scrittura File Header ---
    with open(output_file, 'w') as outfile:
        # Scrivi sezione Colors
        outfile.write("const unsigned char g_DataMapGM2_Colors[] =\n")
        outfile.write("{\n")
        for line in colors_data:
            outfile.write(f"    {line},\n")
        outfile.write("};\n\n")

        # Scrivi sezione Patterns
        outfile.write("const unsigned char g_DataMapGM2_Patterns[] =\n")
        outfile.write("{\n")
        for line in patterns_data:
            outfile.write(f"    {line},\n")
        outfile.write("};\n")

def main():
    if len(sys.argv) != 3:
        print("Uso: python3 convert.py <file_input.txt> <file_output.h>")
        print("Esempio: python3 convert.py dati.txt mio_header.h")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    convert_file(input_file, output_file)

if __name__ == "__main__":
    main()