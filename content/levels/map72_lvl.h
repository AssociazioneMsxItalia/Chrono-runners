#pragma once

#include "content/screens/screen_72.h"

struct Level level_map72 = {
	0, 0,       // start_x start_y
	2, 15,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen72,   // layout
	"CUTSCENE - LEONARDO VETTORI",  // name
};

// Unknown sprite placements:
//   sprite 0 at pixel (40, 136)
//   sprite 1 at pixel (40, 136)
//   sprite 63 at pixel (128, 104)
//   sprite 26 at pixel (128, 136)
