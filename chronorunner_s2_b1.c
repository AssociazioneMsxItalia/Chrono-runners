#include "msxgl.h"

#include "level_defs.h"

//=============================================================================
// LEVELS
//=============================================================================

#include "content/levels/3_1_punisher_18.h"
#include "content/levels/3_4_challenge_20.h"
#include "content/levels/4_1_easypeasy_30.h"
#include "content/levels/4_3_darkdescent_24.h"
#include "content/levels/4_4_twister_29.h"

extern struct Level g_Levels[];

void InitializeLevels_2() {
    u8 i = 10;
    g_Levels[i++] = level_punisher;
    g_Levels[i++] = level_challenge;
    g_Levels[i++] = level_easypeasy;
    g_Levels[i++] = level_darkdescent;
    g_Levels[i++] = level_twister;
}
