extern unsigned char g_Level3PunisherGianlucaGaggi[];

struct Enemy enemies_punisher[] = {
  {7*8, 14*8,  // pos_x pos_y
		   1,  // dir_x
   4*8, 17*8,   // min_x max_x
			2,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_punisher = {
	2, 4,       // start_x start_y
	20, 14,       // end_x end_y
	29, 21,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	1,           // num_enemies
	enemies_punisher,        // enemies
	g_Level3PunisherGianlucaGaggi,   // layout
	"PUNISHER - GIANLUCA GAGGI", // name
};
