extern unsigned char g_Screen23[];

struct Platform platforms_map23[] = {
   {13*8, 17*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    1*8, 17*8,  // min_x min_y
    22*8, 17*8} // max_x max_y
};

struct Enemy enemies_map23[] = {
  {22*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	16*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {9*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	6*8, 13*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map23 = {
	28, 15,       // start_x start_y
	20, 12,       // end_x end_y
	8, 11,      // key_x key_y
	17, 4,        // crystal_x crystal_y
	numberof(platforms_map23),  // num_platforms
	platforms_map23,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map23),  // num_enemies
	enemies_map23,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen23,   // layout
	"PIPES AND ROBOTS - O. CACCIOLA",  // name
};
