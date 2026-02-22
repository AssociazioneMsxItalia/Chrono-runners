#include "sprite_defs.h"
#include "cutscene.h"

//=============================================================================
// CUTSCENES
//=============================================================================

// Player character walk definition (2 layers, 3 animation frames)
// Adjust frame values to match your player sprite frames
static const CutSpriteAnimDef g_PlayerWalkRight = {
    .baseId = 0,  // First sprite ID (uses 0 and 1 for 2 layers)
    .numLayers = 2,                 // Player has 2 sprite layers
    .numFrames = 3,                 // 3-frame walk cycle
    .frames = { PLAYER_FRAME(1), PLAYER_FRAME(2), PLAYER_FRAME(3) },
    .idleFrame = PLAYER_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,                 // Frames between animation changes
    .layerOffset = laySize,         // Pattern offset between layers
    .colors = { COLOR_BLACK, COLOR_DARK_YELLOW }  // Layer colors
};

static const CutSpriteAnimDef g_PlayerWalkLeft = {
    .baseId = 0,
    .numLayers = 2,
    .numFrames = 3,                 // 3-frame walk cycle
    .frames = { PLAYER_FRAME(4), PLAYER_FRAME(5), PLAYER_FRAME(6) },
    .idleFrame = PLAYER_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, COLOR_DARK_YELLOW }
};

static const CutSpriteAnimDef g_DocWalkLeft = {
    .baseId = 2,
    .numLayers = 1,
    .numFrames = 2,
    .frames = { DOC_FRAME(0), DOC_FRAME(1), NULL },
    .idleFrame = DOC_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, 0 }
};

static const CutSpriteAnimDef g_DocWalkRight = {
    .baseId = 2,
    .numLayers = 1,
    .numFrames = 2,
    .frames = { DOC_FRAME(2), DOC_FRAME(3), NULL },
    .idleFrame = DOC_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 6,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, 0 }
};

static const CutSpriteAnimDef g_PlayerFreaked = {
    .baseId = 0,
    .numLayers = 2,
    .numFrames = 2,
    .frames = { PLAYER_FRAME(7), PLAYER_FRAME(8), NULL },
    .idleFrame = PLAYER_FRAME(0),   // Idle frame when walk completes
    .animSpeed = 12,
    .layerOffset = laySize,
    .colors = { COLOR_BLACK, COLOR_LIGHT_YELLOW }
};

// Initial cutscene
#include "content/screens/screen_5.h"

// End of world cutscenes
#include "content/screens/screen_42.h"
#include "content/screens/screen_43.h"
#include "content/screens/screen_44.h"
#include "content/screens/screen_45.h"

// Midworld cutscenes
#include "content/screens/screen_78.h"
#include "content/screens/screen_77.h"
#include "content/screens/screen_63.h"
#include "content/screens/screen_76.h"


//-----------------------------------------------------------------------------
// Vortex animation definition (for world-end cutscenes)
//-----------------------------------------------------------------------------
static const CutSpriteAnimDef g_VortexAnim = {
    VORTEX_SPRITE_ID,                                   // baseId
    1,                                                  // numLayers
    3,                                                  // numFrames
    { VORTEX_FRAME(0), VORTEX_FRAME(1), VORTEX_FRAME(2) }, // frames
    VORTEX_FRAME(0),                                    // idleFrame
    8,                                                  // animSpeed
    laySize,                                            // layerOffset
    { COLOR_WHITE, 0 }                                  // colors
};

