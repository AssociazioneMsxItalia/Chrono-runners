extern unsigned char g_Screen6[];

struct Platform plat1[] = {
   {23*8, 12*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    23*8,  7*8,  // min_x min_y
    23*8, 21*8}, // max_x max_y
};

struct Mine mines1[] = {
	{11*8, 14*8, TRUE}, // pos_x pos_y
};

struct Level level_theprison = {
	1, 5,        // start_x start_y
	1, 20,       // end_x end_y
	27, 19,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(plat1),           // num_platforms
	plat1,       // platforms
	numberof(mines1),           // num_mines
	mines1,      // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Screen6,   // layout
	"THE PRISON - L.VETTORI", // name
};
