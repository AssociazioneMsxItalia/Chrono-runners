#pragma once

#include "msxgl.h"
#include "level_defs.h"

//=============================================================================
// SNAPSHOT SYSTEM FOR TIME REWIND
//=============================================================================

/*
Il personaggio principale può riavvolgere il tempo per tutto il livello, ripercorrendo
lo spazio che ha attraversato nella direzione inversa. Noi ogni fotogramma del
gioco ci salviamo lo stato del livello.

Usiamo un buffer circolare, implementato con strategia a contatore, che ha due
vantaggi in questo caso:
 * permette di usare tutto lo spazio a disposizione (anche l'ultimo slot);
 * permette di risparmiare un po' di codice che sarebbe altrimenti servito per
   calcolare il numero degli elementi contenuti.

Le invarianti di questo buffer sono:

 * head cresce verso "destra";
 * tail logicamente si trova sempre a "sinistra" di head;
 * head punta alla posizione vuota che ospiterà il prossimo valore;
 * il buffer è vuoto quando count = 0
 * il buffer è pieno quando count = SNAPSHOT_BUFFER_SIZE;

 tail           head
   |              |
   v              v
|--0123456789ABCDE-----------|

  head           tail
   |              |
   v              v
|BC---------------0123456789A|

*/

// Snapshot buffer size: 100 frames = 2 seconds at 50 fps
#define SNAPSHOT_BUFFER_SIZE 100

/**
 * @brief Complete snapshot of all dynamic game object states
 *
 * This structure captures the state of all moving/changing objects at a single frame.
 * Designed to be memory-efficient while preserving all necessary state for rewind.
 */
typedef struct {
    // Player state (3 bytes)
    u8 player_x;
    u8 player_y;
    u8 player_frame;

    // Platform positions (2 bytes × 4 = 8 bytes)
    u8 platform_x[MAX_PLATFORMS];
    u8 platform_y[MAX_PLATFORMS];

    // Enemy positions and critical state (7 bytes × 4 = 28 bytes)
    u8 enemy_x[MAX_ENEMIES];
    u8 enemy_y[MAX_ENEMIES];
    i8 enemy_dir[MAX_ENEMIES];           // Movement direction (-1 or 1)
    u8 enemy_stunned[MAX_ENEMIES];       // Stunned timer
    u8 enemy_field_state[MAX_ENEMIES];   // 0=no field, 1=stationary, 2=projectile
    u8 enemy_field_x[MAX_ENEMIES];       // Energy field X position
    u8 enemy_field_y[MAX_ENEMIES];       // Energy field Y position
} GameSnapshot;

/**
 * @brief Capture current game state into the snapshot buffer
 *
 * @param lvl Pointer to current level
 * @param player_x Player X position
 * @param player_y Player Y position
 * @param player_frame Player animation frame
 */
void Snapshot_Capture(struct Level *lvl, u8 player_x, u8 player_y, u8 player_frame);

/**
 * @brief Restore game state from the snapshot at the current rewind position
 *
 * @param lvl Pointer to current level
 * @param snapshot_index Index in the circular buffer to restore from
 * @param out_player_x Output: restored player X position
 * @param out_player_y Output: restored player Y position
 * @param out_player_frame Output: restored player animation frame
 */
void Snapshot_Restore(struct Level *lvl, u8 snapshot_index, u8 *out_player_x, u8 *out_player_y, u8 *out_player_frame);

/**
 * @brief Initialize the snapshot system
 */
void Snapshot_Initialize();

/**
 * @brief Get number of available rewind steps
 */
u8 Snapshot_GetRewindCount();

/**
 * @brief Perform one rewind step (moves head backward)
 * @return New snapshot index to use for restoration
 */
u8 Snapshot_RewindStep();
