#pragma once

#include "content/screens/screen_17.h"

struct Platform platforms_map17[] = {
   {8*8, 18*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    1*8, 18*8,  // min_x min_y
    30*8, 18*8} // max_x max_y
};

struct Enemy enemies_map17[] = {
  {22*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	18*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {4*8, 9*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 25*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map17 = {
	2, 6,       // start_x start_y
	28, 21,       // end_x end_y
	19, 14,      // key_x key_y
	3, 14,        // crystal_x crystal_y
	numberof(platforms_map17),  // num_platforms
	platforms_map17,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map17),  // num_enemies
	enemies_map17,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen17,   // layout
	"SF - STEFANO FORTE",  // name
};
