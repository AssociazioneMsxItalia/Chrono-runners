# Chrono Runner By Associazione MSX Italia

# Estrazione sprite da Magellan

Esportare da Magellan gli sprite in formato "Assembly Data". Selezionare solo "Include Sprite Data", assicurarsi di scegliere l'inizio e la fine dell'esportazione in modo da prendere tutti i layer.

Se necessario, ridefinire al suo interno le variabili che contengono il numero di sprite e il numero di layer.

Eseguire lo script Python mag2msx.py per convertire gli sprite nel formato MSXgl header.

macOS:
```console
python3 Toolkit/spr2msx.py <file assembly>
````

Windows
```console
py Toolkit/spr2msx.py <file assembly>
````

Il programma produrrà automaticamente un file `data_sprt_layer.h` nella directory corrente; spostare quest'ultimo nella directory `content/sprite`.
