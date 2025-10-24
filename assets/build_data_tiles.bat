@echo on
setlocal EnableDelayedExpansion

set Dest=..\content
set FullPath=%~d0%~p0

if not exist %Dest% md %Dest%

::-----------------------------------------------------------------------------
:: Build tiles data

rem C:\Users\87003101\msxgl-workspace\SVILUPPO\MSXgtTemplateWorkSpace\MSXgl\tools\MSXtk\bin\MSXimg.exe img\prova.png -out nicola_data_tile_gm2.h -mode gm2 -name g_DataMapGM2_Patterns -pos 0 0 -size 192  144 -offset 0
C:\Users\87003101\msxgl-workspace\SVILUPPO\MSXgtTemplateWorkSpace\MSXgl\tools\MSXtk\bin\MSXimg.exe img\mappa.png -out prova.h  -mode gm2 -name g_DataMapGM2  -pos 0 0 -size 256 192 -offset 0



REM %MSXtk%\MSXimg.exe img\city.png -out %Dest%\tile\data_tile_gm1.h -mode gm1 -name g_DataTileGM1 -pos 0 160 -size 192 144 -offset 0
rem %MSXtk%\MSXimg.exe misc\naruto2413_qrcode.png -mode gm2 -name g_QRCode -pos 0 0 -size 64 64 -out %Dest%\img\naruto2413_qrcode.h



pause