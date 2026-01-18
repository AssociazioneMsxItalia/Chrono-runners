extern unsigned char g_Screen21[];

struct Enemy enemies_fielddescent[] = {
  {23*8, 10*8,  // pos_x pos_y
	       -1,  // dir_x
	8*8, 26*8,  // min_x max_x
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {8*8, 16*8,  // pos_x pos_y
	       1,  // dir_x
    4*8, 24*8,  // min_x max_x
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_fielddescent = {
	2, 4,       // start_x start_y
	2, 21,      // end_x end_y
	15, 13,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	2,           // num_enemies
	enemies_fielddescent,        // enemies
	g_Screen21,   // layout
	"FIELD DESCENT - E. DEL CUCINA", // name
};
