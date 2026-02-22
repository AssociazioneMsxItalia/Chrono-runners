#pragma once

#include "content/screens/screen_48.h"

struct Level level_map48 = {
	2, 21,       // start_x start_y
	28, 21,       // end_x end_y
	15, 19,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen48,   // layout
	"",  // name
};
