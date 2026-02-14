extern unsigned char g_Screen32[];

struct Platform platforms_map32[] = {
   {19*8, 11*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    19*8, 4*8,  // min_x min_y
    19*8, 13*8}, // max_x max_y
   {3*8, 22*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    1*8, 22*8,  // min_x min_y
    23*8, 22*8} // max_x max_y
};

struct Mine mines_map32[] = {
	{11*8, 10*8, TRUE} // pos_x pos_y
};

struct Enemy enemies_map32[] = {
  {10*8, 15*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	4*8, 21*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map32 = {
	2, 6,       // start_x start_y
	25, 19,       // end_x end_y
	29, 20,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map32),  // num_platforms
	platforms_map32,       // platforms
	numberof(mines_map32),  // num_mines
	mines_map32,        // mines
	numberof(enemies_map32),  // num_enemies
	enemies_map32,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen32,   // layout
	"LEVEL 32 - ORLANDO DI SIENA",  // name
};
