extern unsigned char g_Screen12[];

struct Platform plat7[] = {
   {11*8, 8*8,  // pos_x pos_y
      0,    1,  // dir_x dir_y
    11*8, 8*8,  // min_x min_y
    11*8, 17*8}, // max_x max_y

   {19*8,  20*8,  // pos_x pos_y
       0,    -1,  // dir_x dir_y
    19*8,  9*8,  // min_x min_y
    19*8,  21*8}, // max_x max_y
};

struct Level level7 = {
	1, 10,       // start_x start_y
	29, 10,      // end_x end_y
	15, 21,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat7,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Screen12,   // layout
	"THE SUMMIT - L.VETTORI", // name
};
