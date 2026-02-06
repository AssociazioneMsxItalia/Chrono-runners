#include "content/splash/ami_screen1.h"

void ShowAmi() {
	VDP_SetMode(VDP_MODE_SCREEN1);
	VDP_EnableVBlank(TRUE);
	VDP_ClearVRAM();
	VDP_WriteVRAM_16K(g_AMI_PatternTable, 0x0000, 2048);
	VDP_WriteVRAM_16K(g_AMI_ColorTable,   0x2000, 6);
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
