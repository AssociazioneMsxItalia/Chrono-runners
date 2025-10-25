package it.associazionemsx;

import java.io.*;
import java.nio.file.*;

public class ExtarctTilesMagellan {	
	public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Uso: java Main <file_input>");
            return;
        }

        String inputFile = args[0];

        try {
            Extractor extractor = new Extractor(inputFile);
            extractor.process();
            System.out.println("Operazione completata: files generati (tiles.txt, colors.txt)");
        } catch (Exception e) {
            System.err.println("Errore: " + e.getMessage());
        }
    }
}
