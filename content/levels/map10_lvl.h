#pragma once

#include "content/screens/screen_10.h"

struct Platform platforms_map10[] = {
   {14*8, 7*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    10*8, 7*8,  // min_x min_y
    21*8, 7*8}, // max_x max_y
   {22*8, 15*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    22*8, 9*8,  // min_x min_y
    22*8, 21*8}, // max_x max_y
   {8*8, 15*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    8*8, 9*8,  // min_x min_y
    8*8, 21*8} // max_x max_y
};

struct Enemy enemies_map10[] = {
  {15*8, 12*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	11*8, 19*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map10 = {
	2, 5,       // start_x start_y
	28, 5,       // end_x end_y
	15, 20,      // key_x key_y
	27, 19,        // crystal_x crystal_y
	numberof(platforms_map10),  // num_platforms
	platforms_map10,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map10),  // num_enemies
	enemies_map10,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen10,   // layout
	"THE ABYSS - LEONARDO VETTORI",  // name
};
