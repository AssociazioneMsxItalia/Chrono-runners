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
#include "content/levels/level04.h"
#include "content/levels/level05.h"
#include "content/levels/level06.h"
#include "content/levels/level07.h"
#include "content/levels/level08.h"

struct Platform plat1[] = {
   {23*8,  6*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    23*8,  6*8,  // min_x min_y
    23*8, 21*8}, // max_x max_y
};

struct Platform plat3[] = {
   {23*8, 13*8,  // pos_x pos_y
      -1,    0,  // dir_x dir_y
     7*8, 11*8,  // min_x min_y
    24*8, 11*8}, // max_x max_y

   {6*8,  19*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
     1*8, 19*8,  // min_x min_y
    11*8, 19*8}, // max_x max_y

   {24*8, 19*8,  // pos_x pos_y
      -1,    0,  // dir_x dir_y
    19*8, 19*8,  // min_x min_y
    30*8, 19*8}, // max_x max_y
};

struct Platform plat4[] = {
   {1*8,  7*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
    1*8,  5*8,  // min_x min_y
    1*8, 18*8}, // max_x max_y

   {29*8, 16*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    29*8,  5*8,  // min_x min_y
    29*8, 18*8}, // max_x max_y
};

struct Platform plat5[] = {
   {8*8,  20*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    8*8,  11*8,  // min_x min_y
    8*8,  21*8}, // max_x max_y

   {12*8, 15*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    12*8, 6*8,  // min_x min_y
    12*8, 16*8}, // max_x max_y

   {18*8, 6*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
     18*8, 6*8,  // min_x min_y
     18*8, 16*8}, // max_x max_y

	{22*8, 11*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
     22*8, 9*8,  // min_x min_y
     22*8, 21*8}, // max_x max_y
};

struct Platform plat6[] = {
   {29*8,  8*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8,  7*8,  // min_x min_y
    29*8, 14*8}, // max_x max_y
};

struct Platform plat7[] = {
   {11*8, 8*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
    11*8, 8*8,  // min_x min_y
    11*8, 17*8}, // max_x max_y

   {19*8,  20*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
    19*8,  9*8,  // min_x min_y
    19*8,  21*8}, // max_x max_y
};

struct Platform plat8[] = {
   {15*8, 19*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    15*8,  8*8,  // min_x min_y
    15*8, 20*8}, // max_x max_y
};

struct Level g_Levels[] =
{
    {
            1,  4, // start_x start_y
            1, 20, // end_x end_y
           27, 19, // key_x key_y
             0, 0, // enemy_x enemy_y
             0, 0, // crystal_x crystal_y
                2, // next_level
				1, // num_platforms
			plat1, // platforms
        g_Level01, // layout
    },
    {
           29,  6,
            9, 13,
           29, 11,
            0,  0,
            2, 11,
                3,
				0,
			 NULL,
        g_Level02,
    },
    {
             9, 8,
           21,  8,
           15, 17,
            0,  0,
            0,  0,
                4,
				3,
		    plat3,
        g_Level03,
    },
	{
            2, 21,
           28, 21,
           15, 11,
            0,  0,
           15, 21,
                5,
				2,
		    plat4,
        g_Level04,
    },
	{
            2,  4,
           28,  4,
           15, 20,
            0,  0,
            0,  0,
                6,
				4,
		    plat5,
        g_Level05,
    },
	{
            1, 20,
           29, 20,
            1,  3,
            0,  0,
            13, 3,
                7,
				1,
		    plat6,
        g_Level06,
    },
	{
            1, 10,
           29, 10,
           15, 21,
            0,  0,
            0,  0,
                8,
				2,
		    plat7,
        g_Level07,
    },
	{
            2, 20,
           15,  8,
           15, 14,
            0,  0,
           28, 20,
                1,
				1,
		    plat8,
        g_Level08,
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
void UpdatePlayerMovement(struct Platform *platform);
void UpdatePlayerAction();

void UpdateEnemyInput();
void UpdateEnemyMovement();
void UpdateEnemyAction();

void PrintGFXText(c8 *text, u8 x, u8 y);
void PrintGFXNumber(u8 number, u8 x, u8 y);

void DrawPlatforms();
struct Platform* isPlayerOnPlatform();

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

void UpdatePlayerMovement(struct Platform *platform) {

	g_PlayerMovingRight = g_PlayerMovingLeft = FALSE;

	u8 x_incr = 8;

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

	// Se preme su E non è su una piattaforma né sta già saltando, allora
	// esegue il salto
	if (g_PlayerInputUp && (platform != NULL || !g_PlayerJumping)) {
		// Inizia il salto
		g_PlayerJumping = TRUE;
		g_VelocityY = FORCE;
		// Ora NON è più su piattaforma
		platform = NULL;
	}

	// Gestisce esplicitamente la piattaforma. Entra qua solo se non abbiamo
	// appena saltato
	if (platform != NULL) {
		// Forza posizione Y sulla piattaforma
		g_PlayerPawn.PositionY = platform->pos_y - 16;

		// Siccome è sulla piattaforma, il personaggio NON sta saltando
		g_PlayerJumping = FALSE;

		// Aggiungi movimento piattaforma (moltiplicato per 8 per ottavi di pixel)
		g_mDX += (platform->dir_x * 8);
	}

	if (g_PlayerJumping)
	{
		UpdatePlayerGravity();
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
			tile = 40;
		}
		// '
		else if (c == 39) {
			tile = 38;
		}
		// ,
		else if (c == 44) {
			tile = 39;
		}
		// .
		else if (c == 46) {
			tile = 37;
		}
		// :
		else if (c == 58) {
			tile = 41;
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

struct Platform* isPlayerOnPlatform() {
	u8 np = g_Levels[g_CurrentLevel].num_platforms;
	struct Platform *platforms = g_Levels[g_CurrentLevel].platforms;

	for (u8 p=0; p < np; p++) {
		// -16 / +16 per permettere al giocatore di sfruttare lo spazio in
		// orizzontale della piattaforma fino all'ultimo pixel
		bool in_x = g_PlayerPawn.PositionX > platforms[p].pos_x - 16
		         && g_PlayerPawn.PositionX < platforms[p].pos_x + 16;

		// Perché -4? Serve per considerare anche l'area immediatamente sopra
		// la piattaforma come parte della piattaforma stessa. Senza questo
		// buffer, su una piattaforma che va verso il basso si vedrebbe
		// "saltare" il giocatore, perché ad ogni fotogramma gli manca la
		// terra sotto i piedi
		bool in_y = g_PlayerPawn.PositionY > platforms[p].pos_y - 16 - 4
		         && g_PlayerPawn.PositionY < platforms[p].pos_y;

		if (in_x && in_y)
		{
			return &platforms[p];
		}
	}

	return NULL;
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
