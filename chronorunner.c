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

// Sequence system: interleave levels and cutscenes
#define SEQ_LEVEL    0
#define SEQ_CUTSCENE 1
#define MAX_CUTSCENES 11
#define MAX_SEQUENCE  (NUM_LEVELS + MAX_CUTSCENES)

typedef struct { u8 type; u8 idx; } SequenceEntry;

// Function prototypes
bool State_Initialize();
bool State_Menu();
bool State_Credits();
bool State_Game();
bool State_Death();
bool State_Rewind();
bool State_Intermission();
bool State_MessageScreen();
bool State_Cutscene();

void ChangeLevel();
void AdvanceSequence();

void tick();

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Player sprite layers
const Pawn_Sprite g_PlayerLayers[] =
{//   X  Y  DataOffset    Color               Flag
	{ 0, 0, 0,            COLOR_BLACK,        0 },
	{ 0, 0, laySize,      COLOR_DARK_YELLOW,  0 },
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
// SEGMENT 1, BANK 1
//=============================================================================

// Menu screen
extern const u8 g_Screen2[];

// Credits screen
extern const u8 g_Screen80[];
extern const u8 g_Screen81[];

// Intermission screen
extern const u8 g_Screen15[];

// World 1
extern struct Level level_map25;
extern struct Level level_map4;
extern struct Level level_map16;
extern struct Level level_map34;
extern struct Level level_map7;
extern struct Level level_map6;
extern struct Level level_map19;

// World 2
extern struct Level level_map26;
extern struct Level level_map9;
extern struct Level level_map8;
extern struct Level level_map17;
extern struct Level level_map22;
extern struct Level level_map23;
extern struct Level level_map31;

//=============================================================================
// SEGMENT 2, BANK 1
//=============================================================================

// World 3
extern struct Level level_map27;
extern struct Level level_map18;
extern struct Level level_map11;
extern struct Level level_map33;
extern struct Level level_map10;
extern struct Level level_map35;
extern struct Level level_map20;

// World 4
extern struct Level level_map28;
extern struct Level level_map30;
extern struct Level level_map12;
extern struct Level level_map24;
extern struct Level level_map29;
extern struct Level level_map21;
extern struct Level level_map32;

// Final
extern struct Level level_map13;

// Secret
extern struct Level level_map48;
extern struct Level level_map49;
extern struct Level level_map47;
extern struct Level level_map50;

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
// SEGMENT 5, BANK 1
//=============================================================================

extern const CutCmd g_IntroCutscene[];
extern const CutCmd g_World1MidCutscene[];
extern const CutCmd g_World2MidCutscene[];
extern const CutCmd g_World3MidCutscene[];
extern const CutCmd g_World4MidCutscene[];
extern const CutCmd g_World1EndCutscene[];
extern const CutCmd g_World2EndCutscene[];
extern const CutCmd g_World3EndCutscene[];
extern const CutCmd g_World4EndCutscene[];
extern const CutCmd g_TrueColorsCutscene[];
extern const CutCmd g_PreBossCutscene[];

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

extern bool isPlayerHitByEnergyFields(struct Level *lvl);
extern bool isPlayerHitByEnemies(struct Level *lvl);
extern struct Platform* isPlayerOnPlatform(struct Level *lvl);
extern bool isPlayerAtExit();
extern bool isPlayerOnSpikes();
extern struct Mine* isPlayerOnMines(struct Level *lvl);
extern bool isPlayerInPit();

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

extern void InitBoss();
extern bool State_Boss();
extern void DrawBossStance();
void AllocateCurrentLevelSprites();

//=============================================================================
// LEVELS
//=============================================================================

// Per partire velocemente da un punto della sequenza per il debug
// 0 = dall'inizio (mostra il menu), >0 = salta direttamente a quel punto
#define START_SEQUENCE_IDX 0

u8 g_CurrentLevelIdx;
u8 g_NextLevelIdx;
i8 g_SecretNextLevelIdx = -1;

#define NUM_LEVELS 29
#define SECRET_LEVELS 4

// Array con l'ordine dei livelli
const struct Level* g_LevelOrder[NUM_LEVELS + SECRET_LEVELS];

// Sequence system: unified level/cutscene order
SequenceEntry g_Sequence[MAX_SEQUENCE];
u8 g_SequenceLength;
u8 g_SequenceIdx;
const CutCmd* g_CutsceneScripts[MAX_CUTSCENES];

// Copia locale del livello attuale
struct Level g_ActiveLevel;

u8 SegmentForLevel(u8 lvlidx) {
	if (lvlidx < 14) {
		return 1;
	} else {
		return 2;
	}
}

// Helper macros for building the sequence
#define ADD_LEVEL(lvl) do { \
	g_LevelOrder[l] = &lvl; \
	g_Sequence[s].type = SEQ_LEVEL; g_Sequence[s].idx = l; \
	s++; l++; \
} while(0)

