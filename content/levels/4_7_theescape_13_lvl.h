extern unsigned char g_Screen13[];

struct Platform plat8[] = {
   {15*8, 19*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    15*8, 12*8,  // min_x min_y
    15*8, 20*8}, // max_x max_y
};

struct Enemy enemies8[] = {
  {22*8, 20*8,  // pos_x pos_y
	       -1,  // dir_x
	4*8, 29*8,  // min_x max_x
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level8 = {
	2, 20,       // start_x start_y
	15, 8,       // end_x end_y
	15, 14,      // key_x key_y
	0, 0,      // crystal_x crystal_y
	1,           // num_platforms
	plat8,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies8), // num_enemies
	enemies8,    // enemies
	g_Screen13,   // layout
	"THE ESCAPE - L.VETTORI", // name
};
