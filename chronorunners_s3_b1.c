//=============================================================================
// SEGMENT: 3
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "content/sprite/player_sprt_layer.h"
#include "content/sprite/key_sprt_layer.h"
#include "content/sprite/enemy_sprt_layer.h"
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
	// XXX: bug in MSXgl?
	//VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 0, 0);
	//VDP_LoadColor_GM2(g_DataMapGM2_Colors, 0, 0);

	VDP_WriteVRAM(g_DataMapGM2_Patterns, 0x0, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Patterns, 0x800, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Patterns, 0x1000, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Colors, 0x2000, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Colors, 0x2800, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Colors, 0x3000, 0, 256*8);
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

	VDP_LoadSpritePattern(g_PlayerSprtLayer,           0,               13 * sprSize);
	VDP_LoadSpritePattern(g_KeySprtLayer,   13 * sprSize,                2 * laySize);
	VDP_LoadSpritePattern(g_EnemySprtLayer, 13 * sprSize + 2 * laySize,  7 * laySize);
}
