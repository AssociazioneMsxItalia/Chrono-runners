#pragma once

#include "content/screens/screen_26.h"

struct Level level_map26 = {
	1, 19,       // start_x start_y
	29, 19,       // end_x end_y
	26, 19,      // key_x key_y
	15, 19,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen26,   // layout
	"GATEWAY TO WORLD 2",  // name
};
