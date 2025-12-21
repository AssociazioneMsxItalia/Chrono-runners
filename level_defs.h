#pragma once

struct Platform {
    u8 pos_x;
    u8 pos_y;
    i8 dir_x;
    i8 dir_y;
    u8 min_x;
    u8 min_y;
    u8 max_x;
    u8 max_y;
};

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
    u8 num_platforms;
    struct Platform *platforms;
    u8 *layout;
};
