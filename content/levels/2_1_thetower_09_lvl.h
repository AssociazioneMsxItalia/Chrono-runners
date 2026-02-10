extern unsigned char g_Screen9[];

struct Platform plat4[] = {
   {1*8, 13*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
    1*8,  6*8,  // min_x min_y
    1*8, 15*8}, // max_x max_y

   {29*8, 14*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    29*8,  7*8,  // min_x min_y
    29*8, 22*8}, // max_x max_y
};

struct Enemy enemies4[] = {
  {6*8, 16*8,  // pos_x pos_y
	       -1,  // dir_x
    1*8, 8*8,  // min_x max_x
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {26*8, 21*8,  // pos_x pos_y
	       -1,  // dir_x
    22*8, 28*8,  // min_x max_x
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_thetower = {
	2, 21,       // start_x start_y
	23, 5,      // end_x end_y
	15, 12,      // key_x key_y
	0, 0,      // crystal_x crystal_y
	numberof(plat4),           // num_platforms
	plat4,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies4), // num_enemies
	enemies4,    // enemies
	g_Screen9,   // layout
	"THE TOWER - L.VETTORI", // name
};