#define ADD_CUTSCENE(cs) do { \
	g_CutsceneScripts[c] = cs; \
	g_Sequence[s].type = SEQ_CUTSCENE; g_Sequence[s].idx = c; \
	s++; c++; \
} while(0)

void InitLevels() {
	u8 s = 0, l = 0, c = 0;

	// Intro cutscene
	ADD_CUTSCENE(g_IntroCutscene);

	// Levels
	// World 1
	ADD_LEVEL(level_map25);
	ADD_LEVEL(level_map4);
	ADD_LEVEL(level_map16);
	ADD_LEVEL(level_map34);
	ADD_CUTSCENE(g_World1MidCutscene);
	ADD_LEVEL(level_map7);
	ADD_LEVEL(level_map6);
	ADD_LEVEL(level_map19);
	ADD_CUTSCENE(g_World1EndCutscene);

	// World 2
	ADD_LEVEL(level_map26);
	ADD_LEVEL(level_map9);
	ADD_LEVEL(level_map8);
	ADD_LEVEL(level_map17);
	ADD_CUTSCENE(g_World2MidCutscene);
	ADD_LEVEL(level_map22);
	ADD_LEVEL(level_map23);
	ADD_LEVEL(level_map31);
	ADD_CUTSCENE(g_World2EndCutscene);

	// World 3
	ADD_LEVEL(level_map27);
	ADD_LEVEL(level_map18);
	ADD_LEVEL(level_map11);
	ADD_LEVEL(level_map33);
	ADD_CUTSCENE(g_World3MidCutscene);
	ADD_LEVEL(level_map10);
	ADD_LEVEL(level_map35);
	ADD_LEVEL(level_map20);
	ADD_CUTSCENE(g_World3EndCutscene);

	// World 4
	ADD_LEVEL(level_map28);
	ADD_LEVEL(level_map30);
	ADD_LEVEL(level_map12);
	ADD_LEVEL(level_map24);
	ADD_CUTSCENE(g_World4MidCutscene);
	ADD_LEVEL(level_map29);
	ADD_LEVEL(level_map21);
	ADD_LEVEL(level_map32);
	ADD_CUTSCENE(g_World4EndCutscene);

	// Final
	ADD_CUTSCENE(g_TrueColorsCutscene);
	ADD_LEVEL(level_map13);
	ADD_CUTSCENE(g_PreBossCutscene);

	g_SequenceLength = s;

	// Secret levels (not part of the sequence, accessed via vortex detour)
	g_LevelOrder[l++] = &level_map48;
	g_LevelOrder[l++] = &level_map49;
	g_LevelOrder[l++] = &level_map47;
	g_LevelOrder[l++] = &level_map50;
}

void SetMessageScreen(const c8* text, i8 songId, u16 duration);

void AdvanceSequence()
{
	if (g_SequenceIdx >= g_SequenceLength) {
		InitBoss();
		return;
	}

	SequenceEntry* entry = &g_Sequence[g_SequenceIdx];

	if (entry->type == SEQ_CUTSCENE) {
		g_SequenceIdx++;
		Cutscene_Start(g_CutsceneScripts[entry->idx]);
	} else {
		// Level: don't advance g_SequenceIdx yet, State_ChangeLevel will
		g_NextLevelIdx = entry->idx;
		Game_SetState(State_Intermission);
	}
}

//=============================================================================
// LEVEL LOADING
//=============================================================================

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
// TRAMPOLINE FUNCTIONS FOR OTHER SEGMENTS
//=============================================================================

void FxPlay(u8 id) {
WITH_SEGMENT(4) {
	S4_FxPlay(id);
}
}

void SetSong(u8 id) {
WITH_SEGMENT(4) {
	SoundSetSong(id);
}
}

void Play() {
WITH_SEGMENT(4) {
	SoundPlay();
}
}

void Loop(bool value) {
WITH_SEGMENT(4) {
	SoundLoop(value);
}
}

// Shim callable from segment 7 code
void AllocateCurrentLevelSprites()
{
WITH_SEGMENT(3) {
	AllocateSpriteIDs(&g_ActiveLevel);
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
	g_PlayerRewindEnergy += (SNAPSHOT_BUFFER_SIZE / 4);
	if (g_PlayerRewindEnergy > SNAPSHOT_BUFFER_SIZE)
		g_PlayerRewindEnergy = SNAPSHOT_BUFFER_SIZE;

	FxPlay(FX_GET_CRYSTAL);
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
	SoundStop();
}

	// Imposta tempo iniziale
	g_RemainingMinutes = 60;
	g_RemainingSeconds = 0;
	g_RemainingFS = 0;

	// Reset rewind energy
	g_PlayerRewindEnergy = 0;

	// Reset livelli
	g_CurrentLevelIdx = 0;
	g_SequenceIdx = START_SEQUENCE_IDX;

	// Initialize cutscene system
	Cutscene_Initialize();

	// Start at main menu or skip directly to sequence point
	if (g_SequenceIdx == 0) {
WITH_SEGMENT(4) {
		// Start menu music
		SoundSetSong(MUSIC_MENU);
		SoundLoop(TRUE);
		SoundPlay();
}
		Game_SetState(State_Menu);
	} else {
		AdvanceSequence();
	}

	return TRUE;
}

