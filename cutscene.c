//=============================================================================
// CUTSCENE SYSTEM
//=============================================================================

#include "cutscene.h"

// Global cutscene context
CutsceneContext g_Cutscene;

// Forward declarations
void Cutscene_UpdateTypewriter(void);
void Cutscene_UpdateMovement(void);

// External functions
extern u8 CharToTile(c8 c);

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void Cutscene_Initialize(void) {
    g_Cutscene.isActive = FALSE;
    g_Cutscene.script = NULL;
    g_Cutscene.cmdIndex = 0;
}

void Cutscene_Start(const CutCmd* script, GameState nextState, CutsceneEventCB eventCB) {
    g_Cutscene.script = script;
    g_Cutscene.cmdIndex = 0;
    g_Cutscene.frameCounter = 0;
    g_Cutscene.charIndex = 0;
    g_Cutscene.isActive = TRUE;
    g_Cutscene.isWaiting = FALSE;
    g_Cutscene.isMoving = FALSE;
    g_Cutscene.isTyping = FALSE;
    g_Cutscene.canSkip = TRUE;
    g_Cutscene.eventCB = eventCB;
    g_Cutscene.nextState = nextState;

    VDP_FillScreen_GM2(CUTSCENE_TILE_EMPTY);
    VDP_HideAllSprites();
    Game_SetState(State_Cutscene);
}

void Cutscene_Skip(void) {
    if (g_Cutscene.canSkip && g_Cutscene.isActive) {
        g_Cutscene.isActive = FALSE;
        if (g_Cutscene.eventCB) {
            g_Cutscene.eventCB(CUTSCENE_EVENT_SKIP);
        }
        Game_SetState(g_Cutscene.nextState);
    }
}

//-----------------------------------------------------------------------------
// Command handlers
//-----------------------------------------------------------------------------

void Cmd_End(void) {
    g_Cutscene.isActive = FALSE;
    if (g_Cutscene.eventCB) {
        g_Cutscene.eventCB(CUTSCENE_EVENT_END);
    }
    Game_SetState(g_Cutscene.nextState);
}

void Cmd_WaitFrames(void) {
    g_Cutscene.frameCounter = 0;
    g_Cutscene.isWaiting = TRUE;
}

void Cmd_WaitKey(void) {
    g_Cutscene.isWaiting = TRUE;
}

void Cmd_Text(const CutCmd* cmd) {
    u8 x = (CUTSCENE_SCREEN_W - String_Length(cmd->p.text.text)) / 2;
    PrintGFXText(cmd->p.text.text, x, cmd->p.text.y);
    g_Cutscene.cmdIndex++;
}

void Cmd_TextTypewriter(const CutCmd* cmd) {
    g_Cutscene.charIndex = 0;
    g_Cutscene.currentText = cmd->p.text.text;
    g_Cutscene.textX = (CUTSCENE_SCREEN_W - String_Length(cmd->p.text.text)) / 2;
    g_Cutscene.textY = cmd->p.text.y;
    g_Cutscene.isTyping = TRUE;
}

void Cutscene_UpdateTypewriter(void) {
    c8 c = g_Cutscene.currentText[g_Cutscene.charIndex];
    if (c != 0) {
        VDP_Poke_GM2(g_Cutscene.textX + g_Cutscene.charIndex, g_Cutscene.textY, CharToTile(c));
        g_Cutscene.charIndex++;
    } else {
        g_Cutscene.isTyping = FALSE;
        g_Cutscene.cmdIndex++;
    }
}

void Cmd_ClearText(void) {
    VDP_FillLayout_GM2(CUTSCENE_TILE_EMPTY, 0, CUTSCENE_TEXT_Y,
                       CUTSCENE_SCREEN_W, CUTSCENE_TEXT_ROWS);
    g_Cutscene.cmdIndex++;
}

void Cmd_SpriteShow(const CutCmd* cmd) {
    VDP_SetSpriteSM1(cmd->p.sprite.id,
                     cmd->p.sprite.x,
                     cmd->p.sprite.y,
                     cmd->p.sprite.pattern,
                     cmd->p.sprite.color);
    g_Cutscene.cmdIndex++;
}

void Cmd_SpriteHide(const CutCmd* cmd) {
    VDP_HideSprite(cmd->p.hide.id);
    g_Cutscene.cmdIndex++;
}

