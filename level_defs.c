#include "level_defs.h"

#include "content/levels/level01.h"
#include "content/levels/level03.h"

struct Level g_Levels[] =
{//  start_x, start_y, end_x, end_y, key_pos_x, key_pos_y, next_level, layout
    {1,       19,      29,    19,    2,         4,         2,          g_Level01},
    {1,       4,        1,    19,    22,        4,         1,          g_Level03},
};
