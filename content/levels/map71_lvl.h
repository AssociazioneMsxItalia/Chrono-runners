#pragma once

#include "content/screens/screen_71.h"

struct Enemy enemies_map71[] = {
  {5*8, 13*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 6*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {5*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	4*8, 11*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {25*8, 8*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	22*8, 27*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map71 = {
	2, 4,       // start_x start_y
	25, 17,       // end_x end_y
	29, 12,      // key_x key_y
	2, 8,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map71),  // num_enemies
	enemies_map71,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen71,   // layout
	"SWASTIKA - LEONARDO VETTORI",  // name
};
