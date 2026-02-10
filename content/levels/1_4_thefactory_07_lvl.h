extern unsigned char g_Screen7[];

struct Mine mines2[] = {
	{2*8, 23*8, TRUE}, // pos_x pos_y
	{24*8, 23*8, TRUE},
};

struct Platform platforms2[] = {
   {29*8,  14*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    29*8,  3*8,  // min_x min_y
    29*8,  22*8}, // max_x max_y
};

struct Enemy enemies2[] = {
	{9*8, 7*8,  // pos_x pos_y
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

  {19*8, 14*8,  // pos_x pos_y
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

   {11*8, 21*8,  // pos_x pos_y
	        1,  // dir_x
	8*8, 22*8,  // min_x max_x
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
	25, 7,       // start_x start_y
	9, 14,       // end_x end_y
	2, 12,      // key_x key_y
	0, 0,       // crystal_x crystal_y
	numberof(platforms2),           // num_platforms
	platforms2,        // platforms
	numberof(mines2),           // num_mines
	mines2,      // mines
	numberof(enemies2), // num_enemies
	enemies2,    // enemies
	g_Screen7,   // layout
	"THE FACTORY - L.VETTORI", // name
};
