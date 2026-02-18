extern unsigned char g_Screen34[];

struct Platform platforms_map34[] = {
   {14*8, 7*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    14*8, 4*8,  // min_x min_y
    14*8, 10*8}, // max_x max_y
   {5*8, 16*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    5*8, 12*8,  // min_x min_y
    5*8, 20*8} // max_x max_y
};

struct Enemy enemies_map34[] = {
  {9*8, 10*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	8*8, 11*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {15*8, 20*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	7*8, 19*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map34 = {
	2, 20,       // start_x start_y
	28, 6,       // end_x end_y
	21, 18,      // key_x key_y
	23, 12,        // crystal_x crystal_y
	numberof(platforms_map34),  // num_platforms
	platforms_map34,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map34),  // num_enemies
	enemies_map34,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen34,   // layout
	"THE CLIMB - FABRIZIO LUGLIO",  // name
};
