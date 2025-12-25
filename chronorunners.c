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

//=============================================================================
// DEFINES
//=============================================================================

// Function prototypes
bool State_Initialize();
bool State_Game();
bool State_Death();
bool State_Rewind();
bool State_ChangeLevel();

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


// Key sprite layers
const Pawn_Sprite g_KeyLayers[] =
{//   X  Y  DataOffset    Color            Flag
	{ 0, 0, 0,            COLOR_BLACK,     0 },
};

// Idle animation frames
const Pawn_Frame g_KeyFramesIdle[] =
{//   Pattern        Time Function
	{ KEY_FRAME(0),	 20,  NULL },
	{ KEY_FRAME(1),	 20,  NULL },
};

// List of all key actions
const Pawn_Action g_KeyAnimActions[] =
{//   Frames             Number                          Loop? Interrupt?
	{ g_KeyFramesIdle,   numberof(g_KeyFramesIdle),      TRUE, TRUE },
};


// Enemy sprite layers
const Pawn_Sprite g_EnemyLayers[] =
{//   X  Y  DataOffset    Color            Flag
	{ 0, 0, 0,            COLOR_BLACK,     0 },
};

const Pawn_Frame g_EnemyFramesIdle[] =
{//   Pattern           Time Function
	{ ENEMY_FRAME(2),	20,  NULL },
};

const Pawn_Frame g_EnemyFramesMoveLeft[] =
{//   Pattern           Time Function
	{ ENEMY_FRAME(0),	20,  NULL },
	{ ENEMY_FRAME(1),	20,  NULL },
};

const Pawn_Frame g_EnemyFramesMoveRight[] =
{//   Pattern           Time Function
	{ ENEMY_FRAME(3),	20,  NULL },
	{ ENEMY_FRAME(4),	20,  NULL },
};

const Pawn_Frame g_EnemyFramesShocked[] =
{//   Pattern           Time Function
	{ ENEMY_FRAME(5),	20,  NULL },
	{ ENEMY_FRAME(6),	20,  NULL },
};

const Pawn_Action g_EnemyAnimActions[] =
{//   Frames                    Number                              Loop? Interrupt?
	{ g_EnemyFramesIdle,		numberof(g_EnemyFramesIdle),		TRUE, TRUE },
	{ g_EnemyFramesMoveRight,	numberof(g_EnemyFramesMoveRight),	TRUE, TRUE },
	{ g_EnemyFramesMoveLeft,	numberof(g_EnemyFramesMoveLeft),	TRUE, TRUE },
	{ g_EnemyFramesShocked,		numberof(g_EnemyFramesShocked),		TRUE, TRUE },
};


// Crystal sprite layers
const Pawn_Sprite g_CrystalLayers[] =
{//   X  Y  DataOffset    Color              Flag
	{ 0, 0, 0,            COLOR_MEDIUM_RED,     0 },
};

const Pawn_Frame g_CrystalFramesIdle[] =
{//   Pattern           Time Function
	{ CRYSTAL_FRAME(0),	16,  NULL },
	{ CRYSTAL_FRAME(1),	16,  NULL },
};

// List of all crystal actions
const Pawn_Action g_CrystalAnimActions[] =
{//   Frames             Number                          Loop? Interrupt?
	{ g_CrystalFramesIdle,   numberof(g_CrystalFramesIdle),      TRUE, TRUE },
};


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
extern void UpdatePlayerInput();
extern void UpdatePlayerGravity();
extern void UpdatePlayerMovement(struct Platform *platform);
extern void UpdatePlayerAction();
extern void UpdateEnemyInput();
extern void UpdateEnemyMovement();
extern void UpdateEnemyAction();
i8 GetDPos(i8* m);

extern void PrintGFXText(c8 *text, u8 x, u8 y);
extern void PrintGFXNumber(u8 number, u8 x, u8 y);

extern struct Platform* isPlayerOnPlatform();

extern void UpdatePlatforms();
extern void DrawPlatforms();

extern void DrawMines();

extern void AllocateSpriteIDs();

extern struct Level g_Levels[];

u8 g_PreviousSegment = 0;

/**
 * @brief Switch dei segmenti nel banco 1
 *
 * @param u8 segment, imposta il numero del segmento da assegnare. Se il valore è 0, viene ripristinato il segmento precedente
 */
