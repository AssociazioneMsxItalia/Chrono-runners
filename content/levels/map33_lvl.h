#pragma once

#include "content/screens/screen_33.h"

struct Platform platforms_map33[] = {
   {15*8, 11*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    15*8, 8*8,  // min_x min_y
    15*8, 19*8} // max_x max_y
};

struct Enemy enemies_map33[] = {
  {21*8, 7*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	20*8, 26*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {17*8, 20*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	10*8, 17*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map33 = {
	2, 9,       // start_x start_y
	23, 13,       // end_x end_y
	28, 18,      // key_x key_y
	26, 6,        // crystal_x crystal_y
	numberof(platforms_map33),  // num_platforms
	platforms_map33,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map33),  // num_enemies
	enemies_map33,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen33,   // layout
	"LEVEL 33 - FABRIZIO LUGLIO",  // name
};
