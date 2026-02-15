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
#include "cutscene.h"
#include "fx_sounds.h"

//=============================================================================
// DEFINES
//=============================================================================

#define TILE_EMPTY 47

// Function prototypes
bool State_Initialize();
bool State_Menu();
bool State_Game();
bool State_Death();
bool State_Rewind();
bool State_ChangeLevel();
bool State_Intermission();
bool State_MessageScreen();
bool State_Cutscene();  // Cutscene system state

void tick();

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Player sprite layers
const Pawn_Sprite g_PlayerLayers[] =
{//   X  Y  DataOffset    Color               Flag
	{ 0, 0, 0,            COLOR_BLACK,        0 },
	{ 0, 0, laySize,      COLOR_LIGHT_YELLOW, 0 },
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
 * @brief Switch dei segmenti nel banco 1. In uscita reimposta il 7 che è il default.
 *
 * @param u8 seg numero del segmento da assegnare al banco 1.
 */

u8 g_seg_guard = 1;

#define WITH_SEGMENT(seg) \
    for (SET_BANK_SEGMENT(1, seg), g_seg_guard = 0; \
         !g_seg_guard; \
         g_seg_guard = 1, SET_BANK_SEGMENT(1, 7))


//=============================================================================
// LEVELS
//=============================================================================

// Normal levels
#include "content/levels/map25_lvl.h"
#include "content/levels/map04_lvl.h"
#include "content/levels/map16_lvl.h"
#include "content/levels/map34_lvl.h"
#include "content/levels/map07_lvl.h"
#include "content/levels/map06_lvl.h"
#include "content/levels/map19_lvl.h"
#include "content/levels/map26_lvl.h"
#include "content/levels/map09_lvl.h"
#include "content/levels/map08_lvl.h"
#include "content/levels/map17_lvl.h"
#include "content/levels/map22_lvl.h"
#include "content/levels/map23_lvl.h"
#include "content/levels/map31_lvl.h"
#include "content/levels/map27_lvl.h"
#include "content/levels/map18_lvl.h"
#include "content/levels/map11_lvl.h"
#include "content/levels/map33_lvl.h"
#include "content/levels/map10_lvl.h"
#include "content/levels/map35_lvl.h"
#include "content/levels/map20_lvl.h"
#include "content/levels/map28_lvl.h"
#include "content/levels/map30_lvl.h"
#include "content/levels/map12_lvl.h"
#include "content/levels/map24_lvl.h"
#include "content/levels/map29_lvl.h"
#include "content/levels/map21_lvl.h"
#include "content/levels/map32_lvl.h"
#include "content/levels/map13_lvl.h"

// Secret levels
#include "content/levels/map48_lvl.h"
#include "content/levels/map49_lvl.h"
#include "content/levels/map47_lvl.h"
#include "content/levels/map50_lvl.h"

#define NUM_LEVELS 29
#define SECRET_LEVELS 4

// Array con l'ordine dei livelli
const struct Level* g_LevelOrder[NUM_LEVELS + SECRET_LEVELS];

// Copia locale del livello attuale
struct Level g_ActiveLevel;

u8 SegmentForLevel(u8 lvlidx) {
	if (lvlidx < 14) {
		return 1;
	} else {
		return 2;
	}
}

void InitLevels() {
	u8 i = 0;

	g_LevelOrder[i++] = &level_map25;
	g_LevelOrder[i++] = &level_map4;
	g_LevelOrder[i++] = &level_map16;
	g_LevelOrder[i++] = &level_map34;
	g_LevelOrder[i++] = &level_map7;
	g_LevelOrder[i++] = &level_map6;
	g_LevelOrder[i++] = &level_map19;
	g_LevelOrder[i++] = &level_map26;
	g_LevelOrder[i++] = &level_map9;
	g_LevelOrder[i++] = &level_map8;
	g_LevelOrder[i++] = &level_map17;
	g_LevelOrder[i++] = &level_map22;
	g_LevelOrder[i++] = &level_map23;
	g_LevelOrder[i++] = &level_map31;
	g_LevelOrder[i++] = &level_map27;
	g_LevelOrder[i++] = &level_map18;
	g_LevelOrder[i++] = &level_map11;
	g_LevelOrder[i++] = &level_map33;
	g_LevelOrder[i++] = &level_map10;
	g_LevelOrder[i++] = &level_map35;
	g_LevelOrder[i++] = &level_map20;
	g_LevelOrder[i++] = &level_map28;
	g_LevelOrder[i++] = &level_map30;
	g_LevelOrder[i++] = &level_map12;
	g_LevelOrder[i++] = &level_map24;
	g_LevelOrder[i++] = &level_map29;
	g_LevelOrder[i++] = &level_map21;
	g_LevelOrder[i++] = &level_map32;
	g_LevelOrder[i++] = &level_map13;

	// Secret levels
	g_LevelOrder[i++] = &level_map48;
	g_LevelOrder[i++] = &level_map49;
	g_LevelOrder[i++] = &level_map47;
	g_LevelOrder[i++] = &level_map50;
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
extern void AllocateSpriteIDs(struct Level *lvl);

//=============================================================================
// SEGMENT 4, BANK 1
//=============================================================================
// Intermission screen
extern const u8 g_Screen15[];

extern const u8 g_chronorunner[];
extern const u8 g_gameover[];
extern const u8 g_menu[];

extern const u8* g_SongData[3];

extern void SoundInit();
extern void SoundPlay();
extern void SoundSetSong(u8 songId);
extern u8 SoundGetSong();
extern void SoundStop();
extern void SoundLoop(bool enable);
extern void SoundMute(u8 chan, bool bMute);
extern void SoundUpdate();

extern void S4_FxPlay(u8 id);

//=============================================================================
// SEGMENT 6, BANK 1
//=============================================================================

extern void ShowAmi();
extern void ShowSplashScreen();

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
extern void DrawVortex();

extern void UpdatePlayerInput();
extern void UpdatePlayerGravity();
extern void UpdatePlayerMovement(struct Platform *platform);
extern void UpdatePlayerAction();
extern void UpdatePlatforms(struct Level *lvl);
extern void UpdateEnemies(struct Level *lvl);

extern void LoadLevel(u8 levelIndex);

extern const CutCmd g_IntroCutscene[];

//=============================================================================
// TRAMPOLINE FUNCTIONS FOR OTHER SEGMENTS
//=============================================================================

void FxPlay(u8 id) {
WITH_SEGMENT(4) {
	S4_FxPlay(id);
}
}

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

// Contatore per lo stato di message screen (intro, game over, you won, etc.)
u16 g_MessageScreenCounter = 0;

// Configurazione message screen
const c8* g_MessageScreenText = NULL;
i8 g_MessageScreenSongId = 1;
u16 g_MessageScreenDuration = 500;

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
u8 g_PlayerMaxRewindEnergy;
u8 g_PlayerRewindEnergy;

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

// Vortex sprite (easter egg)
u8 g_VortexAnimFrame;

// Allocazione sprite
u8 g_PlatformSpritesBaseID;
u8 g_MineSpritesBaseID;
u8 g_EnemySpritesBaseID;
u8 g_EnemyAnimCounter;
u8 g_EnemyKeyHintCounter;
u8 g_EnergyFieldSpritesBaseID;
u8 g_EnergyFieldAnimCounter;

//=============================================================================
// LEVELS
//=============================================================================

u8 g_CurrentLevelIdx;
u8 g_NextLevelIdx = 0;
i8 g_SecretlNextLevelIdx = -1;

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
	return tile >= 203;
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

	u8 door_x = g_ActiveLevel.end_x;
	u8 door_y = g_ActiveLevel.end_y;

	// Luce verde sopra l'uscita
	VDP_Poke_GM2(door_x, door_y - 1, 48);
	VDP_Poke_GM2(door_x + 1, door_y - 1, 49);

	// Porta si apre
	VDP_FillLayout_GM2(TILE_EMPTY, door_x, door_y, 2, 2);

	FxPlay(FX_GET_KEY);
}

void TakeCrystal() {
	g_PlayerMaxRewindEnergy += (SNAPSHOT_BUFFER_SIZE / 4);
	if (g_PlayerMaxRewindEnergy > SNAPSHOT_BUFFER_SIZE)
		g_PlayerMaxRewindEnergy = SNAPSHOT_BUFFER_SIZE;
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
	if (tile >= 196 && tile <= 202) {
		return TRUE;
	}
    return FALSE;
}

bool isPlayerOnMines(struct Level *lvl) {

	u8 nm = lvl->num_mines;
	struct Mine *mines = lvl->mines;

	for (u8 m=0; m < nm; m++) {
		// Confronta un bbox parziale (8x16) del giocatore con un bbox 2x1 della mina
		if (rectCollide(g_PlayerPawn.PositionX +  4, g_PlayerPawn.PositionY,
					    g_PlayerPawn.PositionX + 11, g_PlayerPawn.PositionY + 15,
				        mines[m].pos_x + 7, mines[m].pos_y - 1,
				        mines[m].pos_x + 7 + 2, mines[m].pos_y)) {
			return TRUE;
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

void SetMessageScreen(const c8* text, i8 songId, u16 duration) {
	g_MessageScreenText = text;
	g_MessageScreenSongId = songId;
	g_MessageScreenDuration = duration;
}

//=============================================================================
// STATES
//=============================================================================

bool State_Initialize()
{
WITH_SEGMENT(3) {
	SetVRAMTable();			// RAM Tables Address and Setup video
	LoadPatternAndColor();  // Load Pattern and color
	InitializeSprite();	    // Initialize sprite
}

	InitLevels();

WITH_SEGMENT(4) {
	g_SongData[0] = g_chronorunner;
	g_SongData[1] = g_gameover;
	g_SongData[2] = g_menu;
	SoundStop();
}

	// Imposta tempo iniziale
	g_RemainingMinutes = 60;
	g_RemainingSeconds = 0;
	g_RemainingFS = 0;

	// Reset cristalli e rewind energy
	g_PlayerMaxRewindEnergy = 0;
	g_PlayerRewindEnergy = 0;

	// Reset livelli
	g_CurrentLevelIdx = 0;
	g_NextLevelIdx = 0;

	// Initialize cutscene system
	Cutscene_Initialize();

	// Start at main menu
	if (g_NextLevelIdx == 0) {
WITH_SEGMENT(4) {
		// Start menu music
		SoundSetSong(2);
		SoundLoop(TRUE);
		SoundPlay();
}
		Game_SetState(State_Menu);
	} else {
		Game_SetState(State_Intermission);
	}

	return TRUE;
}

void PlayerRestart()
{
	// Reset level state (mines, enemies, platforms) from source data
	LoadLevel(g_CurrentLevelIdx);

	// Reset key state
	g_KeyPosX = g_ActiveLevel.key_x * 8;
	g_KeyPosY = g_ActiveLevel.key_y * 8;
	g_KeyAnimFrame = 0;
	g_KeyEnabled = (g_ActiveLevel.key_trigger_enemy == -1);
	// Quando il giocatore viene ucciso dopo aver già recuperato la chiave,
	// questa va di nuovo nascosta
	if (!g_KeyEnabled) {
		VDP_HideSprite(KEY_SPRITE_ID);
	}
	g_PlayerHasKey = FALSE;

	// Reset crystal state
	g_CrystalPosX = g_ActiveLevel.crystal_x * 8;
	g_CrystalPosY = g_ActiveLevel.crystal_y * 8;
	g_CrystalAnimFrame = 0;
	if (g_CrystalPosX == 0 && g_CrystalPosY == 0)
		g_CrystalEnabled = FALSE;
	else
		g_CrystalEnabled = TRUE;

	// Reset vortex state
	g_VortexAnimFrame = 0;
	VDP_HideSprite(VORTEX_SPRITE_ID);

	// Init player pawn
	u8 start_x = g_ActiveLevel.start_x;
	u8 start_y = g_ActiveLevel.start_y;
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

	// Redraw level layout (resets door appearance if key was taken)
	u8 seg = SegmentForLevel(g_CurrentLevelIdx);
WITH_SEGMENT(seg) {
	VDP_WriteLayout_GM2(g_ActiveLevel.layout, 0, 2, 32, 24);
}
}

bool State_Intermission()
{
	if (g_IntermissionState == 0) {

		// Nasconde tutti gli sprite, verranno riabilitati quelli che servono
		// in ciascun livello
		VDP_HideAllSprites();

WITH_SEGMENT(4) {
		if (SoundGetSong() != 0) {
			SoundSetSong(0);
			SoundLoop(TRUE);
			SoundPlay();
		}
		VDP_WriteLayout_GM2(g_Screen15, 0, 0, 32, 24);
}

		// Prende il nome del prossimo livello
		const c8 *level_name = g_LevelOrder[g_NextLevelIdx]->name;

		// Progresso
		u16 prog = (g_NextLevelIdx * 30) / NUM_LEVELS;

		// Barra verde
		if (prog != 0)
			VDP_FillLayout_GM2(49, 2, 16, prog, 1);

		// Barra rossa
		VDP_FillLayout_GM2(51, 2 + prog, 16, (30 - prog), 1);

		// Posizione
		VDP_Poke_GM2(2 + prog, 16, 48);

		PrintGFXText("ROOM", 5, 18);
		PrintGFXNumber(g_NextLevelIdx + 1, 10, 18);

		// Tempo rimanente
		PrintGFXText("TIME", 18, 18);
		PrintGFXNumber(g_RemainingMinutes, 23, 18);
		PrintGFXText("'", 25, 18);
		PrintGFXNumber(g_RemainingSeconds, 26, 18);
		PrintGFXText("\"", 28, 18);

		// Nome del livello (centrato)
		PrintGFXText(level_name, 16 - (String_Length(level_name) / 2), 20);

		PrintGFXText("PRESS SPACE KEY", 8, 23);

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
	// Nasconde tutti gli sprite, verranno riabilitati quelli che servono
	// in ciascun livello
	VDP_HideAllSprites();

	if (g_SecretlNextLevelIdx != -1) {
		// Passa al livello segreto. Lascia il livello successivo
		// impostato, così ci salta automaticamente al termine del
		// livello segreto
		g_CurrentLevelIdx = g_SecretlNextLevelIdx;
		g_SecretlNextLevelIdx = -1;
	} else {
		// Passa al livello successivo
		g_CurrentLevelIdx = g_NextLevelIdx;
		g_NextLevelIdx += 1;
	}

	// Cancella lo schermo
	VDP_FillScreen_GM2(TILE_EMPTY);

	// Reset level and player state
	PlayerRestart();

WITH_SEGMENT(3) {
	AllocateSpriteIDs(&g_ActiveLevel);
}

	PrintGFXText("TIME   '  \"", 2, 0);
	PrintTime();

	Game_SetState(State_Game);

	return TRUE;
}

bool State_Game()
{
	// Recupera livello corrente per passarlo esplicitamente alle funzioni che
	// ne hanno bisogno
	struct Level *lvl = &g_ActiveLevel;

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
		PrintTime();
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
	DrawVortex();

	// Easter egg: vortex teleportation
	if (bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, 0, 0)) {
		FxPlay(FX_EXIT_DOOR);
		g_SecretlNextLevelIdx = NUM_LEVELS + 2;
		Game_SetState(State_ChangeLevel);
		return TRUE;
	}

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
		FxPlay(FX_EXIT_DOOR);

		// Se era l'ultimo livello, mostra uno schermo di vittoria e reinizializza
		if (g_CurrentLevelIdx == NUM_LEVELS - 1) {
			SetMessageScreen("YOU WON!", -1, 500);
			Game_SetState(State_MessageScreen);
		} else {
			Game_SetState(State_Intermission);
		}
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
			FxPlay(FX_REWIND);
			return TRUE;
		}
	} else {
		if (g_PlayerRewindEnergy < g_PlayerMaxRewindEnergy) {
			g_PlayerRewindEnergy++;
		}
	}

	DrawRewindGauge();

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

		FxPlay(FX_DEATH);
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
				SetMessageScreen("GAME OVER", 1, 500);
				Game_SetState(State_MessageScreen);
			} else {
				PlayerRestart();
				Game_SetState(State_Game);
			}
		}
	}
	return TRUE;
}

