extern unsigned char g_Screen8[];

struct Platform platforms_map8[] = {
   {15*8, 10*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    15*8, 5*8,  // min_x min_y
    15*8, 13*8}, // max_x max_y
   {7*8, 20*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    1*8, 20*8,  // min_x min_y
    14*8, 20*8} // max_x max_y
};

struct Enemy enemies_map8[] = {
  {15*8, 14*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	8*8, 17*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map8 = {
	9, 5,       // start_x start_y
	17, 19,       // end_x end_y
	21, 11,      // key_x key_y
	1, 17,        // crystal_x crystal_y
	numberof(platforms_map8),  // num_platforms
	platforms_map8,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map8),  // num_enemies
	enemies_map8,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen8,   // layout
	"THE CHASM - LEONARDO VETTORI",  // name
};
