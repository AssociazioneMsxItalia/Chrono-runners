package it.associazionemsx;

import java.io.BufferedWriter;
import java.io.IOException;

import java.io.IOException;

import java.io.BufferedWriter;
import java.io.IOException;

public class ColorWriter extends AbstractWriter {
    public ColorWriter(BufferedWriter writer) {
        super(writer);
    }

    @Override
    public void write(String processedLine) throws IOException {
        writeLine(processedLine);
    }
}
