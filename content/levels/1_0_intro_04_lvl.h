extern unsigned char g_Level_intro[];

struct Mine mines0[] = {
	{5*8,  22*8, TRUE}, // pos_x pos_y
	{24*8, 22*8, TRUE},
};

struct Level level_intro = {
	1, 4,        // start_x start_y
	1, 20,       // end_x end_y
	27, 19,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	1,           // num_platforms
	plat1,       // platforms
	2,           // num_mines
	mines1,      // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Level_intro,   // layout
	"SO IT BEGINS - LEONARDO VETTORI", // name
};
