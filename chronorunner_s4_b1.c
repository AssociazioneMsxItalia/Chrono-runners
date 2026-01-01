//=============================================================================
// SEGMENT: 4
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/pawn.h"
#include "debug.h"

#include "PawnData.h"
#include "math_utils.h"
#include "level_defs.h"
#include "sprite_defs.h"

//=============================================================================
// LEVELS
//=============================================================================

#include "content/levels/level01.h"
#include "content/levels/level02.h"
#include "content/levels/level03.h"
#include "content/levels/level04.h"
#include "content/levels/level05.h"
#include "content/levels/level06.h"
#include "content/levels/level07.h"
#include "content/levels/level08.h"

#include "content/levels/intermission.h"

// Declare array - will be initialized at runtime
struct Level g_Levels[8];

u8 g_NumLevels = numberof(g_Levels);

//=============================================================================
// MEMORY DATA
//=============================================================================

u8 g_PlatformSpritesBaseID;
u8 g_MineSpritesBaseID;
u8 g_EnemySpritesBaseID;
u8 g_EnemyAnimCounter;
u8 g_EnergyFieldSpritesBaseID;
u8 g_EnergyFieldAnimCounter;

//=============================================================================
// EXTERN MEMORY DATA
//=============================================================================

extern u8 g_RemainingFS;

extern Pawn g_PlayerPawn;
extern u8	g_PlayerAction;
extern bool g_PlayerMovingRight;
extern bool g_PlayerMovingLeft;
extern bool g_PlayerJumping;
extern bool g_PlayerDying;
extern bool g_PlayerInputRight;
extern bool g_PlayerInputLeft;
extern bool g_PlayerInputUp;
extern i8   g_VelocityY;
extern i8   g_mDX;
extern i8	g_mDY;
extern i8   g_DX;
extern i8   g_DY;

extern u8 g_CurrentLevel;

extern bool g_KeyEnabled;
extern u8 g_KeyPosX;
extern u8 g_KeyPosY;
extern u8 g_KeyAnimFrame;

extern bool g_CrystalEnabled;
extern u8 g_CrystalPosX;
extern u8 g_CrystalPosY;
extern u8 g_CrystalAnimFrame;

//=============================================================================
// PROTOTYPES
//=============================================================================
void InitializeLevels();
void UpdatePlayerInput();
void UpdatePlayerGravity();
void UpdatePlayerMovement(struct Platform *platform);
void UpdatePlayerAction();

void PrintGFXText(const c8 *text, u8 x, u8 y);
void PrintGFXNumber(u8 number, u8 x, u8 y);

void DrawPlatforms(struct Level *lvl);
void UpdatePlatforms(struct Level *lvl);
struct Platform* isPlayerOnPlatform(struct Level *lvl);

void UpdateEnemies(struct Level *lvl);
void DrawEnemies(struct Level *lvl);
void DrawEnergyFields(struct Level *lvl);
void DrawMines(struct Level *lvl);
void DrawKey();
void DrawCrystal();

void AllocateSpriteIDs(struct Level *lvl);

//=============================================================================
// EXTERN PROTOTYPES
//=============================================================================

//=============================================================================
// FUNCTION
//=============================================================================

void InitializeLevels() {
	// Copy level structs from header files into the global array
	g_Levels[0] = level1;
	g_Levels[1] = level2;
	g_Levels[2] = level3;
	g_Levels[3] = level4;
	g_Levels[4] = level5;
	g_Levels[5] = level6;
	g_Levels[6] = level7;
	g_Levels[7] = level8;
}

i8 GetDPos(i8* m) {
	i8 rv = sdiv((*m), 3);

	while ((*m) >= 8)
		(*m) -= 8;

	while ((*m) <= -8)
		(*m) += 8;

	return rv;
}

void UpdatePlayerInput() {
	g_PlayerInputRight = FALSE;
	g_PlayerInputLeft = FALSE;
	g_PlayerInputUp = FALSE;

	u8 row8 = Keyboard_Read(8);
	u8 joy = Joystick_Read(JOY_PORT_1);

	if (IS_KEY_PRESSED(row8, KEY_RIGHT)) {
		g_PlayerInputRight = TRUE;
	} else if (IS_KEY_PRESSED(row8, KEY_LEFT)) {
		g_PlayerInputLeft = TRUE;
	}

	if (IS_KEY_PRESSED(row8, KEY_UP) || IS_JOY_PRESSED(joy, JOY_INPUT_TRIGGER_B)) {
		g_PlayerInputUp = TRUE;
	}
}

void UpdatePlayerGravity() {
	g_mDY -= g_VelocityY;

	g_VelocityY -= GRAVITY;
	if (g_VelocityY < -FORCE)
		g_VelocityY = -FORCE;
}

