extern unsigned char g_Screen5[];

struct Level level_map5 = {
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
	0xFF,        // key_trigger_enemy (0xFF = visible from start)
	g_Screen5,   // layout
	"",  // name (assigned in InitLevels)
};

// Unknown sprite placements:
//   sprite 23 at pixel (120, 80)
