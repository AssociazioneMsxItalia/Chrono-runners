extern unsigned char g_Screen12[];

struct Platform platforms_map12[] = {
   {8*8, 12*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    8*8, 8*8,  // min_x min_y
    8*8, 21*8}, // max_x max_y
   {22*8, 13*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    22*8, 8*8,  // min_x min_y
    22*8, 21*8} // max_x max_y
};

struct Enemy enemies_map12[] = {
  {16*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	11*8, 19*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map12 = {
	1, 10,       // start_x start_y
	29, 10,       // end_x end_y
	15, 21,      // key_x key_y
	28, 18,        // crystal_x crystal_y
	numberof(platforms_map12),  // num_platforms
	platforms_map12,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map12),  // num_enemies
	enemies_map12,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen12,   // layout
	"THE SUMMIT - LEONARDO VETTORI",  // name
};
