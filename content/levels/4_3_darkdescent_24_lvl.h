extern unsigned char g_Screen24[];

struct Platform plat_darkdescent[] = {
   {1*8,  17*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    1*8,  12*8,  // min_x min_y
    1*8,  21*8}, // max_x max_y

   {8*8,  22*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
     1*8, 22*8,  // min_x min_y
    10*8, 22*8}, // max_x max_y

   {29*8, 19*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    29*8,  6*8,  // min_x min_y
    29*8, 21*8}, // max_x max_y
};

struct Enemy enemies_darkdescent[] = {
  {21*8, 6*8,  // pos_x pos_y
		   1,  // dir_x
   12*8, 26*8,   // min_x max_x
			3,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_darkdescent = {
	2, 4,       // start_x start_y
	13, 6,       // end_x end_y
	17, 21,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	3,           // num_platforms
	plat_darkdescent,       // platforms
	0,           // num_mines
	NULL,        // mines
	1,           // num_enemies
	enemies_darkdescent,        // enemies
	g_Screen24,   // layout
	"DARK DESCENT - ALFIO A. CORSARO", // name
};
