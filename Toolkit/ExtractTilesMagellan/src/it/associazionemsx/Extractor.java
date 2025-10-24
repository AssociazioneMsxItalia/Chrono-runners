package it.associazionemsx;

import java.io.*;
import java.nio.file.*;

public class Extractor {
    private final String inputFile;

    public Extractor(String inputFile) {
        this.inputFile = inputFile;
    }

    public void process() throws IOException {
        try (
            BufferedReader reader = Files.newBufferedReader(Paths.get(inputFile));
            BufferedWriter tilesWriter = Files.newBufferedWriter(Paths.get("tiles.txt"));
            BufferedWriter colorsWriter = Files.newBufferedWriter(Paths.get("colors.txt"))
        ) {
            LineProcessor processor = new LineProcessor();

            TileWriter tileWriter = new TileWriter(tilesWriter);
            ColorWriter colorWriter = new ColorWriter(colorsWriter);

            String line;
            while ((line = reader.readLine()) != null) {
                if (line.startsWith("PAT")) {
                    String processed = processor.processLine(line, false);
                    tileWriter.write(processed);
                } else if (line.startsWith("COL")) {
                    String processed = processor.processLine(line, false);
                    colorWriter.write(processed);
                }
            }
        }
    }
}
