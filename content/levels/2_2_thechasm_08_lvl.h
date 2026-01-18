extern unsigned char g_Screen8[];

struct Platform plat3[] = {
   {23*8, 13*8,  // pos_x pos_y
      -1,    0,  // dir_x dir_y
     7*8, 11*8,  // min_x min_y
    24*8, 11*8}, // max_x max_y

   {6*8,  19*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
     1*8, 19*8,  // min_x min_y
    12*8, 19*8}, // max_x max_y

   {24*8, 19*8,  // pos_x pos_y
      -1,    0,  // dir_x dir_y
    18*8, 19*8,  // min_x min_y
    29*8, 19*8}, // max_x max_y
};

struct Level level_thechasm = {
	9, 8,        // start_x start_y
	21, 8,       // end_x end_y
	15, 17,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	3,           // num_platforms
	plat3,       // platforms
	0,           // num_mines
	NULL,        // mines
	0,           // num_enemies
	NULL,        // enemies
	g_Screen8,   // layout
	"THE CHASM - L.VETTORI", // name
};
