extern unsigned char g_Screen33[];

struct Platform plat_level33[] = {
   {17*8,  10*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
    17*8,   8*8,  // min_x min_y
    17*8,  12*8}, // max_x max_y

   {11*8, 11*8, // pos_x pos_y
       1,    0,  // dir_x dir_y
     9*8, 11*8,  // min_x min_y
    15*8, 11*8}, // max_x max_y

};

struct Enemy enemies_level33[] = {
  {23*8,  6*8,  // pos_x pos_y
		   -1,  // dir_x
   20*8, 26*8,   // min_x max_x
			2,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_level33 = {
	2, 9,        // start_x start_y
	23, 13,        // end_x end_y
	10, 20,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat_level33,       // platforms
	0,           // num_mines
	NULL,        // mines
	1,           // num_enemies
	enemies_level33,        // enemies
	g_Screen33,   // layout
	"LEVEL 33 - FABRIZIO LUGLIO", // name
};
