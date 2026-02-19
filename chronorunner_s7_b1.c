#include "msxgl.h"
#include "game/pawn.h"
#include "game/state.h"
#include "debug.h"

#include "sprite_defs.h"
#include "level_defs.h"
#include "PawnData.h"
#include "snapshot.h"
#include "fx_sounds.h"

extern u8 g_RemainingMinutes;
extern u8 g_RemainingSeconds;
extern u8 g_PlayerRewindEnergy;

extern Pawn g_PlayerPawn;
extern i8   g_VelocityY;
extern i8   g_mDY;
extern bool g_PlayerJumping;
extern bool g_KeyEnabled;
extern bool g_PlayerHasKey;

//-----------------------------------------------------------------------------
// Character to tile conversion
//-----------------------------------------------------------------------------
u8 CharToTile(c8 c) {
	// Numbers 0-9
	if (c >= 48 && c <= 57) {
		return 1 + (c - 48);
	}
	// Letters A-Z
	else if (c >= 65 && c <= 90) {
		return 11 + (c - 65);
	}
	// Special characters
	else if (c == 33) return 43;  // !
	else if (c == 34) return 40;  // "
	else if (c == 39) return 38;  // '
	else if (c == 40) return 45;  // (
	else if (c == 41) return 46;  // )
	else if (c == 44) return 39;  // ,
	else if (c == 45) return 42;  // -
	else if (c == 46) return 37;  // .
	else if (c == 58) return 41;  // :
	else if (c == 63) return 44;  // ?

	return 0;  // Space or unknown
}

