extern unsigned char g_Screen31[];

struct Platform platforms_map31[] = {
   {28*8, 9*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    28*8, 5*8,  // min_x min_y
    28*8, 14*8}, // max_x max_y
   {16*8, 6*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
    15*8, 6*8,  // min_x min_y
    26*8, 6*8} // max_x max_y
};

struct Enemy enemies_map31[] = {
  {14*8, 14*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	12*8, 19*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {5*8, 21*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	4*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {20*8, 17*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	13*8, 24*8,  // min_x max_x  TODO: adjust patrol range
	        1,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0} // field_mDX
};

struct Level level_crystalchem = {
	28, 21,       // start_x start_y
	12, 4,       // end_x end_y
	5, 9,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map31),  // num_platforms
	platforms_map31,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map31),  // num_enemies
	enemies_map31,  // enemies
	g_Screen31,   // layout
	"CRYSTALCHEM - MAX PRIVRAT",  // name
};
