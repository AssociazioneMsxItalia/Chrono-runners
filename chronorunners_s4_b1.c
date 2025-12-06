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

//=============================================================================
// MEMORY DATA
//=============================================================================

#define GRAVITY	5
#define FORCE	75

//=============================================================================
// EXTERN MEMORY DATA
//=============================================================================
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
void UpdatePlayerGravity(u8 gravity, u8 force);
void UpdatePlayerMovement();
void UpdatePlayerAction();

void UpdateEnemyInput();
void UpdateEnemyMovement();
void UpdateEnemyAction();

void PrintGFXText(c8 *text, u8 x, u8 y);
void PrintGFXNumber(u8 number, u8 x, u8 y);

//=============================================================================
// EXTERN PROTOTYPES
//=============================================================================
i16 abs(i16 a);

//=============================================================================
// FUNCTION
//=============================================================================

i8 GetDPos(i8* m) {
	i8 rv = (*m) / 10;

	while ((*m) >= 10)
		(*m) -= 10;

	while ((*m) <= -10)
		(*m) += 10;

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

void UpdatePlayerGravity(u8 gravity, u8 force) {
	g_mDY -= g_VelocityY / 2;

	g_VelocityY -= gravity;
	if (g_VelocityY < -force)
		g_VelocityY = -force;
}

void UpdatePlayerMovement() {

	g_PlayerMovingRight = g_PlayerMovingLeft = FALSE;

	u8 x_incr = g_PlayerDamped ? 3 : 10;

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
		UpdatePlayerGravity(GRAVITY, FORCE);
	}
	else if (g_PlayerInputUp)
	{
		g_PlayerJumping = TRUE;
		g_VelocityY = FORCE;
	}

	// Gli spostamenti sono espressi in decimi di pixel. In questo modo può
	// rallentare fino alla velocità di 0.1 pixel per fotogramma
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
		g_EnemymDX -= 3;
	} else if (g_EnemyMovingRight) {
		g_EnemymDX += 3;
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
