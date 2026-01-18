extern unsigned char g_Screen4[];

struct Mine mines0[] = {
	{22*8,  20*8, TRUE}, // pos_x pos_y
};

struct Level level_intro = {
	1, 18,        // start_x start_y
	29, 18,       // end_x end_y
	14, 18,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	1,           // num_mines
	mines0,      // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Screen4,   // layout
	"SO IT BEGINS - LEONARDO VETTORI", // name
};
