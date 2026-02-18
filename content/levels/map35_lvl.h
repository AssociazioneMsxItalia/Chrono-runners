extern unsigned char g_Screen35[];

struct Platform platforms_map35[] = {
   {23*8, 14*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    23*8, 8*8,  // min_x min_y
    23*8, 20*8}, // max_x max_y
   {7*8, 12*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    7*8, 8*8,  // min_x min_y
    7*8, 20*8} // max_x max_y
};

struct Enemy enemies_map35[] = {
  {18*8, 18*8,  // pos_x pos_y
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
			0}, // field_dir
  {4*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {16*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	11*8, 18*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map35 = {
	2, 13,       // start_x start_y
	28, 13,       // end_x end_y
	17, 10,      // key_x key_y
	28, 17,        // crystal_x crystal_y
	numberof(platforms_map35),  // num_platforms
	platforms_map35,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map35),  // num_enemies
	enemies_map35,  // enemies
	2,           // key_trigger_enemy (enemy 2 reveals key)
	g_Screen35,   // layout
	"LEVEL 35 - FABRIZIO LUGLIO",  // name
};
