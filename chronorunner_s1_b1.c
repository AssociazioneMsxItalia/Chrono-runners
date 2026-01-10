#include "msxgl.h"

#include "level_defs.h"

//=============================================================================
// LEVELS
//=============================================================================

#include "content/levels/1_1_ronin_16.h"
#include "content/levels/1_2_thefactory_07.h"
#include "content/levels/1_3_theprison_06.h"
#include "content/levels/1_4_suicidepit_19.h"
#include "content/levels/2_1_thetower_09.h"
#include "content/levels/2_2_thechasm_08.h"
#include "content/levels/2_3_sf_17.h"
#include "content/levels/2_4_whatliesbeneath_22.h"
#include "content/levels/2_5_pipesandrobots_23.h"
#include "content/levels/2_6_crystalchem_31.h"

extern struct Level g_Levels[];

void InitializeLevels_1() {
	u8 i = 0;
	g_Levels[i++] = level_ronin;
	g_Levels[i++] = level_thefactory;
	g_Levels[i++] = level_theprison;
	g_Levels[i++] = level_suicidepit;
	g_Levels[i++] = level_thetower;
	g_Levels[i++] = level_thechasm;
	g_Levels[i++] = level_sf;
	g_Levels[i++] = level_whatliesbeneath;
	g_Levels[i++] = level_pipesandrobots;
	g_Levels[i++] = level_crystalchem;
}
