extern const unsigned char g_level35[];

struct Platform plat_level35[] = {
   { 7*8,  17*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
     7*8,   8*8,  // min_x min_y
     7*8,  20*8}, // max_x max_y

   {23*8,  10*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
    23*8,   8*8,  // min_x min_y
    23*8,  20*8}, // max_x max_y

};

struct Enemy enemies_level35[] = {
  {13*8,  6*8,  // pos_x pos_y
		   -1,  // dir_x
   10*8, 19*8,   // min_x max_x
			2,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {17*8, 21*8,  // pos_x pos_y
		   -1,  // dir_x
    1*8, 29*8,   // min_x max_x
			2,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_level35 = {
	2, 13,        // start_x start_y
	28, 13,        // end_x end_y
	17, 12,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat_level35,       // platforms
	0,           // num_mines
	NULL,        // mines
	2,           // num_enemies
	enemies_level35,        // enemies
	g_level35,   // layout
	"LEVEL 35 - FABRIZIO LUGLIO", // name
};
