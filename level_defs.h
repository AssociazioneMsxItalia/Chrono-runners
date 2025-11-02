#pragma once
#include "core.h"

struct Level
{
    u8 start_x;
    u8 start_y;
    u8 next_level;
    u8 *layout;
};

extern struct Level g_Levels[];
