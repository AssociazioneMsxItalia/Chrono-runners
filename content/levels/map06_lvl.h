extern unsigned char g_Screen6[];

struct Platform platforms_map6[] = {
   {23*8, 12*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    23*8, 7*8,  // min_x min_y
    23*8, 21*8} // max_x max_y
};

struct Mine mines_map6[] = {
	{11*8, 14*8} // pos_x pos_y
};

struct Enemy enemies_map6[] = {
  {11*8, 20*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	5*8, 14*8,  // min_x max_x  TODO: adjust patrol range
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

struct Level level_map6 = {
	1, 5,       // start_x start_y
	1, 20,       // end_x end_y
	27, 19,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map6),  // num_platforms
	platforms_map6,       // platforms
	numberof(mines_map6),  // num_mines
	mines_map6,        // mines
	numberof(enemies_map6),  // num_enemies
	enemies_map6,  // enemies
	0,           // key_trigger_enemy (enemy 0 reveals key)
	g_Screen6,   // layout
	"THE PRISON - LEONARDO VETTORI",  // name
};
