#include "msxgl.h"

#include "sprite_defs.h"
#include "level_defs.h"

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

extern bool g_KeyEnabled;
extern bool g_CrystalEnabled;
extern u8 g_KeyAnimFrame;
extern u8 g_CrystalAnimFrame;

extern u8 g_PlatformSpritesBaseID;
extern u8 g_MineSpritesBaseID;

extern u8 g_EnemyAnimCounter;
extern u8 g_EnemySpritesBaseID;

extern u8 g_EnergyFieldAnimCounter;

extern u8 g_EnergyFieldSpritesBaseID;

extern u8 g_RemainingFS;

void DrawKey(struct Level *lvl) {
	if (!g_KeyEnabled)
		return;

	// Alterna i due fotogrammi ogni 20 tick
	g_KeyAnimFrame++;
	if (g_KeyAnimFrame >= 40) {
		g_KeyAnimFrame = 0;
	}

	u8 pattern = KEY_PATTERN_OFFSET;
	if(g_KeyAnimFrame >= 20) {
		pattern += KEY_PATTERN_SIZE;
	}

	VDP_SetSpriteSM1(KEY_SPRITE_ID, lvl->key_x * 8, lvl->key_y * 8, pattern, COLOR_BLACK);
}

void DrawCrystal(struct Level *lvl) {
	if (!g_CrystalEnabled)
		return;

	// Alterna i due fotogrammi ogni 16 tick
	g_CrystalAnimFrame++;
	if (g_CrystalAnimFrame >= 32) {
		g_CrystalAnimFrame = 0;
	}

	u8 pattern = CRYSTAL_PATTERN_OFFSET;
	if (g_CrystalAnimFrame >= 16) {
		pattern += CRYSTAL_PATTERN_SIZE;
	}

	VDP_SetSpriteSM1(CRYSTAL_SPRITE_ID, lvl->crystal_x * 8, lvl->crystal_y * 8, pattern, COLOR_MEDIUM_RED);
}

void DrawPlatforms(struct Level *lvl, bool rewind) {
	struct Platform *platforms = lvl->platforms;

	for (u8 p=0; p < lvl->num_platforms; p++) {
		u8 index = g_PlatformSpritesBaseID + p;
		u8 shape;
		if (lvl->platforms[p].dir_x != 0) {
			shape = PLATFORMH_PATTERN_OFFSET;
		} else {
			shape = PLATFORMV_PATTERN_OFFSET;
		}
		u8 color;
		if (rewind) {
			color = COLOR_WHITE;
		} else {
			color = COLOR_BLACK;
		}
		VDP_SetSpriteSM1(index, platforms[p].pos_x, platforms[p].pos_y, shape, color);
	}
}

void DrawMines(struct Level *lvl) {
	struct Mine *mines = lvl->mines;

	for (u8 m=0; m < lvl->num_mines; m++) {
		u8 index = g_MineSpritesBaseID + m;
		u8 color;

		if (g_RemainingFS < 25) {
			color = COLOR_BLACK;
		} else {
			color = COLOR_LIGHT_RED;
		}

        if (mines[m].enabled) {
			VDP_SetSpriteSM1(index, mines[m].pos_x, mines[m].pos_y, MINE_PATTERN_OFFSET, color);
        } else {
			VDP_HideSprite(index);
		}
    }
}

void DrawEnemies(struct Level *lvl, bool rewind) {
	struct Enemy *enemies = lvl->enemies;

	// Simple animation: alternate between 2 frames every 10 ticks
	g_EnemyAnimCounter++;
	if (g_EnemyAnimCounter >= 20) {
		g_EnemyAnimCounter = 0;
	}

	for (u8 e=0; e < lvl->num_enemies; e++) {
		u8 index = g_EnemySpritesBaseID + e;

		// Calculate base pattern offset for this enemy type
		u8 type_offset = enemies[e].type * ENEMY_TYPE_PATTERN_TOTAL;

		// Choose pattern based on state
		u8 pattern;
		u8 anim_pattern;
		if (enemies[e].stunned_timer > 0) {
			// Stunned
			anim_pattern = 4 * 4;
		} else if (enemies[e].dir_x < 0) {
			// Moving left
			anim_pattern = 0 * 4;
		} else {
			// Moving right
			anim_pattern = 2 * 4;
		}

		if (g_EnemyAnimCounter >= 10) {
			anim_pattern += 4;
		}

		pattern = ENEMY_PATTERN_OFFSET + type_offset + anim_pattern;

		u8 color;
		if (rewind) {
			color = COLOR_WHITE;
		} else {
			color = COLOR_BLACK;
		}

		VDP_SetSpriteSM1(index, enemies[e].pos_x, enemies[e].pos_y, pattern, color);
	}
}

void DrawEnergyFields(struct Level *lvl, bool rewind) {
	struct Enemy *enemies = lvl->enemies;

	// Animate energy fields
	g_EnergyFieldAnimCounter++;
	if (g_EnergyFieldAnimCounter >= 16) {
		g_EnergyFieldAnimCounter = 0;
	}

	u8 pattern = ENERGYFIELD_PATTERN_OFFSET;

	if (g_EnergyFieldAnimCounter >= 8) {
		pattern += ENERGYFIELD_PATTERN_SIZE;
	}

	// Draw energy field for each enemy that has one active
	for (u8 e=0; e < lvl->num_enemies; e++) {
		u8 sprite_id = g_EnergyFieldSpritesBaseID + e;

		if (enemies[e].field_state != 0) {
			u8 color;
			if (rewind) {
				color = COLOR_WHITE;
			} else {
				if (g_EnergyFieldAnimCounter & 1) {
					color = COLOR_DARK_RED;
				} else {
					color = COLOR_MAGENTA;
				}
			}
			// Field is active (either Type 2 stationary or Type 3 projectile)
			VDP_SetSpriteSM1(sprite_id, enemies[e].field_x, enemies[e].field_y, pattern, color);
		} else {
			// No active field - hide the sprite
			VDP_HideSprite(sprite_id);
		}
	}
}