void SetActiveSegment(u8 segment) {
	if(segment == 0) {
		SET_BANK_SEGMENT(1, g_PreviousSegment);
	} else {
		g_PreviousSegment = GET_BANK_SEGMENT(1);
		SET_BANK_SEGMENT(1, segment);
	}
}

//=============================================================================
// MEMORY DATA
//=============================================================================

// Tempo rimanente. Separa le componenti in modo da non dover
// effettuare divisioni per trovarle.
u8 g_RemainingMinutes = 0;
u8 g_RemainingSeconds = 0;
u8 g_RemainingFS = 0;

// Contatore per l'animazione dei minuti persi dopo la morte
u8 g_CountDownTicks;

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

Pawn g_KeyPawn;
bool g_KeyEnabled;

Pawn g_EnemyPawn;
bool g_EnemyEnabled;
u8   g_EnemyAction;
bool g_EnemyMovingRight = FALSE;
bool g_EnemyMovingLeft = FALSE;
i8   g_EnemymDX = 0;
i8   g_EnemyDX = 0;
i8	 g_EnemyDY = 0;

Pawn g_CrystalPawn;
bool g_CrystalEnabled;

//=============================================================================
// LEVELS
//=============================================================================

u8 g_CurrentLevel = 0;
u8 g_NextLevel = 0;

//=============================================================================
// REWIND DATA
//=============================================================================

/*

Il personaggio principale può riavvolgere il tempo per sé stesso, ripercorrendo
lo spazio che ha attraversato nella direzione inversa. Noi ogni fotogramma del
gioco ci salviamo posizione e fotogramma.

Usiamo un buffer circolare, implementato con strategia a contatore, che ha due
vantaggi in questo caso:
 * permette di usare tutto lo spazio a disposizione (anche l'ultimo slot);
 * permette di risparmiare un po' di codice che sarebbe altrimenti servito per
   calcolare il numero degli elementi contenuti.

Le invarianti di questo buffer sono:

 * head cresce verso "destra";
 * tail logicamente si trova sempre a "sinistra" di head;
 * head punta alla posizione vuota che ospiterà il prossimo valore;
 * il buffer è vuoto quando count = 0
 * il buffer è pieno quando count = rewindSize;

 tail           head
   |              |
   v              v
|--0123456789ABCDE-----------|

  head           tail
   |              |
   v              v
|BC---------------0123456789A|

*/

// 5 secondi * 50 fps per coordinata
#define rewindSize 250
u8 x_rewind[rewindSize];
u8 y_rewind[rewindSize];
u8 f_rewind[rewindSize];
u8 rewind_head;
u8 rewind_tail;
u8 rewind_count;

void DrawRewindGauge() {
	// La barra di rewind può essere grande fino a 8 slot, un cristallo
	// ne riempie due

	u8 ntiles = g_PlayerRewindEnergy >> 5; // g_PlayerRewindEnergy / 32

	if (ntiles != 0)
		VDP_FillLayout_GM2(45, 21, 0, ntiles, 1);
	if (ntiles != 8)
		VDP_FillLayout_GM2(47, 21 + ntiles, 0, 8 - ntiles, 1);
}

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
	// XXX: da rivedere coi livelli messi
	return (tile >= 196 && tile <= 217)
		|| (tile >= 224);
}

void EnemyPhysicsEvent(u8 event, u8 tile)
{
	event; tile;
}

bool EnemyPhysicsCollision(u8 tile)
{
	tile;
	return FALSE;
}

//=============================================================================
// UTILITIES
//=============================================================================

void ReinitPlayer(Pawn *pawn, Pawn_Sprite *spr_layers, u8 n_spr_layers, u8 x, u8 y) {
	Pawn_Initialize(pawn, spr_layers, n_spr_layers, PLAYER_SPRITE_ID, g_AnimActions);
	Pawn_InitializePhysics(pawn, PlayerPhysicsEvent, PlayerPhysicsCollision, 16, 16);
	Pawn_SetPosition(pawn, x, y);
}

