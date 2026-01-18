//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/state.h"
#include "game/pawn.h"
#include "debug.h"

#include "PawnData.h"
#include "level_defs.h"
#include "sprite_defs.h"
#include "math_utils.h"
#include "snapshot.h"

//=============================================================================
// DEFINES
//=============================================================================

// Function prototypes
bool State_Initialize();
bool State_Game();
bool State_Death();
bool State_Rewind();
bool State_ChangeLevel();
bool State_Intermission();
bool State_GameOver();

void tick();

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Player sprite layers
const Pawn_Sprite g_PlayerLayers[] =
{//   X  Y  DataOffset    Color            Flag
	{ 0, 0, 0,            COLOR_BLACK,     0 },
	{ 0, 0, laySize,      COLOR_LIGHT_RED, 0 },
};

// Player sprite layers in rewind mode
const Pawn_Sprite g_PlayerRewindLayers[] =
{//   X  Y  DataOffset    Color            Flag
	{ 0, 0, 0,            COLOR_WHITE,     0 },
	{ 0, 0, laySize,      COLOR_GRAY,      0 },
};

// Idle animation frames
const Pawn_Frame g_PlayerFramesIdle[] =
{//   Pattern           Time  Function
	{ PLAYER_FRAME(0),	250,  NULL },
	{ PLAYER_FRAME(9),   10,  NULL },
	{ PLAYER_FRAME(10),  10,  NULL },
	{ PLAYER_FRAME(9),   10,  NULL },
	{ PLAYER_FRAME(10),  10,  NULL },
	{ PLAYER_FRAME(9),   10,  NULL },
	{ PLAYER_FRAME(10),  10,  NULL },
	{ PLAYER_FRAME(9),   10,  NULL },
};

// Move animation frames
const Pawn_Frame g_PlayerFramesMoveRight[] =
{
	{ PLAYER_FRAME(1),	6,	NULL },
	{ PLAYER_FRAME(2),	6,	NULL },
	{ PLAYER_FRAME(3),	6,	NULL },
};

const Pawn_Frame g_PlayerFramesMoveLeft[] =
{
	{ PLAYER_FRAME(4),	6,	NULL },
	{ PLAYER_FRAME(5),	6,	NULL },
	{ PLAYER_FRAME(6),	6,	NULL },
};

// Jump animation frames
const Pawn_Frame g_PlayerFramesJumpRight[] =
{
	{ PLAYER_FRAME(3),	4,	NULL },
};

const Pawn_Frame g_PlayerFramesJumpLeft[] =
{
	{ PLAYER_FRAME(6),	4,	NULL },
};

// Fall animation frames
const Pawn_Frame g_PlayerFramesFallRight[] =
{
	{ PLAYER_FRAME(1),	4,	NULL },
};

const Pawn_Frame g_PlayerFramesFallLeft[] =
{
	{ PLAYER_FRAME(4),	4,	NULL },
};

const Pawn_Frame g_PlayerFramesDeath[] =
{
	{ PLAYER_FRAME(7),	10,	NULL },
	{ PLAYER_FRAME(8),	10,	NULL },
};

// List of all player actions
const Pawn_Action g_AnimActions[] =
{//   Frames                   Number                             Loop? Interrupt?
	{ g_PlayerFramesIdle,      numberof(g_PlayerFramesIdle),      TRUE, TRUE },
	{ g_PlayerFramesMoveRight, numberof(g_PlayerFramesMoveRight), TRUE, TRUE },
	{ g_PlayerFramesMoveLeft,  numberof(g_PlayerFramesMoveRight), TRUE, TRUE },
	{ g_PlayerFramesJumpRight, numberof(g_PlayerFramesJumpRight), TRUE, TRUE },
	{ g_PlayerFramesJumpLeft,  numberof(g_PlayerFramesJumpLeft),  TRUE, TRUE },
	{ g_PlayerFramesFallRight, numberof(g_PlayerFramesFallRight), TRUE, TRUE },
	{ g_PlayerFramesFallLeft,  numberof(g_PlayerFramesFallLeft),  TRUE, TRUE },
	{ g_PlayerFramesDeath,     numberof(g_PlayerFramesDeath),     TRUE, TRUE },
};

