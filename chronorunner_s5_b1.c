#include "cutscene.h"

#include "sprite_defs.h"
#include "fx_sounds.h"

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

static const CutSpriteAnimDef g_DocPanic = {
    .baseId = 2,
    .numLayers = 1,
    .numFrames = 2,
    .frames = { DOC_FRAME(0), DOC_FRAME(2), NULL },
    .idleFrame = DOC_FRAME(0),
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

// Instructions cutscene
#include "content/screens/screen_95.h"
#include "content/screens/screen_98.h"
#include "content/screens/screen_99.h"

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

// Preboss cutscene
#include "content/screens/screen_82.h"
#include "content/screens/screen_77.h"

// Prefight cutscene
#include "content/screens/screen_90.h"

// True ending cutscene
#include "content/screens/screen_100.h"

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
// Crystal animation definition
//-----------------------------------------------------------------------------
static const CutSpriteAnimDef g_CrystalAnim = {
    CRYSTAL_SPRITE_ID,                                  // baseId
    1,                                                  // numLayers
    2,                                                  // numFrames
    { CRYSTAL_FRAME(0), CRYSTAL_FRAME(1), NULL },       // frames
    CRYSTAL_FRAME(0),                                   // idleFrame
    12,                                                 // animSpeed
    laySize,                                            // layerOffset
    { COLOR_MEDIUM_RED, 0 }                             // colors
};

//-----------------------------------------------------------------------------
// Force field animation definitions (for electrician robots in World 4)
//-----------------------------------------------------------------------------
static const CutSpriteAnimDef g_ForceFieldAnim = {
    5,                                                  // baseId (VORTEX_SPRITE_ID)
    1,                                                  // numLayers
    2,                                                  // numFrames
    { ENERGYFIELD_FRAME(0), ENERGYFIELD_FRAME(1), NULL }, // frames
    ENERGYFIELD_FRAME(0),                               // idleFrame
    8,                                                  // animSpeed
    laySize,                                            // layerOffset
    { COLOR_MAGENTA, 0 }                                   // colors
};

//-----------------------------------------------------------------------------
// World end cutscenes
//-----------------------------------------------------------------------------
const CutCmd g_World1EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen42, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("THE TACHYON SINGULARITY FLOATS", 21),
	CUT_TEXT_TYPE("TOWARDS YOU... IT'S WONDERFUL!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOU HEAD TO THE SECOND BUILDING,", 21),
	CUT_TEXT_TYPE("YOUR JOURNEY HAS JUST STARTED.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World2EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen43, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("YOU'RE PAST THE CHEMICAL HAZARDS", 21),
	CUT_TEXT_TYPE("THE TACHYON SINGULARITY IS YOURS", 23),
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
    CUT_LOAD_LAYOUT(g_Screen44, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("THAT WAS CLOSE! THE ELECTRICAL", 21),
	CUT_TEXT_TYPE("ROBOTS WERE A REAL CHALLENGE.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOU'RE ALMOST THERE! CAN YOU", 21),
	CUT_TEXT_TYPE("RECOVER THE LAST SINGULARITY?", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World4EndCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen45, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 60),
    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 10*8, 15*8, 17*8, 1),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),
    CUT_TEXT_TYPE("THE LAST SINGULARITY SHINES AS", 21),
	CUT_TEXT_TYPE("IT REUNITES WITH ITS SIBLINGS.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("YOUR MISSION IS DONE! YOU'RE", 21),
	CUT_TEXT_TYPE("READY TO STOP DR. CRAZY.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

//-----------------------------------------------------------------------------
// Midworld cutscenes
//-----------------------------------------------------------------------------

const CutCmd g_World1MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen78, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_SPRITE_SHOW(2, 27*8, 4*8, CRYSTAL_FRAME(0), COLOR_MEDIUM_RED),
    CUT_SPRITE_SHOW(3, 5*8 - 4, 9*8 - 4, CRYSTAL_FRAME(0), COLOR_MEDIUM_RED),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 16*8, 15*8, 16*8, 2),
    CUT_TEXT_TYPE("YOU LOOK AT THE FORMULA ON THE", 21),
	CUT_TEXT_TYPE("BLACKBOARD, THERE ARE NO DOUBTS", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("THIS MUST BE PART OF THE SECRET", 21),
	CUT_TEXT_TYPE("TIME TRAVEL PROCESS... BUT HOW?", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 16*8, 31*8, 16*8, 2),
    CUT_END(),
};

const CutCmd g_World2MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen77, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
	CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_TEXT_TYPE("HERE'S THE ANSWER... THE", 21),
	CUT_TEXT_TYPE("TACHYON SINGULARITY MACHINE!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
	CUT_TEXT_TYPE("FOUR SLOTS... YOU ARE SURE THAT", 21),
	CUT_TEXT_TYPE("YOU ALREADY SAW ONE LIKE THEM...", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World3MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen63, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
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
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

const CutCmd g_World4MidCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen76, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_CLEAR_TEXT(),
    CUT_WAIT(10),
    CUT_SPRITE_SHOW(2, 2*8, 5*8, ENEMY_FRAME(2, 3), COLOR_BLACK),
    CUT_SPRITE_SHOW(3, 7*8, 12*8, ENEMY_FRAME(2, 3), COLOR_BLACK),
    CUT_SPRITE_SHOW(4, 21*8, 8*8, ENEMY_FRAME(2, 1), COLOR_BLACK),
    CUT_SFX(FX_FORCE_FIELD),
    CUT_SPRITE_ANIM(&g_ForceFieldAnim, 4*8, 5*8, 0),
    CUT_SFX(FX_FORCE_FIELD),
    CUT_SPRITE_ANIM(&g_ForceFieldAnim, 9*8, 12*8, 0),
    CUT_SFX(FX_FORCE_FIELD),
    CUT_SPRITE_ANIM(&g_ForceFieldAnim, 19*8, 8*8, 0),
    CUT_SPRITE_HIDE(5),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0, 17*8, 15*8, 17*8, 2),
    CUT_TEXT_TYPE("YOU STARE AT THE GIANT ROBOT", 21),
    CUT_TEXT_TYPE("IN TERROR FOR A MOMENT.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SFX(FX_FORCE_FIELD),
    CUT_SPRITE_ANIM(&g_ForceFieldAnim, 9*8, 12*8, 0),
    CUT_SFX(FX_FORCE_FIELD),
    CUT_SPRITE_ANIM(&g_ForceFieldAnim, 19*8, 8*8, 0),
    CUT_SPRITE_HIDE(5),
    CUT_TEXT_TYPE("IT'S THE FINAL WEAPON BY DR", 21),
    CUT_TEXT_TYPE("CRAZY! YOU NEED TO HURRY!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
    CUT_SFX(FX_FORCE_FIELD),
    CUT_SPRITE_ANIM(&g_ForceFieldAnim, 4*8, 5*8, 0),
    CUT_SPRITE_HIDE(5),
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 15*8, 17*8, 31*8, 17*8, 2),
    CUT_END(),
};

//-----------------------------------------------------------------------------
// Instructions cutscene
//-----------------------------------------------------------------------------

const CutCmd g_InstructionsCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen95, 0, CUTSCENE_TOP_Y, CUTSCENE_SCREEN_W, CUTSCENE_SCREEN_H),
    CUT_WAIT_KEY(),

    CUT_LOAD_LAYOUT(g_Screen98, 0, CUTSCENE_TOP_Y, CUTSCENE_SCREEN_W, CUTSCENE_SCREEN_H),
    CUT_SPRITE_SHOW(0, 23*8, 6*8 - 1, VORTEX_FRAME(0), COLOR_WHITE),

    CUT_SPRITE_SHOW(1, 2*8, 17*8, PLAYER_FRAME(1), COLOR_BLACK),
    CUT_SPRITE_SHOW(2, 13*8, 19*8 - 1, CRYSTAL_FRAME(0), COLOR_MEDIUM_RED),
    CUT_SPRITE_SHOW(3, 2*8, 20*8, PLAYER_FRAME(1), COLOR_BLACK),
    CUT_WAIT_KEY(),

    CUT_LOAD_LAYOUT(g_Screen99, 0, CUTSCENE_TOP_Y, CUTSCENE_SCREEN_W, CUTSCENE_SCREEN_H),
    CUT_SPRITE_SHOW(0, 1*8, 5*8 - 1, PLAYER_FRAME(3), COLOR_BLACK),
    CUT_SPRITE_SHOW(1, 5*8, 1*8, PLAYER_FRAME(1), COLOR_BLACK),
    CUT_SPRITE_SHOW(2, 8*8, 5*8 - 1, ENEMY_FRAME(0, 4), COLOR_MEDIUM_RED),
    CUT_SPRITE_SHOW(3, 14*8, 5*8 - 1, KEY_FRAME(0), COLOR_BLACK),

    CUT_SPRITE_SHOW(4, 23*8, 11*8 - 1, KEY_FRAME(0), COLOR_BLACK),
    CUT_SPRITE_SHOW(5, 29*8, 11*8 - 1, PLAYER_FRAME(6), COLOR_BLACK),

    CUT_SPRITE_SHOW(6, 1*8, 20*8 - 1, PLAYER_FRAME(3), COLOR_BLACK),
    CUT_SPRITE_SHOW(7, 5*8, 16*8 - 1, PLAYER_FRAME(1), COLOR_BLACK),
    CUT_SPRITE_SHOW(8, 8*8, 20*8 - 1, PLAYER_FRAME(3), COLOR_BLACK),
    CUT_WAIT_KEY(),

    CUT_SPRITE_HIDE(0),
    CUT_SPRITE_HIDE(1),
    CUT_SPRITE_HIDE(2),
    CUT_SPRITE_HIDE(3),
    CUT_SPRITE_HIDE(4),
    CUT_SPRITE_HIDE(5),
    CUT_SPRITE_HIDE(6),
    CUT_SPRITE_HIDE(7),
    CUT_SPRITE_HIDE(8),

    CUT_END(),
};

//-----------------------------------------------------------------------------
// Intro cutscene
//-----------------------------------------------------------------------------

const CutCmd g_IntroCutscene[] = {
	CUT_WAIT(25),

	// CUT_TEXT("PRESS SPACE KEY", 23),
    CUT_TEXT("PRESS ESC TO SKIP", 23), // @keysman75
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_LOAD_LAYOUT(g_Screen5, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS), 

    CUT_SPRITE_WALK(&g_DocWalkLeft, 29*8, 16*8, 20*8, 17*8, 1),

    CUT_TEXT_TYPE("TIME IS RUNNING OUT...", 21),
    CUT_TEXT("- PRESS SPACE KEY -", 23),

    CUT_WAIT(40),

	CUT_TEXT_TYPE("...WHERE IS CHRONO RUNNER?", 21),

	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	// Entra Chrono
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0*8, 16*8, 14*8, 17*8, 2),

	CUT_TEXT_TYPE("AT LAST. AGENT, THIS WILL BE", 21),
	CUT_TEXT_TYPE("YOUR HARDEST MISSION SO FAR.", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),

	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_TEXT_TYPE("THE EVIL DOCTOR CRAZY IS", 21),
	CUT_TEXT_TYPE("BUILDING A TIME MACHINE.", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("HE WANTS TO BRING BACK", 21),
	CUT_TEXT_TYPE("THE NAZI REGIME!", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_ANIM(&g_PlayerFreaked, 14*8, 17*8, 100),

	CUT_TEXT_TYPE("YOU HAVE ONE HOUR TO STEAL", 21),
	CUT_TEXT_TYPE("THE FOUR TACHYON SINGULARITIES", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_SPRITE_ANIM(&g_VortexAnim, 15*8, 10*8, 100),

	CUT_TEXT_TYPE("OTHERWISE, DR. CRAZY WILL USE", 21),
	CUT_TEXT_TYPE("THEM TO TRAVEL BACK IN TIME!", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),

	CUT_TEXT_TYPE("IN YOUR JOURNEY YOU WILL FIND", 21),
	CUT_TEXT_TYPE("THE TIME CRYSTALS.", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_SPRITE_ANIM(&g_CrystalAnim, 15*8, 10*8, 100),

	CUT_TEXT_TYPE("THEY ALLOW YOU TO REWIND TIME.", 21),
	CUT_TEXT_TYPE("USE THEM WISELY!", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_HIDE(CRYSTAL_SPRITE_ID),

	CUT_TEXT_TYPE("BRING THE SINGULARITIES TO ME", 21),
	CUT_TEXT_TYPE("WE WILL SAVE THE WORLD! GO!!!", 23),
    //CUT_TEXT("- PRESS SPACE KEY -", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    // Chrono esce
    CUT_SPRITE_WALK(&g_PlayerWalkLeft, 14*8, 16*8, 0*8, 17*8, 2),
	CUT_SPRITE_HIDE(0),
	CUT_SPRITE_HIDE(1),

	CUT_TEXT_TYPE("...THIS WILL BE OUR TRIUMPH...", 21),
	CUT_TEXT_TYPE("FINALLY.", 23),
	CUT_WAIT_KEY(),
	//CUT_CLEAR_TEXT(),

	CUT_SPRITE_WALK(&g_DocWalkRight, 20*8, 16*8, 29*8, 17*8, 1),

    //CUT_TEXT("- PRESS SPACE KEY -", 23),
    //CUT_WAIT_KEY(),
    
	CUT_WAIT(10),

    CUT_END(),
};

//-----------------------------------------------------------------------------
// True colors cutscene
//-----------------------------------------------------------------------------

const CutCmd g_TrueColorsCutscene[] = {

    // Clear and show new text
    CUT_CLEAR_TEXT(),

    CUT_LOAD_LAYOUT(g_Screen5, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),
    CUT_SPRITE_SHOW(2, 20*8, 17*8, DOC_FRAME(0), COLOR_BLACK),

	// Entra Chrono
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0*8, 17*8, 14*8, 17*8, 2),

    CUT_SPRITE_WALK(&g_VortexAnim, 14*8, 17*8, 20*8, 17*8, 3),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_WALK(&g_VortexAnim, 14*8, 17*8, 20*8, 17*8, 3),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_WALK(&g_VortexAnim, 14*8, 17*8, 20*8, 17*8, 3),
    CUT_SFX(FX_GET_VORTEX),
    CUT_SPRITE_WALK(&g_VortexAnim, 14*8, 17*8, 20*8, 17*8, 3),
    CUT_SFX(FX_GET_VORTEX),

    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),

	CUT_TEXT_TYPE("AGENT, GREAT! YOU MADE IT!", 21),
	CUT_TEXT_TYPE("THE WORLD IS NOW SAFE!", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    // Chrono inizia ad uscire
    CUT_SPRITE_WALK(&g_PlayerWalkLeft, 14*8, 17*8, 5*8, 17*8, 2),

    CUT_TEXT_TYPE("DANKE!", 22),
    CUT_WAIT(100),
    CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("AH AH AH!!! YOU FOOL!!!", 21),
    CUT_TEXT_TYPE("THANKS FOR DOING ALL THE WORK!!!", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

	CUT_SPRITE_ANIM(&g_PlayerFreaked, 5*8, 17*8, 100),

	CUT_TEXT_TYPE("THE ROBOTS WERE DEFENDING THE", 21),
	CUT_TEXT_TYPE("SINGULARITIES... FROM ME!!!", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_SPRITE_ANIM(&g_PlayerFreaked, 5*8, 17*8, 100),

	CUT_TEXT_TYPE("I AM DOCTOR CRAZY!!!", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_SPRITE_ANIM(&g_PlayerFreaked, 5*8, 17*8, 100),

    CUT_TEXT_TYPE("NOW I CAN TRAVEL BACK TO 1945 TO", 21),
	CUT_TEXT_TYPE("BRING THE FINAL WEAPON TO HITLER", 23),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    // Dottore scappa
    CUT_SPRITE_WALK(&g_DocWalkRight, 20*8, 17*8, 29*8, 17*8, 2),

    // Chrono rintronato
    CUT_SPRITE_ANIM(&g_PlayerFreaked, 5*8, 17*8, 100),

    // Chrono lo rincorre
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 5*8, 17*8, 31*8, 17*8, 2),
	CUT_SPRITE_HIDE(0),
	CUT_SPRITE_HIDE(1),

	CUT_WAIT_KEY(),

    CUT_END(),
};

//-----------------------------------------------------------------------------
// Pre-boss cutscene
//-----------------------------------------------------------------------------

const CutCmd g_PreBossCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen82, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),

    CUT_SPRITE_SHOW(2, 22*8, 17*8, DOC_FRAME(0), COLOR_BLACK),

    CUT_SPRITE_SHOW(6, 3*8, 5*8, VORTEX_FRAME(0), COLOR_WHITE),
    CUT_SPRITE_SHOW(7, 9*8, 5*8, VORTEX_FRAME(0), COLOR_WHITE),
    CUT_SPRITE_SHOW(8, 21*8, 5*8, VORTEX_FRAME(0), COLOR_WHITE),
    CUT_SPRITE_SHOW(9, 27*8, 5*8, VORTEX_FRAME(0), COLOR_WHITE),

    CUT_SPRITE_HIDE(6),
    CUT_SPRITE_WALK(&g_VortexAnim, 3*8, 5*8, 15*8, 13*8, 3),
    CUT_SPRITE_HIDE(7),
    CUT_SPRITE_WALK(&g_VortexAnim, 9*8, 5*8, 15*8, 13*8, 3),
    CUT_SPRITE_HIDE(8),
    CUT_SPRITE_WALK(&g_VortexAnim, 21*8, 5*8, 15*8, 13*8, 3),
    CUT_SPRITE_HIDE(9),
    CUT_SPRITE_WALK(&g_VortexAnim, 27*8, 5*8, 15*8, 13*8, 3),

    CUT_SFX(FX_ROBOT_ACTIVATE),

    CUT_SPRITE_MOVE(2, 22*8, 17*8, 15*8, 13*8, 10),

    CUT_TEXT_TYPE("ACTIVATE TIME TRAVEL!", 22),
	CUT_WAIT_KEY(),
	CUT_CLEAR_TEXT(),

    CUT_SFX(FX_ENTER_VORTEX),
    CUT_SPRITE_HIDE(2),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),

    CUT_LOAD_LAYOUT(g_Screen77, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),

    CUT_SPRITE_WALK(&g_VortexAnim, 15*8, 12*8, 15*8, 17*8, 1),

    // Entra Chrono
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0*8, 17*8, 5*8, 17*8, 2),
    CUT_WAIT(50),

    CUT_SPRITE_WALK(&g_PlayerWalkRight, 5*8, 17*8, 15*8, 17*8, 2),

    CUT_SFX(FX_ENTER_VORTEX),

    CUT_SPRITE_HIDE(0),
    CUT_SPRITE_HIDE(1),
    CUT_SPRITE_HIDE(VORTEX_SPRITE_ID),

    CUT_WAIT_KEY(),
    CUT_END(),
};

//-----------------------------------------------------------------------------
// Post-boss cutscene
//-----------------------------------------------------------------------------

const CutCmd g_PreFightCutscene[] = {
    CUT_LOAD_LAYOUT(g_Screen90, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),

    CUT_SPRITE_SHOW(2, 14*8, 17*8, DOC_FRAME(2), COLOR_BLACK),

    CUT_TEXT_TYPE("MEIN FUHRER, I MADE IT IN YOUR", 21),
    CUT_TEXT_TYPE("IMAGE AND LIKENESS.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("(MIT DIESER WUNDERWAFFE...", 21),
    CUT_TEXT_TYPE("WITH THIS GREAT WEAPON...", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("(...DER ENDSIEG IST UNSER!", 21),
    CUT_TEXT_TYPE("...THE FINAL VICTORY IS OURS!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("(ICH BRAUCHE SIE NICHT MEHR.", 21),
    CUT_TEXT_TYPE("I DON'T NEED YOU ANYMORE.", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    // Doctor panics
    CUT_TEXT_TYPE("NO, WAIT...", 22),
    CUT_SPRITE_ANIM(&g_DocPanic, 14*8, 17*8, 60),

    // Entra Chrono
    CUT_SPRITE_WALK(&g_PlayerWalkRight, 0*8, 17*8, 8*8, 17*8, 2),

    // Doctor escapes!
    CUT_SPRITE_WALK(&g_DocWalkRight, 14*8, 17*8, 29*8, 17*8, 2),
    CUT_SPRITE_HIDE(2),

    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_TEXT_TYPE("(DEM SCHICKSAL NICHT TROTZEN!", 21),
    CUT_TEXT_TYPE("DON'T OPPOSE FATE!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_SPRITE_HIDE(0),
    CUT_SPRITE_HIDE(1),

    CUT_END(),
};

//-----------------------------------------------------------------------------
// Final cutscene (after boss defeat)
//-----------------------------------------------------------------------------

extern u8 g_RemainingMinutesBackup;
extern u8 g_RemainingSecondsBackup;

const CutCmd g_FinalCutscene[] = {
    CUT_FILL(CUTSCENE_TILE_EMPTY, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),

    CUT_TEXT_TYPE("CONGRATULATIONS!", 0),
    CUT_WAIT_KEY(),

    CUT_TEXT_TYPE("SINGULARITIES RECOVERED WITH:", 3),
    CUT_TEXT("'  \"", 5),
    CUT_NUMBER(g_RemainingMinutesBackup, 12, 5),
    CUT_NUMBER(g_RemainingSecondsBackup, 15, 5),
    CUT_TEXT_TYPE("OF REMAINING TIME", 7),
    CUT_WAIT_KEY(),

    CUT_TEXT_TYPE("AFTER DEFEATING THE WUNDERWAFFE", 10),
    CUT_TEXT_TYPE("YOU RETURN TO 2026, BRINGING", 12),
    CUT_TEXT_TYPE("BACK THE SINGULARITIES.", 14),
    CUT_WAIT_KEY(),

    CUT_TEXT_TYPE("DR. CRAZY IS PRESUMED DEAD, HE", 17),
    CUT_TEXT_TYPE("WON'T MESS WITH TIME ANYMORE.", 19),
    CUT_WAIT_KEY(),

    CUT_TEXT_TYPE("...OR WILL HE?", 23),
    CUT_WAIT_KEY(),

    CUT_END(),
};

//-----------------------------------------------------------------------------
// True ending cutscene (after final one)
//-----------------------------------------------------------------------------

const CutCmd g_TrueEndingCutscene[] = {
    CUT_FILL(CUTSCENE_TILE_EMPTY, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),

    CUT_TEXT_TYPE("ENGLAND, 1895", 21),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_LOAD_LAYOUT(g_Screen100, 0, CUTSCENE_GFX_Y, 32, CUTSCENE_GFX_ROWS),

    CUT_TEXT_TYPE("H.G. WELLS' HOUSE", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),

    CUT_SPRITE_WALK(&g_DocWalkLeft, 29*8, 17*8, 20*8, 17*8, 1),

    CUT_TEXT_TYPE("HERBERT... I THINK WE WILL DO", 21),
    CUT_TEXT_TYPE("GREAT THINGS TOGETHER!", 23),
    CUT_WAIT_KEY(),
    CUT_CLEAR_TEXT(),
};