// Semplice rilevazione di collisioni a bounding box. Assume oggetti 16x16
bool bboxCollide(u8 x1, u8 y1, u8 x2, u8 y2) {
	if (abs((i16)x1 - x2) < 16 && abs((i16)y1 - y2) < 16)
		return TRUE;

	return FALSE;
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

bool doPawnsCollide(Pawn *p1, Pawn *p2) {
	return bboxCollide(p1->PositionX, p1->PositionY, p2->PositionX, p2->PositionY);
}

void TakeKey() {
	g_PlayerHasKey = TRUE;

	SetActiveSegment(4);
	u8 door_x = g_Levels[g_CurrentLevel].end_x;
	u8 door_y = g_Levels[g_CurrentLevel].end_y;
	SetActiveSegment(0);

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
	g_PlayerMaxRewindEnergy += 62;
}

bool isPlayerAtExit() {
	if (!g_PlayerHasKey)
		return FALSE;

	SetActiveSegment(4);
	u8 door_x = g_Levels[g_CurrentLevel].end_x;
	u8 door_y = g_Levels[g_CurrentLevel].end_y;
	SetActiveSegment(0);

	return bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, door_x * 8, door_y * 8);
}

bool isPlayerOnSpikes() {
    for (i8 t = -1; t < 2; t++) {
        u8 tile = VDP_Peek_GM2((g_PlayerPawn.PositionX >> 3) + t,
                               (g_PlayerPawn.PositionY >> 3) + 2);
        if (tile >= 192 && tile <= 195)
            return TRUE;
    }
    return FALSE;
}

bool isPlayerOnMines() {
	// Recupera livello corrente
	struct Level *lvl;
	lvl = &g_Levels[g_CurrentLevel];

	u8 nm = lvl->num_mines;
	struct Mine *mines = lvl->mines;
	for (u8 m=0; m < nm; m++) {
		// Confronta il bbox 16x16 del giocatore con un bbox 2x1 della mina
		if (rectCollide(g_PlayerPawn.PositionX,      g_PlayerPawn.PositionY,
					    g_PlayerPawn.PositionX + 16, g_PlayerPawn.PositionY + 16,
				        mines[m].pos_x + 7,     mines[m].pos_y - 1,
				        mines[m].pos_x + 7 + 2, mines[m].pos_y)) {
			return TRUE;
		}
	}
	return FALSE;
}

void PrintTime() {
	PrintGFXNumber(g_RemainingMinutes, 7, 0);
	PrintGFXNumber(g_RemainingSeconds, 10, 0);
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

	// Imposta tempo iniziale
	g_RemainingMinutes = 60;

	Game_SetState(State_ChangeLevel);

	return TRUE;
}

void PlayerRestart()
{
	SetActiveSegment(4);
	u8 start_x = g_Levels[g_CurrentLevel].start_x;
	u8 start_y = g_Levels[g_CurrentLevel].start_y;
	SetActiveSegment(0);

	// Init player pawn
	ReinitPlayer(&g_PlayerPawn,
		         g_PlayerLayers, numberof(g_PlayerLayers),
				 start_x * 8, start_y * 8);

	g_PlayerDying = FALSE;
	g_CountDownTicks = 50 - 1;

	rewind_head = rewind_tail = rewind_count = 0;
}

