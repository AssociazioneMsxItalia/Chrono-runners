extern unsigned char g_Level2PipesAndRobotsOrazioCacciola[];

struct Platform plat_pipesandrobots[] = {
   {6*8,  17*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
     1*8,  17*8,  // min_x min_y
    21*8,  17*8}, // max_x max_y
};

struct Enemy enemies_pipesandrobots[] = {
  {7*8, 6*8,  // pos_x pos_y
			1,  // dir_x
   6*8, 11*8,  // min_x max_x
			1,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

  {20*8, 6*8,  // pos_x pos_y
		   -1,  // dir_x
   18*8, 23*8,  // min_x max_x
	     	1,  // type (0-3)
	     	0,  // mDX (initialized to 0)
	     	0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_pipesandrobots = {
	19, 11,       // start_x start_y
	25, 15,       // end_x end_y
	28,  9,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	1,           // num_platforms
	plat_pipesandrobots,       // platforms
	0,           // num_mines
	NULL,        // mines
	2,           // num_enemies
	enemies_pipesandrobots,        // enemies
	g_Level2PipesAndRobotsOrazioCacciola,   // layout
	"PIPES AND ROBOTS - O. CACCIOLA", // name
};
