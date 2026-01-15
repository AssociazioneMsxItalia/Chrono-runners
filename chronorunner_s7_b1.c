#include "msxgl.h"

extern u8 g_RemainingMinutes;
extern u8 g_RemainingSeconds;
extern u8 g_PlayerRewindEnergy;

void DrawRewindGauge();

void PrintGFXText(const c8 *text, u8 x, u8 y) {
	while (*text != 0) {
		c8 c = *text;
		u8 tile = 0;
		// Numero
		if (c >= 48 && c <= 57) {
			tile = 1 + (c - 48);
		}
		// Lettera
		else if (c >= 65 && c <= 90) {
			tile = 11 + (c - 65);
		}
		// "
		else if (c == 34) {
			tile = 40;
		}
		// '
		else if (c == 39) {
			tile = 38;
		}
		// ,
		else if (c == 44) {
			tile = 39;
		}
		// -
		else if (c == 45) {
			tile = 47;
		}
		// .
		else if (c == 46) {
			tile = 37;
		}
		// :
		else if (c == 58) {
			tile = 41;
		}

		VDP_Poke_GM2(x, y, tile);

		x++;
		text++;
	}
}

void PrintGFXNumber(u8 number, u8 x, u8 y) {
	u8 tile10 = (number / 10) + 1;
	u8 tile1 = (number % 10) + 1;
	VDP_Poke_GM2(x, y, tile10);
	VDP_Poke_GM2(x+1, y, tile1);
}

void PrintTime() {
	PrintGFXNumber(g_RemainingMinutes, 7, 0);
	PrintGFXNumber(g_RemainingSeconds, 10, 0);
}

void DrawRewindGauge() {
	// La barra di rewind può essere grande fino a 8 slot, un cristallo
	// ne riempie due

	u8 ntiles = g_PlayerRewindEnergy >> 4; // / 16

	if (ntiles != 0)
		VDP_FillLayout_GM2(45, 21, 0, ntiles, 1);
	if (ntiles != 8)
		VDP_FillLayout_GM2(47, 21 + ntiles, 0, 8 - ntiles, 1);
}