bool State_ChangeLevel()
{
	SetActiveSegment(4);

	// Passa al livello successivo
	g_CurrentLevel = g_NextLevel;
	g_NextLevel = g_Levels[g_CurrentLevel].next_level - 1;

	u8 key_x = g_Levels[g_CurrentLevel].key_x;
	u8 key_y = g_Levels[g_CurrentLevel].key_y;
	u8 enemy_x = g_Levels[g_CurrentLevel].enemy_x;
	u8 enemy_y = g_Levels[g_CurrentLevel].enemy_y;
	u8 crystal_x = g_Levels[g_CurrentLevel].crystal_x;
	u8 crystal_y = g_Levels[g_CurrentLevel].crystal_y;

	PrintGFXText("TIME   '  \"", 2, 0);
	PrintTime();

	VDP_WriteLayout_GM2(g_Levels[g_CurrentLevel].layout, 0, 2, 32, 24);

	// Nasconde tutti gli sprite, questa funzione e la State_Game
	// riabiliteranno quelli che servono in ciascun livello
	VDP_HideAllSprites();

	AllocateSpriteIDs();

	SetActiveSegment(0);

	g_PlayerHasKey = FALSE;
	PlayerRestart();

	// Init key pawn
	Pawn_Initialize(&g_KeyPawn,
		            g_KeyLayers, numberof(g_KeyLayers),
					KEY_SPRITE_ID, g_KeyAnimActions);
	Pawn_SetPosition(&g_KeyPawn,
		             key_x * 8, key_y * 8);
	g_KeyEnabled = TRUE;
	Pawn_SetEnable(&g_KeyPawn, g_KeyEnabled);

	// Init enemy pawn
	Pawn_Initialize(&g_EnemyPawn,
		            g_EnemyLayers, numberof(g_EnemyLayers),
					ENEMY_SPRITE_ID, g_EnemyAnimActions);
	Pawn_InitializePhysics(&g_EnemyPawn, EnemyPhysicsEvent, EnemyPhysicsCollision, 16, 16);
	Pawn_SetPosition(&g_EnemyPawn,
		             enemy_x * 8, enemy_y * 8);

	if (enemy_x == 0 && enemy_y == 0)
		g_EnemyEnabled = FALSE;
	else
		g_EnemyEnabled = TRUE;
	Pawn_SetEnable(&g_EnemyPawn, g_EnemyEnabled);

	// Init crystal pawn
	Pawn_Initialize(&g_CrystalPawn,
		            g_CrystalLayers, numberof(g_CrystalLayers),
					CRYSTAL_SPRITE_ID, g_CrystalAnimActions);
	Pawn_SetPosition(&g_CrystalPawn,
		             crystal_x * 8, crystal_y * 8);

	if (crystal_x == 0 && crystal_y == 0)
		g_CrystalEnabled = FALSE;
	else
		g_CrystalEnabled = TRUE;
	Pawn_SetEnable(&g_CrystalPawn, g_CrystalEnabled);

	Game_SetState(State_Game);

	return TRUE;
}