bool State_MessageScreen()
{
	// Prima volta in questo stato: prepara lo schermo
	if (g_MessageScreenCounter == 0) {

WITH_SEGMENT(4) {
		SoundStop();
		if (g_MessageScreenSongId != -1) {
			SoundSetSong(g_MessageScreenSongId);
			SoundLoop(FALSE);
			SoundPlay();
		}
}

		// Nasconde tutti gli sprite
		VDP_HideAllSprites();

		// Cancella lo schermo con tile vuoto
		VDP_FillScreen_GM2(TILE_EMPTY);

		// Stampa il testo centrato
		u8 textLen = String_Length(g_MessageScreenText);
		PrintGFXText(g_MessageScreenText, 16 - (textLen / 2), 12);
	}

	g_MessageScreenCounter++;

	if (g_MessageScreenCounter >= g_MessageScreenDuration) {
		g_MessageScreenCounter = 0;
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

	DrawRewindGauge();

	// Imposta lo stato del livello a questo passo di rewind
	struct Level *lvl = &g_ActiveLevel;
	u8 px, py, pf;
	Snapshot_Restore(lvl, snapshot_idx, &px, &py, &pf);

	// Aggiorna la posizione e il fotogramma. Per forzare quest'ultimo,
	// imposta a mano il flag di aggiornamento pattern.
	Pawn_SetPosition(&g_PlayerPawn, px, py);
	g_PlayerPawn.Update |= PAWN_UPDATE_PATTERN;
	g_PlayerPawn.AnimFrame = pf;
	Pawn_Draw(&g_PlayerPawn);

	// Disegna oggetti in bianco per evidenziare che sono soggetti a rewind
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
	// Qua non può usare il context manager WITH_SEGMENT perché l'interruzione
	// potrebbe arrivare mentre il banco 1 è già puntato su un segmento
	// diverso dal 7 che è il default. Quindi semplicemente salva il segmento
	// corrente, aggiorna il PSG e lo reimposta subito dopo.
	u8 prevSeg = GET_BANK_SEGMENT(1);
	SET_BANK_SEGMENT(1, 4);
	SoundUpdate();
	SET_BANK_SEGMENT(1, prevSeg);

    if (Game_GetCurrentState() != State_Game)
        return;

    tick();
}

void main()
{
	DEBUG_INIT();

WITH_SEGMENT(4) {
	SoundInit();
}

WITH_SEGMENT(6) {
	if (g_NextLevelIdx == 0) {
#if RELEASE
		ShowAmi();
		ShowSplashScreen();
#endif
	}
}

	Game_SetState(State_Initialize);
	Game_SetVSyncCallback(InterruptHook);
	Game_Start(VDP_MODE_GRAPHIC2, FALSE);
}