//-----------------------------------------------------------------------------
// World end cutscenes
//-----------------------------------------------------------------------------
const CutCmd g_World1EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen42, 0, 2, 32, 22),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("THE FIRST TIME VORTEX FLOATS", 21),
	CUT_TEXT_TYPE("TOWARDS YOU... IT'S WONDERFUL!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOU HEAD TO THE SECOND WORLD,", 21),
	CUT_TEXT_TYPE("YOUR JOURNEY HAS JUST STARTED.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World2EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen43, 0, 2, 32, 22),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("YOU'RE PAST THE CHEMICAL HAZARDS", 21),
	CUT_TEXT_TYPE("AND RECEIVE THE SECOND VORTEX.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOU'RE HALFWAY THERE! BUT YOU", 21),
	CUT_TEXT_TYPE("FEAR IT'LL GET HARDER FROM HERE.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World3EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen44, 0, 2, 32, 22),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("THAT WAS CLOSE! THE ELECTRICAL", 21),
	CUT_TEXT_TYPE("ROBOTS WERE A REAL CHALLENGE.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOU'RE ALMOST THERE! CAN YOU", 21),
	CUT_TEXT_TYPE("RECOVER THE LAST VORTEX?", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World4EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen45, 0, 2, 32, 22),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("THE LAST VORTEX SHINES AS IT", 21),
	CUT_TEXT_TYPE("REUNITES WITH ITS SIBLINGS.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOUR MISSION IS DONE! YOU'RE", 21),
	CUT_TEXT_TYPE("READY TO STOP DR.CRAZY.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

//-----------------------------------------------------------------------------
// Midworld cutscenes
//-----------------------------------------------------------------------------

const CutCmd g_World1MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen78, 0, 2, 32, 22),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 15*8, 15*8, 15*8, 2),
    CUT_TEXT_TYPE("YOU LOOK AT THE FORMULA ON THE", 21),
	CUT_TEXT_TYPE("BLACKBOARD, THERE ARE NO DOUBTS", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("THIS MUST BE PART OF THE SECRET", 21),
	CUT_TEXT_TYPE("TRAVEL PROCESS... BUT HOW?", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 15*8, 31*8, 15*8, 2),
    CUT_END(),
};

const CutCmd g_World2MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen78, 0, 2, 32, 22),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 15*8, 15*8, 15*8, 2),
    CUT_TEXT_TYPE("HERE'S THE ANSWER... THE", 21),
	CUT_TEXT_TYPE("TACHYONIC SINGULARITY MACHINE!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("FOUR SLOTS... YOU ARE SURE YOU", 21),
	CUT_TEXT_TYPE("ALREADY SAW ONE OF THEM.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 15*8, 31*8, 15*8, 2),
    CUT_END(),
};

const CutCmd g_World3MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen78, 0, 2, 32, 22),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_TEXT_TYPE("IT SAYS \"BEAUTIFUL MEMORIES\"", 21),
	CUT_TEXT_TYPE("A PAINTING BY LEO VIKTOR", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOU'RE SURE YOU KNOW BOTH", 21),
	CUT_TEXT_TYPE("THESE PEOPLE... COULD THEY BE...", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 15*8, 31*8, 15*8, 2),
    CUT_END(),
};

const CutCmd g_World4MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen78, 0, 2, 32, 22),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_TEXT_TYPE("YOU STARE AT THE GIANT ROBOT", 21),
	CUT_TEXT_TYPE("IN TERROR FOR A MOMENT.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("IT'S THE FINAL WEAPON BY DR", 21),
	CUT_TEXT_TYPE("CRAZY! YOU NEED TO HURRY!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 15*8, 31*8, 15*8, 2),
    CUT_END(),
};

//-----------------------------------------------------------------------------
// Intro cutscene
//-----------------------------------------------------------------------------

const CutCmd g_IntroCutscene[] = {
	CUT_WAIT(25),

	CUT_TEXT("PRESS SPACE KEY", 23),

    CUT_WAIT_KEY(),
    CUT_WAIT(10),

    // Clear and show new text
    CUT_CLEAR_TEXT(),

    CUT_LOAD_LAYOUT(g_Screen5, 0, CUTSCENE_GFX_Y, CUTSCENE_SCREEN_W, CUTSCENE_GFX_ROWS),

    CUT_TEXT_TYPE("(...ALARM BELLS RINGING...)", 20),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_SPRITE_WALK(&g_DocWalkLeft, 29*8, 11*8, 20*8, 11*8, 1),

    CUT_TEXT_TYPE("TIME IS RUNNING OUT...", 20),
	CUT_TEXT_TYPE("...WHERE IS CHRONO RUNNER?", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	// Entra Chrono
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0*8, 11*8, 14*8, 11*8, 2),

	CUT_TEXT_TYPE("AT LAST. AGENT, THIS WILL BE", 20),
	CUT_TEXT_TYPE("YOUR HARDEST MISSION SO FAR.", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("THE EVIL DOCTOR CRAZY IS", 20),
	CUT_TEXT_TYPE("BUILDING A TIME MACHINE.", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("HE WANTS TO BRING BACK", 20),
	CUT_TEXT_TYPE("THE NAZI REGIME!", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_ANIM(&g_PlayerFreaked, 14*8, 11*8, 100),

	CUT_TEXT_TYPE("YOU ONLY HAVE ONE HOUR TO", 20),
	CUT_TEXT_TYPE("RECOVER ALL FOUR TIME VORTICES.", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("BRING THEM ALL TO ME", 20),
	CUT_TEXT_TYPE("AND WE WILL SAVE THE WORLD!", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    // Chrono esce
    CUT_SPRITE_WALK(&g_PlayerWalkLeft, 14*8, 11*8, 0*8, 11*8, 2),
	CUT_SPRITE_HIDE(0),
	CUT_SPRITE_HIDE(1),

	CUT_TEXT_TYPE("...THIS WILL BE OUR TRIUMPH...", 20),
	CUT_TEXT_TYPE("FINALLY.", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_WALK(&g_DocWalkRight, 20*8, 11*8, 29*8, 11*8, 1),

    CUT_TEXT("PRESS SPACE KEY", 23),

    CUT_WAIT_KEY(),
	CUT_WAIT(10),

    CUT_END(),
};
