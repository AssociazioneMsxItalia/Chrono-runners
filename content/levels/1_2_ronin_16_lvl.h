extern unsigned char g_Screen16[];

struct Enemy enemies_ronin[] = {
  {6*8, 7*8,  // pos_x pos_y
	        1,  // dir_x
	1*8, 22*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
   {13*8, 13*8,  // pos_x pos_y
	        -1,  // dir_x
	7*8, 24*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Mine mines_ronin[] = {
	{14*8, 20*8, TRUE}, // pos_x pos_y
};

struct Level level_ronin = {
	2, 4,        // start_x start_y
	28, 21,      // end_x end_y
	14, 17,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,        // platforms
	1,           // num_mines
	mines_ronin,        // mines
	2,           // num_enemies
	enemies_ronin,  // enemies
	g_Screen16,   // layout
	"RONIN - GIANLUCA GAGGI", // name
};