/**
 * @brief Switch dei segmenti nel banco 1
 *
 * @param u8 segment, imposta il numero del segmento da assegnare. Se il valore è 0, viene ripristinato il segmento precedente
 */

i8 g_PreviousSegment = -1;

void SetActiveSegment(u8 segment) {
	if(segment == 0) {
		SET_BANK_SEGMENT(1, g_PreviousSegment);
		g_PreviousSegment = -1;
	} else {
		if (g_PreviousSegment != -1) {
			DEBUG_PRINT("ERR %i\n", g_PreviousSegment);
		}
		g_PreviousSegment = GET_BANK_SEGMENT(1);
		SET_BANK_SEGMENT(1, segment);
	}
}

//=============================================================================
// LEVELS
//=============================================================================

#include "content/levels/1_0_corridor_25_lvl.h"
#include "content/levels/1_1_intro_04_lvl.h"
#include "content/levels/1_2_ronin_16_lvl.h"
#include "content/levels/1_3_theclimb_34_lvl.h"
#include "content/levels/1_4_thefactory_07_lvl.h"
#include "content/levels/1_5_theprison_06_lvl.h"
#include "content/levels/1_6_suicidepit_19_lvl.h"
#include "content/levels/2_0_corridor_26_lvl.h"
#include "content/levels/2_1_thetower_09_lvl.h"
#include "content/levels/2_2_thechasm_08_lvl.h"
#include "content/levels/2_3_sf_17_lvl.h"
#include "content/levels/2_4_whatliesbeneath_22_lvl.h"
#include "content/levels/2_5_pipesandrobots_23_lvl.h"
#include "content/levels/2_6_crystalchem_31_lvl.h"
#include "content/levels/3_0_corridor_27_lvl.h"
#include "content/levels/3_1_punisher_18_lvl.h"
#include "content/levels/3_2_thefortress_11_lvl.h"
#include "content/levels/3_3_level_33_lvl.h"
#include "content/levels/3_4_theabyss_10_lvl.h"
#include "content/levels/3_5_level_35_lvl.h"
#include "content/levels/3_6_challenge_20_lvl.h"
#include "content/levels/4_0_corridor_28_lvl.h"
#include "content/levels/4_1_easypeasy_30_lvl.h"
#include "content/levels/4_2_thesummit_12_lvl.h"
#include "content/levels/4_3_darkdescent_24_lvl.h"
#include "content/levels/4_4_twister_29_lvl.h"
#include "content/levels/4_5_fielddescent_21_lvl.h"
#include "content/levels/4_6_level_32_lvl.h"
#include "content/levels/4_7_theescape_13_lvl.h"

u8 g_NumLevels = 29;
struct Level g_Levels[29];

void SetSegmentForLevel(u8 lvlidx) {
	if (lvlidx < 14) {
		SetActiveSegment(1);
	} else {
		SetActiveSegment(2);
	}
}

void InitLevels() {
	u8 i = 0;
	g_Levels[i++] = level_corridor1;
	g_Levels[i++] = level_intro;
	g_Levels[i++] = level_ronin;
	g_Levels[i++] = level_theclimb;
	g_Levels[i++] = level_thefactory;
	g_Levels[i++] = level_theprison;
	g_Levels[i++] = level_suicidepit;
	g_Levels[i++] = level_corridor2;
	g_Levels[i++] = level_thetower;
	g_Levels[i++] = level_thechasm;
	g_Levels[i++] = level_sf;
	g_Levels[i++] = level_whatliesbeneath;
	g_Levels[i++] = level_pipesandrobots;
	g_Levels[i++] = level_crystalchem;
	g_Levels[i++] = level_corridor3;
	g_Levels[i++] = level_punisher;
	g_Levels[i++] = level_thefortress;
	g_Levels[i++] = level_level33;
	g_Levels[i++] = level_theabyss;
	g_Levels[i++] = level_level35;
	g_Levels[i++] = level_challenge;
	g_Levels[i++] = level_corridor4;
	g_Levels[i++] = level_easypeasy;
	g_Levels[i++] = level_thesummit;
	g_Levels[i++] = level_darkdescent;
	g_Levels[i++] = level_twister;
	g_Levels[i++] = level_fielddescent;
	g_Levels[i++] = level_level32;
	g_Levels[i++] = level_theescape;
}

