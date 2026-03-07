#pragma once

#include "content/screens/screen_100.h"

struct Level level_map100 = {
	22, 11,       // start_x start_y
	22, 13,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen100,   // layout
	"CUTSCENE - LEONARDO VETTORI",  // name
};

// Unknown sprite placements:
//   sprite 67 at pixel (48, 136)
//   sprite 26 at pixel (160, 136)
//   sprite 30 at pixel (120, 48)
