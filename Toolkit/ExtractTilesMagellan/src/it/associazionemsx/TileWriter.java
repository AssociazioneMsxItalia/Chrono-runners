package it.associazionemsx;

import java.io.BufferedWriter;
import java.io.IOException;

import java.io.IOException;

import java.io.BufferedWriter;
import java.io.IOException;

public class TileWriter extends AbstractWriter {
    private int tileCount = 0;

    public TileWriter(BufferedWriter writer) {
        super(writer);
    }

    @Override
    public void write(String processedLine) throws IOException {
        String[] values = processedLine.split(",");
        StringBuilder tileLine = new StringBuilder();
        int count = 0;

        for (String val : values) {
            val = val.trim();
            if (val.isEmpty()) continue;

            if (tileLine.length() > 0) {
                tileLine.append(",");
            }
            tileLine.append(val);
            count++;

            if (count % 8 == 0) {
                //writeLine("; Tile " + tileCount++);
                writeLine(tileLine.toString() + ",");
                tileLine.setLength(0);
            }
        }

        if (tileLine.length() > 0) {
            //writeLine("; Tile " + tileCount++);
            writeLine(tileLine.toString() + ",");
        }
    }
}
