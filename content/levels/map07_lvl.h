extern unsigned char g_Screen7[];

struct Platform platforms_map7[] = {
   {29*8, 14*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8, 3*8,  // min_x min_y
    29*8, 22*8} // max_x max_y
};

struct Mine mines_map7[] = {
	{2*8, 23*8, TRUE}, // pos_x pos_y
	{24*8, 23*8, TRUE} // pos_x pos_y
};

struct Enemy enemies_map7[] = {
  {19*8, 14*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	12*8, 22*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {9*8, 7*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	5*8, 22*8,  // min_x max_x  TODO: adjust patrol range
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {11*8, 21*8,  // pos_x pos_y
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

struct Level level_map7 = {
	25, 7,       // start_x start_y
	9, 14,       // end_x end_y
	2, 12,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map7),  // num_platforms
	platforms_map7,       // platforms
	numberof(mines_map7),  // num_mines
	mines_map7,        // mines
	numberof(enemies_map7),  // num_enemies
	enemies_map7,  // enemies
	2,           // key_trigger_enemy (enemy 2 reveals key)
	g_Screen7,   // layout
	"THE FACTORY - LEONARDO VETTORI",  // name
};
