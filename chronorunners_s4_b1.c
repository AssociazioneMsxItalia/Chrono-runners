//=============================================================================
// SEGMENT: 4
// BANK: 1
// DESCRIPTION:
//=============================================================================

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"

//=============================================================================
// EXTERN MEMORY DATA
//=============================================================================
extern bool g_bMovingRight;
extern bool g_bMovingLeft;
extern bool g_bJumping;
extern i8   g_VelocityY;
extern u8   g_PrevRow8;
extern i8   g_DX;
extern i8   g_DY;
extern u8   FORCE;
extern u8   GRAVITY;


//=============================================================================
// FUNCTION
//=============================================================================

/**
 * @brief
 * @param void
 * @return void
 */
void UpdateMovment() {
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
}