//=============================================================================
// SEGMENT 1, BANK 1
//=============================================================================

//=============================================================================
// SEGMENT 2, BANK 1
//=============================================================================

//=============================================================================
// SEGMENT 3, BANK 1
//=============================================================================
extern const unsigned char g_DataSprtLayer[];
extern const unsigned char g_DataMapGM2_Patterns[];
extern const unsigned char g_DataMapGM2_Colors[];
extern void LoadPatternAndColor();
extern void SetVRAMTable();
extern void InitializeSprite();

//=============================================================================
// SEGMENT 4, BANK 1
//=============================================================================
extern const u8 g_Intermission[];
extern unsigned char g_chronorunner[];
extern unsigned char g_gameover[];

extern const unsigned char* g_SongData[2];

extern void SoundInit();
extern void SoundPlay();
extern void SoundSetSong(u8 songId);
extern void SoundStop();
extern void SoundLoop(bool enable);
extern void SoundMute(u8 chan, bool bMute);
extern void SoundUpdate();

//=============================================================================
// SEGMENT 7, BANK 1
//=============================================================================
extern void PrintGFXText(const c8 *text, u8 x, u8 y);
extern void PrintGFXNumber(u8 number, u8 x, u8 y);
extern void PrintTime();

extern void DrawRewindGauge();
extern void DrawKey(struct Level *lvl);
extern void DrawCrystal(struct Level *lvl);
extern void DrawPlatforms(struct Level *lvl, bool rewind);
extern void DrawMines(struct Level *lvl);
extern void DrawEnemies(struct Level *lvl, bool rewind);
extern void DrawEnergyFields(struct Level *lvl, bool rewind);

//=============================================================================
// MEMORY DATA
//=============================================================================

// Tempo rimanente. Separa le componenti in modo da non dover
// effettuare divisioni per trovarle.
u8 g_RemainingMinutes;
u8 g_RemainingSeconds;
u8 g_RemainingFS;

// Contatore per l'animazione dei minuti persi dopo la morte
u8 g_CountDownTicks;

// Controlla lo stato dell'intermission
u8 g_IntermissionState = 0;

// Contatore per lo stato di game over (50 fps * 5 secondi = 250)
u8 g_GameOverCounter = 0;

Pawn g_PlayerPawn;
u8	 g_PlayerAction;
bool g_PlayerMovingRight = FALSE;
bool g_PlayerMovingLeft = FALSE;
bool g_PlayerJumping = FALSE;
bool g_PlayerDying = FALSE;
bool g_PlayerInputRight = FALSE;
bool g_PlayerInputLeft = FALSE;
bool g_PlayerInputUp = FALSE;
i8   g_VelocityY;
i8   g_mDX = 0;
i8	 g_mDY = 0;
i8   g_DX;
i8   g_DY;
bool g_PlayerHasKey = FALSE;
u8 g_PlayerMaxRewindEnergy = 0;
u8 g_PlayerRewindEnergy = 0;

// Key sprite
bool g_KeyEnabled;
u8 g_KeyPosX;
u8 g_KeyPosY;
u8 g_KeyAnimFrame;

// Crystal sprite
bool g_CrystalEnabled;
u8 g_CrystalPosX;
u8 g_CrystalPosY;
u8 g_CrystalAnimFrame;

// Allocazione sprite
u8 g_PlatformSpritesBaseID;
u8 g_MineSpritesBaseID;
u8 g_EnemySpritesBaseID;
u8 g_EnemyAnimCounter;
u8 g_EnergyFieldSpritesBaseID;
u8 g_EnergyFieldAnimCounter;

//=============================================================================
// LEVELS
//=============================================================================

u8 g_CurrentLevel;
u8 g_NextLevel;