void UpdatePlayerMovement(struct Platform *platform) {

	g_PlayerMovingRight = g_PlayerMovingLeft = FALSE;

	u8 x_incr = 8;

	if (g_PlayerInputRight)
	{
		g_mDX += x_incr;
		g_PlayerMovingRight = TRUE;
	}
	else if (g_PlayerInputLeft)
	{
		g_mDX -= x_incr;
		g_PlayerMovingLeft = TRUE;
	}

	// Se preme su E non è su una piattaforma né sta già saltando, allora
	// esegue il salto
	if (g_PlayerInputUp && (platform != NULL || !g_PlayerJumping)) {
		// Inizia il salto
		g_PlayerJumping = TRUE;
		g_VelocityY = FORCE;
		// Ora NON è più su piattaforma
		platform = NULL;
	}

	// Gestisce esplicitamente la piattaforma. Entra qua solo se non abbiamo
	// appena saltato
	if (platform != NULL) {
		// Forza posizione Y sulla piattaforma
		g_PlayerPawn.PositionY = platform->pos_y - 16;

		// Siccome è sulla piattaforma, il personaggio NON sta saltando
		g_PlayerJumping = FALSE;

		// Aggiungi movimento piattaforma (moltiplicato per 8 per ottavi di pixel)
		g_mDX += (platform->dir_x * 8);
	}

	if (g_PlayerJumping)
	{
		UpdatePlayerGravity();
	}

	// Gli spostamenti sono espressi in ottavi di pixel
	g_DX = GetDPos(&g_mDX);
	g_DY = GetDPos(&g_mDY);
}

void UpdatePlayerAction() {
	g_PlayerAction = ACTION_IDLE;
	if (g_PlayerDying) {
		g_PlayerAction = ACTION_DEATH;
	}
	else if (g_PlayerJumping && (g_VelocityY >= 0))
	{
		if (g_PlayerMovingRight)
			g_PlayerAction = ACTION_JUMPRIGHT;
		else
			g_PlayerAction = ACTION_JUMPLEFT;
	}
	else if (g_PlayerJumping) {
		if (g_PlayerMovingRight)
			g_PlayerAction = ACTION_FALLRIGHT;
		else
			g_PlayerAction = ACTION_FALLLEFT;
	}
	else if (g_PlayerMovingRight)
		g_PlayerAction = ACTION_MOVERIGHT;
	else if (g_PlayerMovingLeft)
		g_PlayerAction = ACTION_MOVELEFT;
}

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

struct Platform* isPlayerOnPlatform(struct Level *lvl) {
	struct Platform *platforms = lvl->platforms;

	for (u8 p=0; p < lvl->num_platforms; p++) {
		// -16 / +16 per permettere al giocatore di sfruttare lo spazio in
		// orizzontale della piattaforma fino all'ultimo pixel
		bool in_x = g_PlayerPawn.PositionX > platforms[p].pos_x - 16
		         && g_PlayerPawn.PositionX < platforms[p].pos_x + 16;

		// Perché -4? Serve per considerare anche l'area immediatamente sopra
		// la piattaforma come parte della piattaforma stessa. Senza questo
		// buffer, su una piattaforma che va verso il basso si vedrebbe
		// "saltare" il giocatore, perché ad ogni fotogramma gli manca la
		// terra sotto i piedi
		bool in_y = g_PlayerPawn.PositionY > platforms[p].pos_y - 16 - 4
		         && g_PlayerPawn.PositionY < platforms[p].pos_y;

		if (in_x && in_y)
		{
			return &platforms[p];
		}
	}

	return NULL;
}

void UpdatePlatforms(struct Level *lvl) {
	struct Platform *platforms = lvl->platforms;

	for (u8 p=0; p < lvl->num_platforms; p++) {
		struct Platform *plat = &platforms[p];

		plat->pos_x += plat->dir_x;
		if (plat->pos_x == plat->max_x || plat->pos_x == plat->min_x) {
			plat->dir_x = (-plat->dir_x);
		}

		plat->pos_y += plat->dir_y;
		if (plat->pos_y == plat->max_y || plat->pos_y == plat->min_y) {
			plat->dir_y = (-plat->dir_y);
		}
	}
}

void DrawPlatforms(struct Level *lvl) {
	struct Platform *platforms = lvl->platforms;

	for (u8 p=0; p < lvl->num_platforms; p++) {
		u8 index = g_PlatformSpritesBaseID + p;
		VDP_SetSpritePosition(index, platforms[p].pos_x, platforms[p].pos_y);
	}
}

