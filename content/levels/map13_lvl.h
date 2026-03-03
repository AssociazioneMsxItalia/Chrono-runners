#pragma once

#include "content/screens/screen_13.h"

struct Platform platforms_map13[] = {
   {15*8, 15*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    15*8, 10*8,  // min_x min_y
    15*8, 20*8} // max_x max_y
};

struct Enemy enemies_map13[] = {
  {12*8, 20*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	2*8, 21*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map13 = {
	2, 20,       // start_x start_y
	15, 5,       // end_x end_y
	28, 19,      // key_x key_y
	18, 12,        // crystal_x crystal_y
	numberof(platforms_map13),  // num_platforms
	platforms_map13,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map13),  // num_enemies
	enemies_map13,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen13,   // layout
	"THE ESCAPE - LEONARDO VETTORI",  // name
};
