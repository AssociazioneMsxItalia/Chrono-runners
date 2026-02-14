extern unsigned char g_Screen22[];

struct Platform platforms_map22[] = {
   {17*8, 10*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    17*8, 7*8,  // min_x min_y
    17*8, 20*8}, // max_x max_y
   {7*8, 10*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    7*8, 7*8,  // min_x min_y
    7*8, 20*8} // max_x max_y
};

struct Enemy enemies_map22[] = {
  {12*8, 14*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	11*8, 14*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {27*8, 10*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	25*8, 28*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map22 = {
	2, 4,       // start_x start_y
	21, 15,       // end_x end_y
	27, 16,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map22),  // num_platforms
	platforms_map22,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map22),  // num_enemies
	enemies_map22,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen22,   // layout
	"WHAT LIES BENEATH - O. CACCIOLA",  // name
};
