extern unsigned char g_Screen30[];

struct Platform platforms_map30[] = {
   {1*8, 11*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    1*8, 5*8,  // min_x min_y
    1*8, 22*8} // max_x max_y
};

struct Mine mines_map30[] = {
	{29*8, 23*8}, // pos_x pos_y
	{29*8, 14*8}, // pos_x pos_y
	{4*8, 17*8}, // pos_x pos_y
	{1*8, 23*8}, // pos_x pos_y
	{6*8, 17*8} // pos_x pos_y
};

struct Enemy enemies_map30[] = {
  {20*8, 12*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	17*8, 28*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {15*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	12*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {10*8, 15*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	8*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {11*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	4*8, 26*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map30 = {
	9, 6,       // start_x start_y
	4, 6,       // end_x end_y
	5, 9,      // key_x key_y
	15, 19,        // crystal_x crystal_y
	numberof(platforms_map30),  // num_platforms
	platforms_map30,       // platforms
	numberof(mines_map30),  // num_mines
	mines_map30,        // mines
	numberof(enemies_map30),  // num_enemies
	enemies_map30,  // enemies
	2,           // key_trigger_enemy (enemy 2 reveals key)
	g_Screen30,   // layout
	"EASY PEASY - LORENZO MIRETTI",  // name
};
