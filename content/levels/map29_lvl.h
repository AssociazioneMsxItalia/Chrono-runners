extern unsigned char g_Screen29[];

struct Enemy enemies_map29[] = {
  {11*8, 15*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	10*8, 21*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {19*8, 9*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	6*8, 21*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {10*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	6*8, 18*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map29 = {
	22, 21,       // start_x start_y
	2, 21,       // end_x end_y
	29, 11,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map29),  // num_enemies
	enemies_map29,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen29,   // layout
	"TWISTER - ORAZIO CACCIOLA",  // name
};