// Helper: Set up animation/movement state (common to Move, Anim, Walk commands)
void SetupAnimation(const CutSpriteAnimDef* def, u8 startX, u8 startY,
                    i8 deltaX, i8 deltaY, u8 totalFrames) {
    g_Cutscene.walkDef = def;
    g_Cutscene.moveStartX = startX;
    g_Cutscene.moveStartY = startY;
    g_Cutscene.moveDeltaX = deltaX;
    g_Cutscene.moveDeltaY = deltaY;
    g_Cutscene.moveTotalFrames = totalFrames;
    g_Cutscene.frameCounter = 0;
    g_Cutscene.isMoving = TRUE;

    if (def != NULL) {
        g_Cutscene.walkAnimTimer = 0;
        g_Cutscene.walkAnimFrame = 0;
        u8 basePattern = def->frames[0];
        for (u8 i = 0; i < def->numLayers; i++) {
            VDP_SetSpriteSM1(def->baseId + i, startX, startY,
                             basePattern + (i * def->layerOffset),
                             def->colors[i]);
        }
    }
}

void Cmd_SpriteMove(const CutCmd* cmd) {
    g_Cutscene.moveSpriteId = cmd->p.move.id;
    SetupAnimation(NULL,
        cmd->p.move.start_x, cmd->p.move.start_y,
        (i8)(cmd->p.move.end_x - cmd->p.move.start_x),
        (i8)(cmd->p.move.end_y - cmd->p.move.start_y),
        cmd->p.move.frames);
    VDP_SetSpritePosition(cmd->p.move.id, cmd->p.move.start_x, cmd->p.move.start_y);
}

void Cmd_SpriteAnim(const CutCmd* cmd) {
    u8 duration = (cmd->p.anim.duration == 0) ? 60 : cmd->p.anim.duration;
    SetupAnimation(cmd->p.anim.def,
        cmd->p.anim.x, cmd->p.anim.y,
        0, 0,  // No movement
        duration);
}

void Cmd_SpriteWalk(const CutCmd* cmd) {
    i8 deltaX = (i8)(cmd->p.walk.end_x - cmd->p.walk.start_x);
    i8 deltaY = (i8)(cmd->p.walk.end_y - cmd->p.walk.start_y);

    // Calculate duration from distance and speed
    u8 absX = (deltaX < 0) ? -deltaX : deltaX;
    u8 absY = (deltaY < 0) ? -deltaY : deltaY;
    u8 distance = (absX > absY) ? absX : absY;
    u8 speed = cmd->p.walk.speed;
    if (speed == 0) speed = 1;
    u8 totalFrames = distance / speed;
    if (totalFrames == 0) totalFrames = 1;

    SetupAnimation(cmd->p.walk.def,
        cmd->p.walk.start_x, cmd->p.walk.start_y,
        deltaX, deltaY,
        totalFrames);
}

void Cutscene_UpdateMovement(void) {
    g_Cutscene.frameCounter++;

    i16 x = g_Cutscene.moveStartX +
            ((i16)g_Cutscene.moveDeltaX * g_Cutscene.frameCounter) /
            g_Cutscene.moveTotalFrames;
    i16 y = g_Cutscene.moveStartY +
            ((i16)g_Cutscene.moveDeltaY * g_Cutscene.frameCounter) /
            g_Cutscene.moveTotalFrames;

    if (g_Cutscene.walkDef == NULL) {
        VDP_SetSpritePosition(g_Cutscene.moveSpriteId, (u8)x, (u8)y);
    } else {
        const CutSpriteAnimDef* def = g_Cutscene.walkDef;
        g_Cutscene.walkAnimTimer++;
        if (g_Cutscene.walkAnimTimer >= def->animSpeed) {
            g_Cutscene.walkAnimTimer = 0;
            g_Cutscene.walkAnimFrame++;
            if (g_Cutscene.walkAnimFrame >= def->numFrames) {
                g_Cutscene.walkAnimFrame = 0;
            }
        }
        u8 basePattern = def->frames[g_Cutscene.walkAnimFrame];
        for (u8 i = 0; i < def->numLayers; i++) {
            VDP_SetSpriteSM1(def->baseId + i, (u8)x, (u8)y,
                             basePattern + (i * def->layerOffset),
                             def->colors[i]);
        }
    }

    if (g_Cutscene.frameCounter >= g_Cutscene.moveTotalFrames) {
        if (g_Cutscene.walkDef != NULL) {
            const CutSpriteAnimDef* def = g_Cutscene.walkDef;
            for (u8 i = 0; i < def->numLayers; i++) {
                VDP_SetSpritePattern(def->baseId + i,
                                     def->idleFrame + (i * def->layerOffset));
            }
        }
        g_Cutscene.isMoving = FALSE;
        g_Cutscene.cmdIndex++;
    }
}

void Cmd_FillArea(const CutCmd* cmd) {
    VDP_FillLayout_GM2(cmd->p.fill.tile,
                       cmd->p.fill.x, cmd->p.fill.y,
                       cmd->p.fill.w, cmd->p.fill.h);
    g_Cutscene.cmdIndex++;
}

