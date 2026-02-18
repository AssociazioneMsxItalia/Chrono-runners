extern unsigned char g_Screen9[];

struct Platform platforms_map9[] = {
   {1*8, 13*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    1*8, 6*8,  // min_x min_y
    1*8, 15*8}, // max_x max_y
   {29*8, 14*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8, 7*8,  // min_x min_y
    29*8, 22*8} // max_x max_y
};

struct Enemy enemies_map9[] = {
  {6*8, 16*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 8*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {26*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	20*8, 29*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map9 = {
	2, 21,       // start_x start_y
	23, 7,       // end_x end_y
	15, 12,      // key_x key_y
	20, 21,        // crystal_x crystal_y
	numberof(platforms_map9),  // num_platforms
	platforms_map9,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map9),  // num_enemies
	enemies_map9,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen9,   // layout
	"THE TOWER - LEONARDO VETTORI",  // name
};
