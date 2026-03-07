#pragma once

#include "content/screens/screen_97.h"

struct Mine mines_map97[] = {
	{13*8, 3*8} // pos_x pos_y
};

struct Level level_map97 = {
	0, 0,       // start_x start_y
	0, 0,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	numberof(mines_map97),  // num_mines
	mines_map97,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen97,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 30 at pixel (184, 120)
//   sprite 2 at pixel (64, 48)
//   sprite 6 at pixel (40, 16)
//   sprite 2 at pixel (8, 48)
