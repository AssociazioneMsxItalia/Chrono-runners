#!/usrusr/bin/env python3
# -*- coding: utf-8 -*-

"""
Utility essenziale per convertire un file dati (formato MD)
in un file header C (.h) contenente un array di byte.

Uso: python3 map2msx.py <input_file> <output_file>
"""

import sys

def convert_to_c_header(input_file, output_file):
    """
    Legge il file di input, processa i dati dalla sezione MD
    e scrive l'header C nel file di output.
    """
    byte_list = []
    processing = False  # Flag per sezione MD

    with open(input_file, 'r', encoding='utf-8') as infile:
        for line in infile:
            line = line.strip()

            # Attiva il processing alla prima riga MD
            if not processing and line.startswith("MD"):
                processing = True

            # Se stiamo processando e la riga contiene "DATA"
            if processing and "DATA" in line:
                try:
                    data_part = line.split("DATA", 1)[1]
                except IndexError:
                    continue

                # Pulisce la stringa e splitta per word
                data_part = data_part.replace(">", "").strip()
                words = data_part.split(',')

                for word in words:
                    word = word.strip()
                    # Divide la word (es. "2C2C") in due byte (es. "0x2c", "0x2c")
                    if len(word) == 4:
                        byte_list.append(f"0x{word[0:2].lower()}")
                        byte_list.append(f"0x{word[2:4].lower()}")

    if not byte_list:
        print("Attenzione: Nessun dato 'MD' trovato.", file=sys.stderr)

    # Scrittura del file di output
    with open(output_file, 'w', encoding='utf-8') as outfile:
        outfile.write("const unsigned char g_Level001[] = {\n")

        if byte_list:
            outfile.write("    ")  # Indentazione
            bytes_per_line = 32

            for i, byte_val in enumerate(byte_list):
                outfile.write(byte_val)

                if i < len(byte_list) - 1:
                    outfile.write(", ")

                if (i + 1) % bytes_per_line == 0 and i < len(byte_list) - 1:
                    outfile.write("\n    ")

        outfile.write("\n};\n")

if __name__ == "__main__":
    # Verifica il numero corretto di argomenti
    if len(sys.argv) != 3:
        print(f"Errore: Numero di argomenti non valido.", file=sys.stderr)
        print(f"Uso: python3 {sys.argv[0]} <input_file> <output_file>", file=sys.stderr)
        sys.exit(1) # Esce con codice di errore

    # Estrae gli argomenti
    input_file_path = sys.argv[1]
    output_file_path = sys.argv[2]

    # Lancia la funzione di conversione
    convert_to_c_header(input_file_path, output_file_path)
