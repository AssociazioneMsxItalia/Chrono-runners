//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/state.h"
#include "game/pawn.h"

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

// Actions id
enum ANIM_ACTION_ID
{
	ACTION_IDLE = 0,
	ACTION_MOVERIGHT,
	ACTION_MOVELEFT,
	ACTION_JUMPRIGHT,
	ACTION_JUMPLEFT,
	ACTION_FALL,
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
u8   g_PrevRow8 = 0xFF;
i8   g_DX = 0;
i8   g_DY = 0;
u8   g_LastEvent = 0;
u8   FORCE = 15;
u8   GRAVITY = 1;

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
	// VRAM Tables Address
	VDP_SetLayoutTable(0x3800);
	VDP_SetColorTable(0x2000);
	VDP_SetPatternTable(0x0000);
	VDP_SetSpritePatternTable(0x1800);
	VDP_SetSpriteAttributeTable(0x3E00);

	// Setup video
	VDP_SetColor(0xF0);
	VDP_ClearVRAM();

	// Tiles and nametable data
	SetActiveSegment(3);
	VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 255, 0);
	VDP_LoadColor_GM2(g_DataMapGM2_Colors, 255, 0);
	VDP_WriteLayout_GM2(g_DataMapGM2_Names, 0, 0, 32, 24);

	// Initialize sprite
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);

	// 15 fotogrammi per adesso
	VDP_LoadSpritePattern(g_DataSprtLayer, 0, 15 * sprSize);
	SetActiveSegment(0);

	// Init player pawn
	Pawn_Initialize(&g_PlayerPawn, g_SpriteLayers, numberof(g_SpriteLayers), 0, g_AnimActions);
	Pawn_SetPosition(&g_PlayerPawn, 70, 111);
	Pawn_InitializePhysics(&g_PlayerPawn, PhysicsEvent, PhysicsCollision, 16, 16);

	Game_SetState(State_Game);
	return FALSE;
}

bool State_Game()
{
	u8 act = ACTION_IDLE;
	if (g_bJumping && (g_VelocityY >= 0))
	{
		if (g_bMovingRight)
			act = ACTION_JUMPRIGHT;
		else
			act = ACTION_JUMPLEFT;
	}
	else if (g_bJumping)
		act = ACTION_FALL;
	else if (g_bMovingRight)
		act = ACTION_MOVERIGHT;
	else if (g_bMovingLeft)
		act = ACTION_MOVELEFT;

	// Blocca su bordo sinistro
	if (g_PlayerPawn.PositionX == 0 && g_DX < 0)
		g_DX = 0;

	// Blocca su bordo destro
	if (g_PlayerPawn.PositionX >= 240 && g_DX > 0)
		g_DX = 0;

	Pawn_SetAction(&g_PlayerPawn, act);
	Pawn_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	Pawn_Update(&g_PlayerPawn);
	Pawn_Draw(&g_PlayerPawn);

	// Update movement
	g_DX = 0;
	g_DY = 0;
	u8 row8 = Keyboard_Read(8);

	g_bMovingRight = g_bMovingLeft = FALSE;

	if (IS_KEY_PRESSED(row8, KEY_RIGHT))
	{
		g_DX++;
		g_bMovingRight = TRUE;
	}
	else if (IS_KEY_PRESSED(row8, KEY_LEFT))
	{
		g_DX--;
		g_bMovingLeft = TRUE;
	}

	if (g_bJumping)
	{
		g_DY -= g_VelocityY / 4;

		g_VelocityY -= GRAVITY;
		if (g_VelocityY < -FORCE)
			g_VelocityY = -FORCE;

	}
	else if (IS_KEY_PRESSED(row8, KEY_SPACE) || IS_KEY_PRESSED(row8, KEY_UP))
	{
		g_bJumping = TRUE;
		g_VelocityY = FORCE;
	}

	g_PrevRow8 = row8;

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
