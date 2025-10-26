#include "msxgl.h"


//=============================================================================
// DEFINES
//=============================================================================

// Library's logo
#define MSX_GL "\x02\x03\x04\x05"

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Fonts
//#include "font/font_mgl_sample8.h"  		// Fonts
//#include "content/tile/data_tile_gm2.h"
//#include "content/maps/data_maps_gm2.h"
//#include "content/sprite/data_sprt_layer.h"		// Sprite


//=============================================================================
// SEGMENT 3, BANK 1
//=============================================================================
extern const unsigned char g_DataSprtLayer[];
extern const unsigned char g_DataMapGM2_Names[];
extern const unsigned char g_DataMapGM2_Patterns[];
extern const unsigned char g_DataMapGM2_Colors[];


//=============================================================================
// MEMORY DATA
//=============================================================================

// V-blank synchronization flag
u8 g_VBlank = 0;
u8 sprt = 0;
u8 sprtX = 70;
u8 sprtY = 111;
u8 dirRight = 1; // 1 = destra, 0 = sinistra
u8 g_PreviousSegment = 0;
// Sprite grandi 4 pattern
const u8 sprSize = 4;
// Due layer per sprite
const u8 nLayers = 2;


/**
 * @brief Switch dei segmenti nel banco 1
 *
 * @param u8 segment, imposta il numero del segmento da assegnare. Se il valore è 0, viene ripristinato il segmento precedente
 */
void SetActiveSegment(u8 segment) {
	if(segment == 0) {
		SET_BANK_SEGMENT(1,g_PreviousSegment);
	} else {
		g_PreviousSegment = GET_BANK_SEGMENT(1);
		SET_BANK_SEGMENT(1,segment);
	}
}

//-----------------------------------------------------------------------------
// H_TIMI interrupt hook
void VBlankHook()
{
	g_VBlank = 1;
}

//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{
	while (g_VBlank == 0) {}
	g_VBlank = 0;

}

//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	// Initialize video

	VDP_SetMode(VDP_MODE_GRAPHIC2); // Screen mode 2 (G2)

	// VRAM Tables Address
	VDP_SetLayoutTable(0x3800);
	VDP_SetColorTable(0x2000);
	VDP_SetPatternTable(0x0000);
	VDP_SetSpritePatternTable(0x1800);
	VDP_SetSpriteAttributeTable(0x3E00);

	// Setup video
	VDP_SetColor(0xF0);
	VDP_ClearVRAM();
	VDP_EnableVBlank(TRUE);

	Bios_SetHookCallback(H_TIMI, VBlankHook);

	SetActiveSegment(3);
	// Load tiles pattern
	VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataMapGM2_Colors, 255, 0);

	VDP_WriteLayout_GM2(g_DataMapGM2_Names, 0, 0, 32, 24);

	// Initialize sprite
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);

	// 15 fotogrammi per adesso
	VDP_LoadSpritePattern(g_DataSprtLayer, 0, 15 * nLayers * sprSize);

	// Primo layer nero
	VDP_SetSpriteSM1(sprt+0, sprtX, sprtY, 0, COLOR_BLACK);
	// Secondo layer rosso chiaro, posizionato immediatamente dopo quello nero (+ 4)
	VDP_SetSpriteSM1(sprt+1, sprtX, sprtY, 0 + 4, COLOR_LIGHT_RED);
	VDP_DisableSpritesFrom(sprt+2);

	u8 frame = 0;
	u8 prevRow8 = 0xFF;
	while (1)
	{
		// Wait for v-synch
		WaitVBlank();

		VDP_SetSpritePositionX(sprt+0, sprtX);
		VDP_SetSpritePositionX(sprt+1, sprtX);

		u8 shape = 1;

		u8 row8 = Keyboard_Read(8);

		if (IS_KEY_PRESSED(row8, KEY_SPACE) && !IS_KEY_PRESSED(prevRow8, KEY_SPACE))
		{

		}
		else if (IS_KEY_PRESSED(row8, KEY_HOME) && !IS_KEY_PRESSED(prevRow8, KEY_HOME))
		{

		}

		if (IS_KEY_PRESSED(row8, KEY_RIGHT))
		{
			dirRight = 1;
			sprtX++;
			shape = (frame >> 2) % 3;
		}
		else if (IS_KEY_PRESSED(row8, KEY_LEFT))
		{
			dirRight = 0;
			sprtX--;
			shape = (frame >> 2) % 3;
		}

		if (IS_KEY_PRESSED(row8, KEY_DOWN))
		{

		}
		else if (IS_KEY_PRESSED(row8, KEY_UP))
		{

		}
		prevRow8 = row8;

		// Selezione base dei pattern in base alla direzione
		// 1 è il primo pattern verso destra, 4 il primo verso sinistra
		u8 base = dirRight ? 1 : 4;
		// Prende il fotogramma a cui è arrivato
		u8 curFrame = (base + shape) * sprSize * nLayers;

		// Prende il pattern nero
		VDP_SetSpritePattern(sprt+0, curFrame);
		// Quello rosso è il layer immediatamente successivo (+ 4)
		VDP_SetSpritePattern(sprt+1, curFrame + 4);

		frame++;
	}
}