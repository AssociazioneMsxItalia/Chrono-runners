//=============================================================================
// SEGMENT: 4
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
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
extern bool g_PlayerMovingRight;
extern bool g_PlayerMovingLeft;
extern bool g_PlayerJumping;
extern bool g_PlayerDamped;
extern i8   g_VelocityY;
extern i8   g_mDX;
extern i8   g_DX;
extern i8   g_DY;

//=============================================================================
// PROTOTYPES
//=============================================================================
void UpdatePlayerMovement();
u8 UpdatePlayerAction(u8 act);

//=============================================================================
// EXTERN PROTOTYPES
//=============================================================================
i16 abs(i16 a);

//=============================================================================
// FUNCTION
//=============================================================================

/**
 * @brief
 * @param void
 * @return void
 */
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


/**
 * @brief Update action Pawn
 * @param u8 Current action
 * @return new action
 */
u8 UpdatePlayerAction(u8 act) {
	if (g_PlayerJumping && (g_VelocityY >= 0))
	{
		if (g_PlayerMovingRight)
			act = ACTION_JUMPRIGHT;
		else
			act = ACTION_JUMPLEFT;
	}
	else if (g_PlayerJumping) {
		if (g_PlayerMovingRight)
			act = ACTION_FALLRIGHT;
		else
			act = ACTION_FALLLEFT;
	}
	else if (g_PlayerMovingRight)
		act = ACTION_MOVERIGHT;
	else if (g_PlayerMovingLeft)
		act = ACTION_MOVELEFT;

	return act;
}