void UpdateEnemies(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	for (u8 e=0; e < lvl->num_enemies; e++) {
		// Muovi i proiettili indipendentemente dallo stato del nemico
		if (enemies[e].field_state == 2) {

			enemies[e].field_mDX += 12 * enemies[e].dir_x;
			i8 dx = GetDPos(&enemies[e].field_mDX);
			enemies[e].field_x += dx;

			// Se esce dallo schermo, resettalo
			if (enemies[e].field_x < 8 || enemies[e].field_x > 240) {
				enemies[e].field_state = 0;
			}
		}

		// Stato stunned. Per un nemico in questo stato ci fermiamo qua:
		// no movimento, no nuovi field.
		if (enemies[e].stunned_timer > 0) {
			enemies[e].stunned_timer--;
			continue;
		}

		if (enemies[e].field_cooldown > 0) {
			enemies[e].field_cooldown--;
		}

		// Nemici tipo 2, campo di forza locale
		if (enemies[e].type == 2) {
			if (enemies[e].field_state == 1) {
				enemies[e].field_timer--;

				if (enemies[e].field_timer == 0) {
					// Al termine del timer disattiva il campo di forza
					enemies[e].field_state = 0;
					enemies[e].field_cooldown = 150;
				}

				// Se il campo di forza è attivo, il nemico non si muove
				continue;

			} else if (enemies[e].field_state == 0 && enemies[e].field_cooldown == 0) {

				if (Math_GetRandom8() < 10) {
					enemies[e].field_state = 1;
					enemies[e].field_timer = 100;

					// Il campo di forza appare davanti al nemico
					enemies[e].field_x = enemies[e].pos_x + (enemies[e].dir_x * 16);
					enemies[e].field_y = enemies[e].pos_y;
				}
			}
		}

		// Nemici tipo 3, campo di forza proiettile
		if (enemies[e].type == 3) {
			if (enemies[e].field_state == 0 && enemies[e].field_cooldown == 0) {

				if (Math_GetRandom8() < 5) {
					enemies[e].field_state = 2;
					enemies[e].field_cooldown = 120;

					enemies[e].field_x = enemies[e].pos_x + (enemies[e].dir_x * 16);
					enemies[e].field_y = enemies[e].pos_y;
					enemies[e].field_mDX = 0;
				}
			}
		}

		// Check boundaries and reverse direction
		if (enemies[e].pos_x < enemies[e].min_x) {
			enemies[e].pos_x = enemies[e].min_x;
			enemies[e].dir_x = -enemies[e].dir_x;
		}

		if (enemies[e].pos_x > enemies[e].max_x) {
			enemies[e].pos_x = enemies[e].max_x;
			enemies[e].dir_x = -enemies[e].dir_x;
		}

		// Add speed to accumulated movement (in eighths of pixel)
		enemies[e].mDX += enemies[e].speed * enemies[e].dir_x;

		// Convert eighths-of-pixel to actual pixel movement
		i8 dx = GetDPos(&enemies[e].mDX);
		enemies[e].pos_x += dx;
	}
}

void DrawEnemies(struct Level *lvl) {
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
		if (enemies[e].stunned_timer > 0) {
			// Stunned animation - frames 5 and 6
			pattern = ENEMY_PATTERN_OFFSET + type_offset + (g_EnemyAnimCounter >= 10 ? 16 : 20);
		} else if (enemies[e].dir_x < 0) {
			// Moving left - frames 0 and 1
			pattern = ENEMY_PATTERN_OFFSET + type_offset + (g_EnemyAnimCounter >= 10 ? 4 : 0);
		} else {
			// Moving right - frames 3 and 4
			pattern = ENEMY_PATTERN_OFFSET + type_offset + (g_EnemyAnimCounter >= 10 ? 12 : 8);
		}

		VDP_SetSpriteSM1(index, enemies[e].pos_x, enemies[e].pos_y, pattern, COLOR_BLACK);
	}
}

void DrawEnergyFields(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	// Animate energy fields
	g_EnergyFieldAnimCounter++;
	if (g_EnergyFieldAnimCounter >= 16) {
		g_EnergyFieldAnimCounter = 0;
	}

	u8 pattern = ENERGYFIELD_PATTERN_OFFSET + (g_EnergyFieldAnimCounter >= 8 ? ENERGYFIELD_PATTERN_SIZE : 0);

	// Draw energy field for each enemy that has one active
	for (u8 e=0; e < lvl->num_enemies; e++) {
		u8 sprite_id = g_EnergyFieldSpritesBaseID + e;

		if (enemies[e].field_state != 0) {
			u8 color = (g_EnergyFieldAnimCounter & 1) ? COLOR_LIGHT_BLUE : COLOR_WHITE;
			// Field is active (either Type 2 stationary or Type 3 projectile)
			VDP_SetSpriteSM1(sprite_id, enemies[e].field_x, enemies[e].field_y, pattern, color);
		} else {
			// No active field - hide the sprite
			VDP_HideSprite(sprite_id);
		}
	}
}

