#include "msxgl.h"
#include "pt3/pt3_player.h"
#include "ayfx/ayfx_player.h"
#include "pt3/pt3_notetable2.h"

#include "content/pt3/chronorunner.h"
#include "content/pt3/gameover.h"
#include "content/pt3/menu.h"
#include "content/pt3/hitlerfinale.h"

#include "content/ayfx/fx_chronorunner.h"

void SoundInit();
void SoundPlay();
void SoundSetSong(u8 songId);
u8 SoundGetSong();
void SoundStop();
void SoundLoop(bool enable);
void SoundMute(u8 chan, bool bMute);
void SoundUpdate();

void S4_FxPlay(u8 id);

// Songs data table
const unsigned char* g_SongData[4];

u8   g_CurrentSong = 0;

bool g_Loop = FALSE;
bool g_Mute[3] = { FALSE, FALSE, FALSE };

//=============================================================================
// FUNCTION
//=============================================================================

void SoundInit() {
    PT3_Init();
	PT3_SetNoteTable(PT3_NT2);
	PT3_SetLoop(TRUE);
	PT3_SetFinishCB(SoundStop);

	ayFX_InitBank(g_fx_chronorunner);
	ayFX_SetChannel(PSG_CHANNEL_C);
	ayFX_SetMode(AYFX_MODE_FIXED);

	g_SongData[0] = g_chronorunner;
	g_SongData[1] = g_gameover;
	g_SongData[2] = g_menu;
	g_SongData[3] = g_hitlerfinale;
}

void SoundPlay() {
    PT3_Resume();
}

// Set a new song
void SoundSetSong(u8 songId)
{
	g_CurrentSong = songId;
	PT3_InitSong(g_SongData[g_CurrentSong]);
}

u8 SoundGetSong() {
	return g_CurrentSong;
}

//-----------------------------------------------------------------------------
// Stop the current music
void SoundStop()
{
	PT3_Pause();
}

//-----------------------------------------------------------------------------
// Set the current music loop flag
void SoundLoop(bool enable)
{
	g_Loop = enable;
	PT3_SetLoop(enable);
	if (enable)
		PT3_ResetFinishCB();
	else
		PT3_SetFinishCB(SoundStop);
}

//-----------------------------------------------------------------------------
// Mute one of the channels
void SoundMute(u8 chan, bool bMute)
{
	g_Mute[chan] = bMute;
	PT3_Mute(chan, bMute);
}

void SoundUpdate() {
    PT3_Decode();
	ayFX_Update();
	PT3_UpdatePSG();
}

void S4_FxPlay(u8 id) {
	ayFX_PlayBank(id, 0);
}
