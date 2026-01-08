#include "msxgl.h"
#include "snapshot.h"
#include "level_defs.h"

//=============================================================================
// SNAPSHOT BUFFER STORAGE
//=============================================================================

// Circular buffer for snapshots (~14.6 KB)
GameSnapshot g_SnapshotBuffer[SNAPSHOT_BUFFER_SIZE];

// Circular buffer management (same pattern as player rewind)
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
 */
void Snapshot_Capture(struct Level *lvl) {
    // Get pointer to the snapshot slot we're about to fill
    GameSnapshot *snap = &g_SnapshotBuffer[g_SnapshotHead];

    // Capture platform positions
    for (u8 p = 0; p < lvl->num_platforms && p < MAX_PLATFORMS; p++) {
        snap->platform_x[p] = lvl->platforms[p].pos_x;
        snap->platform_y[p] = lvl->platforms[p].pos_y;
    }

    // Capture enemy states
    for (u8 e = 0; e < lvl->num_enemies && e < MAX_ENEMIES; e++) {
        snap->enemy_x[e] = lvl->enemies[e].pos_x;
        snap->enemy_y[e] = lvl->enemies[e].pos_y;
        snap->enemy_dir[e] = lvl->enemies[e].dir_x;
        snap->enemy_stunned[e] = lvl->enemies[e].stunned_timer;
        snap->enemy_field_state[e] = lvl->enemies[e].field_state;
        snap->enemy_field_x[e] = lvl->enemies[e].field_x;
        snap->enemy_field_y[e] = lvl->enemies[e].field_y;
    }

    // Capture mine enabled states (packed into bits)
    snap->mine_enabled = 0;
    for (u8 m = 0; m < lvl->num_mines && m < MAX_MINES; m++) {
        if (lvl->mines[m].enabled) {
            snap->mine_enabled |= (1 << m);
        }
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
 */
void Snapshot_Restore(struct Level *lvl, u8 snapshot_index) {
    // Get pointer to the snapshot we're restoring
    GameSnapshot *snap = &g_SnapshotBuffer[snapshot_index];

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

    // Restore mine enabled states (unpacked from bits)
    for (u8 m = 0; m < lvl->num_mines && m < MAX_MINES; m++) {
        lvl->mines[m].enabled = (snap->mine_enabled >> m) & 1;
    }
}
