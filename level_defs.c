#include "level_defs.h"

#include "content/levels/level01.h"
#include "content/levels/level02.h"
#include "content/levels/level03.h"

struct Level g_Levels[] =
{
    {
            1, 19, // start_x start_y
           29, 19, // end_x end_y
             2, 4, // key_x key_y
             0, 0, // enemy_x enemy_y
             0, 0, // crystal_x crystal_y
                2, // next_level
        g_Level01, // layout
    },
    {
            1, 19,
            29, 4,
           15, 14,
             0, 0,
             0, 0,
                3,
        g_Level02,
    },
    {
             1, 4,
            1, 19,
            22, 4,
           15, 19,
           16, 11,
                1,
        g_Level03,
    },
};
