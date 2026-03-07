#pragma once

#include "content/screens/screen_96.h"

struct Enemy enemies_map96[] = {
  {8*8, 6*8,  // pos_x pos_y
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

struct Level level_map96 = {
	0, 0,       // start_x start_y
	2, 20,       // end_x end_y
	14, 6,      // key_x key_y
	28, 13,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map96),  // num_enemies
	enemies_map96,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen96,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 12 at pixel (112, 160)
//   sprite 2 at pixel (136, 88)
//   sprite 2 at pixel (136, 112)
//   sprite 6 at pixel (40, 16)
//   sprite 2 at pixel (8, 48)