//=============================================================================
// PHYSICS
//=============================================================================

// Physics callback
void PlayerPhysicsEvent(u8 event, u8 tile)
{
	tile;
	switch (event)
	{
	case PAWN_PHYSICS_COL_DOWN: // Handle downward collision
		g_PlayerJumping = FALSE;
		break;

	case PAWN_PHYSICS_COL_UP: // Handle upward collisions
		g_VelocityY = 0;
		break;

	case PAWN_PHYSICS_FALL: // Handle falling
		g_PlayerJumping = TRUE;
		break;
	};
}

// Collision callback
bool PlayerPhysicsCollision(u8 tile)
{
	return tile >= 200;
}

//=============================================================================
// UTILITIES
//=============================================================================

void ReinitPlayer(Pawn *pawn, Pawn_Sprite *spr_layers, u8 n_spr_layers, u8 x, u8 y) {
	Pawn_Initialize(pawn, spr_layers, n_spr_layers, PLAYER_SPRITE_ID, g_AnimActions);
	Pawn_InitializePhysics(pawn, PlayerPhysicsEvent, PlayerPhysicsCollision, 16, 16);
	Pawn_SetPosition(pawn, x, y);
}

// Verifica collisione tra due rettangoli definiti da:
// (x1, y1) -> Angolo in alto a sinistra
// (x2, y2) -> Angolo in basso a destra
bool rectCollide(u8 ax1, u8 ay1, u8 ax2, u8 ay2,
				 u8 bx1, u8 by1, u8 bx2, u8 by2) {

	// 1. Il rettangolo A è completamente a sinistra di B?
	if (ax2 < bx1) return FALSE;

	// 2. Il rettangolo A è completamente a destra di B?
	if (ax1 > bx2) return FALSE;

	// 3. Il rettangolo A è completamente sopra B?
	if (ay2 < by1) return FALSE;

	// 4. Il rettangolo A è completamente sotto B?
	if (ay1 > by2) return FALSE;

	// Se nessuna delle condizioni sopra è vera, i rettangoli si intersecano
	return TRUE;
}

// Semplice rilevazione di collisioni a bounding box. Assume oggetti 16x16
bool bboxCollide(u8 x1, u8 y1, u8 x2, u8 y2) {
	return rectCollide(x1, y1, x1 + 15, y1 + 15,
	                   x2, y2, x2 + 15, y2 + 15);
}

void TakeKey() {
	g_PlayerHasKey = TRUE;

	u8 door_x = g_Levels[g_CurrentLevel].end_x;
	u8 door_y = g_Levels[g_CurrentLevel].end_y;

	// Luce verde sopra l'uscita
	VDP_Poke_GM2(door_x, door_y - 1, 48);
	VDP_Poke_GM2(door_x + 1, door_y - 1, 49);

	// Porta si apre
	VDP_Poke_GM2(door_x, door_y, 44);
	VDP_Poke_GM2(door_x + 1, door_y, 44);
	VDP_Poke_GM2(door_x, door_y + 1, 44);
	VDP_Poke_GM2(door_x + 1, door_y + 1, 44);
}

void TakeCrystal() {
	g_PlayerMaxRewindEnergy += 25;
}

bool isPlayerAtExit() {
	if (!g_PlayerHasKey)
		return FALSE;

	u8 door_x = g_Levels[g_CurrentLevel].end_x;
	u8 door_y = g_Levels[g_CurrentLevel].end_y;

	return bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, door_x * 8, door_y * 8);
}

bool isPlayerOnSpikes() {
    for (i8 t = -1; t < 2; t++) {
        u8 tile = VDP_Peek_GM2((g_PlayerPawn.PositionX >> 3) + t,
                               (g_PlayerPawn.PositionY >> 3) + 2);
        if (tile >= 192 && tile <= 199) {
            return TRUE;
        }
    }
    return FALSE;
}