void PlayerRestart()
{
	u8 seg = SegmentForLevel(g_CurrentLevelIdx);

	// Reset level state (mines, enemies, platforms) from source data
WITH_SEGMENT(seg) {
	LoadLevel(g_CurrentLevelIdx);
}

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

	// Reset vortex and boom state
	g_VortexAnimFrame = 0;
	VDP_HideSprite(VORTEX_SPRITE_ID);
	VDP_HideSprite(BOOM_SPRITE_ID);

	// Init player pawn
	u8 start_x = g_ActiveLevel.start_x;
	u8 start_y = g_ActiveLevel.start_y;
	ReinitPlayer(&g_PlayerPawn,
		         g_PlayerLayers, numberof(g_PlayerLayers),
				 start_x * 8, start_y * 8);

	g_PlayerDying = FALSE;
	g_CountDownTicks = 50 - 1;

	// Initialize snapshot system for player and object rewind.
	// Il buffer va sempre resettato perché le coordinate del livello
	// precedente non sono valide.
	Snapshot_Initialize();

	// Redraw level layout (resets door appearance if key was taken)
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
			SoundSetSong(MUSIC_GAME);
			SoundLoop(TRUE);
			SoundPlay();
		}
}

WITH_SEGMENT(1) {
		VDP_WriteLayout_GM2(g_Screen15, 0, 0, 32, 24);
}

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
		PrintGFXText("TIME   '  \"", 18, 18);
		PrintGFXNumber(g_RemainingMinutes, 23, 18);
		PrintGFXNumber(g_RemainingSeconds, 26, 18);

		u8 next_lvl_seg = SegmentForLevel(g_NextLevelIdx);

		c8 level_name[32];
WITH_SEGMENT(next_lvl_seg) {
		// Prende il nome del prossimo livello
		String_Copy(level_name, g_LevelOrder[g_NextLevelIdx]->name);
}

		// Nome del livello (centrato)
		PrintGFXText(level_name, 16 - (String_Length(level_name) / 2), 20);

		PrintGFXText("GET READY!", 11, 23);
	}

	g_IntermissionState++;

	if (g_IntermissionState > 100 || Keyboard_IsKeyPressed(KEY_F1)) {
		g_IntermissionState = 0;
		ChangeLevel();
		return FALSE;
	}

	return TRUE;
}

void ChangeLevel()
{
	// Nasconde tutti gli sprite, verranno riabilitati quelli che servono
	// in ciascun livello
	VDP_HideAllSprites();

	if (g_SecretNextLevelIdx != -1) {
		// Passa al livello segreto. Non avanza g_SequenceIdx perché è
		// una deviazione: al termine si torna alla sequenza normale.
		g_CurrentLevelIdx = g_SecretNextLevelIdx;
		g_SecretNextLevelIdx = -1;
	} else {
		// Passa al livello successivo e avanza nella sequenza
		g_CurrentLevelIdx = g_NextLevelIdx;
		g_SequenceIdx++;
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
		FxPlay(FX_VORTEX);

		// Seleziona la giusta stanza segreta in base al livello in cui ci troviamo
		if (g_CurrentLevelIdx < 7) {
			g_SecretNextLevelIdx = NUM_LEVELS;
		} else if (g_CurrentLevelIdx < 14) {
			g_SecretNextLevelIdx = NUM_LEVELS + 1;
		} else if (g_CurrentLevelIdx < 21) {
			g_SecretNextLevelIdx = NUM_LEVELS + 2;
		} else {
			g_SecretNextLevelIdx = NUM_LEVELS + 3;
		}

		ChangeLevel();
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
    struct Mine *hitMine = isPlayerOnMines(lvl);
    if (hitMine)
        VDP_SetSpriteSM1(BOOM_SPRITE_ID,
			hitMine->pos_x,
			hitMine->pos_y - 16,
			BOOM_PATTERN_OFFSET,
			COLOR_WHITE);

	if (isPlayerOnSpikes() || hitMine || isPlayerHitByEnemies(lvl)
     || isPlayerHitByEnergyFields(lvl) || isPlayerInPit()) {
        // Effetto "morte" del giocatore.
        Game_SetState(State_Death);
        return TRUE;
    }

	// Controlla se il giocatore ha raggiunto l'uscita
	if (isPlayerAtExit() || Keyboard_IsKeyPressed(KEY_F1)) {
		FxPlay(FX_EXIT_DOOR);
		AdvanceSequence();
		return TRUE;
	}

	// Capture snapshot of all game objects and player state
	Snapshot_Capture(lvl, g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, g_PlayerPawn.AnimFrame);

	u8 row8 = Keyboard_Read(8);
	if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
		if (g_PlayerRewindEnergy > 1 && Snapshot_GetRewindCount() >= g_PlayerRewindEnergy) {

			// Sostituisce i colori dello sprite principale
			ReinitPlayer(&g_PlayerPawn,
							g_PlayerRewindLayers, numberof(g_PlayerRewindLayers),
							g_PlayerPawn.PositionX, g_PlayerPawn.PositionY);

			// Entra in modalità rewind
			Game_SetState(State_Rewind);
			FxPlay(FX_REWIND);
			return TRUE;
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
				SetMessageScreen("GAME OVER", MUSIC_GAMEOVER, 500);
				Game_SetState(State_MessageScreen);
			} else {
				// Flash su respawn personaggio
				VDP_SetColor(COLOR_WHITE);
				PlayerRestart();
				Game_SetState(State_Game);
				Halt();
				VDP_SetColor(COLOR_BLACK);
			}
		}
	}
	return TRUE;
}

