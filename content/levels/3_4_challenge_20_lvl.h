extern unsigned char g_Level3ChallengeEmanueleDelCucina[];

struct Platform plat_challenge[] = {
   {9*8,  17*8,  // pos_x pos_y
      0,    -1,  // dir_x dir_y
    9*8,   5*8,  // min_x min_y
    9*8,  18*8}, // max_x max_y

   {7*8,  22*8,  // pos_x pos_y
       1,    0,  // dir_x dir_y
     4*8, 22*8,  // min_x min_y
    29*8, 22*8}, // max_x max_y
};

struct Enemy enemies_challenge[] = {
  {22*8, 6*8,  // pos_x pos_y
		  -1,  // dir_x
   17*8, 29*8,   // min_x max_x
			2,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

  {25*8, 17*8,  // pos_x pos_y
		   -1,  // dir_x
   16*8, 26*8,  // min_x max_x
	     	1,  // type (0-3)
	     	0,  // mDX (initialized to 0)
	     	0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_challenge = {
	1, 20,       // start_x start_y
	28, 6,       // end_x end_y
	5, 10,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat_challenge,       // platforms
	0,           // num_mines
	NULL,        // mines
	2,           // num_enemies
	enemies_challenge,        // enemies
	g_Level3ChallengeEmanueleDelCucina,   // layout
	"CHALLENGE - E. DEL CUCINA", // name
};
