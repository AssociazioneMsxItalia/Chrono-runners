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

struct Mine {
    u8 pos_x;
    u8 pos_y;
};

struct Enemy {
    u8 pos_x;
    u8 pos_y;
    i8 dir_x;      // Movement direction (-1 left, 1 right)
    u8 min_x;      // Left boundary
    u8 max_x;      // Right boundary
    u8 speed;      // Movement speed (added to mDX each frame)
    u8 type;       // Enemy type (0-3 for different sprite sets)
    i8 mDX;        // Accumulated movement in eighths of pixel
    u8 stunned_timer; // Frames remaining while stunned (0 = not stunned)
};

struct Level
{
    u8 start_x;
    u8 start_y;
    u8 end_x;
    u8 end_y;
    u8 key_x;
    u8 key_y;
    u8 crystal_x;
    u8 crystal_y;
    u8 next_level;
    u8 num_platforms;
    struct Platform *platforms;
    u8 num_mines;
    struct Mine *mines;
    u8 num_enemies;
    struct Enemy *enemies;
    u8 *layout;
};