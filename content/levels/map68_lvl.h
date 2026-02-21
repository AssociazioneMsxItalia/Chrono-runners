extern unsigned char g_Screen68[];

struct Level level_map68 = {
	0, 0,       // start_x start_y
	0, 0,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen68,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 30 at pixel (120, 96)