void Cmd_LoadLayout(const CutCmd* cmd) {
    VDP_WriteLayout_GM2(cmd->p.layout.data,
                        cmd->p.layout.x, cmd->p.layout.y,
                        cmd->p.layout.w, cmd->p.layout.h);
    g_Cutscene.cmdIndex++;
}

void Cmd_PlayMusic(const CutCmd* cmd) {
    cmd;
    // Sound functions are in segment 4, need to switch
    /*u8 prevSeg = GET_BANK_SEGMENT(1);
    SET_BANK_SEGMENT(1, 4);
    SoundStop();
    SoundSetSong(cmd->p.sound.id);
    SoundLoop(TRUE);
    SoundPlay();
    SET_BANK_SEGMENT(1, prevSeg);*/
    g_Cutscene.cmdIndex++;
}

void Cmd_StopMusic(void) {
    // TODO: Implement with segment switching
    /*u8 prevSeg = GET_BANK_SEGMENT(1);
    SET_BANK_SEGMENT(1, 4);
    SoundStop();
    SET_BANK_SEGMENT(1, prevSeg);*/
    g_Cutscene.cmdIndex++;
}

void Cmd_PlaySfx(const CutCmd* cmd) {
    FxPlay(cmd->p.sound.id);
    g_Cutscene.cmdIndex++;
}

void Cmd_Callback(const CutCmd* cmd) {
    if (cmd->p.cb.func) {
        cmd->p.cb.func();
    }
    g_Cutscene.cmdIndex++;
}

//-----------------------------------------------------------------------------
// Main state function
//-----------------------------------------------------------------------------

bool State_Cutscene(void) {
    if (!g_Cutscene.isActive) {
        return FALSE;
    }

    u8 row7 = Keyboard_Read(7);
    if (IS_KEY_PRESSED(row7, KEY_ESC)) {
        Cutscene_Skip();
        return TRUE;
    }

    if (g_Cutscene.isMoving) {
        Cutscene_UpdateMovement();
        return TRUE;
    }

    if (g_Cutscene.isTyping) {
        Cutscene_UpdateTypewriter();
        return TRUE;
    }

    const CutCmd* cmd = &g_Cutscene.script[g_Cutscene.cmdIndex];

    if (g_Cutscene.isWaiting) {
        if (cmd->type == CUTCMD_WAIT_FRAMES) {
            g_Cutscene.frameCounter++;
            if (g_Cutscene.frameCounter >= cmd->p.wait.frames) {
                g_Cutscene.isWaiting = FALSE;
                g_Cutscene.cmdIndex++;
            }
        } else if (cmd->type == CUTCMD_WAIT_KEY) {
            u8 row8 = Keyboard_Read(8);
            if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
                g_Cutscene.isWaiting = FALSE;
                g_Cutscene.cmdIndex++;
            }
        }
        return TRUE;
    }

    switch (cmd->type) {
        case CUTCMD_END:            Cmd_End(); break;
        case CUTCMD_WAIT_FRAMES:    Cmd_WaitFrames(); break;
        case CUTCMD_WAIT_KEY:       Cmd_WaitKey(); break;
        case CUTCMD_TEXT:           Cmd_Text(cmd); break;
        case CUTCMD_TEXT_TYPEWRITER: Cmd_TextTypewriter(cmd); break;
        case CUTCMD_CLEAR_TEXT:     Cmd_ClearText(); break;
        case CUTCMD_SPRITE_SHOW:    Cmd_SpriteShow(cmd); break;
        case CUTCMD_SPRITE_HIDE:    Cmd_SpriteHide(cmd); break;
        case CUTCMD_SPRITE_MOVE:    Cmd_SpriteMove(cmd); break;
        case CUTCMD_SPRITE_WALK:    Cmd_SpriteWalk(cmd); break;
        case CUTCMD_SPRITE_ANIM:    Cmd_SpriteAnim(cmd); break;
        case CUTCMD_FILL_AREA:      Cmd_FillArea(cmd); break;
        case CUTCMD_LOAD_LAYOUT:    Cmd_LoadLayout(cmd); break;
        case CUTCMD_PLAY_MUSIC:     Cmd_PlayMusic(cmd); break;
        case CUTCMD_STOP_MUSIC:     Cmd_StopMusic(); break;
        case CUTCMD_PLAY_SFX:       Cmd_PlaySfx(cmd); break;
        case CUTCMD_CALLBACK:       Cmd_Callback(cmd); break;
        default: g_Cutscene.cmdIndex++; break;
    }

    return TRUE;
}
