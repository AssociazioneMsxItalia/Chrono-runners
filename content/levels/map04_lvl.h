extern unsigned char g_Screen4[];

struct Enemy enemies_map4[] = {
  {9*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	6*8, 15*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map4 = {
	1, 18,       // start_x start_y
	29, 18,       // end_x end_y
	21, 18,      // key_x key_y
	19, 14,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map4),  // num_enemies
	enemies_map4,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen4,   // layout
	"SO IT BEGINS - LEONARDO VETTORI",  // name
};
