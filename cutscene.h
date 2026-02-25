//=============================================================================
// CUTSCENE SYSTEM - PUBLIC API
//=============================================================================
// Scriptable cutscene system for mid-level narrative sequences.
//
// Usage:
//   1. Define a cutscene as an array of CutCmd using helper macros
//   2. Call Cutscene_Start() to begin playback
//   3. When the script ends (or is skipped), AdvanceSequence() is called
//
// Example:
//   static const CutCmd g_MyCutscene[] = {
//       CUT_TEXT("HELLO WORLD", 10, 21),
//       CUT_WAIT_KEY(),
//       CUT_END()
//   };
//   Cutscene_Start(g_MyCutscene);
//=============================================================================
#pragma once

#include "msxgl.h"
#include "game/state.h"

//=============================================================================
// SCREEN LAYOUT CONSTANTS
//=============================================================================

#define CUTSCENE_TOP_Y          0       // Top black area start
#define CUTSCENE_TOP_ROWS       2       // Rows for top black area
#define CUTSCENE_GFX_Y          2       // Graphics area start
#define CUTSCENE_GFX_ROWS       18      // Rows for graphics
#define CUTSCENE_TEXT_Y         20      // Text area start
#define CUTSCENE_TEXT_ROWS      4       // Rows for text
#define CUTSCENE_SCREEN_W       32      // Screen width in tiles
#define CUTSCENE_SCREEN_H       24      // Screen height in tiles
#define CUTSCENE_TILE_EMPTY     47      // Empty/black tile

//=============================================================================
// SPRITE ANIMATION DEFINITION
//=============================================================================

// Maximum number of sprite layers for walk animation
#define CUTSCENE_MAX_ANIM_LAYERS 2

// Definition for animated sprite walking (define once, use in multiple commands)
typedef struct CutSpriteAnimkDef {
    u8 baseId;                          // First sprite ID
    u8 numLayers;                       // Number of sprite layers (1-4)
    u8 numFrames;                       // Number of animation frames (2 or 3)
    u8 frames[3];                       // Animation patterns (base) - use 2 or 3
    u8 idleFrame;                       // Frame to show when walk completes
    u8 animSpeed;                       // Frames between animation changes
    u8 layerOffset;                     // Pattern offset between layers
    u8 colors[CUTSCENE_MAX_ANIM_LAYERS]; // Color for each layer
} CutSpriteAnimDef;

//=============================================================================
// COMMAND TYPES
//=============================================================================

typedef enum CutCmd_Type {
    CUTCMD_END = 0,           // End script (required as final command)
    CUTCMD_WAIT_FRAMES,       // Wait N frames
    CUTCMD_WAIT_KEY,          // Wait for keypress (SPACE)
    CUTCMD_TEXT,              // Display text at position
    CUTCMD_TEXT_TYPEWRITER,   // Display text with typewriter effect
    CUTCMD_CLEAR_TEXT,        // Clear text area (rows 20-23)
    CUTCMD_SPRITE_SHOW,       // Show sprite at position
    CUTCMD_SPRITE_HIDE,       // Hide sprite
    CUTCMD_SPRITE_MOVE,       // Move sprite from A to B over N frames
    CUTCMD_SPRITE_WALK,       // Move multi-layer sprite with walk animation
    CUTCMD_SPRITE_ANIM,       // Set sprite pattern
    CUTCMD_FILL_AREA,         // Fill tile area with single tile
    CUTCMD_LOAD_LAYOUT,       // Load background layout from data
    CUTCMD_PLAY_MUSIC,        // Start music playback
    CUTCMD_STOP_MUSIC,        // Stop music
    CUTCMD_PLAY_SFX,          // Play sound effect
    CUTCMD_CALLBACK           // Call custom function
} CutCmd_Type;

//=============================================================================
// COMMAND STRUCTURE
//=============================================================================

