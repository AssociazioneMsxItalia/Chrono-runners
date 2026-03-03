#pragma once

#include "content/screens/screen_82.h"

struct Level level_map82 = {
	2, 17,       // start_x start_y
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
	g_Screen82,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 30 at pixel (216, 40)
//   sprite 32 at pixel (168, 40)
//   sprite 31 at pixel (72, 40)
//   sprite 30 at pixel (24, 40)
//   sprite 0 at pixel (56, 136)
//   sprite 1 at pixel (56, 136)
//   sprite 26 at pixel (176, 136)
