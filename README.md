# Chrono-runners By Associazione MSX Italia

# Estrazione sprite da Magellan

Esportare da Magellan gli sprite in formato "Assembly Data". Selezionare solo "Include Sprite Data", assicurarsi di scegliere l'inizio e la fine dell'esportazione in modo da prendere tutti i layer.

Eseguire lo script Python mag2msx.py per convertire gli sprite nel formato MSXgl header.

Se necessario, ridefinire al suo interno le variabili che contengono il numero di sprite, il numero di layer e la distanza in fotogrammi tra i layer.

macOS:
```console
python3 Toolkit/mag2msx.py <file assembly>
````

Windows
```console
py Toolkit/mag2msx.py <file assembly>
````

Il programma produrrà automaticamente un file `data_sprt_layer.h` nella directory corrente; copiare quest'ultimo nella directory `content/sprite`.
