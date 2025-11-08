//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/state.h"
#include "game/pawn.h"
#include "debug.h"

#include "PawnData.h"
#include "level_defs.h"

//=============================================================================
// DEFINES
//=============================================================================

// Layer grandi 4 pattern
#define laySize 4
// Due layer per sprite
#define nLayers 2
// Dimensione sprite
#define sprSize laySize * nLayers

// Function prototypes
bool State_Initialize();
bool State_Game();
bool State_Rewind();
bool State_ChangeLevel();

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Per ora il player è visualizzato usando gli sprite 0 e 1
#define PLAYER_SPRITE_ID 0
#define PLAYER_PATTERN_OFFSET 0
#define PLAYER_PATTERN_SIZE laySize * nLayers

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
{//   Pattern                                              Time  Function
	{  0 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,    250,  NULL },
	{ 11 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
	{ 12 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
	{ 11 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
	{ 12 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
	{ 11 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
	{ 12 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
	{ 11 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,     10,  NULL },
};

// Move animation frames
const Pawn_Frame g_PlayerFramesMoveRight[] =
{
	{ 1 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	6,	NULL },
	{ 2 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	6,	NULL },
	{ 3 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	6,	NULL },
};

const Pawn_Frame g_PlayerFramesMoveLeft[] =
{
	{ 4 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	6,	NULL },
	{ 5 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	6,	NULL },
	{ 6 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	6,	NULL },
};

// Jump animation frames
const Pawn_Frame g_PlayerFramesJumpRight[] =
{
	{ 3 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	4,	NULL },
};

const Pawn_Frame g_PlayerFramesJumpLeft[] =
{
	{ 6 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	4,	NULL },
};

// Fall animation frames
const Pawn_Frame g_PlayerFramesFallRight[] =
{
	{ 1 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	4,	NULL },
};

const Pawn_Frame g_PlayerFramesFallLeft[] =
{
	{ 4 * PLAYER_PATTERN_SIZE + PLAYER_PATTERN_OFFSET,	4,	NULL },
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
};

// Per ora la chiave è visualizzata usando lo sprite 2
#define KEY_SPRITE_ID 2
#define KEY_PATTERN_OFFSET 13 * sprSize
#define KEY_PATTERN_SIZE laySize

// Key sprite layers
const Pawn_Sprite g_KeyLayers[] =
{//   X  Y  DataOffset    Color            Flag
	{ 0, 0, 0,            COLOR_BLACK,     0 },
};

// Idle animation frames
const Pawn_Frame g_KeyFramesIdle[] =
{//   Pattern                                       Time Function
	{ 0 * KEY_PATTERN_SIZE + KEY_PATTERN_OFFSET,          	20,  NULL },
	{ 1 * KEY_PATTERN_SIZE + KEY_PATTERN_OFFSET,	20,  NULL },
};

// List of all key actions
const Pawn_Action g_KeyAnimActions[] =
{//   Frames             Number                          Loop? Interrupt?
	{ g_KeyFramesIdle,   numberof(g_KeyFramesIdle),      TRUE, TRUE },
};

#define ENEMY_SPRITE_ID 3
#define ENEMY_PATTERN_OFFSET 13 * sprSize + 2 * laySize
#define ENEMY_PATTERN_SIZE laySize

// Enemy sprite layers
const Pawn_Sprite g_EnemyLayers[] =
{//   X  Y  DataOffset    Color            Flag
	{ 0, 0, 0,            COLOR_BLACK,     0 },
};

const Pawn_Frame g_EnemyFramesIdle[] =
{//   Pattern                                           Time Function
	{ 2 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
};

const Pawn_Frame g_EnemyFramesMoveLeft[] =
{//   Pattern                                           Time Function
	{ 0 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
	{ 1 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
};

const Pawn_Frame g_EnemyFramesMoveRight[] =
{//   Pattern                                           Time Function
	{ 3 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
	{ 4 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
};

const Pawn_Frame g_EnemyFramesShocked[] =
{//   Pattern                                           Time Function
	{ 5 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
	{ 6 * ENEMY_PATTERN_SIZE + ENEMY_PATTERN_OFFSET,	20,  NULL },
};

const Pawn_Action g_EnemyAnimActions[] =
{//   Frames                    Number                              Loop? Interrupt?
	{ g_EnemyFramesIdle,		numberof(g_EnemyFramesIdle),		TRUE, TRUE },
	{ g_EnemyFramesMoveLeft,	numberof(g_EnemyFramesMoveLeft),	TRUE, TRUE },
	{ g_EnemyFramesMoveRight,	numberof(g_EnemyFramesMoveRight),	TRUE, TRUE },
	{ g_EnemyFramesShocked,		numberof(g_EnemyFramesShocked),		TRUE, TRUE },
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
extern void UpdatePlayerMovement();
extern u8 UpdatePlayerAction(u8 act);

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

Pawn g_PlayerPawn;
bool g_PlayerMovingRight = FALSE;
bool g_PlayerMovingLeft = FALSE;
bool g_PlayerJumping = FALSE;
bool g_PlayerDamped = FALSE;
i8   g_VelocityY;
i8   g_mDX = 0;
i8   g_DX = 0;
i8   g_DY = 0;
bool g_PlayerHasKey = FALSE;

Pawn g_KeyPawn;

Pawn g_EnemyPawn;

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
	u8 i;
	for (i = 1; i < 11; i++) {
		if (rewind_count >= 25 * i)
			VDP_Poke_GM2(i, 1, 45);
		else
			VDP_Poke_GM2(i, 1, 47);
	}
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
		if (!g_PlayerJumping)
		{
			g_PlayerJumping = TRUE;
		}
		break;
	};
}

// Collision callback
bool PlayerPhysicsCollision(u8 tile)
{
	return (tile >= 208 && tile <= 210) || (tile >= 224);
}

void KeyPhysicsEvent(u8 event, u8 tile)
{
	event; tile;
}

bool KeyPhysicsCollision(u8 tile)
{
	tile;
	return TRUE;
}

void EnemyPhysicsEvent(u8 event, u8 tile)
{
	event; tile;
}

bool EnemyPhysicsCollision(u8 tile)
{
	tile;
	return TRUE;
}

//=============================================================================
// UTILITIES
//=============================================================================

void ReinitPlayer(Pawn *pawn, Pawn_Sprite *spr_layers, u8 n_spr_layers, u8 x, u8 y) {
	Pawn_Initialize(pawn, spr_layers, n_spr_layers, PLAYER_SPRITE_ID, g_AnimActions);
	Pawn_InitializePhysics(pawn, PlayerPhysicsEvent, PlayerPhysicsCollision, 16, 16);
	Pawn_SetPosition(pawn, x, y);
}

i16 abs(i16 a)
{
	return (a > 0) ? a : -a;
}

// Semplice rilevazione di collisioni a bounding box. Assume oggetti 16x16
bool bboxCollide(u8 x1, u8 y1, u8 x2, u8 y2) {
	if (abs((i16)x1 - x2) < 16 && abs((i16)y1 - y2) < 16)
		return TRUE;

	return FALSE;
}

bool doPawnsCollide(Pawn *p1, Pawn *p2) {
	return bboxCollide(p1->PositionX, p1->PositionY, p2->PositionX, p2->PositionY);
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

void CheckPlayerOnDampers() {
	g_PlayerDamped = FALSE;
	u8 tx = (g_PlayerPawn.PositionX + 8) / 8;
	u8 ty = (g_PlayerPawn.PositionY) / 8;

	// Controlla la tile sotto i piedi del personaggio
	u8 tile = VDP_Peek_GM2(tx, ty + 2);

	if (tile == 210) {
		// Se sta passando dal terreno accidentato deve rallentare
		g_PlayerDamped = TRUE;
	}
}

bool isPlayerAtExit() {
	if (!g_PlayerHasKey)
		return FALSE;

	u8 door_x = g_Levels[g_CurrentLevel].end_x;
	u8 door_y = g_Levels[g_CurrentLevel].end_y;

	bool collide = bboxCollide(g_PlayerPawn.PositionX, g_PlayerPawn.PositionY, door_x * 8, door_y * 8);

	return collide;
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

	Game_SetState(State_ChangeLevel);

	return FALSE;
}

bool State_ChangeLevel()
{
	// Passa al livello successivo
	g_CurrentLevel = g_NextLevel;
	g_NextLevel = g_Levels[g_CurrentLevel].next_level - 1;

	struct Level lvl;
	lvl = g_Levels[g_CurrentLevel];

	g_PlayerHasKey = FALSE;

	VDP_WriteLayout_GM2(lvl.layout, 0, 2, 32, 24);

	// Init player pawn
	ReinitPlayer(&g_PlayerPawn,
		         g_PlayerLayers, numberof(g_PlayerLayers),
				 lvl.start_x * 8, lvl.start_y * 8);

	// Init key pawn
	Pawn_Initialize(&g_KeyPawn,
		            g_KeyLayers, numberof(g_KeyLayers),
					KEY_SPRITE_ID, g_KeyAnimActions);
	Pawn_InitializePhysics(&g_KeyPawn, KeyPhysicsEvent, KeyPhysicsCollision, 16, 16);
	Pawn_SetPosition(&g_KeyPawn,
		             lvl.key_pos_x * 8, lvl.key_pos_y * 8);

	// Init enemy pawn
	Pawn_Initialize(&g_EnemyPawn,
		            g_EnemyLayers, numberof(g_EnemyLayers),
					ENEMY_SPRITE_ID, g_EnemyAnimActions);
	Pawn_InitializePhysics(&g_EnemyPawn, EnemyPhysicsEvent, EnemyPhysicsCollision, 16, 16);
	Pawn_SetPosition(&g_EnemyPawn,
		             lvl.enemy_x * 8, lvl.enemy_y * 8);

	if (lvl.enemy_x == 0 && lvl.enemy_y == 0) {
		Pawn_Disable(&g_EnemyPawn);
	}

	rewind_head = rewind_tail = rewind_count = 0;

	Game_SetState(State_Game);

	return TRUE;
}

bool State_Game()
{
	u8 act = ACTION_IDLE;

	// Switch Segment 4
	SetActiveSegment(4);
	UpdatePlayerMovement();
	act = UpdatePlayerAction(act);
	SetActiveSegment(0);

	Pawn_SetAction(&g_PlayerPawn, act);
	Pawn_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	Pawn_Update(&g_PlayerPawn);

	Pawn_SetAction(&g_KeyPawn, ACTION_IDLE);
	Pawn_Update(&g_KeyPawn);

	Pawn_SetAction(&g_EnemyPawn, ACTION_IDLE);
	Pawn_Update(&g_EnemyPawn);

	// Controlla la collisione tra giocatore e chiave
	if (doPawnsCollide(&g_PlayerPawn, &g_KeyPawn)) {
		TakeKey();
		Pawn_Disable(&g_KeyPawn);
	}

	// Controlla se il giocatore è sul terreno accidentato
	CheckPlayerOnDampers();

	// Controlla se il giocatore ha raggiunto l'uscita
	if (isPlayerAtExit()) {
		Game_SetState(State_ChangeLevel);
		return FALSE;
	}

	Pawn_Draw(&g_PlayerPawn);
	Pawn_Draw(&g_KeyPawn);
	Pawn_Draw(&g_EnemyPawn);

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

	DrawRewindGauge();

	u8 row8 = Keyboard_Read(8);
	if (IS_KEY_PRESSED(row8, KEY_SPACE)) {

		// Sostituisce i colori dello sprite principale
		ReinitPlayer(&g_PlayerPawn,
			         g_PlayerRewindLayers, numberof(g_PlayerRewindLayers),
					 x_rewind[rewind_head], y_rewind[rewind_head]);

		// Entra in modalità rewind
		Game_SetState(State_Rewind);
	}

	return TRUE;
}

bool State_Rewind()
{
	// Usciamo dal rewind in due casi: se non abbiamo più elementi, o se il
	// giocatore lo ha interrotto

	u8 row8 = Keyboard_Read(8);
	if (rewind_count == 0 || IS_KEY_RELEASED(row8, KEY_SPACE)) {

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

void main()
{
	DEBUG_INIT();

	Bios_SetKeyClick(FALSE);

	Game_SetState(State_Initialize);
	Game_Start(VDP_MODE_GRAPHIC2, FALSE);

	Bios_Exit(0);
}
