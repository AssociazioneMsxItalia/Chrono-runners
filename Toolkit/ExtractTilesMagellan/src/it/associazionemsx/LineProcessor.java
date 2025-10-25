package it.associazionemsx;

import java.util.ArrayList;
import java.util.List;

public class LineProcessor {  
    /**
     * Elabora una riga proveniente dal file sorgente, applicando
     * le trasformazioni richieste (pulizia, separazione byte, eventuale complemento, prefisso 0x).
     *
     * @param line         Riga di testo originale
     * @param complemento  true se va applicato il complemento a 1 (PAT), false per i colori (COL)
     * @return             Riga elaborata pronta per la scrittura
     */
    public String processLine(String line, boolean complemento) {
        String data = extractDataAfterMarker(line);
        if (data.isEmpty()) return "";

        List<String> bytes = splitInBytes(data);

        if (complemento) {
            bytes = applyOnesComplement(bytes);
        }

        bytes = addHexPrefix(bytes);

        // Riassembla la linea in formato "0xAA,0xBB,0xCC,"
        return String.join(",", bytes) + ",";
    }

    /**
     * Estrae solo la parte utile della riga, cioè il contenuto dopo il primo '>'.
     */
    private String extractDataAfterMarker(String line) {
        int index = line.indexOf('>');
        if (index == -1 || index >= line.length() - 1) return "";

        // Rimuove tutto prima e dopo il marker, e simboli inutili
        return line.substring(index + 1)
                   .replace(">", "")
                   .replace(",", "")
                   .trim();
    }

    /**
     * Divide la stringa in byte da 2 caratteri ciascuno.
     */
    private List<String> splitInBytes(String data) {
        List<String> bytes = new ArrayList<>();
        for (int i = 0; i < data.length(); i += 2) {
            if (i + 2 <= data.length()) {
                bytes.add(data.substring(i, i + 2).toUpperCase());
            }
        }
        return bytes;
    }

    /**
     * Applica il complemento a 1 a ciascun byte esadecimale.
     */
    private List<String> applyOnesComplement(List<String> bytes) {
        List<String> result = new ArrayList<>(bytes.size());
        for (String b : bytes) {
            try {
                int value = Integer.parseInt(b, 16);
                int complemented = 0xFF - value;
                result.add(String.format("%02X", complemented));
            } catch (NumberFormatException e) {
                result.add("??");
            }
        }
        return result;
    }

    /**
     * Aggiunge il prefisso "0x" davanti a ogni valore.
     */
    private List<String> addHexPrefix(List<String> bytes) {
        List<String> result = new ArrayList<>(bytes.size());
        for (String b : bytes) {
            result.add("0x" + b);
        }
        return result;
    }
}

