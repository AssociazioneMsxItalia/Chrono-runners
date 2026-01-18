extern unsigned char g_Screen10[];

struct Platform plat5[] = {
   {8*8,  20*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    8*8,  11*8,  // min_x min_y
    8*8,  21*8}, // max_x max_y

   {12*8, 15*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    12*8, 6*8,  // min_x min_y
    12*8, 16*8}, // max_x max_y

   {18*8, 6*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
     18*8, 6*8,  // min_x min_y
     18*8, 16*8}, // max_x max_y

	{22*8, 11*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
     22*8, 9*8,  // min_x min_y
     22*8, 21*8}, // max_x max_y
};

struct Level level5 = {
	2, 4,        // start_x start_y
	28, 4,       // end_x end_y
	15, 20,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	4,           // num_platforms
	plat5,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Screen10,   // layout
	"THE ABYSS - L.VETTORI", // name
};
