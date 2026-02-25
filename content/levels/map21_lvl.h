#pragma once

#include "content/screens/screen_21.h"

struct Platform platforms_map21[] = {
   {27*8, 10*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    27*8, 7*8,  // min_x min_y
    27*8, 22*8} // max_x max_y
};

struct Mine mines_map21[] = {
	{1*8, 17*8} // pos_x pos_y
};

struct Enemy enemies_map21[] = {
  {10*8, 16*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	5*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {21*8, 10*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	8*8, 24*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map21 = {
	8, 4,       // start_x start_y
	2, 21,       // end_x end_y
	1, 13,      // key_x key_y
	5, 15,        // crystal_x crystal_y
	numberof(platforms_map21),  // num_platforms
	platforms_map21,       // platforms
	numberof(mines_map21),  // num_mines
	mines_map21,        // mines
	numberof(enemies_map21),  // num_enemies
	enemies_map21,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen21,   // layout
	"FIELD DESCENT - E. DEL CUCINA",  // name
};

// Unknown sprite placements:
//   sprite 62 at pixel (64, 136)
