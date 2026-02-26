#pragma once

#include "content/screens/screen_83.h"

struct Enemy enemies_map83[] = {
  {5*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 19*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {4*8, 17*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map83 = {
	0, 0,       // start_x start_y
	28, 15,       // end_x end_y
	7, 12,      // key_x key_y
	23, 12,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map83),  // num_enemies
	enemies_map83,  // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen83,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 30 at pixel (216, 40)
//   sprite 30 at pixel (168, 40)
//   sprite 30 at pixel (72, 40)
//   sprite 30 at pixel (24, 40)
