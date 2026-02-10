extern unsigned char g_Screen4[];

struct Enemy enemies_intro[] = {
  {9*8, 21*8,  // pos_x pos_y
	        1,  // dir_x
	6*8, 15*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_intro = {
	1, 18,        // start_x start_y
	29, 18,       // end_x end_y
	21, 18,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,      // mines
	1,           // num_enemies
	enemies_intro,  // enemies
	g_Screen4,   // layout
	"SO IT BEGINS - LEONARDO VETTORI", // name
};