// Cutscene command structure (uses union for memory efficiency)
// Note: SDCC doesn't properly support anonymous unions with designated
// initializers, so we use a named union 'p' (kept short for convenience)
typedef struct CutCmd {
    u8 type;                  // CutCmd_Type
    union {
        // CUTCMD_WAIT_FRAMES
        struct {
            u8 frames;        // Number of frames to wait (1-255)
        } wait;

        // CUTCMD_TEXT / CUTCMD_TEXT_TYPEWRITER
        struct {
            u8 x;             // Tile X position (0-31)
            u8 y;             // Tile Y position (0-23)
            const c8* text;   // Pointer to null-terminated string
        } text;

        // CUTCMD_SPRITE_SHOW
        struct {
            u8 id;            // Sprite ID (0-31)
            u8 x;             // X position (pixels)
            u8 y;             // Y position (pixels)
            u8 pattern;       // Pattern index
            u8 color;         // Sprite color
        } sprite;

        // CUTCMD_SPRITE_ANIM (animated sprite in place, reuses walk definition)
        struct {
            const CutSpriteAnimDef* def;  // Animation definition
            u8 x;             // X position (pixels)
            u8 y;             // Y position (pixels)
            u8 duration;      // Duration in frames (0 = 60 frames default)
        } anim;

        // CUTCMD_SPRITE_HIDE
        struct {
            u8 id;            // Sprite ID to hide
        } hide;

        // CUTCMD_SPRITE_MOVE
        struct {
            u8 id;            // Sprite ID
            u8 start_x;       // Start X (pixels)
            u8 start_y;       // Start Y (pixels)
            u8 end_x;         // End X (pixels)
            u8 end_y;         // End Y (pixels)
            u8 frames;        // Duration in frames
        } move;

        // CUTCMD_SPRITE_WALK (animated multi-layer movement)
        struct {
            const CutSpriteAnimDef* def;  // Pointer to anim definition
            u8 start_x;       // Start X (pixels)
            u8 start_y;       // Start Y (pixels)
            u8 end_x;         // End X (pixels)
            u8 end_y;         // End Y (pixels)
            u8 speed;         // Movement speed (pixels per frame, 1-8 typical)
        } walk;

        // CUTCMD_FILL_AREA
        struct {
            u8 tile;          // Tile index to fill
            u8 x;             // Start X (tiles)
            u8 y;             // Start Y (tiles)
            u8 w;             // Width (tiles)
            u8 h;             // Height (tiles)
        } fill;

        // CUTCMD_LOAD_LAYOUT
        struct {
            const u8* data;   // Pointer to layout data
            u8 x;             // Start X (tiles)
            u8 y;             // Start Y (tiles)
            u8 w;             // Width (tiles)
            u8 h;             // Height (tiles)
        } layout;

        // CUTCMD_PLAY_MUSIC / CUTCMD_PLAY_SFX
        struct {
            u8 id;            // Music/SFX ID
        } sound;
    } p;  // Short name 'p' for less verbose access (cmd->p.text.x)
} CutCmd;

//=============================================================================
// CUTSCENE CONTEXT
//=============================================================================

// Cutscene playback context
typedef struct CutsceneContext {
    const CutCmd* script;       // Pointer to command array
    u16 cmdIndex;               // Current command index
    u8  frameCounter;           // Frame counter for waits/animations
    u8  charIndex;              // Character index for typewriter effect

    // Sprite movement interpolation state
    u8  moveStartX;
    u8  moveStartY;
    i16 moveDeltaX;             // Delta X (can be negative)
    i16 moveDeltaY;             // Delta Y (can be negative)
    u8  moveTotalFrames;
    u8  moveSpriteId;

    // Current text command (for typewriter)
    c8 currentText[32];
    u8  textX;
    u8  textY;

    // Walk animation state
    const CutSpriteAnimDef* walkDef;  // Current walk definition
    u8  walkAnimTimer;          // Timer for animation frame changes
    u8  walkAnimFrame;          // Current animation frame (0 or 1)

    // State flags
    u8  isActive    : 1;        // Cutscene is running
    u8  isWaiting   : 1;        // Waiting for condition (frames/key)
    u8  isMoving    : 1;        // Movement in progress (simple or animated)
    u8  isTyping    : 1;        // Typewriter effect in progress
    u8  canSkip     : 1;        // Allow skip with ESC key
    u8  reserved    : 3;

} CutsceneContext;


