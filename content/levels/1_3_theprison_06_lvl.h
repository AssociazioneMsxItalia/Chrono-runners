extern unsigned char g_Level01[];

struct Platform plat1[] = {
   {23*8,  6*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    23*8,  6*8,  // min_x min_y
    23*8, 21*8}, // max_x max_y
};

struct Mine mines1[] = {
	{10*8, 13*8, TRUE}, // pos_x pos_y
	{10*8, 22*8, TRUE},
};

struct Level level_theprison = {
	1, 4,        // start_x start_y
	1, 20,       // end_x end_y
	27, 19,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	1,           // num_platforms
	plat1,       // platforms
	2,           // num_mines
	mines1,      // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Level01,   // layout
	"\"THE PRISON\" BY L.VETTORI", // name
};
