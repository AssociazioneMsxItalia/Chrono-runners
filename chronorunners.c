// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │   ▄▄▄                ▄▄      
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘                 ▀▀
//  Multi-directionnal scrolling sample
//
// Scrolling parameters (source data format, output window and used fetaures) 
// are defined in  msxgl_config.h for optimization purpose (the 'scroll' module 
// is optimized at compile time to useonly necessary features).
//─────────────────────────────────────────────────────────────────────────────
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
#include "content/tile/data_tile_gm2.h"
#include "content/maps/data_maps_gm2.h"
#include "content/sprite/data_sprt_layer.h"		// Sprite


//=============================================================================
// MEMORY DATA
//=============================================================================

// V-blank synchronization flag
u8 g_VBlank = 0;
u8 sprt= 0;
u8 sprtOffset = 0;


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

	
	// Load tiles pattern
	VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataMapGM2_Colors, 255, 0);
	
	// Initialize text
	//Print_SetTextFont(g_Font_MGL_Sample8, 96);
	//VDP_FillLayout_GM2(1, 0, 0, 32, 24);
	
	// Header
	//Print_DrawText(MSX_GL " SCREEN 2 SAMPLE");
	//Print_DrawLineH(0, 1, 32);



	VDP_WriteLayout_GM2(g_DataMapGM2_Names, 0, 0, 32, 24);

	// Initialize sprite
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);
	VDP_LoadSpritePattern(g_DataSprtLayer, 32, 13*4*4);
	VDP_SetSpriteSM1(sprt+0, 70, 130, 32, COLOR_BLACK);
	VDP_SetSpriteSM1(sprt+1, 70, 130, 36, COLOR_WHITE);
	VDP_SetSpriteSM1(sprt+2, 70, 130, 40, COLOR_LIGHT_RED);
	VDP_DisableSpritesFrom(sprt+3);

	u8 frame = 0;
	u8 prevRow8 = 0xFF;
	while (1)
	{
		// Wait for v-synch
		WaitVBlank();

	
		VDP_SetSpritePositionX(sprt+0, 70 + sprtOffset);
		VDP_SetSpritePositionX(sprt+1, 70 + sprtOffset);
		VDP_SetSpritePositionX(sprt+2, 70 + sprtOffset);

		u8 shape = 6;

		u8 row8 = Keyboard_Read(8);
		
		if (IS_KEY_PRESSED(row8, KEY_SPACE) && !IS_KEY_PRESSED(prevRow8, KEY_SPACE))
		{

		}
		else if (IS_KEY_PRESSED(row8, KEY_HOME) && !IS_KEY_PRESSED(prevRow8, KEY_HOME))
		{
		
		}
		
		if (IS_KEY_PRESSED(row8, KEY_RIGHT))
		{
			sprtOffset++;
			shape = (frame >> 2) % 6;
		}
		else if (IS_KEY_PRESSED(row8, KEY_LEFT))
		{
			sprtOffset--;
			shape = (frame >> 2) % 6;
		}
		
		if (IS_KEY_PRESSED(row8, KEY_DOWN))
		{
			
		}
		else if (IS_KEY_PRESSED(row8, KEY_UP))
		{
			
		}
		prevRow8 = row8;

		VDP_SetSpritePattern(sprt+0, 32 + shape * 16);
		VDP_SetSpritePattern(sprt+1, 36 + shape * 16);
		VDP_SetSpritePattern(sprt+2, 40 + shape * 16);

		frame++;
	}
}