void DrawMines(struct Level *lvl) {
	struct Mine *mines = lvl->mines;

	for (u8 m=0; m < lvl->num_mines; m++) {
		u8 index = g_MineSpritesBaseID + m;
		u8 color = g_RemainingFS < 25 ? COLOR_DARK_RED : COLOR_LIGHT_RED;

        if (mines[m].enabled) {
			VDP_SetSpriteColorSM1(index, color);
        } else {
			VDP_HideSprite(index);
        }
    }
}

void DrawKey() {
	if (!g_KeyEnabled)
		return;

	// Alterna i due fotogrammi ogni 20 tick
	g_KeyAnimFrame++;
	if (g_KeyAnimFrame >= 40) {
		g_KeyAnimFrame = 0;
	}

	u8 pattern = KEY_PATTERN_OFFSET + (g_KeyAnimFrame >= 20 ? KEY_PATTERN_SIZE : 0);
	VDP_SetSpritePattern(KEY_SPRITE_ID, pattern);
}

void DrawCrystal() {
	if (!g_CrystalEnabled)
		return;

	// Alterna i due fotogrammi ogni 16 tick
	g_CrystalAnimFrame++;
	if (g_CrystalAnimFrame >= 32) {
		g_CrystalAnimFrame = 0;
	}

	u8 pattern = CRYSTAL_PATTERN_OFFSET + (g_CrystalAnimFrame >= 16 ? CRYSTAL_PATTERN_SIZE : 0);
	VDP_SetSpritePattern(CRYSTAL_SPRITE_ID, pattern);
}

void AllocateSpriteIDs(struct Level *lvl) {

	// Gli sprite ID allocati fissi sono:
	// 0-1: Player (2 layers)
	// 2: Key
	// 3: Crystal
	// 4+: Platforms, Mines, Enemies, Energy Fields

	VDP_SetSpriteSM1(KEY_SPRITE_ID, g_KeyPosX, g_KeyPosY, KEY_PATTERN_OFFSET, COLOR_BLACK);

	VDP_SetSpriteSM1(CRYSTAL_SPRITE_ID, g_CrystalPosX, g_CrystalPosY, CRYSTAL_PATTERN_OFFSET, COLOR_MEDIUM_RED);
	if (!g_CrystalEnabled) {
		VDP_HideSprite(CRYSTAL_SPRITE_ID);
	}

	g_PlatformSpritesBaseID = 4;

	// Imposta gli sprite piattaforma da usare nel livello corrente
	u8 np = lvl->num_platforms;
	struct Platform *platforms = lvl->platforms;
	for (u8 p=0; p < np; p++) {
		u8 index = g_PlatformSpritesBaseID + p;
		u8 shape = platforms[p].dir_x != 0 ? PLATFORMH_PATTERN_OFFSET : PLATFORMV_PATTERN_OFFSET;
		VDP_SetSpriteSM1(index, platforms[p].pos_x, platforms[p].pos_y, shape, COLOR_BLACK);
	}

	g_MineSpritesBaseID = g_PlatformSpritesBaseID + np;

	// Imposta sprite per le mine
	u8 nm = lvl->num_mines;
	struct Mine *mines = lvl->mines;
	for (u8 m=0; m < nm; m++) {
		u8 index = g_MineSpritesBaseID + m;
		VDP_SetSpriteSM1(index, mines[m].pos_x, mines[m].pos_y, MINE_PATTERN_OFFSET, COLOR_DARK_RED);
	}

	g_EnemySpritesBaseID = g_MineSpritesBaseID + nm;
	g_EnemyAnimCounter = 0;

	// Imposta sprite per i nemici
	u8 ne = lvl->num_enemies;
	struct Enemy *enemies = lvl->enemies;
	for (u8 e=0; e < ne; e++) {
		u8 index = g_EnemySpritesBaseID + e;
		VDP_SetSpriteSM1(index, enemies[e].pos_x, enemies[e].pos_y, ENEMY_PATTERN_OFFSET, COLOR_BLACK);
	}

	// Campi di forza, uno per ciascun nemico
	g_EnergyFieldSpritesBaseID = g_EnemySpritesBaseID + ne;
	g_EnergyFieldAnimCounter = 0;

	for (u8 e = 0; e < ne; e++) {
		u8 sprite_id = g_EnergyFieldSpritesBaseID + e;
		VDP_SetSpriteSM1(sprite_id, 0, 0, ENERGYFIELD_PATTERN_OFFSET, COLOR_LIGHT_BLUE);
	}
}