//=============================================================================
// PUBLIC API
//=============================================================================

// Initialize the cutscene system (call once at game startup)
void Cutscene_Initialize(void);

// Start a cutscene with the given script
// script: Pointer to command array (must end with CUTCMD_END)
// When the cutscene ends or is skipped, AdvanceSequence() is called automatically
void Cutscene_Start(const CutCmd* script);

// Skip current cutscene (if skippable)
void Cutscene_Skip(void);

// Main cutscene state - use with Game_SetState()
bool State_Cutscene(void);

//=============================================================================
// HELPER MACROS FOR COMMAND DEFINITION
//=============================================================================

// End script marker (required as final command)
#define CUT_END() \
    { CUTCMD_END }

// Wait for N frames (1-255)
#define CUT_WAIT(n) \
    { CUTCMD_WAIT_FRAMES, { .wait = { (n) } } }

// Wait for SPACE key press
#define CUT_WAIT_KEY() \
    { CUTCMD_WAIT_KEY }

// Display centered text instantly on the given row
#define CUT_TEXT(str, py) \
    { CUTCMD_TEXT, { .text = { 0, (py), (str) } } }

// Display centered text with typewriter effect (one char per frame)
// Text is automatically centered horizontally on the given row
#define CUT_TEXT_TYPE(str, py) \
    { CUTCMD_TEXT_TYPEWRITER, { .text = { 0, (py), (str) } } }

// Clear the text area (rows 20-23)
#define CUT_CLEAR_TEXT() \
    { CUTCMD_CLEAR_TEXT }

// Show sprite at position with pattern and color
#define CUT_SPRITE_SHOW(sid, px, py, pat, col) \
    { CUTCMD_SPRITE_SHOW, { .sprite = { (sid), (px), (py), (pat), (col) } } }

// Hide sprite by ID
#define CUT_SPRITE_HIDE(sid) \
    { CUTCMD_SPRITE_HIDE, { .hide = { (sid) } } }

// Move sprite from (start_x, start_y) to (end_x, end_y) over N frames
#define CUT_SPRITE_MOVE(sid, sx, sy, ex, ey, dur) \
    { CUTCMD_SPRITE_MOVE, { .move = { (sid), (sx), (sy), (ex), (ey), (dur) } } }

// Move multi-layer sprite with walk animation
// def: pointer to CutSpriteWalkDef structure
// sx, sy: start position (pixels)
// ex, ey: end position (pixels)
// spd: movement speed in pixels per frame (1=slow, 2=normal, 4=fast)
#define CUT_SPRITE_WALK(def, sx, sy, ex, ey, spd) \
    { CUTCMD_SPRITE_WALK, { .walk = { (def), (sx), (sy), (ex), (ey), (spd) } } }

// Animate sprite in place using walk definition (no movement)
// def: pointer to CutSpriteWalkDef structure
// px, py: position (pixels)
// dur: duration in frames (0 = 60 frames default)
#define CUT_SPRITE_ANIM(def, px, py, dur) \
    { CUTCMD_SPRITE_ANIM, { .anim = { (def), (px), (py), (dur) } } }

// Fill rectangular area with a tile
#define CUT_FILL(tile, px, py, pw, ph) \
    { CUTCMD_FILL_AREA, { .fill = { (tile), (px), (py), (pw), (ph) } } }

// Load layout data to screen area
#define CUT_LOAD_LAYOUT(data, px, py, pw, ph) \
    { CUTCMD_LOAD_LAYOUT, { .layout = { (data), (px), (py), (pw), (ph) } } }

// Start music playback
#define CUT_MUSIC(mid) \
    { CUTCMD_PLAY_MUSIC, { .sound = { (mid) } } }

// Stop music playback
#define CUT_MUSIC_STOP() \
    { CUTCMD_STOP_MUSIC }

// Play sound effect
#define CUT_SFX(sid) \
    { CUTCMD_PLAY_SFX, { .sound = { (sid) } } }
