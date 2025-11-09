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

#define FORCE	15
#define GRAVITY	1

//=============================================================================
// EXTERN MEMORY DATA
//=============================================================================
extern u8	g_PlayerAction;
extern bool g_PlayerMovingRight;
extern bool g_PlayerMovingLeft;
extern bool g_PlayerJumping;
extern bool g_PlayerDamped;
extern i8   g_VelocityY;
extern i8   g_mDX;
extern i8   g_DX;
extern i8   g_DY;

extern Pawn g_EnemyPawn;
extern u8	g_EnemyAction;
extern bool g_EnemyMovingRight;
extern bool g_EnemyMovingLeft;
extern i8   g_EnemymDX;
extern i8   g_EnemyDX;

//=============================================================================
// PROTOTYPES
//=============================================================================
void UpdatePlayerMovement();
void UpdatePlayerAction();

void UpdateEnemyMovement();
void UpdateEnemyAction();

//=============================================================================
// EXTERN PROTOTYPES
//=============================================================================
i16 abs(i16 a);

//=============================================================================
// FUNCTION
//=============================================================================

void UpdatePlayerMovement() {
	g_DX = 0;
	g_DY = 0;
	u8 row8 = Keyboard_Read(8);

	g_PlayerMovingRight = g_PlayerMovingLeft = FALSE;

	u8 x_incr = g_PlayerDamped ? 5 : 10;

	if (IS_KEY_PRESSED(row8, KEY_RIGHT))
	{
		g_mDX += x_incr;
		g_PlayerMovingRight = TRUE;
	}
	else if (IS_KEY_PRESSED(row8, KEY_LEFT))
	{
		g_mDX -= x_incr;
		g_PlayerMovingLeft = TRUE;
	}

	// Lo spostamento in orizzontale è espresso in decimi di pixel. In questo modo
	// può rallentare fino alla velocità di 0.1 pixel per fotogramma
	if (abs(g_mDX) >= 10) {
		g_DX = g_mDX / 10;

		while (g_mDX >= 10) {
			g_mDX -= 10;
		}

		while (g_mDX <= -10) {
			g_mDX += 10;
		}
	}

	if (g_PlayerJumping)
	{
		g_DY -= g_VelocityY / 4;

		g_VelocityY -= GRAVITY;
		if (g_VelocityY < -FORCE)
			g_VelocityY = -FORCE;
	}
	else if (IS_KEY_PRESSED(row8, KEY_UP))
	{
		g_PlayerJumping = TRUE;
		g_VelocityY = FORCE;
	}
}

void UpdatePlayerAction() {
	g_PlayerAction = ACTION_IDLE;
	if (g_PlayerJumping && (g_VelocityY >= 0))
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

void UpdateEnemyMovement() {

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

	if (g_EnemyMovingLeft) {
		g_EnemymDX -= 3;
	} else if (g_EnemyMovingRight) {
		g_EnemymDX += 3;
	}

	// XXX: unificare questo codice con la versione sopra
	g_EnemyDX = 0;
	if (abs(g_EnemymDX) >= 10) {
		g_EnemyDX = g_EnemymDX / 10;

		while (g_EnemymDX >= 10) {
			g_EnemymDX -= 10;
		}

		while (g_EnemymDX <= -10) {
			g_EnemymDX += 10;
		}
	}
}

void UpdateEnemyAction() {
	g_EnemyAction = ACTION_IDLE;
	if (g_EnemyMovingRight) {
		g_EnemyAction = ACTION_MOVERIGHT;
	} else if (g_EnemyMovingLeft) {
		g_EnemyAction = ACTION_MOVELEFT;
	}
}
