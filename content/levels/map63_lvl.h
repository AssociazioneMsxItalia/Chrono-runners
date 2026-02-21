extern unsigned char g_Screen63[];

struct Level level_map63 = {
	1, 14,       // start_x start_y
	29, 14,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen63,   // layout
	"CUTSCENE - LEONARDO VETTORI",  // name
};

// Unknown sprite placements:
//   sprite 26 at pixel (160, 112)
//   sprite 32 at pixel (120, 112)
//   sprite 0 at pixel (72, 112)
//   sprite 1 at pixel (72, 112)
