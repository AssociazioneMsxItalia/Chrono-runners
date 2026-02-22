//=============================================================================
// CUTSCENE SYSTEM
//=============================================================================

#include "cutscene.h"

// Global cutscene context
CutsceneContext g_Cutscene;

CutSpriteAnimDef g_animdef;

extern void Cmd_End(void);
extern void Cmd_WaitFrames(void);
extern void Cmd_WaitKey(void);
extern void Cmd_ClearText(void);
extern void Cmd_SpriteShow(const CutCmd* cmd);
extern void Cmd_SpriteHide(const CutCmd* cmd);
extern void Cmd_SpriteMove(const CutCmd* cmd);
extern void Cmd_FillArea(const CutCmd* cmd);
extern void Cmd_PlaySfx(const CutCmd* cmd);

extern void Cutscene_UpdateMovement(void);
extern void Cutscene_UpdateTypewriter(void);
extern void SetupAnimation(const CutSpriteAnimDef* def,
                           u8 startX, u8 startY,
                           i16 deltaX, i16 deltaY,
                           u8 totalFrames);

//-----------------------------------------------------------------------------
// Command handlers
//-----------------------------------------------------------------------------

void Cmd_Text(const CutCmd* cmd) {
    c8 text[32];
WITH_SEGMENT(5) {
    String_Copy(text, cmd->p.text.text);
}
    u8 x = (CUTSCENE_SCREEN_W - String_Length(text)) / 2;
    PrintGFXText(text, x, cmd->p.text.y);
    g_Cutscene.cmdIndex++;
}

void Cmd_TextTypewriter(const CutCmd* cmd) {
WITH_SEGMENT(5) {
    String_Copy(g_Cutscene.currentText, cmd->p.text.text);
}
    g_Cutscene.charIndex = 0;
    g_Cutscene.textX = (CUTSCENE_SCREEN_W - String_Length(g_Cutscene.currentText)) / 2;
    g_Cutscene.textY = cmd->p.text.y;
    g_Cutscene.isTyping = TRUE;
}

void Cmd_SpriteAnim(const CutCmd* cmd) {
WITH_SEGMENT(5) {
    g_animdef = *cmd->p.anim.def;
}
    u8 duration = (cmd->p.anim.duration == 0) ? 60 : cmd->p.anim.duration;
    SetupAnimation(&g_animdef,
        cmd->p.anim.x, cmd->p.anim.y,
        0, 0,  // No movement
        duration);
}

void Cmd_SpriteWalk(const CutCmd* cmd) {
    i16 deltaX = (i16)cmd->p.walk.end_x - (i16)cmd->p.walk.start_x;
    i16 deltaY = (i16)cmd->p.walk.end_y - (i16)cmd->p.walk.start_y;

    // Calculate duration from distance and speed
    u8 absX = (deltaX < 0) ? -deltaX : deltaX;
    u8 absY = (deltaY < 0) ? -deltaY : deltaY;
    u8 distance = (absX > absY) ? absX : absY;
    u8 speed = cmd->p.walk.speed;
    if (speed == 0) speed = 1;
    u8 totalFrames = distance / speed;
    if (totalFrames == 0) totalFrames = 1;

WITH_SEGMENT(5) {
    g_animdef = *cmd->p.walk.def;
}

    SetupAnimation(&g_animdef,
        cmd->p.walk.start_x, cmd->p.walk.start_y,
        deltaX, deltaY,
        totalFrames);
}

void Cmd_LoadLayout(const CutCmd* cmd) {
WITH_SEGMENT(5) {
    VDP_WriteLayout_GM2(cmd->p.layout.data,
                        cmd->p.layout.x, cmd->p.layout.y,
                        cmd->p.layout.w, cmd->p.layout.h);
}
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

    CutCmd cmd;
WITH_SEGMENT(5) {
    cmd = g_Cutscene.script[g_Cutscene.cmdIndex];
}

    if (g_Cutscene.isWaiting) {
        if (cmd.type == CUTCMD_WAIT_FRAMES) {
            g_Cutscene.frameCounter++;
            if (g_Cutscene.frameCounter >= cmd.p.wait.frames) {
                g_Cutscene.isWaiting = FALSE;
                g_Cutscene.cmdIndex++;
            }
        } else if (cmd.type == CUTCMD_WAIT_KEY) {
            u8 row8 = Keyboard_Read(8);
            if (IS_KEY_PRESSED(row8, KEY_SPACE)) {
                g_Cutscene.isWaiting = FALSE;
                g_Cutscene.cmdIndex++;
            }
        }
        return TRUE;
    }

    switch (cmd.type) {
        case CUTCMD_END:            Cmd_End(); break;
        case CUTCMD_WAIT_FRAMES:    Cmd_WaitFrames(); break;
        case CUTCMD_WAIT_KEY:       Cmd_WaitKey(); break;
        case CUTCMD_TEXT:           Cmd_Text(&cmd); break;
        case CUTCMD_TEXT_TYPEWRITER: Cmd_TextTypewriter(&cmd); break;
        case CUTCMD_CLEAR_TEXT:     Cmd_ClearText(); break;
        case CUTCMD_SPRITE_SHOW:    Cmd_SpriteShow(&cmd); break;
        case CUTCMD_SPRITE_HIDE:    Cmd_SpriteHide(&cmd); break;
        case CUTCMD_SPRITE_MOVE:    Cmd_SpriteMove(&cmd); break;
        case CUTCMD_SPRITE_WALK:    Cmd_SpriteWalk(&cmd); break;
        case CUTCMD_SPRITE_ANIM:    Cmd_SpriteAnim(&cmd); break;
        case CUTCMD_FILL_AREA:      Cmd_FillArea(&cmd); break;
        case CUTCMD_LOAD_LAYOUT:    Cmd_LoadLayout(&cmd); break;
        case CUTCMD_PLAY_MUSIC:     Cmd_PlayMusic(&cmd); break;
        case CUTCMD_STOP_MUSIC:     Cmd_StopMusic(); break;
        case CUTCMD_PLAY_SFX:       Cmd_PlaySfx(&cmd); break;
        default: g_Cutscene.cmdIndex++; break;
    }

    return TRUE;
}
