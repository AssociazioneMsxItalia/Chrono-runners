#pragma once
#include "core.h"

struct Level
{
    u8 start_x;
    u8 start_y;
    u8 end_x;
    u8 end_y;
    u8 key_x;
    u8 key_y;
    u8 enemy_x;
    u8 enemy_y;
    u8 crystal_x;
    u8 crystal_y;
    u8 next_level;
    u8 *layout;
};

extern struct Level g_Levels[];
