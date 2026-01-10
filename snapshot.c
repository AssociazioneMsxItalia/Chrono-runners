#include "msxgl.h"
#include "snapshot.h"
#include "level_defs.h"

//=============================================================================
// SNAPSHOT BUFFER STORAGE
//=============================================================================

// Circular buffer for snapshots
GameSnapshot g_SnapshotBuffer[SNAPSHOT_BUFFER_SIZE];

// Circular buffer management
u8 g_SnapshotHead = 0;
u8 g_SnapshotTail = 0;
u8 g_SnapshotCount = 0;

//=============================================================================
// SNAPSHOT FUNCTIONS
//=============================================================================

/**
 * @brief Initialize the snapshot system
 *
 * Resets the circular buffer to empty state
 */
void Snapshot_Initialize() {
    g_SnapshotHead = 0;
    g_SnapshotTail = 0;
    g_SnapshotCount = 0;
}

/**
 * @brief Capture current game state into the snapshot buffer
 *
 * This function is called every frame during gameplay to record the state
 * of all dynamic objects. Uses a circular buffer with the same strategy
 * as the player position rewind.
 *
 * @param lvl Pointer to current level
 * @param player_x Player X position
 * @param player_y Player Y position
 * @param player_frame Player animation frame
 */
void Snapshot_Capture(struct Level *lvl, u8 player_x, u8 player_y, u8 player_frame) {
    // Get pointer to the snapshot slot we're about to fill
    GameSnapshot *snap = &g_SnapshotBuffer[g_SnapshotHead];

    // Capture player state
    snap->player_x = player_x;
    snap->player_y = player_y;
    snap->player_frame = player_frame;

    // Capture platform positions
    for (u8 p = 0; p < lvl->num_platforms && p < MAX_PLATFORMS; p++) {
        snap->platform_x[p] = lvl->platforms[p].pos_x;
        snap->platform_y[p] = lvl->platforms[p].pos_y;
    }

    // Capture enemy states
    for (u8 e = 0; e < lvl->num_enemies && e < MAX_ENEMIES; e++) {
        struct Enemy *enemy = &lvl->enemies[e];
        snap->enemy_x[e] = enemy->pos_x;
        snap->enemy_y[e] = enemy->pos_y;
        snap->enemy_dir[e] = enemy->dir_x;
        snap->enemy_stunned[e] = enemy->stunned_timer;
        snap->enemy_field_state[e] = enemy->field_state;
        snap->enemy_field_x[e] = enemy->field_x;
        snap->enemy_field_y[e] = enemy->field_y;
    }

    // Advance head pointer (circular)
    g_SnapshotHead = (g_SnapshotHead + 1) % SNAPSHOT_BUFFER_SIZE;

    // Update circular buffer state
    if (g_SnapshotCount == SNAPSHOT_BUFFER_SIZE) {
        // Buffer was full, consume oldest snapshot
        g_SnapshotTail = (g_SnapshotTail + 1) % SNAPSHOT_BUFFER_SIZE;
    } else {
        // Buffer still filling up
        g_SnapshotCount++;
    }
}

/**
 * @brief Restore game state from a snapshot
 *
 * This function is called during rewind to restore all game objects to their
 * state at a previous frame.
 *
 * @param lvl Pointer to current level
 * @param snapshot_index Index in the circular buffer to restore from
 * @param out_player_x Output: restored player X position
 * @param out_player_y Output: restored player Y position
 * @param out_player_frame Output: restored player animation frame
 */
void Snapshot_Restore(struct Level *lvl, u8 snapshot_index, u8 *out_player_x, u8 *out_player_y, u8 *out_player_frame) {
    // Get pointer to the snapshot we're restoring
    GameSnapshot *snap = &g_SnapshotBuffer[snapshot_index];

    // Restore player state
    *out_player_x = snap->player_x;
    *out_player_y = snap->player_y;
    *out_player_frame = snap->player_frame;

    // Restore platform positions
    for (u8 p = 0; p < lvl->num_platforms && p < MAX_PLATFORMS; p++) {
        lvl->platforms[p].pos_x = snap->platform_x[p];
        lvl->platforms[p].pos_y = snap->platform_y[p];
    }

    // Restore enemy states
    for (u8 e = 0; e < lvl->num_enemies && e < MAX_ENEMIES; e++) {
        lvl->enemies[e].pos_x = snap->enemy_x[e];
        lvl->enemies[e].pos_y = snap->enemy_y[e];
        lvl->enemies[e].dir_x = snap->enemy_dir[e];
        lvl->enemies[e].stunned_timer = snap->enemy_stunned[e];
        lvl->enemies[e].field_state = snap->enemy_field_state[e];
        lvl->enemies[e].field_x = snap->enemy_field_x[e];
        lvl->enemies[e].field_y = snap->enemy_field_y[e];
    }
}

/**
 * @brief Get number of available rewind steps
 */
u8 Snapshot_GetRewindCount() {
    return g_SnapshotCount;
}

/**
 * @brief Perform one rewind step (moves head backward)
 * @return New snapshot index to use for restoration
 */
u8 Snapshot_RewindStep() {
    g_SnapshotHead = (g_SnapshotHead - 1 + SNAPSHOT_BUFFER_SIZE) % SNAPSHOT_BUFFER_SIZE;
    g_SnapshotCount--;
    return g_SnapshotHead;
}
