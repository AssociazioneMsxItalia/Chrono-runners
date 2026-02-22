#pragma once

#include "content/screens/screen_76.h"

struct Enemy enemies_map76[] = {
  {7*8, 11*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	6*8, 21*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {21*8, 7*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	21*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {2*8, 4*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 3*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map76 = {
	2, 15,       // start_x start_y
	28, 15,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map76),  // num_enemies
	enemies_map76,  // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen76,   // layout
	"CUTSCENE - LEONARDO VETTORI",  // name
};

// Unknown sprite placements:
//   sprite 60 at pixel (72, 88)
//   sprite 60 at pixel (152, 56)
//   sprite 59 at pixel (32, 32)
