#pragma once

#include "content/screens/screen_93.h"

struct Enemy enemies_map93[] = {
  {8*8, 10*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 14*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map93 = {
	1, 19,       // start_x start_y
	29, 19,       // end_x end_y
	14, 10,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map93),  // num_enemies
	enemies_map93,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen93,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 6 at pixel (40, 48)
//   sprite 2 at pixel (8, 80)
