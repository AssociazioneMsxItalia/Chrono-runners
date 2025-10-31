//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/state.h"
#include "game/pawn.h"

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

// Idle animation frames
const Pawn_Frame g_FramesIdle[] =
{//   Pattern       Time Function
	{ 0 * sprSize, 	48,  NULL },
};

// Move animation frames
const Pawn_Frame g_FramesMoveRight[] =
{
	{ 1 * sprSize,	4,	NULL },
	{ 2 * sprSize,	4,	NULL },
	{ 3 * sprSize,	4,	NULL },
};

const Pawn_Frame g_FramesMoveLeft[] =
{
	{ 4 * sprSize,	4,	NULL },
	{ 5 * sprSize,	4,	NULL },
	{ 6 * sprSize,	4,	NULL },
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
const Pawn_Frame g_FramesFall[] =
{
	{ 13 * sprSize,	4,	NULL },
	{ 14 * sprSize,	4,	NULL },
};

// List of all player actions
const Pawn_Action g_AnimActions[] =
{//   Frames             Number                       Loop? Interrupt?
	{ g_FramesIdle,      numberof(g_FramesIdle),      TRUE, TRUE },
	{ g_FramesMoveRight, numberof(g_FramesMoveRight), TRUE, TRUE },
	{ g_FramesMoveLeft,  numberof(g_FramesMoveRight), TRUE, TRUE },
	{ g_FramesJumpRight, numberof(g_FramesJumpRight), TRUE, TRUE },
	{ g_FramesJumpLeft,  numberof(g_FramesJumpLeft),  TRUE, TRUE },
	{ g_FramesFall,      numberof(g_FramesFall),      TRUE, TRUE },
};

//=============================================================================
// SEGMENT 3, BANK 1
//=============================================================================
extern const unsigned char g_DataSprtLayer[];
extern const unsigned char g_DataMapGM2_Names[];
extern const unsigned char g_DataMapGM2_Patterns[];
extern const unsigned char g_DataMapGM2_Colors[];
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
u8   g_LastEvent = 0;
u8   FORCE = 15;
u8   GRAVITY = 1;

// 5 secondi * 50 fps per coordinata
#define rewindSize 250
u8 x_rewind[rewindSize];
u8 y_rewind[rewindSize];
u8 f_rewind[rewindSize];
u8 rewind_start;
u8 rewind_end;
i16 rewind_ptr;

bool isRewindCharged() {
	return (rewind_end + 1) % rewindSize == rewind_start;
}

void rewindReset() {
	rewind_start = rewind_end = 0;
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
	case PAWN_PHYSICS_BORDER_LEFT:
	case PAWN_PHYSICS_BORDER_RIGHT:
		g_LastEvent = event;
		break;

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
	return (tile == 214 || tile == 215
		 || tile == 216 || tile == 217 || tile == 218
		 || tile == 221);
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

	VDP_WriteLayout_GM2(g_DataMapGM2_Names, 0, 0, 32, 24);

	SetActiveSegment(0);

	// Init player pawn
	Pawn_Initialize(&g_PlayerPawn, g_SpriteLayers, numberof(g_SpriteLayers), 0, g_AnimActions);
	Pawn_SetPosition(&g_PlayerPawn, 70, 111);
	Pawn_InitializePhysics(&g_PlayerPawn, PhysicsEvent, PhysicsCollision, 16, 16);

	rewindReset();

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

	// Array pieno, consuma la posizione acquisita più vecchia
	if (isRewindCharged()) {
		rewind_start = (rewind_start + 1) % rewindSize;
		VDP_Poke_GM2(0, 0, 57);
	} else {
		VDP_Poke_GM2(0, 0, 59);
	}

	rewind_end = (rewind_end + 1) % rewindSize;
	x_rewind[rewind_end] = g_PlayerPawn.PositionX;
	y_rewind[rewind_end] = g_PlayerPawn.PositionY;
	f_rewind[rewind_end] = g_PlayerPawn.AnimFrame;

	u8 row8 = Keyboard_Read(8);
	if (IS_KEY_PRESSED(row8, KEY_SPACE) && isRewindCharged()) {
		rewind_ptr = (rewind_end - 1) % rewindSize;
		VDP_Poke_GM2(0, 0, 65);
		Game_SetState(State_Rewind);
	}

	return TRUE;
}

bool State_Rewind()
{
	Pawn_SetPosition(&g_PlayerPawn, x_rewind[rewind_ptr], y_rewind[rewind_ptr]);
	// Per forzare il fotogramma, imposta a mano il flag di aggiornamento pattern
	g_PlayerPawn.Update |= PAWN_UPDATE_PATTERN;
	g_PlayerPawn.AnimFrame = f_rewind[rewind_ptr];
	Pawn_Draw(&g_PlayerPawn);

	if (rewind_ptr <= 0) {
		rewind_ptr = rewindSize - 1;

	} else {

		// Definisce di quanto spingere il rewind a questo passo, in funzione
		// di quanto ancora distiamo dalla fine (attenzione: cast a u8
		// necessario per evitare di dividere un numero negativo)
		u8 delta = (u8)(rewind_ptr - rewind_end) / 10;
		if (delta == 0)
			delta = 1;

		// Nuova posizione a cui stiamo per saltare
		i16 new_ptr = (rewind_ptr - delta) % rewindSize;

		// Ha superato la fine del buffer di rewind, siamo arrivati
		if (rewind_ptr >= rewind_end && new_ptr <= rewind_end) {

			// Svuota il rewind
			rewindReset();

			Game_SetState(State_Game);
		} else {
			rewind_ptr = new_ptr;
		}
	}

	return TRUE;
}

//=============================================================================
// MAIN LOOP
//=============================================================================

void main()
{
	Bios_SetKeyClick(FALSE);

	Game_SetState(State_Initialize);
	Game_Start(VDP_MODE_GRAPHIC2, FALSE);

	Bios_Exit(0);
}
