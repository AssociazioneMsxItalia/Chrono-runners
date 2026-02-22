#pragma once

#include "content/screens/screen_44.h"

struct Level level_map44 = {
	2, 17,       // start_x start_y
	28, 17,       // end_x end_y
	15, 19,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen44,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 30 at pixel (120, 80)