void PrintGFXText(const c8 *text, u8 x, u8 y) {
	while (*text != 0) {
		VDP_Poke_GM2(x, y, CharToTile(*text));
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
	// La barra di rewind è grande 8 slot, ogni cristallo ne riempie 2.
	u8 ntiles = g_PlayerRewindEnergy / (SNAPSHOT_BUFFER_SIZE / 8);

	if (ntiles != 0)
		VDP_FillLayout_GM2(62, 21, 0, ntiles, 1);
	if (ntiles != 8)
		VDP_FillLayout_GM2(63, 21 + ntiles, 0, 8 - ntiles, 1);
}

//=============================================================================
// FIELD COLLISION FUNCTIONS
//=============================================================================

extern bool bboxCollide(u8 x1, u8 y1, u8 x2, u8 y2);
extern bool rectCollide(u8 ax1, u8 ay1, u8 ax2, u8 ay2,
				        u8 bx1, u8 by1, u8 bx2, u8 by2);
extern void FxPlay(u8 id);

bool isPlayerHitByEnergyFields(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	// Controlla i campi di forza di ciascun nemico
	for (u8 e=0; e < lvl->num_enemies; e++) {

		if (enemies[e].field_state != 0) {
			if (bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY,
			                enemies[e].field_x, enemies[e].field_y)) {

				// Se il proiettile colpisce il personaggio, lo consuma
				if (enemies[e].field_state == 2) {
					enemies[e].field_state = 0;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

bool isPlayerHitByEnemies(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	for (u8 e=0; e < lvl->num_enemies; e++) {

		if (rectCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY,
					    g_PlayerPawn.PositionX + 15, g_PlayerPawn.PositionY + 15,
				        enemies[e].pos_x + 4, enemies[e].pos_y,
				        enemies[e].pos_x + 11, enemies[e].pos_y + 15)) {

			// Se siamo sopra il nemico e stiamo scendendo
			if (g_PlayerPawn.PositionY < enemies[e].pos_y && g_VelocityY < 0) {
				// Stordiamo il nemico
				enemies[e].stunned_timer = 100;

				// Disattiva solo l'eventuale campo di forza locale, non i
				// proiettili
				if (enemies[e].field_state == 1) {
					enemies[e].field_state = 0;
				}

				// Piccolo salto verso l'alto
				g_VelocityY = FORCE;
				g_PlayerJumping = TRUE;

				FxPlay(FX_STOMP_ROBOT);

				// Reveal hidden key if this is the trigger enemy
				if (!g_KeyEnabled && !g_PlayerHasKey && (i8)e == lvl->key_trigger_enemy) {
					g_KeyEnabled = TRUE;
					FxPlay(FX_SHOW_KEY);
				}

				return FALSE;
			} else if (enemies[e].stunned_timer == 0) {
				return TRUE;
			}
		}
	}
	return FALSE;
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

//=============================================================================
// DRAW FUNCTIONS
//=============================================================================

extern bool g_CrystalEnabled;
extern u8 g_KeyAnimFrame;
extern u8 g_CrystalAnimFrame;

extern u8 g_PlatformSpritesBaseID;
extern u8 g_MineSpritesBaseID;

extern u8 g_EnemySpritesBaseID;
extern u8 g_EnemyAnimCounter;
extern u8 g_EnemyKeyHintCounter;

extern u8 g_EnergyFieldSpritesBaseID;
extern u8 g_EnergyFieldAnimCounter;

extern u8 g_VortexAnimFrame;

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

void DrawVortex() {
	if (g_PlayerPawn.PositionY < 8) {

		g_VortexAnimFrame++;
		if (g_VortexAnimFrame >= 30) {
			g_VortexAnimFrame = 0;
		}

		u8 frame = g_VortexAnimFrame / 10;
		u8 pattern = VORTEX_FRAME(frame);

		VDP_SetSpriteSM1(VORTEX_SPRITE_ID, 0, 0, pattern, COLOR_WHITE);
	}
}

void DrawPlatforms(struct Level *lvl, bool rewind) {
	struct Platform *platforms = lvl->platforms;

	for (u8 p=0; p < lvl->num_platforms; p++) {
		u8 index = g_PlatformSpritesBaseID + p;
		u8 color;
		if (rewind) {
			color = COLOR_WHITE;
		} else {
			color = COLOR_BLACK;
		}
		VDP_SetSpriteSM1(index, platforms[p].pos_x, platforms[p].pos_y, PLATFORM_PATTERN_OFFSET, color);
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

		VDP_SetSpriteSM1(index, mines[m].pos_x, mines[m].pos_y, MINE_PATTERN_OFFSET, color);
    }
}

void DrawEnemies(struct Level *lvl, bool rewind) {
	struct Enemy *enemies = lvl->enemies;

	// Simple animation: alternate between 2 frames every 10 ticks
	g_EnemyAnimCounter++;
	if (g_EnemyAnimCounter >= 20) {
		g_EnemyAnimCounter = 0;
	}

	// Key hint blink counter: 250 frames = 5 seconds at 50fps
	g_EnemyKeyHintCounter++;
	if (g_EnemyKeyHintCounter >= 250) {
		g_EnemyKeyHintCounter = 0;
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
		} else if (!g_KeyEnabled
			&& !g_PlayerHasKey
			&& (i8)e == lvl->key_trigger_enemy
			&& g_EnemyKeyHintCounter < 10) {
			// Blink the trigger enemy in another color for 10 frames every 5 seconds
			color = COLOR_DARK_RED;
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

		if (enemies[e].field_state == 2) {
			// Type 3 projectile: use bullet sprite
			u8 color;
			if (rewind) {
				color = COLOR_WHITE;
			} else {
				color = COLOR_CYAN;
			}
			VDP_SetSpriteSM1(sprite_id, enemies[e].field_x, enemies[e].field_y, BULLET_PATTERN_OFFSET, color);
		} else if (enemies[e].field_state == 1) {
			// Type 2 stationary field: use animated energy field sprite
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
			VDP_SetSpriteSM1(sprite_id, enemies[e].field_x, enemies[e].field_y, pattern, color);
		} else {
			// No active field - hide the sprite
			VDP_HideSprite(sprite_id);
		}
	}
}

//=============================================================================
// UPDATE FUNCTIONS
//=============================================================================

extern bool g_PlayerInputRight;
extern bool g_PlayerInputLeft;
extern bool g_PlayerInputUp;

extern u8   g_PlayerAction;
extern bool g_PlayerMovingRight;
extern bool g_PlayerMovingLeft;

extern bool g_PlayerDying;
extern i8   g_mDX;
extern i8   g_DX;
extern i8   g_DY;

extern i8 GetDPos(i8* m);

extern void FxPlay(u8 id);

void UpdatePlayerInput() {
	g_PlayerInputRight = FALSE;
	g_PlayerInputLeft = FALSE;
	g_PlayerInputUp = FALSE;

	u8 row8 = Keyboard_Read(8);
	u8 joy = Joystick_Read(JOY_PORT_1);

	if (IS_KEY_PRESSED(row8, KEY_RIGHT) || Keyboard_IsKeyPressed(KEY_D)) {
		g_PlayerInputRight = TRUE;
	} else if (IS_KEY_PRESSED(row8, KEY_LEFT) || Keyboard_IsKeyPressed(KEY_A)) {
		g_PlayerInputLeft = TRUE;
	}

	if (IS_KEY_PRESSED(row8, KEY_UP)
	|| IS_JOY_PRESSED(joy, JOY_INPUT_TRIGGER_B)
    || Keyboard_IsKeyPressed(KEY_W)) {
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

		FxPlay(FX_JUMP);
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

void UpdateEnemies(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	for (u8 e=0; e < lvl->num_enemies; e++) {
		struct Enemy *enemy = &enemies[e];

		// Muovi i proiettili indipendentemente dallo stato del nemico
		if (enemy->field_state == 2) {

			enemy->field_mDX += 12 * enemy->field_dir;
			i8 dx = GetDPos(&enemy->field_mDX);
			enemy->field_x += dx;

			// Se esce dallo schermo, resettalo
			if (enemy->field_x < 8 || enemy->field_x > 240) {
				enemy->field_state = 0;
			}
		}

		// Stato stunned. Per un nemico in questo stato ci fermiamo qua:
		// no movimento, no nuovi field.
		if (enemy->stunned_timer > 0) {
			enemy->stunned_timer--;
			continue;
		}

		// Nemici tipo 2, campo di forza locale
		if (enemy->type == 2) {
			if (enemy->field_state == 1) {
				enemy->field_timer--;

				if (enemy->field_timer == 0) {
					// Al termine del timer disattiva il campo di forza
					enemy->field_state = 0;
				}

				// Se il campo di forza è attivo, il nemico non si muove
				continue;

			} else if (enemy->field_state == 0) {
				// Spawn window based on g_RemainingFS cycle
				// Different enemies check at different counter values (offset by enemy index)
				// This creates a natural phase offset without multiplication
				u8 check_value = g_RemainingFS + e;
				if (check_value >= 50) check_value -= 50;

				// 10-frame window out of 50
				if (check_value < 10 && Math_GetRandom8() < 10) {
					FxPlay(FX_FORCE_FIELD);

					enemy->field_state = 1;
					enemy->field_timer = 100;
					enemy->field_dir = enemy->dir_x;

					// Il campo di forza appare davanti al nemico
					enemy->field_x = enemy->pos_x + (enemy->field_dir * 16);
					enemy->field_y = enemy->pos_y;
				}
			}
		}

		// Nemici tipo 3, campo di forza proiettile
		if (enemy->type == 3) {
			if (enemy->field_state == 0) {
				// Spawn window based on g_RemainingFS cycle
				// Different enemies check at different counter values (offset by enemy index)
				u8 check_value = g_RemainingFS + e;
				if (check_value >= 50) check_value -= 50;

				// 8-frame window out of 50
				if (check_value < 8 && Math_GetRandom8() < 5) {
					FxPlay(FX_FORCE_FIELD);

					enemy->field_state = 2;
					enemy->field_dir = enemy->dir_x;

					enemy->field_x = enemy->pos_x + (enemy->field_dir * 16);
					enemy->field_y = enemy->pos_y + 8;
					enemy->field_mDX = 0;
				}
			}
		}

		// Check boundaries and reverse direction
		if (enemy->pos_x < enemy->min_x) {
			enemy->pos_x = enemy->min_x;
			enemy->dir_x = -enemy->dir_x;
		}

		if (enemy->pos_x > enemy->max_x) {
			enemy->pos_x = enemy->max_x;
			enemy->dir_x = -enemy->dir_x;
		}

		// Add speed to accumulated movement (in eighths of pixel)
		// Type 0: 2 eighths/frame, all others: 4 eighths/frame
		u8 speed = (enemy->type == 0) ? 2 : 4;
		enemy->mDX += speed * enemy->dir_x;

		// Convert eighths-of-pixel to actual pixel movement
		i8 dx = GetDPos(&enemy->mDX);
		enemy->pos_x += dx;
	}
}

//=============================================================================
// LEVEL LOADING
//=============================================================================

extern const struct Level* g_LevelOrder[];
extern struct Level g_ActiveLevel;

struct Platform g_RuntimePlatforms[MAX_PLATFORMS];
struct Mine     g_RuntimeMines[MAX_MINES];
struct Enemy    g_RuntimeEnemies[MAX_ENEMIES];

void LoadLevel(u8 levelIndex) {
	const struct Level* src = g_LevelOrder[levelIndex];

	// Copy scalar fields (this overwrites pointers too)
	g_ActiveLevel = *src;

	// Set pointers to runtime arrays
	g_ActiveLevel.platforms = g_RuntimePlatforms;
	g_ActiveLevel.mines = g_RuntimeMines;
	g_ActiveLevel.enemies = g_RuntimeEnemies;

	// Deep copy platforms
	for (u8 i = 0; i < src->num_platforms; i++) {
		g_RuntimePlatforms[i] = src->platforms[i];
	}

	// Deep copy mines
	for (u8 i = 0; i < src->num_mines; i++) {
		g_RuntimeMines[i] = src->mines[i];
	}

	// Deep copy enemies
	for (u8 i = 0; i < src->num_enemies; i++) {
		g_RuntimeEnemies[i] = src->enemies[i];
	}
}

//=============================================================================
// SNAPSHOT BUFFER STORAGE
//=============================================================================

// Circular buffer for snapshots
GameSnapshot g_SnapshotBuffer[SNAPSHOT_BUFFER_SIZE];

// Circular buffer management
u8 g_SnapshotHead = 0;
u8 g_SnapshotTail = 0;
u8 g_SnapshotCount = 0;

//=============================================================================
// SNAPSHOT FUNCTIONS
//=============================================================================

/**
 * @brief Initialize the snapshot system
 *
 * Resets the circular buffer to empty state
 */
void Snapshot_Initialize() {
    g_SnapshotHead = 0;
    g_SnapshotTail = 0;
    g_SnapshotCount = 0;
}

/**
 * @brief Capture current game state into the snapshot buffer
 *
 * This function is called every frame during gameplay to record the state
 * of all dynamic objects. Uses a circular buffer with the same strategy
 * as the player position rewind.
 *
 * @param lvl Pointer to current level
 * @param player_x Player X position
 * @param player_y Player Y position
 * @param player_frame Player animation frame
 */
void Snapshot_Capture(struct Level *lvl, u8 player_x, u8 player_y, u8 player_frame) {
    // Get pointer to the snapshot slot we're about to fill
    GameSnapshot *snap = &g_SnapshotBuffer[g_SnapshotHead];

    // Capture player state
    snap->player_x = player_x;
    snap->player_y = player_y;
    snap->player_frame = player_frame;

    // Capture platform positions
    for (u8 p = 0; p < lvl->num_platforms && p < MAX_PLATFORMS; p++) {
        snap->platform_x[p] = lvl->platforms[p].pos_x;
        snap->platform_y[p] = lvl->platforms[p].pos_y;
    }

    // Capture enemy states
    for (u8 e = 0; e < lvl->num_enemies && e < MAX_ENEMIES; e++) {
        struct Enemy *enemy = &lvl->enemies[e];
        snap->enemy_x[e] = enemy->pos_x;
        snap->enemy_y[e] = enemy->pos_y;
        snap->enemy_dir[e] = enemy->dir_x;
        snap->enemy_stunned[e] = enemy->stunned_timer;
        snap->enemy_field_state[e] = enemy->field_state;
        snap->enemy_field_x[e] = enemy->field_x;
        snap->enemy_field_y[e] = enemy->field_y;
    }

    // Advance head pointer (circular)
    g_SnapshotHead = (g_SnapshotHead + 1) % SNAPSHOT_BUFFER_SIZE;

    // Update circular buffer state
    if (g_SnapshotCount == SNAPSHOT_BUFFER_SIZE) {
        // Buffer was full, consume oldest snapshot
        g_SnapshotTail = (g_SnapshotTail + 1) % SNAPSHOT_BUFFER_SIZE;
    } else {
        // Buffer still filling up
        g_SnapshotCount++;
    }
}

/**
 * @brief Restore game state from a snapshot
 *
 * This function is called during rewind to restore all game objects to their
 * state at a previous frame.
 *
 * @param lvl Pointer to current level
 * @param snapshot_index Index in the circular buffer to restore from
 * @param out_player_x Output: restored player X position
 * @param out_player_y Output: restored player Y position
 * @param out_player_frame Output: restored player animation frame
 */
void Snapshot_Restore(struct Level *lvl, u8 snapshot_index, u8 *out_player_x, u8 *out_player_y, u8 *out_player_frame) {
    // Get pointer to the snapshot we're restoring
    GameSnapshot *snap = &g_SnapshotBuffer[snapshot_index];

    // Restore player state
    *out_player_x = snap->player_x;
    *out_player_y = snap->player_y;
    *out_player_frame = snap->player_frame;

    // Restore platform positions
    for (u8 p = 0; p < lvl->num_platforms && p < MAX_PLATFORMS; p++) {
        lvl->platforms[p].pos_x = snap->platform_x[p];
        lvl->platforms[p].pos_y = snap->platform_y[p];
    }

    // Restore enemy states
    for (u8 e = 0; e < lvl->num_enemies && e < MAX_ENEMIES; e++) {
        struct Enemy *enemy = &lvl->enemies[e];
        enemy->pos_x = snap->enemy_x[e];
        enemy->pos_y = snap->enemy_y[e];
        enemy->dir_x = snap->enemy_dir[e];
        enemy->stunned_timer = snap->enemy_stunned[e];
        enemy->field_state = snap->enemy_field_state[e];
        enemy->field_x = snap->enemy_field_x[e];
        enemy->field_y = snap->enemy_field_y[e];
    }
}

/**
 * @brief Get number of available rewind steps
 */
u8 Snapshot_GetRewindCount() {
    return g_SnapshotCount;
}

/**
 * @brief Perform one rewind step (moves head backward)
 * @return New snapshot index to use for restoration
 */
u8 Snapshot_RewindStep() {
    g_SnapshotHead = (g_SnapshotHead - 1 + SNAPSHOT_BUFFER_SIZE) % SNAPSHOT_BUFFER_SIZE;
    g_SnapshotCount--;
    return g_SnapshotHead;
}

//=============================================================================
// CUTSCENE FUNCTIONS
//=============================================================================

#include "cutscene.c"

// Player character walk definition (2 layers, 3 animation frames)
// Adjust frame values to match your player sprite frames
static const CutSpriteAnimDef g_PlayerWalkRight = {
    .baseId = 0,  // First sprite ID (uses 0 and 1 for 2 layers)
    .numLayers = 2,                 // Player has 2 sprite layers
    .numFrames = 3,                 // 3-frame walk cycle
    .frames = { PLAYER_FRAME(1), PLAYER_FRAME(2), PLAYER_FRAME(3) },
    .idleFrame = PLAYER_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,                 // Frames between animation changes
    .layerOffset = laySize,         // Pattern offset between layers
    .colors = { COLOR_BLACK, COLOR_DARK_YELLOW }  // Layer colors
};

static const CutSpriteAnimDef g_PlayerWalkLeft = {
    .baseId = 0,
    .numLayers = 2,
    .numFrames = 3,                 // 3-frame walk cycle
    .frames = { PLAYER_FRAME(4), PLAYER_FRAME(5), PLAYER_FRAME(6) },
    .idleFrame = PLAYER_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, COLOR_DARK_YELLOW }
};

static const CutSpriteAnimDef g_DocWalkLeft = {
    .baseId = 2,
    .numLayers = 1,
    .numFrames = 2,
    .frames = { DOC_FRAME(0), DOC_FRAME(1), NULL },
    .idleFrame = DOC_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, 0 }
};

static const CutSpriteAnimDef g_DocWalkRight = {
    .baseId = 2,
    .numLayers = 1,
    .numFrames = 2,
    .frames = { DOC_FRAME(2), DOC_FRAME(3), NULL },
    .idleFrame = DOC_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, 0 }
};

static const CutSpriteAnimDef g_PlayerFreaked = {
    .baseId = 0,
    .numLayers = 2,
    .numFrames = 2,
    .frames = { PLAYER_FRAME(7), PLAYER_FRAME(8), NULL },
    .idleFrame = PLAYER_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 12,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, COLOR_LIGHT_YELLOW }
};

#include "content/screens/screen_5.h"

//-----------------------------------------------------------------------------
// Intro cutscene
//-----------------------------------------------------------------------------
const CutCmd g_IntroCutscene[] = {
	CUT_WAIT(25),

	CUT_TEXT("PRESS SPACE KEY", 23),

    CUT_WAIT_KEY(),
    CUT_WAIT(10),

    // Clear and show new text
    CUT_CLEAR_TEXT(),

    CUT_LOAD_LAYOUT(g_Screen5, 0, CUTSCENE_GFX_Y, CUTSCENE_SCREEN_W, 24),

    CUT_TEXT_TYPE("(...ALARM BELLS RINGING...)", 20),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_SPRITE_WALK(&g_DocWalkLeft, 29*8, 11*8, 20*8, 11*8, 1),

    CUT_TEXT_TYPE("TIME IS RUNNING OUT...", 19),
	CUT_TEXT_TYPE("...WHERE IS CHRONO RUNNER?", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	// Entra Chrono
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0*8, 11*8, 14*8, 11*8, 2),

	CUT_TEXT_TYPE("AT LAST. AGENT, THIS WILL BE", 19),
	CUT_TEXT_TYPE("YOUR HARDEST MISSION SO FAR.", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("DOCTOR CRAZY IS BUILDING A TIME", 19),
	CUT_TEXT_TYPE("MACHINE. HE WANTS TO BRING BACK", 21),
	CUT_TEXT_TYPE("THE NAZI REGIME!", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_ANIM(&g_PlayerFreaked, 14*8, 11*8, 100),

	CUT_TEXT_TYPE("YOU ONLY HAVE ONE HOUR TO", 19),
	CUT_TEXT_TYPE("RECOVER ALL FOUR TIME CRYSTALS.", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("THEY GRANT YOU THE POWER TO", 19),
	CUT_TEXT_TYPE("REWIND TIME. USE IT WISELY!", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("CRYSTALS ALSO PROTECT YOU FROM", 19),
	CUT_TEXT_TYPE("DEATH, BUT BE CAREFUL!", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("EACH TIME YOU ARE REVIVED, YOU", 19),
	CUT_TEXT_TYPE("WILL LOSE 5 MINUTES!", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_ANIM(&g_PlayerFreaked, 14*8, 11*8, 100),

	CUT_TEXT_TYPE("BRING ALL FOUR CRYSTALS TO ME", 19),
	CUT_TEXT_TYPE("AND WE WILL SAVE THE WORLD!", 21),
	CUT_TEXT_TYPE("GO!", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    // Chrono esce
    CUT_SPRITE_WALK(&g_PlayerWalkLeft, 14*8, 11*8, 0*8, 11*8, 2),
	CUT_SPRITE_HIDE(0),
	CUT_SPRITE_HIDE(1),

	CUT_TEXT_TYPE("...THIS WILL BE OUR TRIUMPH...", 19),
	CUT_TEXT_TYPE("FINALLY.", 21),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_WALK(&g_DocWalkRight, 20*8, 11*8, 29*8, 11*8, 1),

    CUT_TEXT("PRESS SPACE KEY", 23),

    CUT_WAIT_KEY(),
	CUT_WAIT(10),

    CUT_END(),
};

//=============================================================================
// MENU STATE
//=============================================================================

#include "content/screens/screen_2.h"

u8 g_MenuState = 0;
u8 g_MenuSelection = 0;
u8 g_MenuWait = 0;

// Menu option Y positions (rows 11, 13, 15, 17)
#define MENU_NUM_OPTIONS    4
#define MENU_HIGHLIGHT_TILE 49
#define MENU_EMPTY_TILE     47

static const u8 g_MenuRows[MENU_NUM_OPTIONS] = { 11, 13, 15, 17 };
static const u8 g_MenuColumns[MENU_NUM_OPTIONS] = { 14, 12, 10, 8 };

extern bool State_Intermission();

bool State_Menu()
{
    if (g_MenuState == 0) {
        VDP_HideAllSprites();
        VDP_WriteLayout_GM2(g_Screen2, 0, 0, 32, 24);
        g_MenuSelection = 0;
        VDP_Poke_GM2(g_MenuColumns[0], g_MenuRows[0], MENU_HIGHLIGHT_TILE);
        g_MenuWait = 0;
        g_MenuState = 1;
    }

    u8 row8 = Keyboard_Read(8);

    // Wait for all keys to be released before accepting new input
    if (g_MenuWait && IS_KEY_RELEASED(row8, KEY_UP) && IS_KEY_RELEASED(row8, KEY_DOWN) && IS_KEY_RELEASED(row8, KEY_SPACE)) {
        g_MenuWait = 0;
    }

    if (g_MenuWait) return TRUE;

    u8 prev = g_MenuSelection;

    if (IS_KEY_PRESSED(row8, KEY_UP) && g_MenuSelection > 0) {
        g_MenuSelection--;
        g_MenuWait = 1;
    } else if (IS_KEY_PRESSED(row8, KEY_DOWN) && g_MenuSelection < MENU_NUM_OPTIONS - 1) {
        g_MenuSelection++;
        g_MenuWait = 1;
    } else if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
        if (g_MenuSelection == 0) {
			g_MenuState = 0;
            Cutscene_Start(g_IntroCutscene, State_Intermission, NULL);
        }
        return TRUE;
    }

    if (prev != g_MenuSelection) {
        VDP_Poke_GM2(g_MenuColumns[prev], g_MenuRows[prev], MENU_EMPTY_TILE);
        VDP_Poke_GM2(g_MenuColumns[g_MenuSelection], g_MenuRows[g_MenuSelection], MENU_HIGHLIGHT_TILE);
    }

    return TRUE;
}
