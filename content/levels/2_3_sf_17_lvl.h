extern unsigned char g_Level2SFStefanoForte[];

struct Platform plat_sf[] = {
   {25*8,  16*8,  // pos_x pos_y
       1,     0,  // dir_x dir_y
    10*8,  16*8,  // min_x min_y
    28*8,  16*8}, // max_x max_y
};

struct Enemy enemies_sf[] = {
  {19*8, 10*8,  // pos_x pos_y
			1,  // dir_x
   1*8, 25*8,  // min_x max_x
			1,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

  {19*8, 21*8,  // pos_x pos_y
		   -1,  // dir_x
   11*8, 24*8,  // min_x max_x
	     	1,  // type (0-3)
	     	0,  // mDX (initialized to 0)
	     	0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_sf = {
	2, 4,       // start_x start_y
	28, 21,       // end_x end_y
	5, 18,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	1,           // num_platforms
	plat_sf,       // platforms
	0,           // num_mines
	NULL,        // mines
	2,           // num_enemies
	enemies_sf,        // enemies
	g_Level2SFStefanoForte,   // layout
	"SF - STEFANO FORTE", // name
};
