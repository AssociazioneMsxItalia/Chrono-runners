extern unsigned char g_Screen11[];

struct Platform plat6[] = {
   {29*8,  8*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8,  7*8,  // min_x min_y
    29*8, 14*8}, // max_x max_y
};

struct Enemy enemies6[] = {
	{9*8, 6*8,  // pos_x pos_y
	        1,  // dir_x
    5*8, 25*8,  // min_x max_x
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {26*8, 13*8,  // pos_x pos_y
	       -1,  // dir_x
   18*8, 29*8,  // min_x max_x
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
   {22*8, 20*8, // pos_x pos_y
	       -1,  // dir_x
    5*8, 25*8,  // min_x max_x
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_thefortress = {
	1, 20,       // start_x start_y
	29, 20,      // end_x end_y
	1, 3,        // key_x key_y
	0, 0,       // crystal_x crystal_y
	1,           // num_platforms
	plat6,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies6), // num_enemies
	enemies6,    // enemies
	g_Screen11,   // layout
	"THE FORTRESS - L.VETTORI", // name
};
