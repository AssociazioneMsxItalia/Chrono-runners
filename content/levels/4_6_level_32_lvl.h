extern unsigned char g_Screen32[];

struct Mine mines_level32[] = {
	{9*8, 10*8, TRUE}, // pos_x pos_y
};

struct Platform plat_level32[] = {
   {19*8,  11*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
    19*8,   5*8,  // min_x min_y
    19*8,  12*8}, // max_x max_y

    {5*8,  22*8, // pos_x pos_y
       1,    0,  // dir_x dir_y
     1*8, 22*8,  // min_x min_y
    19*8, 22*8}, // max_x max_y

};

struct Enemy enemies_level32[] = {
   {5*8, 15*8,  // pos_x pos_y
 		    1,  // dir_x
    3*8, 25*8,  // min_x max_x
			3,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_level32 = {
	2, 6,        // start_x start_y
	24, 19,        // end_x end_y
	29, 21,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat_level32,       // platforms
	1,           // num_mines
	mines_level32,        // mines
	1,           // num_enemies
	enemies_level32,        // enemies
	g_Screen32,   // layout
	"LEVEL 32 - ORAZIO CACCIOLA", // name
};
