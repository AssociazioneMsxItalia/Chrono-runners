#pragma once

#include "content/screens/screen_74.h"

struct Level level_map74 = {
	0, 0,       // start_x start_y
	2, 12,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen74,   // layout
	"CUTSCENE - LEONARDO VETTORI",  // name
};

// Unknown sprite placements:
//   sprite 61 at pixel (160, 48)
//   sprite 0 at pixel (48, 112)
//   sprite 1 at pixel (48, 112)
//   sprite 61 at pixel (104, 64)
//   sprite 61 at pixel (160, 104)
//   sprite 29 at pixel (104, 112)
