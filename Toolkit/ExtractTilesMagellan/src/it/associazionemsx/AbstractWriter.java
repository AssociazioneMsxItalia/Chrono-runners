package it.associazionemsx;

import java.io.BufferedWriter;
import java.io.IOException;

import java.io.BufferedWriter;
import java.io.IOException;

public abstract class AbstractWriter {
    protected final BufferedWriter writer;

    public AbstractWriter(BufferedWriter writer) {
        this.writer = writer;
    }

    protected void writeLine(String line) throws IOException {
        writer.write(line);
        writer.newLine();
    }

    public abstract void write(String processedLine) throws IOException;
}
