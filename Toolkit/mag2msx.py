import sys

n_sprites = 13
n_layers = 2

sprites = []

# Importa i dati del file .asm
with open(sys.argv[1], "r") as fp:
    sprite = []
    for line in fp:

        if line.startswith("SPR"):
            if sprite:
                sprites.append(sprite)
            sprite = []

        # Ignora l'header DATA
        words = line.split(">")[1:]

        # Prende solo i primi 4 caratteri
        words = map(lambda w: w[0:4], words)

        for w in words:

            sprite.append(int(w[0:2], 16))
            sprite.append(int(w[2:4], 16))

    sprites.append(sprite)

# Scrive i dati nel file header in formato MSXgl
with open("data_sprt_layer.h", "w") as fp:

    fp.write("const unsigned char g_DataSprtLayer[] =\n")
    fp.write("{\n")

    for n_spr in range(n_sprites):
        fp.write("// Frame %d\n" % n_spr)
        for n_lay in range(n_layers):
            fp.write("// Layer %d\n" % n_lay)
            for i_val, val in enumerate(sprites[n_spr * n_layers + n_lay]):

                # Ogni 8 byte va a capo per leggibilità
                if i_val != 0 and i_val % 8 == 0:
                    fp.write("\n")

                fp.write("0x"+f"{val:02X}")
                fp.write(",")

            fp.write("\n")

        fp.write("\n")

    fp.write("};\n")