bool State_Game()
{
	SetActiveSegment(4);

	// Gestione input
	UpdatePlayerInput();

	// Aggiorna piattaforme
	UpdatePlatforms();

	// Controlla se il giocatore è atterrato su una piattaforma mobile
	struct Platform *platform = isPlayerOnPlatform();

	UpdatePlayerMovement(platform);

	UpdatePlayerAction();
	if (g_EnemyEnabled) {
		UpdateEnemyInput();
		UpdateEnemyMovement();
		UpdateEnemyAction();
	}

	// Testi a video
	if (g_RemainingFS == 0) {
		PrintTime();
	}

	DrawPlatforms();
	DrawMines();

	SetActiveSegment(0);

	Pawn_SetAction(&g_PlayerPawn, g_PlayerAction);
	Pawn_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	Pawn_Update(&g_PlayerPawn);

	Pawn_SetAction(&g_KeyPawn, ACTION_IDLE);
	Pawn_Update(&g_KeyPawn);

	Pawn_SetAction(&g_EnemyPawn, g_EnemyAction);
	Pawn_SetMovement(&g_EnemyPawn, g_EnemyDX, g_EnemyDY);
	Pawn_Update(&g_EnemyPawn);

	Pawn_SetAction(&g_CrystalPawn, ACTION_IDLE);
	Pawn_Update(&g_CrystalPawn);

	// Controlla la collisione tra giocatore e chiave
	if (g_KeyEnabled && doPawnsCollide(&g_PlayerPawn, &g_KeyPawn)) {
		TakeKey();
		g_KeyEnabled = FALSE;
		Pawn_Disable(&g_KeyPawn);
	}

	// Controlla la collisione tra giocatore e cristallo
	if (g_CrystalEnabled && doPawnsCollide(&g_PlayerPawn, &g_CrystalPawn)) {
		TakeCrystal();
		g_CrystalEnabled = FALSE;
		Pawn_Disable(&g_CrystalPawn);
	}

    // Controlla la collisione tra giocatore e spine
    if (isPlayerOnSpikes() || isPlayerOnMines()) {
        // Effetto "morte" del giocatore.
        Game_SetState(State_Death);
        return TRUE;
    }

	// Controlla la collisione tra giocatore e nemico
	if (g_EnemyEnabled && doPawnsCollide(&g_PlayerPawn, &g_EnemyPawn)) {

		if (g_PlayerPawn.PositionY < g_EnemyPawn.PositionY) {
			g_EnemyEnabled = FALSE;
			Pawn_Disable(&g_EnemyPawn);
		} else {
			// Effetto "morte" del giocatore.
			Game_SetState(State_Death);
			return TRUE;
		}
	}

	// Controlla se il giocatore ha raggiunto l'uscita
	if (isPlayerAtExit()) {
		Game_SetState(State_ChangeLevel);
		return TRUE;
	}

	Pawn_Draw(&g_PlayerPawn);
	Pawn_Draw(&g_KeyPawn);
	Pawn_Draw(&g_EnemyPawn);
	Pawn_Draw(&g_CrystalPawn);

	// Inserisce la posizione attuale nel buffer circolare
	x_rewind[rewind_head] = g_PlayerPawn.PositionX;
	y_rewind[rewind_head] = g_PlayerPawn.PositionY;
	f_rewind[rewind_head] = g_PlayerPawn.AnimFrame;
	rewind_head = (rewind_head + 1) % rewindSize;

	// L'array era già pieno?
	if (rewind_count == rewindSize) {
		// Se sì, deve consumare la posizione acquisita più vecchia
		rewind_tail = (rewind_tail + 1) % rewindSize;
	} else {
		// Altrimenti si limita a incrementare il contatore di elementi
		rewind_count++;
	}

	u8 row8 = Keyboard_Read(8);
	if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
		if (g_PlayerRewindEnergy > 1) {

			// Sostituisce i colori dello sprite principale
			ReinitPlayer(&g_PlayerPawn,
							g_PlayerRewindLayers, numberof(g_PlayerRewindLayers),
							x_rewind[rewind_head], y_rewind[rewind_head]);

			// Entra in modalità rewind
			Game_SetState(State_Rewind);
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
    }

	// Ancora il personaggio non è uscito dallo schermo
	if (g_PlayerPawn.PositionY < 240) {
		SetActiveSegment(4);
		UpdatePlayerGravity();
		g_DY = GetDPos(&g_mDY);
		SetActiveSegment(0);

		Pawn_SetAction(&g_PlayerPawn, ACTION_DEATH);
		Pawn_SetPosition(&g_PlayerPawn,
						  g_PlayerPawn.PositionX,
						  g_PlayerPawn.PositionY + g_DY);
		Pawn_Update(&g_PlayerPawn);

		Pawn_Draw(&g_PlayerPawn);
	}

	if (g_PlayerPawn.PositionY >= 240) {
		// Ogni morte costa al giocatore 5 minuti sul tempo totale

		// Se non li ha più, game over XXX: da gestire
		if (g_RemainingMinutes == 0 && g_RemainingSeconds == 0) {
			DEBUG_PRINT("GAME OVER\n");
		}

		// Scala 6 secondi per fotogramma dal contatore, per fare un'animazione
		// che spieghi visivamente cosa sta succedendo
		for (u16 t=0; t < 300; t++) {
			tick();
		}
		SetActiveSegment(4);
		PrintTime();
		SetActiveSegment(0);

		if (g_CountDownTicks > 0) {
			g_CountDownTicks--;
		} else {
			PlayerRestart();
			Game_SetState(State_Game);
		}
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

	// Ogni passo di rewind consuma un elemento dell'array circolare
	rewind_head = (rewind_head - 1 + rewindSize) % rewindSize;
	rewind_count--;

	// e anche l'energia di rewind del giocatore
	g_PlayerRewindEnergy--;

	DrawRewindGauge();

	// Aggiorna la posizione e il fotogramma. Per forzare quest'ultimo,
	// imposta a mano il flag di aggiornamento pattern.
	Pawn_SetPosition(&g_PlayerPawn, x_rewind[rewind_head], y_rewind[rewind_head]);
	g_PlayerPawn.Update |= PAWN_UPDATE_PATTERN;
	g_PlayerPawn.AnimFrame = f_rewind[rewind_head];
	Pawn_Draw(&g_PlayerPawn);

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
    if (Game_GetCurrentState() != State_Game)
        return;

    tick();
}

void main()
{
	DEBUG_INIT();

	Bios_SetKeyClick(FALSE);

	Game_SetState(State_Initialize);
	Game_SetVSyncCallback(InterruptHook);
	Game_Start(VDP_MODE_GRAPHIC2, FALSE);

	Bios_Exit(0);
}
