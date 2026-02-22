#pragma once

#include "content/screens/screen_19.h"

struct Mine mines_map19[] = {
	{7*8, 11*8}, // pos_x pos_y
	{15*8, 10*8}, // pos_x pos_y
	{23*8, 9*8}, // pos_x pos_y
	{15*8, 17*8} // pos_x pos_y
};

struct Enemy enemies_map19[] = {
  {28*8, 13*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	21*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {11*8, 15*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	8*8, 12*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {17*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 27*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map19 = {
	2, 9,       // start_x start_y
	28, 21,       // end_x end_y
	28, 9,      // key_x key_y
	24, 16,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	numberof(mines_map19),  // num_mines
	mines_map19,        // mines
	numberof(enemies_map19),  // num_enemies
	enemies_map19,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen19,   // layout
	"SUICIDE PIT - E. DEL CUCINA",  // name
};
