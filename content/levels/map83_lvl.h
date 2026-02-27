#pragma once

#include "content/screens/screen_83.h"

struct Level level_map83 = {
	0, 0,       // start_x start_y
	28, 19,       // end_x end_y
	15, 20,      // key_x key_y
	28, 15,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen83,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 59 at pixel (168, 40)
//   sprite 59 at pixel (72, 40)
//   sprite 59 at pixel (216, 40)
//   sprite 59 at pixel (24, 40)
//   sprite 32 at pixel (120, 96)
