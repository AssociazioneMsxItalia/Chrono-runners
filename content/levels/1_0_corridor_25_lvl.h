extern unsigned char g_Screen25[];

struct Level level_corridor1 = {
	1, 19,        // start_x start_y
	29, 19,       // end_x end_y
	26, 19,      // key_x key_y
	15, 19,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,      // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Screen25,   // layout
	"GATEWAY TO WORLD 1", // name
};
