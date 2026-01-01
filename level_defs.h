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
    bool enabled;
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
    u8 field_state;   // 0 = no field, 1 = field active (Type 2), 2 = field active (Type 3)
    u8 field_timer;   // Timer for field duration or cooldown
    u8 field_cooldown; // Cooldown before next field
    // Energy field position (for Type 2 stationary and Type 3 projectile)
    u8 field_x;
    u8 field_y;
    i8 field_mDX;  // Accumulated movement for Type 3 projectiles (in eighths of pixel)
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
    const c8 *name;
};
