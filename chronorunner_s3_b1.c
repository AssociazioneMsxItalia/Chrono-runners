//=============================================================================
// SEGMENT: 3
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "debug.h"

#include "sprite_defs.h"
#include "level_defs.h"

#include "content/sprite/player_sprt_layer.h"
#include "content/sprite/key_sprt_layer.h"
#include "content/sprite/crystal_sprt_layer.h"
#include "content/sprite/doc_sprt_layer.h"

#include "content/sprite/platformv_sprt_layer.h"
#include "content/sprite/platformh_sprt_layer.h"
#include "content/sprite/mine_sprt_layer.h"

#include "content/sprite/enemy0_sprt_layer.h"  // Enemy type 0
#include "content/sprite/enemy1_sprt_layer.h"  // Enemy type 1
#include "content/sprite/enemy2_sprt_layer.h"  // Enemy type 2
#include "content/sprite/enemy3_sprt_layer.h"  // Enemy type 3
#include "content/sprite/energyfield_sprt_layer.h"  // Energy field sprite

#include "content/tile/data_tile_gm2.h"


//=============================================================================
// DEFINE
//=============================================================================


//=============================================================================
// FUNCTION
//=============================================================================

/**
 * @brief Load Pattern and color
 * @param void
 * @return void
 */
void LoadPatternAndColor() {
// Waiting for MSXgl to integrate https://github.com/aoineko-fr/MSXgl/pull/81
#if 0
	VDP_LoadColor_GM2(g_DataMapGM2_Colors, 0, 0);
	VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 0, 0);
#else
	VDP_WriteVRAM(g_DataMapGM2_Patterns, 0x0, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Patterns, 0x800, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Patterns, 0x1000, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Colors, 0x2000, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Colors, 0x2800, 0, 256*8);
	VDP_WriteVRAM(g_DataMapGM2_Colors, 0x3000, 0, 256*8);
#endif
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

	// Load player sprites
	VDP_LoadSpritePattern(g_PlayerSprtLayer, PLAYER_PATTERN_OFFSET, PLAYER_PATTERN_TOTAL);

	// Load key sprite
	VDP_LoadSpritePattern(g_KeySprtLayer, KEY_PATTERN_OFFSET, KEY_PATTERN_TOTAL);

	// Load crystal sprites
	VDP_LoadSpritePattern(g_CrystalSprtLayer, CRYSTAL_PATTERN_OFFSET, CRYSTAL_PATTERN_TOTAL);

	// Load evil doctor sprite
	VDP_LoadSpritePattern(g_DocSprtLayer, DOC_PATTERN_OFFSET, DOC_PATTERN_TOTAL);

	// Load Platform / Mine sprites
	VDP_LoadSpritePattern(g_PlatformVSprtLayer, PLATFORMV_PATTERN_OFFSET, PLATFORMV_PATTERN_TOTAL);
	VDP_LoadSpritePattern(g_PlatformHSprtLayer, PLATFORMH_PATTERN_OFFSET, PLATFORMH_PATTERN_TOTAL);
	VDP_LoadSpritePattern(g_MineSprtLayer, MINE_PATTERN_OFFSET, MINE_PATTERN_TOTAL);

	// Load all 4 enemy types (each type has 6 frames)
	VDP_LoadSpritePattern(g_Enemy0SprtLayer, ENEMY_FRAME(0, 0), ENEMY_TYPE_PATTERN_TOTAL);
	VDP_LoadSpritePattern(g_Enemy1SprtLayer, ENEMY_FRAME(1, 0), ENEMY_TYPE_PATTERN_TOTAL);
	VDP_LoadSpritePattern(g_Enemy2SprtLayer, ENEMY_FRAME(2, 0), ENEMY_TYPE_PATTERN_TOTAL);
	VDP_LoadSpritePattern(g_Enemy3SprtLayer, ENEMY_FRAME(3, 0), ENEMY_TYPE_PATTERN_TOTAL);

	// Load energy field sprite (2 frames for animation)
	VDP_LoadSpritePattern(g_EnergyFieldSprtLayer, ENERGYFIELD_PATTERN_OFFSET, ENERGYFIELD_PATTERN_TOTAL);
}

extern u8 g_PlatformSpritesBaseID;
extern u8 g_MineSpritesBaseID;

extern u8 g_EnemySpritesBaseID;
extern u8 g_EnemyAnimCounter;

extern u8 g_EnergyFieldSpritesBaseID;
extern u8 g_EnergyFieldAnimCounter;

void AllocateSpriteIDs(struct Level *lvl) {

	// Gli sprite ID allocati fissi sono:
	// 0-1: Player (2 layers)
	// 2: Key
	// 3: Crystal
	// 4: Evil doctor
	// 5+: Platforms, Mines, Enemies, Energy Fields

	g_PlatformSpritesBaseID = 5;

	// Imposta gli sprite piattaforma da usare nel livello corrente
	u8 np = lvl->num_platforms;

	g_MineSpritesBaseID = g_PlatformSpritesBaseID + np;

	// Imposta sprite per le mine
	u8 nm = lvl->num_mines;

	g_EnemySpritesBaseID = g_MineSpritesBaseID + nm;
	g_EnemyAnimCounter = 0;

	// Imposta sprite per i nemici
	u8 ne = lvl->num_enemies;

	// Campi di forza, uno per ciascun nemico
	g_EnergyFieldSpritesBaseID = g_EnemySpritesBaseID + ne;
	g_EnergyFieldAnimCounter = 0;
}
