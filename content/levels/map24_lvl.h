extern unsigned char g_Screen24[];

struct Platform platforms_map24[] = {
   {7*8, 22*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    1*8, 22*8,  // min_x min_y
    11*8, 22*8}, // max_x max_y
   {29*8, 12*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8, 6*8,  // min_x min_y
    29*8, 21*8}, // max_x max_y
   {1*8, 12*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    1*8, 8*8,  // min_x min_y
    1*8, 21*8} // max_x max_y
};

struct Enemy enemies_map24[] = {
  {23*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	12*8, 26*8,  // min_x max_x  TODO: adjust patrol range
	        3,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {16*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	13*8, 25*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map24 = {
	2, 4,       // start_x start_y
	13, 6,       // end_x end_y
	4, 8,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map24),  // num_platforms
	platforms_map24,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map24),  // num_enemies
	enemies_map24,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen24,   // layout
	"DARK DESCENT - ALFIO A. CORSARO",  // name
};
