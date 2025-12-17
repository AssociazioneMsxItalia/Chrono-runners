//=============================================================================
// SEGMENT: 4
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "game/pawn.h"
#include "debug.h"

#include "PawnData.h"
#include "math_utils.h"
#include "level_defs.h"
#include "sprite_defs.h"

//=============================================================================
// LEVELS
//=============================================================================

#include "content/levels/level01.h"
#include "content/levels/level02.h"
#include "content/levels/level03.h"

struct Platform plat2[] = {
    {1*8,  9*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
     1*8,  6*8,  // min_x min_y
     1*8, 16*8}, // max_x max_y

    {15*8, 12*8,
       -1,    0,
      6*8, 12*8,
     23*8, 12*8},

    {29*8, 19*8,
        0,   -1,
     29*8, 12*8,
     29*8, 21*8},
};

struct Level g_Levels[] =
{
    {
            1, 19, // start_x start_y
           29, 19, // end_x end_y
             2, 4, // key_x key_y
             0, 0, // enemy_x enemy_y
             0, 0, // crystal_x crystal_y
                3, // next_level
				0,
			 NULL,
        g_Level01, // layout
    },
    {
            1, 19,
            29, 4,
           15, 14,
             0, 0,
             0, 0,
                1,
				3,
			plat2,
        g_Level02,
    },
    {
             1, 4,
            1, 19,
            22, 4,
           15, 19,
           16, 11,
                2,
				0,
			 NULL,
        g_Level03,
    },
};


//=============================================================================
// MEMORY DATA
//=============================================================================

//=============================================================================
// EXTERN MEMORY DATA
//=============================================================================

extern u8 g_CurrentLevel;

extern Pawn g_PlayerPawn;
extern u8	g_PlayerAction;
extern bool g_PlayerMovingRight;
extern bool g_PlayerMovingLeft;
extern bool g_PlayerJumping;
extern bool g_PlayerDamped;
extern bool g_PlayerDying;
extern bool g_PlayerInputRight;
extern bool g_PlayerInputLeft;
extern bool g_PlayerInputUp;
extern i8   g_VelocityY;
extern i8   g_mDX;
extern i8	g_mDY;
extern i8   g_DX;
extern i8   g_DY;

extern Pawn g_EnemyPawn;
extern bool g_EnemyEnabled;
extern u8	g_EnemyAction;
extern bool g_EnemyMovingRight;
extern bool g_EnemyMovingLeft;
extern i8   g_EnemymDX;
extern i8   g_EnemyDX;

//=============================================================================
// PROTOTYPES
//=============================================================================
void UpdatePlayerInput();
void UpdatePlayerGravity();
void UpdatePlayerMovement();
void UpdatePlayerAction();

void UpdateEnemyInput();
void UpdateEnemyMovement();
void UpdateEnemyAction();

void PrintGFXText(c8 *text, u8 x, u8 y);
void PrintGFXNumber(u8 number, u8 x, u8 y);

void DrawPlatforms();
i8 isPlayerOnPlatform();

//=============================================================================
// EXTERN PROTOTYPES
//=============================================================================

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

	if (IS_KEY_PRESSED(row8, KEY_RIGHT)) {
		g_PlayerInputRight = TRUE;
	} else if (IS_KEY_PRESSED(row8, KEY_LEFT)) {
		g_PlayerInputLeft = TRUE;
	}

	if (IS_KEY_PRESSED(row8, KEY_UP)) {
		g_PlayerInputUp = TRUE;
	}
}

void UpdatePlayerGravity() {
	g_mDY -= g_VelocityY;

	g_VelocityY -= GRAVITY;
	if (g_VelocityY < -FORCE)
		g_VelocityY = -FORCE;
}

void UpdatePlayerMovement() {

	g_PlayerMovingRight = g_PlayerMovingLeft = FALSE;

	u8 x_incr = g_PlayerDamped ? 2 : 8;

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

	if (g_PlayerJumping)
	{
		UpdatePlayerGravity();
	}
	else if (g_PlayerInputUp)
	{
		g_PlayerJumping = TRUE;
		g_VelocityY = FORCE;
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

void UpdateEnemyInput() {
	if (!g_EnemyMovingRight && !g_EnemyMovingLeft) {
		// Situazione iniziale, se non si muove partiamo verso sinistra
		g_EnemyMovingLeft = TRUE;
	} else if (g_EnemyMovingLeft) {
		// Se sta andando verso sinistra ed è arrivato a un certo punto, cambia direzione
		if (g_EnemyPawn.PositionX < 30) {
			g_EnemyMovingLeft = FALSE;
			g_EnemyMovingRight = TRUE;
		}
	} else if (g_EnemyMovingRight) {
		// Se sta andando verso destra ed è arrivato a un certo punto, cambia direzione
		if (g_EnemyPawn.PositionX > 170) {
			g_EnemyMovingLeft = TRUE;
			g_EnemyMovingRight = FALSE;
		}
	}
}

void UpdateEnemyMovement() {

	if (g_EnemyMovingLeft) {
		g_EnemymDX -= 2;
	} else if (g_EnemyMovingRight) {
		g_EnemymDX += 2;
	}

	g_EnemyDX = GetDPos(&g_EnemymDX);
}

void UpdateEnemyAction() {
	g_EnemyAction = ACTION_IDLE;
	if (g_EnemyMovingRight) {
		g_EnemyAction = ACTION_MOVERIGHT;
	} else if (g_EnemyMovingLeft) {
		g_EnemyAction = ACTION_MOVELEFT;
	}
}

void PrintGFXText(c8 *text, u8 x, u8 y) {
	while (*text != 0) {
		c8 c = *text;
		u8 tile = 0;
		// Numero
		if (c >= 48 && c <= 57) {
			tile = 1 + (c - 48);
		}
		// Lettera
		else if (c >= 65 && c <= 90) {
			tile = 11 + (c - 65);
		}
		// "
		else if (c == 34) {
			tile = 39;
		}
		// '
		else if (c == 39) {
			tile = 38;
		}
		// .
		else if (c == 46) {
			tile = 37;
		}

		VDP_Poke_GM2(x, y, tile);

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

i8 isPlayerOnPlatform() {
	u8 np = g_Levels[g_CurrentLevel].num_platforms;
	struct Platform *platforms = g_Levels[g_CurrentLevel].platforms;

	for (u8 p=0; p < np; p++) {
		bool in_x = g_PlayerPawn.PositionX > platforms[p].pos_x - 16
		         && g_PlayerPawn.PositionX < platforms[p].pos_x + 16;

		bool in_y = g_PlayerPawn.PositionY > platforms[p].pos_y - 16
		         && g_PlayerPawn.PositionY < platforms[p].pos_y;

		if (in_x && in_y)
		{
			return p;
		}
	}

	return -1;
}

void UpdatePlatforms() {
	u8 np = g_Levels[g_CurrentLevel].num_platforms;
	struct Platform *platforms = g_Levels[g_CurrentLevel].platforms;

	for (u8 p=0; p < np; p++) {
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

void DrawPlatforms() {
	u8 np = g_Levels[g_CurrentLevel].num_platforms;
	struct Platform *platforms = g_Levels[g_CurrentLevel].platforms;

	for (u8 p=0; p < np; p++) {
		u8 index = PLATFORM_SPRITE_BASE_ID + p;
		u8 shape = platforms[p].dir_x != 0 ? PLATFORMH_PATTERN_OFFSET : PLATFORMV_PATTERN_OFFSET;
		VDP_SetSpriteSM1(index, platforms[p].pos_x, platforms[p].pos_y, shape, COLOR_BLACK);	}
}
