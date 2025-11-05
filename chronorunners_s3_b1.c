//=============================================================================
// SEGMENT: 3
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "content/sprite/data_sprt_layer.h"	// Player
#include "content/sprite/key_sprt_layer.h"	// Key
#include "content/tile/data_tile_gm2.h"

//=============================================================================
// DEFINE
//=============================================================================
#define laySize 4  // Layer grandi 4 pattern
#define nLayers 2  // Due layer per sprite
#define sprSize laySize * nLayers  // Dimensione sprite


//=============================================================================
// FUNCTION
//=============================================================================

/**
 * @brief Load Pattern and color
 * @param void
 * @return void
 */
void LoadPatternAndColor() {
   	VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataMapGM2_Colors, 255, 0);
}

/**
 * @brief Set VRAM Table and Setup Video
 * @param void
 * @return void
 */
void SetVRAMTable() {
    // VRAM Tables Address
	VDP_SetLayoutTable(0x3800);
	VDP_SetColorTable(0x2000);
	VDP_SetPatternTable(0x0000);
	VDP_SetSpritePatternTable(0x1800);
	VDP_SetSpriteAttributeTable(0x3E00);

    // Setup Video
    VDP_SetColor(0xF0);
	VDP_ClearVRAM();
}

/**
 * @brief Inizialize Sprite and load Pattern
 * @param void
 * @return void
 */
void InitializeSprite() {
	// Initialize sprite
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);

	VDP_LoadSpritePattern(g_DataSprtLayer,           0, 13 * sprSize);
	VDP_LoadSpritePattern(g_KeySprtLayer, 13 * sprSize,  2 * laySize);
}
