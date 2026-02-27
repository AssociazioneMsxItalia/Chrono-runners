#pragma once

#include "content/screens/screen_84.h"

struct Platform platforms_map84[] = {
   {10*8, 6*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    5*8, 6*8,  // min_x min_y
    26*8, 6*8}, // max_x max_y
   {29*8, 9*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8, 7*8,  // min_x min_y
    29*8, 22*8}, // max_x max_y
   {1*8, 13*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    1*8, 7*8,  // min_x min_y
    1*8, 22*8} // max_x max_y
};

struct Level level_map84 = {
	0, 0,       // start_x start_y
	0, 0,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map84),  // num_platforms
	platforms_map84,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen84,   // layout
	"",  // name
};
