extern unsigned char g_Screen66[];

struct Platform platforms_map66[] = {
   {20*8, 16*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    20*8, 6*8,  // min_x min_y
    20*8, 21*8} // max_x max_y
};

struct Enemy enemies_map66[] = {
  {24*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	19*8, 26*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {24*8, 13*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	24*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {5*8, 8*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	3*8, 8*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map66 = {
	28, 4,       // start_x start_y
	5, 17,       // end_x end_y
	1, 12,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map66),  // num_platforms
	platforms_map66,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map66),  // num_enemies
	enemies_map66,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen66,   // layout
	"AKITSAWS - LEONARDO VETTORI",  // name
};
