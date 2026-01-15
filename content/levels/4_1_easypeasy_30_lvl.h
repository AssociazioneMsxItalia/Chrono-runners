extern unsigned char g_Level4EasyPeasyLorenzoMiretti[];

struct Mine mines_easypeasy[] = {
	{29*8, 13*8, TRUE}, // pos_x pos_y
	{4*8, 18*8, TRUE},
    {29*8, 23*8, TRUE},
};

struct Platform plat_easypeasy[] = {
   {1*8,  16*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    1*8,   8*8,  // min_x min_y
    1*8,  18*8}, // max_x max_y
};

struct Enemy enemies_easypeasy[] = {
  {17*8, 6*8,  // pos_x pos_y
		   -1,  // dir_x
   12*8, 23*8,   // min_x max_x
			0,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {20*8, 11*8,  // pos_x pos_y
		   1,  // dir_x
   18*8, 27*8,   // min_x max_x
			1,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {10*8, 16*8,  // pos_x pos_y
		   -1,  // dir_x
   6*8, 23*8,   // min_x max_x
			2,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {7*8, 21*8,  // pos_x pos_y
		   1,  // dir_x
   4*8, 27*8,   // min_x max_x
			3,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_easypeasy = {
	9, 6,       // start_x start_y
	4, 6,       // end_x end_y
	11, 21,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	1,           // num_platforms
	plat_easypeasy,       // platforms
	3,           // num_mines
	mines_easypeasy,        // mines
	4,           // num_enemies
	enemies_easypeasy,        // enemies
	g_Level4EasyPeasyLorenzoMiretti,   // layout
	"EASY PEASY - LORENZO MIRETTI", // name
};
