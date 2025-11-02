//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/state.h"
#include "game/pawn.h"
#include "debug.h"

#include "PawnData.h"

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

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Pawn sprite layers
const Pawn_Sprite g_SpriteLayers[] =
{//   X  Y  Pattern       Color            Option
	{ 0, 0, 0,            COLOR_BLACK,     0 },
	{ 0, 0, laySize,      COLOR_LIGHT_RED, 0 },
};

// Pawn sprite layers
const Pawn_Sprite g_SpriteRewindLayers[] =
{//   X  Y  Pattern       Color            Option
	{ 0, 0, 0,            COLOR_WHITE,     0 },
	{ 0, 0, laySize,      COLOR_GRAY,      0 },
};

// Idle animation frames
const Pawn_Frame g_FramesIdle[] =
{//   Pattern          Time  Function
	{  0 * sprSize,    250,  NULL },
	{ 11 * sprSize,     10,  NULL },
	{ 12 * sprSize,     10,  NULL },
	{ 11 * sprSize,     10,  NULL },
	{ 12 * sprSize,     10,  NULL },
	{ 11 * sprSize,     10,  NULL },
	{ 12 * sprSize,     10,  NULL },
	{ 11 * sprSize,     10,  NULL },
};

// Move animation frames
const Pawn_Frame g_FramesMoveRight[] =
{
	{ 1 * sprSize,	8,	NULL },
	{ 2 * sprSize,	8,	NULL },
	{ 3 * sprSize,	8,	NULL },
};

const Pawn_Frame g_FramesMoveLeft[] =
{
	{ 4 * sprSize,	8,	NULL },
	{ 5 * sprSize,	8,	NULL },
	{ 6 * sprSize,	8,	NULL },
};

// Jump animation frames
const Pawn_Frame g_FramesJumpRight[] =
{
	{ 3 * sprSize,	4,	NULL },
};

const Pawn_Frame g_FramesJumpLeft[] =
{
	{ 6 * sprSize,	4,	NULL },
};

// Fall animation frames
const Pawn_Frame g_FramesFallRight[] =
{
	{ 1 * sprSize,	4,	NULL },
};

const Pawn_Frame g_FramesFallLeft[] =
{
	{ 4 * sprSize,	4,	NULL },
};

// List of all player actions
const Pawn_Action g_AnimActions[] =
{//   Frames             Number                       Loop? Interrupt?
	{ g_FramesIdle,      numberof(g_FramesIdle),      TRUE, TRUE },
	{ g_FramesMoveRight, numberof(g_FramesMoveRight), TRUE, TRUE },
	{ g_FramesMoveLeft,  numberof(g_FramesMoveRight), TRUE, TRUE },
	{ g_FramesJumpRight, numberof(g_FramesJumpRight), TRUE, TRUE },
	{ g_FramesJumpLeft,  numberof(g_FramesJumpLeft),  TRUE, TRUE },
	{ g_FramesFallRight, numberof(g_FramesFallRight), TRUE, TRUE },
	{ g_FramesFallLeft,  numberof(g_FramesFallLeft),  TRUE, TRUE },
};

//=============================================================================
// SEGMENT 3, BANK 1
//=============================================================================
extern const unsigned char g_DataSprtLayer[];
extern const unsigned char g_DataMapGM2_Patterns[];
extern const unsigned char g_DataMapGM2_Colors[];
extern const unsigned char g_Level001[];
extern void LoadPatternAndColor();
extern void SetVRAMTable();
extern void InitializeSprite();


//=============================================================================
// SEGMENT 4, BANK 1
//=============================================================================
extern void UpdateMovement();
extern u8 UpdateAction(u8 act);

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
bool g_bMovingRight = FALSE;
bool g_bMovingLeft = FALSE;
bool g_bJumping = FALSE;
i8   g_VelocityY;
i8   g_DX = 0;
i8   g_DY = 0;

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
			VDP_Poke_GM2(i, 1, 64);
		else
			VDP_Poke_GM2(i, 1, 68);
	}
}

//=============================================================================
// PHYSICS
//=============================================================================

// Physics callback
void PhysicsEvent(u8 event, u8 tile)
{
	tile;
	switch (event)
	{
	case PAWN_PHYSICS_COL_DOWN: // Handle downward collision
		g_bJumping = FALSE;
		break;

	case PAWN_PHYSICS_COL_UP: // Handle upward collisions
		g_VelocityY = 0;
		break;

	case PAWN_PHYSICS_FALL: // Handle falling
		if (!g_bJumping)
		{
			g_bJumping = TRUE;
		}
		break;
	};
}

// Collision callback
bool PhysicsCollision(u8 tile)
{
	return ((tile >= 228 && tile <= 237) || tile == 248 || tile == 249);
}

//=============================================================================
// UTILITIES
//=============================================================================

void ReinitPawn(Pawn *pawn, Pawn_Sprite *spr_layers, u8 x, u8 y) {
	// Passa sempre g_SpriteLayers a numberof, tanto il numero di fotogrammi
	// non può comunque cambiare
	Pawn_Initialize(pawn, spr_layers, numberof(g_SpriteLayers), 0, g_AnimActions);
	Pawn_InitializePhysics(pawn, PhysicsEvent, PhysicsCollision, 16, 16);
	Pawn_SetPosition(pawn, x, y);
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
	InitializeSprite();	    // Initialize sprite and set 15 fotogrammi per adesso

	VDP_WriteLayout_GM2(g_Level001, 0, 2, 32, 24);

	SetActiveSegment(0);

	// Init player pawn
	ReinitPawn(&g_PlayerPawn, g_SpriteLayers, 70, 111);

	rewind_head = rewind_tail = rewind_count = 0;

	Game_SetState(State_Game);
	return FALSE;
}

bool State_Game()
{
	u8 act = ACTION_IDLE;

	// Switch Segment 4
	SetActiveSegment(4);
	UpdateMovement();
	act = UpdateAction(act);
	SetActiveSegment(0);

	Pawn_SetAction(&g_PlayerPawn, act);
	Pawn_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	Pawn_Update(&g_PlayerPawn);
	Pawn_Draw(&g_PlayerPawn);

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
		ReinitPawn(&g_PlayerPawn, g_SpriteRewindLayers, x_rewind[rewind_head], y_rewind[rewind_head]);

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
		ReinitPawn(&g_PlayerPawn, g_SpriteLayers, g_PlayerPawn.PositionX, g_PlayerPawn.PositionY);
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
