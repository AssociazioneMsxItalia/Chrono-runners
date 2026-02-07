#include "content/splash/ami_screen1.h"

#include "content/splash/splashscreen.h"

void ShowAmi() {
	VDP_SetMode(VDP_MODE_SCREEN1);
	VDP_ClearVRAM();
	VDP_WriteVRAM_16K(g_AMI_PatternTable, 0x0000, 2048);
	VDP_WriteVRAM_16K(g_AMI_ColorTable,   0x2000, 6);
	VDP_EnableVBlank(TRUE);
	for (u8 ii = 20; ii >= 6; ii--) {
		VDP_WriteVRAM_16K(g_AMI_NameTable,    0x1800 + ii*32, 32*4);
		Halt(); Halt();
	}
	for (u8 ii = 0; ii < 150; ii++) {
		Halt();
	}
	VDP_EnableVBlank(FALSE);
	VDP_ClearVRAM();
}

void ShowSplashScreen() {

	// Set Graphic Mode 2 (SCREEN 2)
	VDP_SetMode(VDP_MODE_SCREEN2);
	VDP_SetColor(COLOR_BLACK);
	VDP_ClearVRAM();

	// Load all 3 tables to their VRAM addresses
	VDP_WriteVRAM_16K(g_splashscreen_Patterns, 0x0000, 6144); // Pattern table
	VDP_WriteVRAM_16K(g_splashscreen_Colors,   0x2000, 6144);  // Color table
	VDP_WriteVRAM_16K(g_splashscreen_Names,    0x1800, 768);   // Name table (sequential 0-255 x3)

	VDP_EnableVBlank(TRUE);
	for (u8 ii = 0; ii < 150; ii++) {
		Halt();
	}
	VDP_EnableVBlank(FALSE);
	VDP_ClearVRAM();
}