bool isPlayerOnMines(struct Level *lvl) {

	u8 nm = lvl->num_mines;
	struct Mine *mines = lvl->mines;

	for (u8 m=0; m < nm; m++) {
		// Considera solo mine attive
		if (!mines[m].enabled)
			continue;

		// Confronta un bbox parziale (12x16) del giocatore con un bbox 2x1 della mina
		if (rectCollide(g_PlayerPawn.PositionX +  2, g_PlayerPawn.PositionY,
					    g_PlayerPawn.PositionX + 13, g_PlayerPawn.PositionY + 15,
				        mines[m].pos_x + 7, mines[m].pos_y - 1,
				        mines[m].pos_x + 7 + 1, mines[m].pos_y)) {

			// Disabilita la mina
			mines[m].enabled = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

bool isPlayerHitByEnemies(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	for (u8 e=0; e < lvl->num_enemies; e++) {
		if (enemies[e].stunned_timer > 0)
			continue;

		if (bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY,
		                enemies[e].pos_x, enemies[e].pos_y)) {

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

				return FALSE;
			} else {
				return TRUE;
			}
		}
	}
	return FALSE;
}

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

bool isPlayerInPit() {
	return g_PlayerPawn.PositionY > 192;
}

//=============================================================================
// FUNCTION
//=============================================================================

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

void UpdateEnemies(struct Level *lvl) {
	struct Enemy *enemies = lvl->enemies;

	for (u8 e=0; e < lvl->num_enemies; e++) {
		struct Enemy *enemy = &enemies[e];

		// Muovi i proiettili indipendentemente dallo stato del nemico
		if (enemy->field_state == 2) {

			enemy->field_mDX += 12 * enemy->dir_x;
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
					enemy->field_state = 1;
					enemy->field_timer = 100;

					// Il campo di forza appare davanti al nemico
					enemy->field_x = enemy->pos_x + (enemy->dir_x * 16);
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
					enemy->field_state = 2;

					enemy->field_x = enemy->pos_x + (enemy->dir_x * 16);
					enemy->field_y = enemy->pos_y;
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

void AllocateSpriteIDs(struct Level *lvl) {

	// Gli sprite ID allocati fissi sono:
	// 0-1: Player (2 layers)
	// 2: Key
	// 3: Crystal
	// 4+: Platforms, Mines, Enemies, Energy Fields

	g_PlatformSpritesBaseID = 4;

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


//=============================================================================
// STATES
//=============================================================================

bool State_Initialize()
{
	// Switch Segment 3
	SetActiveSegment(3);
	SetVRAMTable();			// RAM Tables Address and Setup video
	LoadPatternAndColor();  // Load Pattern and color
	InitializeSprite();	    // Initialize sprite
	SetActiveSegment(0);

	InitLevels();

	SetActiveSegment(4);
	g_SongData[0] = g_chronorunner;
	g_SongData[1] = g_gameover;
	SoundStop();
	SoundSetSong(0);
	SoundLoop(TRUE);
	SoundPlay();
	SetActiveSegment(0);

	// Imposta tempo iniziale
	g_RemainingMinutes = 60;
	g_RemainingSeconds = 0;
	g_RemainingFS = 0;

	// Reset livelli
	g_CurrentLevel = 0;
	g_NextLevel = 0;

	Game_SetState(State_Intermission);

	return TRUE;
}

void PlayerRestart()
{
	u8 start_x = g_Levels[g_CurrentLevel].start_x;
	u8 start_y = g_Levels[g_CurrentLevel].start_y;

	// Init player pawn
	ReinitPlayer(&g_PlayerPawn,
		         g_PlayerLayers, numberof(g_PlayerLayers),
				 start_x * 8, start_y * 8);

	g_PlayerDying = FALSE;
	g_CountDownTicks = 50 - 1;

	// Resetta il contatore dei rewind, altrimenti il giocatore può riusare
	// dopo la morte o dopo un livello delle coordinate spurie
	g_PlayerRewindEnergy = 0;

	// Initialize snapshot system for player and object rewind
	Snapshot_Initialize();
}

bool State_Intermission()
{
	if (g_IntermissionState == 0) {

		// Nasconde tutti gli sprite, verranno riabilitati quelli che servono
		// in ciascun livello
		VDP_HideAllSprites();

		SetActiveSegment(4);
		VDP_WriteLayout_GM2(g_Intermission, 0, 0, 32, 24);
		SetActiveSegment(0);

		// Prende il nome del prossimo livello
		const c8 *level_name = g_Levels[g_NextLevel].name;

		// Progresso
		u16 prog = (g_NextLevel * 30) / g_NumLevels;

		// Barra verde
		if (prog != 0)
			VDP_FillLayout_GM2(49, 2, 16, prog, 1);

		// Barra rossa
		VDP_FillLayout_GM2(51, 2 + prog, 16, (30 - prog), 1);

		// Posizione
		VDP_Poke_GM2(2 + prog, 16, 48);

		SetActiveSegment(7);

		PrintGFXText("ROOM", 5, 18);
		PrintGFXNumber(g_NextLevel + 1, 10, 18);

		// Tempo rimanente
		PrintGFXText("TIME", 18, 18);
		PrintGFXNumber(g_RemainingMinutes, 23, 18);
		PrintGFXText("'", 25, 18);
		PrintGFXNumber(g_RemainingSeconds, 26, 18);
		PrintGFXText("\"", 28, 18);

		// Nome del livello (centrato)
		PrintGFXText(level_name, 16 - (String_Length(level_name) / 2), 20);

		PrintGFXText("PRESS SPACE KEY", 8, 23);

		SetActiveSegment(0);

		g_IntermissionState = 1;
	}

	u8 row8 = Keyboard_Read(8);
	if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
		g_IntermissionState = 0;
		Game_SetState(State_ChangeLevel);
		return FALSE;
	}

	return TRUE;
}

bool State_ChangeLevel()
{
	// Passa al livello successivo
	g_CurrentLevel = g_NextLevel;

	// Se i livelli sono finiti, al giro successivo ricomincia da capo XXX: sistemare
	if (g_NextLevel == g_NumLevels - 1) {
		g_NextLevel = 0;
	} else {
		g_NextLevel += 1;
	}

	// Recupera livello corrente per passarlo esplicitamente alle funzioni che
	// ne hanno bisogno
	struct Level *lvl = &g_Levels[g_CurrentLevel];

	g_KeyPosX = lvl->key_x * 8;
	g_KeyPosY = lvl->key_y * 8;
	g_KeyAnimFrame = 0;

	g_KeyEnabled = TRUE;

	g_CrystalPosX = lvl->crystal_x * 8;
	g_CrystalPosY = lvl->crystal_y * 8;
	g_CrystalAnimFrame = 0;

	if (g_CrystalPosX == 0 && g_CrystalPosY == 0)
		g_CrystalEnabled = FALSE;
	else
		g_CrystalEnabled = TRUE;

	// Cancella lo schermo
	VDP_FillScreen_GM2(44);

	SetSegmentForLevel(g_CurrentLevel);
	VDP_WriteLayout_GM2(lvl->layout, 0, 2, 32, 24);
	SetActiveSegment(0);

	AllocateSpriteIDs(lvl);

	SetActiveSegment(7);
	PrintGFXText("TIME   '  \"", 2, 0);
	PrintTime();
	SetActiveSegment(0);

	g_PlayerHasKey = FALSE;
	PlayerRestart();

	Game_SetState(State_Game);

	return TRUE;
}

bool State_Game()
{
	// Recupera livello corrente per passarlo esplicitamente alle funzioni che
	// ne hanno bisogno
	struct Level *lvl = &g_Levels[g_CurrentLevel];

	// Gestione input
	UpdatePlayerInput();

	// Aggiorna piattaforme
	UpdatePlatforms(lvl);

	// Aggiorna nemici
	UpdateEnemies(lvl);

	// Controlla se il giocatore è atterrato su una piattaforma mobile
	struct Platform *platform = isPlayerOnPlatform(lvl);

	UpdatePlayerMovement(platform);

	UpdatePlayerAction();

	// Testi a video
	if (g_RemainingFS == 0) {
		SetActiveSegment(7);
		PrintTime();
		SetActiveSegment(0);
	}

	// Controlla se il tempo è esaurito
	if (g_RemainingMinutes == 0 && g_RemainingSeconds == 0) {
		Game_SetState(State_Death);
		return TRUE;
	}

	Pawn_SetAction(&g_PlayerPawn, g_PlayerAction);
	Pawn_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	Pawn_Update(&g_PlayerPawn);
	Pawn_Draw(&g_PlayerPawn);

	DrawKey(lvl);
	DrawCrystal(lvl);
	DrawPlatforms(lvl, FALSE);
	DrawMines(lvl);
	DrawEnemies(lvl, FALSE);
	DrawEnergyFields(lvl, FALSE);

	// Controlla la collisione tra giocatore e chiave
	if (g_KeyEnabled && bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, g_KeyPosX, g_KeyPosY)) {
		TakeKey();
		g_KeyEnabled = FALSE;
		VDP_HideSprite(KEY_SPRITE_ID);
	}

	// Controlla la collisione tra giocatore e cristallo
	if (g_CrystalEnabled && bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, g_CrystalPosX, g_CrystalPosY)) {
		TakeCrystal();
		g_CrystalEnabled = FALSE;
		VDP_HideSprite(CRYSTAL_SPRITE_ID);
	}

    // Controlla la collisione tra giocatore e spine / mine / nemici / energy fields / baratro
    if (isPlayerOnSpikes() || isPlayerOnMines(lvl) || isPlayerHitByEnemies(lvl)
     || isPlayerHitByEnergyFields(lvl) || isPlayerInPit()) {
        // Effetto "morte" del giocatore.
        Game_SetState(State_Death);
        return TRUE;
    }

	// Controlla se il giocatore ha raggiunto l'uscita
	if (isPlayerAtExit() || Keyboard_IsKeyPressed(KEY_F1)) {
		Game_SetState(State_Intermission);
		return TRUE;
	}

	// Capture snapshot of all game objects and player state
	Snapshot_Capture(lvl, g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, g_PlayerPawn.AnimFrame);

	u8 row8 = Keyboard_Read(8);
	if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
		if (g_PlayerRewindEnergy > 1) {

			// Sostituisce i colori dello sprite principale
			ReinitPlayer(&g_PlayerPawn,
							g_PlayerRewindLayers, numberof(g_PlayerRewindLayers),
							g_PlayerPawn.PositionX, g_PlayerPawn.PositionY);

			// Entra in modalità rewind
			Game_SetState(State_Rewind);
			return TRUE;
		}
	} else {
		if (g_PlayerRewindEnergy < g_PlayerMaxRewindEnergy) {
			g_PlayerRewindEnergy++;
		}
	}

	SetActiveSegment(7);
	DrawRewindGauge();
	SetActiveSegment(0);

	return TRUE;
}

bool State_Death()
{
    // Appena entra imposta lo stato dying e inizia il salto
    if (!g_PlayerDying) {
        g_mDX = 0;
        g_mDY = 0;
        g_VelocityY = FORCE;
        g_PlayerDying = TRUE;
    }

	// Ancora il personaggio non è uscito dallo schermo
	if (g_PlayerPawn.PositionY < 192) {
		UpdatePlayerGravity();
		g_DY = GetDPos(&g_mDY);

		Pawn_SetAction(&g_PlayerPawn, ACTION_DEATH);
		Pawn_SetPosition(&g_PlayerPawn,
						  g_PlayerPawn.PositionX,
						  g_PlayerPawn.PositionY + g_DY);
		Pawn_Update(&g_PlayerPawn);

		Pawn_Draw(&g_PlayerPawn);
	}

	if (g_PlayerPawn.PositionY >= 192) {
		// Ogni morte costa al giocatore 5 minuti sul tempo totale

		// Scala 6 secondi per fotogramma dal contatore, per fare un'animazione
		// che spieghi visivamente cosa sta succedendo
		for (u16 t=0; t < 300; t++) {
			tick();
		}
		PrintTime();

		if (g_CountDownTicks > 0) {
			g_CountDownTicks--;
		} else {
			// Se il tempo è esaurito, vai a Game Over
			if (g_RemainingMinutes == 0 && g_RemainingSeconds == 0) {
				Game_SetState(State_GameOver);
			} else {
				PlayerRestart();
				Game_SetState(State_Game);
			}
		}
	}
	return TRUE;
}

bool State_GameOver()
{
	// Prima volta in questo stato: prepara lo schermo
	if (g_GameOverCounter == 0) {

		SetActiveSegment(4);
		SoundStop();
		SoundSetSong(1);
		SoundLoop(FALSE);
		SoundPlay();
		SetActiveSegment(0);

		// Nasconde tutti gli sprite
		VDP_HideAllSprites();

		// Cancella lo schermo con tile vuoto
		VDP_FillScreen_GM2(44);

		// Stampa "GAME OVER" centrato (10 caratteri = 5 a sinistra del centro)
		PrintGFXText("GAME OVER", 11, 12);
	}

	g_GameOverCounter++;

	// Dopo 5 secondi (250 frame a 50 fps)
	// XXX: taglia un po' di traccia g_gameover
	if (g_GameOverCounter >= 250) {
		g_GameOverCounter = 0;
		Game_SetState(State_Initialize);
	}

	return TRUE;
}

bool State_Rewind()
{
	// Usciamo dal rewind in due casi: se non abbiamo più elementi, o se il
	// giocatore lo ha interrotto

	u8 row8 = Keyboard_Read(8);
	if (g_PlayerRewindEnergy == 0 || IS_KEY_RELEASED(row8, KEY_SPACE)) {

		// Reimposta colori originali
		ReinitPlayer(&g_PlayerPawn,
			         g_PlayerLayers, numberof(g_PlayerLayers),
					 g_PlayerPawn.PositionX, g_PlayerPawn.PositionY);

		Game_SetState(State_Game);
		return TRUE;
	}

	// Ogni passo di rewind consuma un elemento del buffer snapshot
	u8 snapshot_idx = Snapshot_RewindStep();

	// e anche l'energia di rewind del giocatore
	g_PlayerRewindEnergy--;

	SetActiveSegment(7);
	DrawRewindGauge();
	SetActiveSegment(0);

	// Restore all game objects to their state at this frame
	struct Level *lvl = &g_Levels[g_CurrentLevel];

	u8 px, py, pf;
	Snapshot_Restore(lvl, snapshot_idx, &px, &py, &pf);

	// Aggiorna la posizione e il fotogramma. Per forzare quest'ultimo,
	// imposta a mano il flag di aggiornamento pattern.
	Pawn_SetPosition(&g_PlayerPawn, px, py);
	g_PlayerPawn.Update |= PAWN_UPDATE_PATTERN;
	g_PlayerPawn.AnimFrame = pf;
	Pawn_Draw(&g_PlayerPawn);

	// Redraw all objects at their rewound positions in white
	DrawPlatforms(lvl, TRUE);
	DrawEnemies(lvl, TRUE);
	DrawEnergyFields(lvl, TRUE);

	return TRUE;
}

//=============================================================================
// MAIN LOOP
//=============================================================================

void tick() {
	if (g_RemainingFS == 0) {
		if (g_RemainingSeconds == 0) {
			if (g_RemainingMinutes == 0) {
				// Tempo esaurito
				return;
			} else {
				g_RemainingMinutes--;
			}
			g_RemainingSeconds = 59;
		} else {
			g_RemainingSeconds--;
		}
		g_RemainingFS = 49;
	} else {
		g_RemainingFS--;
	}
}

void InterruptHook() {
	SetActiveSegment(4);
	SoundUpdate();
	SetActiveSegment(0);

    if (Game_GetCurrentState() != State_Game)
        return;

    tick();
}

void main()
{
	DEBUG_INIT();

	SetActiveSegment(4);
	SoundInit();
	SetActiveSegment(0);

	Game_SetState(State_Initialize);
	Game_SetVSyncCallback(InterruptHook);
	Game_Start(VDP_MODE_GRAPHIC2, FALSE);
}
