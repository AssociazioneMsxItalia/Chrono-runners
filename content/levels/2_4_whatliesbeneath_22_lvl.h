extern unsigned char g_Level2WhatLiesBeneathOrazioCacciola[];

struct Platform plat_whatliesbeneath[] = {
   {7*8,  9*8,  // pos_x pos_y
       0,   1,  // dir_x dir_y
    7*8,  6*8,  // min_x min_y
    7*8,  20*8}, // max_x max_y

   {17*8,  16*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
    17*8,   6*8,  // min_x min_y
    17*8,  20*8}, // max_x max_y
};

struct Level level_whatliesbeneath = {
	2, 4,       // start_x start_y
	22, 15,       // end_x end_y
	22, 4,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat_whatliesbeneath,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Level2WhatLiesBeneathOrazioCacciola,   // layout
	"WHAT LIES BENEATH - O.CACCIOLA", // name
};
