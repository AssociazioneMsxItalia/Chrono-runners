#pragma once

#include "msxgl.h"

//=============================================================================
// FORWARD DECLARATIONS
//=============================================================================

// Forward declare Level to avoid circular dependency
struct Level;

//=============================================================================
// SNAPSHOT SYSTEM FOR TIME REWIND
//=============================================================================

// Snapshot buffer size: 100 frames = 2 seconds at 50 fps
// Memory usage: ~73 bytes × 100 = ~7.3 KB
#define SNAPSHOT_BUFFER_SIZE 100

// Maximum objects per level (adjust if your levels have more)
#define MAX_PLATFORMS 4
#define MAX_ENEMIES 4
#define MAX_MINES 6

/**
 * @brief Complete snapshot of all dynamic game object states
 *
 * This structure captures the state of all moving/changing objects at a single frame.
 * Designed to be memory-efficient while preserving all necessary state for rewind.
 */
typedef struct {
    // Platform positions (2 bytes × 8 = 16 bytes)
    u8 platform_x[MAX_PLATFORMS];
    u8 platform_y[MAX_PLATFORMS];

    // Enemy positions and critical state (7 bytes × 8 = 56 bytes)
    u8 enemy_x[MAX_ENEMIES];
    u8 enemy_y[MAX_ENEMIES];
    i8 enemy_dir[MAX_ENEMIES];           // Movement direction (-1 or 1)
    u8 enemy_stunned[MAX_ENEMIES];       // Stunned timer
    u8 enemy_field_state[MAX_ENEMIES];   // 0=no field, 1=stationary, 2=projectile
    u8 enemy_field_x[MAX_ENEMIES];       // Energy field X position
    u8 enemy_field_y[MAX_ENEMIES];       // Energy field Y position

    // Mines enabled state - packed into 1 byte (8 mines fit in 8 bits)
    // Bit N = 1 if mine N is enabled, 0 if disabled
    u8 mine_enabled;                     // 1 byte

    // Total: ~73 bytes per snapshot
} GameSnapshot;

/**
 * @brief Circular buffer for game snapshots
 */
extern GameSnapshot g_SnapshotBuffer[SNAPSHOT_BUFFER_SIZE];
extern u8 g_SnapshotHead;
extern u8 g_SnapshotTail;
extern u8 g_SnapshotCount;

/**
 * @brief Capture current game state into the snapshot buffer
 *
 * @param lvl Pointer to current level
 */
void Snapshot_Capture(struct Level *lvl);

/**
 * @brief Restore game state from the snapshot at the current rewind position
 *
 * @param lvl Pointer to current level
 * @param snapshot_index Index in the circular buffer to restore from
 */
void Snapshot_Restore(struct Level *lvl, u8 snapshot_index);

/**
 * @brief Initialize the snapshot system
 */
void Snapshot_Initialize();
