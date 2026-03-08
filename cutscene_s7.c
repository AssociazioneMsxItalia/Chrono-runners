//=============================================================================
// CUTSCENE SYSTEM
//=============================================================================

#include "cutscene.h"

extern CutsceneContext g_Cutscene;
extern void Cmd_End(void);

void SetupAnimation(const CutSpriteAnimDef* def,
                    u8 startX, u8 startY,
                    i16 deltaX, i16 deltaY,
                    u8 totalFrames);

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void Cutscene_Initialize(void) {
    g_Cutscene.isActive = FALSE;
    g_Cutscene.script = NULL;
    g_Cutscene.cmdIndex = 0;
}

void Cutscene_Start(const CutCmd* script, void (*onEnd)(void)) {
    g_Cutscene.onEnd = onEnd;
    g_Cutscene.script = script;
    g_Cutscene.cmdIndex = 0;
    g_Cutscene.frameCounter = 0;
    g_Cutscene.charIndex = 0;
    g_Cutscene.isActive = TRUE;
    g_Cutscene.isWaiting = FALSE;
    g_Cutscene.isMoving = FALSE;
    g_Cutscene.isTyping = FALSE;
    g_Cutscene.canSkip = TRUE;

    VDP_FillScreen_GM2(CUTSCENE_TILE_EMPTY);
    VDP_HideAllSprites();
    Game_SetState(State_Cutscene);
}

void Cutscene_Skip(void) {
    if (g_Cutscene.canSkip && g_Cutscene.isActive) {
        Cmd_End();
    }
}

//-----------------------------------------------------------------------------
// Command handlers
//-----------------------------------------------------------------------------

void Cmd_WaitFrames(void) {
    g_Cutscene.frameCounter = 0;
    g_Cutscene.isWaiting = TRUE;
}

void Cmd_WaitKey(void) {
    g_Cutscene.needsRelease = isSpacePressed();
    g_Cutscene.isWaiting = TRUE;
}

void Cmd_ClearText(void) {
    VDP_FillLayout_GM2(CUTSCENE_TILE_EMPTY,
                       0, CUTSCENE_TEXT_Y,
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

void Cmd_SpriteMove(const CutCmd* cmd) {
    g_Cutscene.moveSpriteId = cmd->p.move.id;
    SetupAnimation(NULL,
        cmd->p.move.start_x, cmd->p.move.start_y,
        (i16)cmd->p.move.end_x - (i16)cmd->p.move.start_x,
        (i16)cmd->p.move.end_y - (i16)cmd->p.move.start_y,
        cmd->p.move.frames);
    VDP_SetSpritePosition(cmd->p.move.id, cmd->p.move.start_x, cmd->p.move.start_y);
}

void Cmd_FillArea(const CutCmd* cmd) {
    VDP_FillLayout_GM2(cmd->p.fill.tile,
                       cmd->p.fill.x, cmd->p.fill.y,
                       cmd->p.fill.w, cmd->p.fill.h);
    g_Cutscene.cmdIndex++;
}

void Cmd_PlaySfx(const CutCmd* cmd) {
    FxPlay(cmd->p.sound.id);
    g_Cutscene.cmdIndex++;
}

// Helper: Set up animation/movement state (common to Move, Anim, Walk commands)
void SetupAnimation(const CutSpriteAnimDef* def,
                    u8 startX, u8 startY,
                    i16 deltaX, i16 deltaY,
                    u8 totalFrames) {
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

//-----------------------------------------------------------------------------
// State handlers
//-----------------------------------------------------------------------------

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
