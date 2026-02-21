extern unsigned char g_Screen75[];

struct Level level_map75 = {
	2, 14,       // start_x start_y
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
	g_Screen75,   // layout
	"CUTSCENE - LEONARDO VETTORI",  // name
};

// Unknown sprite placements:
//   sprite 26 at pixel (160, 112)
//   sprite 30 at pixel (224, 112)
//   sprite 0 at pixel (56, 112)
//   sprite 1 at pixel (56, 112)
