### CONVERTER FROM MIDI TO VORTEX TEXT FILE
### RIADATTATO DA FABRIZIO LUGLIO 

import mido
from collections import defaultdict

class MIDIToVortex:
    def __init__(self):
        # Mappa delle note MIDI (numero) a note VortexTracker (es: C-4, D#3, etc.)
        self.note_names = ['C-', 'C#', 'D-', 'D#', 'E-', 'F-', 'F#', 'G-', 'G#', 'A-', 'A#', 'B-']
        
    def midi_note_to_vortex(self, midi_note):
        """Converte un numero di nota MIDI in formato VortexTracker (es: C-4)"""
        octave = (midi_note // 12) - 1
        note = self.note_names[midi_note % 12]
        return f"{note}{octave}"
    
    def convert(self, midi_file, output_file, speed=6, rows_per_note=4):
        """
        Converte un file MIDI in formato VortexTracker II
        
        Args:
            midi_file: path del file MIDI da leggere
            output_file: path del file .vtxt da creare
            speed: velocità di playback (default 6)
            rows_per_note: quante righe VortexTracker per ogni nota MIDI (default 4)
        """
        mid = mido.MidiFile(midi_file)
        
        # Calcola i tick per riga basandosi sul rapporto desiderato
        ticks_per_row = mid.ticks_per_beat // (rows_per_note // 4) if rows_per_note >= 4 else mid.ticks_per_beat
        
        # Struttura per memorizzare le note per canale e posizione
        patterns = defaultdict(lambda: defaultdict(list))
        tracks_data = []
        
        # Processa ogni traccia MIDI
        for track_idx, track in enumerate(mid.tracks):
            current_tick = 0
            active_notes = {}
            
            for msg in track:
                current_tick += msg.time
                # Moltiplica per rows_per_note per avere la spaziatura corretta
                row = (current_tick * rows_per_note) // mid.ticks_per_beat
                
                if msg.type == 'note_on' and msg.velocity > 0:
                    note = self.midi_note_to_vortex(msg.note)
                    channel = msg.channel % 3  # VortexTracker ha 3 canali
                    active_notes[msg.note] = (row, channel)
                    patterns[row][channel].append(note)
                    
                elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                    if msg.note in active_notes:
                        del active_notes[msg.note]
        
        # Genera il file VTXT
        self.write_vtxt(output_file, patterns, speed)
    
    def write_vtxt(self, output_file, patterns, speed):
        """Scrive il file in formato VortexTracker II"""
        max_pattern = max(patterns.keys()) if patterns else 0
        num_patterns = (max_pattern // 64) + 1  # 64 righe per pattern
        
        with open(output_file, 'w') as f:
            # Header
            f.write("[Module]\n")
            f.write("VortexTrackerII=1\n")
            f.write("Version=3.6\n")
            f.write("Title=MIDI Conversion\n")
            f.write("Author=\n")
            f.write("NoteTable=2\n")
            f.write(f"Speed={speed}\n")
            
            # PlayOrder
            play_order = ','.join([str(i) for i in range(num_patterns)])
            f.write(f"PlayOrder=L{play_order}\n\n")
            
            # Ornaments (vuoti)
            for i in range(1, 16):
                f.write(f"[Ornament{i}]\nL0\n\n")
            
            # Samples
            f.write("[Sample1]\n")
            f.write("Tne +000_ +00_ F_ L\n")
            f.write("Tne +000_ +00_ F_\n")
            f.write("Tne +000_ +00_ F_\n")
            f.write("Tne +000_ +00_ F_\n")
            f.write("Tne +000_ +00_ F-\n\n")
            
            for i in range(2, 32):
                f.write(f"[Sample{i}]\n")
                f.write("tne +000_ +00_ 0_ L\n\n")
            
            # Patterns
            for pattern_num in range(num_patterns):
                f.write(f"[Pattern{pattern_num}]\n")
                
                for row in range(64):
                    global_row = pattern_num * 64 + row
                    channels_data = []
                    
                    for channel in range(3):  # 3 canali
                        if global_row in patterns and channel in patterns[global_row]:
                            notes = patterns[global_row][channel]
                            if notes:
                                note = notes[0]  # Prendi la prima nota se ce ne sono multiple
                                channels_data.append(f"{note} 1...")
                            else:
                                channels_data.append("--- ....")
                        else:
                            channels_data.append("--- ....")
                    
                    # Aggiungi il quarto campo vuoto (senza ....|)
                    channels_data.append("....")
                    
                    # Unisci i canali: primo con ....|..|, altri con ....|, ultimo senza separatore
                    line = "....|..|" + " ....|".join(channels_data[:3]) + " " + channels_data[3]
                    f.write(line + "\n")
                
                f.write("\n")

# MAIN ------------------------------

converter = MIDIToVortex()              
# Specifica il file MIDI di input e output
midi_input = "filename.mid"  # Cambia con il tuo file MIDI
vtxt_output = "filename.txt"
                
try:
# rows_per_note=4 significa che ogni nota MIDI occupa 4 righe nel pattern
# (1 riga con la nota + 3 righe vuote)
    converter.convert(midi_input, vtxt_output, speed=6, rows_per_note=4)
    print(f"Conversione completata! File salvato in: {vtxt_output}")
    print(f"Nota: Ogni nota MIDI occupa 4 righe nel pattern VortexTracker")
except FileNotFoundError:
    print(f"Errore: File {midi_input} non trovato")
except Exception as e:
    print(f"Errore durante la conversione: {e}")