void SetMessageScreen(const c8* text, i8 songId, u16 duration) {
	g_MessageScreenText = text;
	g_MessageScreenSongId = songId;
	g_MessageScreenDuration = duration;
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
// MENU STATE
//=============================================================================

u8 g_MenuState = 0;
u8 g_MenuSelection = 0;
u8 g_MenuWait = 0;

u8 g_CreditsState = 0;
u8 g_CreditsWait = 0;

// Menu option Y positions (rows 11, 13, 15, 17)
#define MENU_NUM_OPTIONS    4
#define MENU_HIGHLIGHT_TILE 49
#define MENU_EMPTY_TILE     47

static const u8 g_MenuRows[MENU_NUM_OPTIONS] = { 11, 13, 15, 17 };
static const u8 g_MenuColumns[MENU_NUM_OPTIONS] = { 14, 12, 10, 8 };

bool State_Menu()
{
    if (g_MenuState == 0) {
        VDP_HideAllSprites();
WITH_SEGMENT(1) {
        VDP_WriteLayout_GM2(g_Screen2, 0, 0, 32, 24);
}
        g_MenuSelection = 0;
        VDP_Poke_GM2(g_MenuColumns[0], g_MenuRows[0], MENU_HIGHLIGHT_TILE);
        g_MenuWait = 1;
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
            g_SequenceIdx = 0;
            AdvanceSequence();
        } else if (g_MenuSelection == 3) {
            g_MenuState = 0;
            Game_SetState(State_Credits);
        }
        return TRUE;
    }

    if (prev != g_MenuSelection) {
        VDP_Poke_GM2(g_MenuColumns[prev], g_MenuRows[prev], MENU_EMPTY_TILE);
        VDP_Poke_GM2(g_MenuColumns[g_MenuSelection], g_MenuRows[g_MenuSelection], MENU_HIGHLIGHT_TILE);
    }

    return TRUE;
}

//=============================================================================
// CREDITS STATE
//=============================================================================

bool State_Credits()
{
    if (g_CreditsState == 0) {
        VDP_HideAllSprites();
WITH_SEGMENT(1) {
        VDP_WriteLayout_GM2(g_Screen80, 0, 0, 32, 24);
}
        g_CreditsState = 1;
        g_CreditsWait = 1;  // Discard the SPACE that triggered credits from menu
    }

    u8 row8 = Keyboard_Read(8);

    // Wait for SPACE to be released before accepting next press
    if (g_CreditsWait) {
        if (IS_KEY_RELEASED(row8, KEY_SPACE)) {
            g_CreditsWait = 0;
        }
        return TRUE;
    }

    if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
        if (g_CreditsState == 1) {
WITH_SEGMENT(1) {
            VDP_WriteLayout_GM2(g_Screen81, 0, 0, 32, 24);
}
            g_CreditsState = 2;
            g_CreditsWait = 1;
        } else if (g_CreditsState == 2) {
            g_CreditsState = 0;
            Game_SetState(State_Menu);
        }
    }

    return TRUE;
}

//=============================================================================
// CUTSCENE FUNCTIONS
//=============================================================================

#include "cutscene_s0.c"

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
	if (START_SEQUENCE_IDX == 0) {
		ShowAmi();
		ShowSplashScreen();
	}
}

	Bios_SetKeyClick(FALSE);

	Game_SetState(State_Initialize);
	Game_SetVSyncCallback(InterruptHook);
	Game_Start(VDP_MODE_GRAPHIC2, FALSE);
}
