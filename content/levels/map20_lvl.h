extern unsigned char g_Screen20[];

struct Platform platforms_map20[] = {
   {11*8, 22*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    4*8, 22*8,  // min_x min_y
    30*8, 22*8}, // max_x max_y
   {8*8, 14*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    8*8, 5*8,  // min_x min_y
    8*8, 17*8} // max_x max_y
};

struct Enemy enemies_map20[] = {
  {20*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	17*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {25*8, 12*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	23*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {22*8, 17*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	16*8, 26*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map20 = {
	1, 20,       // start_x start_y
	28, 6,       // end_x end_y
	4, 11,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map20),  // num_platforms
	platforms_map20,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map20),  // num_enemies
	enemies_map20,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen20,   // layout
	"CHALLENGE - E. DEL CUCINA",  // name
};
