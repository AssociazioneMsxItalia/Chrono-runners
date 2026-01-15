extern unsigned char g_Level02[];

struct Mine mines2[] = {
	{5*8,  22*8, TRUE}, // pos_x pos_y
	{24*8, 22*8, TRUE},
};

struct Enemy enemies2[] = {
	{9*8, 6*8,  // pos_x pos_y
			1,  // dir_x
	5*8, 22*8,  // min_x max_x
			0,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

  {16*8, 13*8,  // pos_x pos_y
		   -1,  // dir_x
   12*8, 22*8,  // min_x max_x
	     	0,  // type (0-3)
	     	0,  // mDX (initialized to 0)
	     	0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

   {8*8, 20*8,  // pos_x pos_y
	        1,  // dir_x
	7*8, 22*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_thefactory = {
	29, 6,       // start_x start_y
	9, 13,       // end_x end_y
	29, 11,      // key_x key_y
	2, 11,       // crystal_x crystal_y
	0,           // num_platforms
	NULL,        // platforms
	2,           // num_mines
	mines2,      // mines
	numberof(enemies2), // num_enemies
	enemies2,    // enemies
	g_Level02,   // layout
	"\"THE FACTORY\" BY L.VETTORI", // name
};
