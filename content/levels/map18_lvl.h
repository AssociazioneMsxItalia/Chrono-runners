extern unsigned char g_Screen18[];

struct Platform platforms_map18[] = {
   {27*8, 8*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    27*8, 5*8,  // min_x min_y
    27*8, 14*8} // max_x max_y
};

struct Mine mines_map18[] = {
	{26*8, 17*8} // pos_x pos_y
};

struct Enemy enemies_map18[] = {
  {15*8, 13*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	9*8, 21*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {16*8, 7*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	9*8, 21*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map18 = {
	2, 4,       // start_x start_y
	15, 19,       // end_x end_y
	29, 19,      // key_x key_y
	1, 20,        // crystal_x crystal_y
	numberof(platforms_map18),  // num_platforms
	platforms_map18,       // platforms
	numberof(mines_map18),  // num_mines
	mines_map18,        // mines
	numberof(enemies_map18),  // num_enemies
	enemies_map18,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen18,   // layout
	"PUNISHER - GIANLUCA GAGGI",  // name
};
