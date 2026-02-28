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

extern struct Level g_ActiveLevel;

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
// COLLISION FUNCTIONS
//=============================================================================

extern void ReinitPlayer(Pawn *pawn, Pawn_Sprite *spr_layers, u8 n_spr_layers, u8 x, u8 y);
extern bool bboxCollide(u8 x1, u8 y1, u8 x2, u8 y2);
extern bool rectCollide(u8 ax1, u8 ay1, u8 ax2, u8 ay2,
				        u8 bx1, u8 by1, u8 bx2, u8 by2);
extern void FxPlay(u8 id);

bool isPlayerHitByEnergyFields(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	// Controlla i campi di forza di ciascun nemico
	for (u8 e=0; e < lvl->num_enemies; e++) {

		if (enemies[e].field_state == 1 || enemies[e].field_state == 2) {
			if (rectCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY,
					        g_PlayerPawn.PositionX + 15, g_PlayerPawn.PositionY + 15,
				        enemies[e].field_x + 2, enemies[e].field_y + 6,
				        enemies[e].field_x + 14, enemies[e].field_y + 9)) {

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
		// Se un nemico è appena stordito disabilita le collisioni,
		// così non sta tra i piedi
		if (enemies[e].stunned_timer > 50)
			continue;

		if (rectCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY,
					    g_PlayerPawn.PositionX + 15, g_PlayerPawn.PositionY + 15,
				        enemies[e].pos_x + 4, enemies[e].pos_y,
				        enemies[e].pos_x + 11, enemies[e].pos_y + 15)) {

			// Se siamo sopra il nemico, stiamo scendendo, e il nemico non è
			// appena stordito
			if (g_PlayerPawn.PositionY < enemies[e].pos_y
				&& g_VelocityY < 0
				&& enemies[e].stunned_timer < 50) {

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

bool isPlayerAtExit() {
	if (!g_PlayerHasKey)
		return FALSE;

	u8 door_x = g_ActiveLevel.end_x;
	u8 door_y = g_ActiveLevel.end_y;

	return bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, door_x * 8, door_y * 8);
}

bool isPlayerOnSpikes() {
	u8 tile = VDP_Peek_GM2((g_PlayerPawn.PositionX >> 3) + 1,
						   (g_PlayerPawn.PositionY >> 3) + 2);
	if (tile >= 198 && tile <= 202) {
		return TRUE;
	}
    return FALSE;
}

struct Mine* isPlayerOnMines(struct Level *lvl) {

	u8 nm = lvl->num_mines;
	struct Mine *mines = lvl->mines;

	for (u8 m=0; m < nm; m++) {
		// Confronta un bbox parziale (8x16) del giocatore con un bbox 2x1 della mina
		if (rectCollide(g_PlayerPawn.PositionX +  4, g_PlayerPawn.PositionY,
					    g_PlayerPawn.PositionX + 11, g_PlayerPawn.PositionY + 15,
				        mines[m].pos_x + 7, mines[m].pos_y - 1,
				        mines[m].pos_x + 7 + 2, mines[m].pos_y)) {
			return &mines[m];
		}
	}
	return NULL;
}

bool isPlayerInPit() {
	return g_PlayerPawn.PositionY > 192;
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

			} else if (enemy->field_state == 3) {
				// Pre-fuoco: il nemico è fermo, il timer conta verso lo sparo
				enemy->field_timer--;

				if (enemy->field_timer == 0) {
					FxPlay(FX_FORCE_FIELD);

					enemy->field_state = 1;
					enemy->field_timer = 100;
					enemy->field_dir = enemy->dir_x;

					// Il campo di forza appare davanti al nemico
					enemy->field_x = enemy->pos_x + (enemy->field_dir * 16);
					enemy->field_y = enemy->pos_y;
				}

				// Il nemico non si muove durante il pre-fuoco
				continue;

			} else if (enemy->field_state == 0) {
				if (Math_GetRandom8() < 2) {
					enemy->field_state = 3;
					enemy->field_timer = 25;
				}
			}
		}

		// Nemici tipo 3, campo di forza proiettile
		if (enemy->type == 3) {
			if (enemy->field_state == 3) {
				// Pre-fuoco: il nemico è fermo, il timer conta verso lo sparo
				enemy->field_timer--;

				if (enemy->field_timer == 0) {
					FxPlay(FX_BULLET);

					enemy->field_state = 2;
					enemy->field_dir = enemy->dir_x;

					enemy->field_x = enemy->pos_x + (enemy->field_dir * 16);
					enemy->field_y = enemy->pos_y;
					enemy->field_mDX = 0;
				}

				// Il nemico non si muove durante il pre-fuoco
				continue;

			} else if (enemy->field_state == 0) {
				if (Math_GetRandom8() < 2) {
					enemy->field_state = 3;
					enemy->field_timer = 25;
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

#include "cutscene_s7.c"

//=============================================================================
// BOSS FIGHT
//=============================================================================

// Frames between stance switches
#define BOSS_SWITCH_FRAMES   25
// Total number of stances: 3 sheets x 4 slots each
#define BOSS_NUM_STANCES      9
// Each stance is a 14x11 tile rectangle inside a 32-wide sheet
#define BOSS_STANCE_W         14
#define BOSS_STANCE_H         11
// Destination tile position on the display where the stance is drawn
// (adjust to match the art in screen 84)
#define BOSS_STANCE_DST_X      9
#define BOSS_STANCE_DST_Y      12
// Head hitbox: 3-tile wide band at tile (15, 12); y covers top half of the tile
#define BOSS_HEAD_X1  112   // (15 - 1) * 8
#define BOSS_HEAD_X2  135   // (15 + 2) * 8 - 1
#define BOSS_HEAD_Y1   96   // 12 * 8
#define BOSS_HEAD_Y2  103   // 13 * 8 - 1
// Player start position in the boss arena (pixels)
#define BOSS_START_X   16
#define BOSS_START_Y  144
// Number of head hits needed to defeat the boss
#define BOSS_HITS_REQUIRED   5
// Boss horizontal movement: ±BOSS_MOVE_RANGE tiles, steps once per stance change
#define BOSS_MOVE_RANGE       3
// Pixel offset of head hitbox from stance left edge: BOSS_HEAD_X1 - BOSS_STANCE_DST_X*8
#define BOSS_HEAD_OFFSET_X   40
// Boss stances
#define BOSS_STUN_STANCE    9   // boss is stunned (head stomped)
#define BOSS_DEFEAT_STANCE 10   // boss is defeated (5th stomp)
// Boss bullet
#define BOSS_FIRE_FRAMES      200               // frames between shots
#define BOSS_BULLET_SPEED       2               // pixels/frame along dominant axis
// Boss vortex (shared motion logic with bullet)
#define BOSS_VORTEX_SPEED   2   // pixels/frame
#define BOSS_ARRIVE_DIST    8   // arrival detection radius in pixels
// Vortex states
#define VORTEX_INACTIVE  0  // not in play
#define VORTEX_TO_SLOT   1  // flying from boss head toward corner slot
#define VORTEX_AT_SLOT   2  // at slot, waiting for player to touch
#define VORTEX_TO_BOSS   3  // flying back as weapon (black)

u8   g_BossFrame          = 0;  // Frames elapsed in current stance
u8   g_BossScreen         = 0;  // Current stance index (0 .. BOSS_NUM_STANCES-1)
u8   g_BossHitCount       = 0;  // Stomps landed on the boss head so far
u8   g_BossDstX           = 0;  // Current stance X position in tiles (runtime)
i8   g_BossMoveDir        = 1;  // Movement direction: +1 right, -1 left
bool g_BossBulletActive   = FALSE;
u8   g_BossBulletX        = 0;
u8   g_BossBulletY        = 0;
i8   g_BossBulletDX       = 0;  // pixels per frame, X
i8   g_BossBulletDY       = 0;  // pixels per frame, Y
u8   g_BossFireTimer      = 0;
// Flying vortex state machine
u8   g_VortexState    = VORTEX_INACTIVE;
i16  g_VortexX        = 0;
i16  g_VortexY        = 0;
i8   g_VortexDX       = 0;
i8   g_VortexDY       = 0;
// g_VortexAnimFrame is already defined in chronorunner.c (declared extern above)
u8   g_VortexSlotIdx  = 0;  // 0-3: which corner slot this vortex targets

// Vortex corner slot positions in tiles
static const u8 g_BossVortexPos[4][2] = {
    {  4, 19 },
    {  6,  4 },
    { 24,  4 },
    { 26, 19 },
};

// Boss fight: background + stance sprite sheets (4 stances each, 2x2 grid)
#include "content/screens/screen_84.h"
#include "content/screens/screen_85.h"
#include "content/screens/screen_86.h"
#include "content/screens/screen_87.h"

// Boss arena platform definitions
static const struct Platform g_BossPlatformsROM[] = {
    {  1*8, 13*8,   // pos_x, pos_y
		0,   -1,   // dir_x, dir_y
	   1*8, 7*8,   // min_x, min_y
	   1*8, 22*8}, // max_x, max_y
    { 10*8, 6*8,
		 1, 0,
	  5*8, 6*8,
      26*8, 6*8 },
    { 20*8, 8*8,
		 -1, 0,
	  5*8, 8*8,
      26*8, 8*8 },
	{ 29*8, 9*8,
		 0, 1,
	  29*8, 7*8,
      29*8, 22*8 },
};
#define BOSS_NUM_PLATFORMS (sizeof(g_BossPlatformsROM) / sizeof(g_BossPlatformsROM[0]))

//=============================================================================
// BOSS STATE
//=============================================================================

extern const Pawn_Sprite g_PlayerLayers[];
extern void SetMessageScreen(const c8* text, i8 songId, u16 duration);
extern bool State_MessageScreen();
extern bool State_Death();
extern u8 g_CountDownTicks;
extern struct Platform g_RuntimePlatforms[];
extern void AllocateCurrentLevelSprites();
extern void SetSong(u8 id);
extern void Loop(bool value);
extern void Play();

bool State_Boss();

// Compute Chebyshev-normalized velocity from (sx,sy) toward (tx,ty) at given speed.
// Used by both boss bullet and flying vortex to avoid code duplication.
static void AimProjectile(u8 sx, u8 sy, u8 tx, u8 ty, u8 speed, i8 *dx, i8 *dy)
{
	i16 ddx = (i16)tx - (i16)sx;
	i16 ddy = (i16)ty - (i16)sy;
	i16 ax  = ddx < 0 ? -ddx : ddx;
	i16 ay  = ddy < 0 ? -ddy : ddy;
	i16 m   = ax > ay ? ax : ay;
	if (m == 0) m = 1;
	*dx = (i8)(ddx * speed / m);
	*dy = (i8)(ddy * speed / m);
}

// Clear the boss area (tile 84 = arena background) then overlay the current
// stance rectangle sourced from sheets 85-87 (4 stances per sheet, 2x2 grid).
// g_Screen84 is drawn once on entry; this only repaints the dynamic region.
void DrawBossStance()
{
	u8 sheet = g_BossScreen >> 2;        // which sheet: 0=85, 1=86, 2=87
	u8 slot  = g_BossScreen & 3;         // which slot in sheet: 0-3
	u8 src_x = (slot & 1) ? 17 : 1;     // bit 0: col 1 or col 17
	u8 src_y = (slot & 2) ? 13 : 1;     // bit 1: row 1 or row 13
	const u8 *sheet_ptr;

	// Clear only the boss rectangle with the arena background tile
	VDP_FillLayout_GM2(84, 6, 10, 20, 13);

	if      (sheet == 0) sheet_ptr = g_Screen85;
	else if (sheet == 1) sheet_ptr = g_Screen86;
	else                 sheet_ptr = g_Screen87;

	for (u8 r = 0; r < BOSS_STANCE_H; r++) {
		VDP_WriteLayout_GM2(sheet_ptr + (u16)(src_y + r) * 32 + src_x,
		                    g_BossDstX, BOSS_STANCE_DST_Y + r,
		                    BOSS_STANCE_W, 1);
	}
}

void InitBoss()
{
	VDP_HideAllSprites();

	g_BossFrame        = 0;
	g_BossScreen       = 0;
	g_BossHitCount     = 0;
	g_BossDstX         = BOSS_STANCE_DST_X;
	g_BossMoveDir      = 1;
	g_BossBulletActive = FALSE;
	g_BossFireTimer    = 0;
	g_VortexState      = VORTEX_INACTIVE;
	g_VortexSlotIdx    = 0;
	g_VortexAnimFrame  = 0;
	// Zero the timer so State_Death always routes to GAME OVER in boss mode
	g_RemainingMinutes   = 0;
	g_RemainingSeconds   = 0;
	g_CountDownTicks     = 0;
	g_PlayerDying        = FALSE;
	g_mDX = 0;
	g_mDY = 0;
	g_VelocityY = 0;

	// Populate g_ActiveLevel with boss arena data (platforms only)
	g_ActiveLevel.num_platforms = BOSS_NUM_PLATFORMS;
	g_ActiveLevel.platforms     = g_RuntimePlatforms;
	g_ActiveLevel.num_mines     = 0;
	g_ActiveLevel.num_enemies   = 0;
	g_ActiveLevel.key_trigger_enemy = -1;

	// Copy ROM platform data into the mutable runtime array
	for (u8 i = 0; i < BOSS_NUM_PLATFORMS; i++) {
		g_RuntimePlatforms[i] = g_BossPlatformsROM[i];
	}

	// Allocate sprite IDs
	AllocateCurrentLevelSprites();

	// Draw the full arena background once; DrawBossStance only refreshes the dynamic region
	VDP_WriteLayout_GM2(g_Screen84, 0, 0, 32, 24);
	DrawBossStance();

	ReinitPlayer(&g_PlayerPawn,
	             g_PlayerLayers, 2,
	             BOSS_START_X, BOSS_START_Y);

	SetSong(3);
	Loop(TRUE);
	Play();

	Game_SetState(State_Boss);
}

bool State_Boss()
{
	struct Level *lvl = &g_ActiveLevel;

	UpdatePlayerInput();
	UpdatePlatforms(lvl);
	struct Platform *platform = isPlayerOnPlatform(lvl);
	UpdatePlayerMovement(platform);
	UpdatePlayerAction();

	Pawn_SetAction(&g_PlayerPawn, g_PlayerAction);
	Pawn_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	Pawn_Update(&g_PlayerPawn);
	Pawn_Draw(&g_PlayerPawn);

	DrawPlatforms(lvl, FALSE);

	// Boss bullet: fire toward player (normal stances only)
	if (g_BossScreen < BOSS_NUM_STANCES) {
		g_BossFireTimer++;
		if (g_BossFireTimer >= BOSS_FIRE_FRAMES && !g_BossBulletActive) {
			g_BossFireTimer = 0;
			g_BossBulletActive = TRUE;
			u8 origin_x = g_BossDstX * 8 + BOSS_HEAD_OFFSET_X + 8;
			u8 origin_y = (BOSS_HEAD_Y1 + BOSS_HEAD_Y2) / 2;
			g_BossBulletX = origin_x;
			g_BossBulletY = origin_y;
			AimProjectile(origin_x, origin_y,
			              g_PlayerPawn.PositionX + 8, g_PlayerPawn.PositionY + 8,
			              BOSS_BULLET_SPEED, &g_BossBulletDX, &g_BossBulletDY);
			FxPlay(FX_BULLET);
		}
	}

	// Boss bullet: move, draw, and collide
	if (g_BossBulletActive) {
		g_BossBulletX += g_BossBulletDX;
		g_BossBulletY += g_BossBulletDY;
		if (g_BossBulletX < 4 || g_BossBulletX > 248 || g_BossBulletY > 192) {
			g_BossBulletActive = FALSE;
			VDP_HideSprite(VORTEX_SPRITE_ID);
		} else if (rectCollide(g_PlayerPawn.PositionX,      g_PlayerPawn.PositionY,
		                       g_PlayerPawn.PositionX + 15, g_PlayerPawn.PositionY + 15,
		                       g_BossBulletX + 6, g_BossBulletY + 6,
		                       g_BossBulletX + 9, g_BossBulletY + 9)) {
			g_BossBulletActive = FALSE;
			VDP_HideSprite(VORTEX_SPRITE_ID);
			g_PlayerDying = FALSE;  // reset so State_Death initialises properly
			Game_SetState(State_Death);
			return TRUE;
		} else {
			VDP_SetSpriteSM1(VORTEX_SPRITE_ID,
			                 g_BossBulletX, g_BossBulletY,
			                 BOSSBULLET_FRAME(0), COLOR_LIGHT_RED);
		}
	}

	// Head hit: vulnerable on any normal stance (not during hit/defeat poses)
	if (g_BossScreen < BOSS_NUM_STANCES) {
		u8 head_x1 = g_BossDstX * 8 + BOSS_HEAD_OFFSET_X;
		u8 head_x2 = head_x1 + (BOSS_HEAD_X2 - BOSS_HEAD_X1);
		if (rectCollide(g_PlayerPawn.PositionX,      g_PlayerPawn.PositionY,
		                g_PlayerPawn.PositionX + 15, g_PlayerPawn.PositionY + 15,
		                head_x1, BOSS_HEAD_Y1,
		                head_x2, BOSS_HEAD_Y2)
		    && g_PlayerPawn.PositionY < BOSS_HEAD_Y1
		    && g_VelocityY < 0) {

			g_BossHitCount++;
			g_VelocityY = FORCE;
			g_PlayerJumping = TRUE;
			FxPlay(FX_STOMP_ROBOT);

			if (g_BossHitCount < BOSS_HITS_REQUIRED) {
				// Spawn vortex flying from boss head toward its corner slot
				g_VortexSlotIdx   = g_BossHitCount - 1;  // stomp 1→slot 0, stomp 2→slot 1, ...
				u8 ox = g_BossDstX * 8 + BOSS_HEAD_OFFSET_X + 8;
				u8 oy = (BOSS_HEAD_Y1 + BOSS_HEAD_Y2) / 2;
				g_VortexX = ox;  g_VortexY = oy;
				AimProjectile(ox, oy,
				              g_BossVortexPos[g_VortexSlotIdx][0] * 8,
				              g_BossVortexPos[g_VortexSlotIdx][1] * 8,
				              BOSS_VORTEX_SPEED, &g_VortexDX, &g_VortexDY);
				g_VortexState     = VORTEX_TO_SLOT;
				g_VortexAnimFrame = 0;
			}
			g_BossFrame  = 0;
			g_BossScreen = (g_BossHitCount >= BOSS_HITS_REQUIRED)
			               ? BOSS_DEFEAT_STANCE : BOSS_STUN_STANCE;
			DrawBossStance();
		}
	}

	// Advance stance after BOSS_SWITCH_FRAMES frames (does NOT tick while stunned)
	if (g_BossScreen != BOSS_STUN_STANCE) {
		g_BossFrame++;
		if (g_BossFrame >= BOSS_SWITCH_FRAMES) {
			g_BossFrame = 0;
			if (g_BossScreen == BOSS_DEFEAT_STANCE) {
				SetMessageScreen("YOU WON!", -1, 500);
				Game_SetState(State_MessageScreen);
				return TRUE;
			}
			// Normal stance cycling
			g_BossScreen = (g_BossScreen + 1) % BOSS_NUM_STANCES;
			// Step position 1 tile in current direction, bounce at limits
			g_BossDstX += g_BossMoveDir;
			if ((i8)g_BossDstX >= (i8)(BOSS_STANCE_DST_X + BOSS_MOVE_RANGE))
				g_BossMoveDir = -1;
			else if ((i8)g_BossDstX <= (i8)(BOSS_STANCE_DST_X - BOSS_MOVE_RANGE))
				g_BossMoveDir = 1;
			DrawBossStance();
		}
	}

	// Flying vortex state machine
	if (g_VortexState != VORTEX_INACTIVE) {
		g_VortexAnimFrame++;
		if (g_VortexAnimFrame >= 30) g_VortexAnimFrame = 0;
		u8 vpattern = VORTEX_FRAME(g_VortexAnimFrame / 10);
		u8 sx = g_BossVortexPos[g_VortexSlotIdx][0] * 8;
		u8 sy = g_BossVortexPos[g_VortexSlotIdx][1] * 8;

		if (g_VortexState == VORTEX_TO_SLOT) {
			g_VortexX += g_VortexDX;
			g_VortexY += g_VortexDY;
			// Arrival: each axis has reached or passed its target (no u8 wrap risk with i16)
			u8 x_arr = (g_VortexDX == 0) || (g_VortexDX > 0 ? (g_VortexX >= (i16)sx) : (g_VortexX <= (i16)sx));
			u8 y_arr = (g_VortexDY == 0) || (g_VortexDY > 0 ? (g_VortexY >= (i16)sy) : (g_VortexY <= (i16)sy));
			if (x_arr && y_arr) {
				g_VortexX = sx;  g_VortexY = sy;
				g_VortexState = VORTEX_AT_SLOT;
			}
			VDP_SetSpriteSM1(VORTEX_SPRITE_ID, (u8)g_VortexX, (u8)g_VortexY, vpattern, COLOR_WHITE);

		} else if (g_VortexState == VORTEX_AT_SLOT) {
			VDP_SetSpriteSM1(VORTEX_SPRITE_ID, (u8)g_VortexX, (u8)g_VortexY, vpattern, COLOR_WHITE);
			// Player touches vortex → becomes weapon, flies back to boss
			if (rectCollide(g_PlayerPawn.PositionX,      g_PlayerPawn.PositionY,
			                g_PlayerPawn.PositionX + 15, g_PlayerPawn.PositionY + 15,
			                (u8)g_VortexX, (u8)g_VortexY, (u8)g_VortexX + 15, (u8)g_VortexY + 15)) {
				u8 bx = g_BossDstX * 8 + BOSS_HEAD_OFFSET_X + 8;
				u8 by = (BOSS_HEAD_Y1 + BOSS_HEAD_Y2) / 2;
				AimProjectile(sx, sy, bx, by, BOSS_VORTEX_SPEED, &g_VortexDX, &g_VortexDY);
				g_VortexX = sx;  g_VortexY = sy;
				g_VortexState = VORTEX_TO_BOSS;
			}

		} else {  // VORTEX_TO_BOSS
			g_VortexX += g_VortexDX;
			g_VortexY += g_VortexDY;
			u8 bx = g_BossDstX * 8 + BOSS_HEAD_OFFSET_X + 8;
			u8 by = (BOSS_HEAD_Y1 + BOSS_HEAD_Y2) / 2;
			// Arrival: each axis has reached or passed its target
			u8 x_arr = (g_VortexDX == 0) || (g_VortexDX > 0 ? (g_VortexX >= (i16)bx) : (g_VortexX <= (i16)bx));
			u8 y_arr = (g_VortexDY == 0) || (g_VortexDY > 0 ? (g_VortexY >= (i16)by) : (g_VortexY <= (i16)by));
			if (x_arr && y_arr) {
				// Vortex hits boss: boss takes damage and resumes patrol
				g_VortexState = VORTEX_INACTIVE;
				VDP_HideSprite(VORTEX_SPRITE_ID);
				FxPlay(FX_STOMP_ROBOT);  // placeholder: swap for dedicated vortex SFX
				g_BossScreen = 0;
				g_BossFrame  = 0;
				DrawBossStance();
			} else {
				VDP_SetSpriteSM1(VORTEX_SPRITE_ID, (u8)g_VortexX, (u8)g_VortexY, vpattern, COLOR_BLACK);
			}
		}
	}

	return TRUE;
}
