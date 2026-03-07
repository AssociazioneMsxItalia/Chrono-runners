#pragma once

#include "content/screens/screen_98.h"

struct Level level_map98 = {
	0, 0,       // start_x start_y
	0, 0,       // end_x end_y
	0, 0,      // key_x key_y
	13, 19,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen98,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 2 at pixel (16, 160)
//   sprite 2 at pixel (16, 136)
//   sprite 30 at pixel (184, 48)
