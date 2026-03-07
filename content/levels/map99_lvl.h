#pragma once

#include "content/screens/screen_99.h"

struct Enemy enemies_map99[] = {
  {8*8, 5*8,  // pos_x pos_y
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

struct Level level_map99 = {
	0, 0,       // start_x start_y
	17, 11,       // end_x end_y
	14, 5,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map99),  // num_enemies
	enemies_map99,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen99,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 12 at pixel (232, 88)
//   sprite 6 at pixel (40, 8)
//   sprite 2 at pixel (8, 40)
//   sprite 2 at pixel (64, 160)
//   sprite 6 at pixel (40, 128)
//   sprite 2 at pixel (8, 160)
