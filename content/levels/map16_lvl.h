extern unsigned char g_Screen16[];

struct Mine mines_map16[] = {
	{18*8, 21*8} // pos_x pos_y
};

struct Enemy enemies_map16[] = {
  {12*8, 13*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	11*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {10*8, 7*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	8*8, 22*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map16 = {
	2, 4,       // start_x start_y
	28, 21,       // end_x end_y
	28, 15,      // key_x key_y
	1, 21,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	numberof(mines_map16),  // num_mines
	mines_map16,        // mines
	numberof(enemies_map16),  // num_enemies
	enemies_map16,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen16,   // layout
	"RONIN - GIANLUCA GAGGI",  // name
};
