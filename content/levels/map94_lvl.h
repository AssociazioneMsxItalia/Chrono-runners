#pragma once

#include "content/screens/screen_94.h"

struct Level level_map94 = {
	0, 0,       // start_x start_y
	0, 0,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen94,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 27 at pixel (80, 56)
