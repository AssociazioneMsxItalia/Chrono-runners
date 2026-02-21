extern unsigned char g_Screen62[];

struct Level level_map62 = {
	3, 14,       // start_x start_y
	27, 14,       // end_x end_y
	0, 0,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	-1,        // key_trigger_enemy (-1 = visible from start)
	g_Screen62,   // layout
	"",  // name
};

// Unknown sprite placements:
//   sprite 30 at pixel (24, 152)
//   sprite 30 at pixel (216, 152)
//   sprite 30 at pixel (216, 40)
//   sprite 30 at pixel (24, 40)
