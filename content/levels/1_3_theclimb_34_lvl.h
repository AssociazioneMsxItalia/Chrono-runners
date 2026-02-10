extern unsigned char g_Screen34[];

struct Platform platforms_theclimb[] = {
   {5*8,  16*8,  // pos_x pos_y
       0,   -1,  // dir_x dir_y
    5*8,  12*8,  // min_x min_y
    5*8,  20*8}, // max_x max_y
   {14*8,  7*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    14*8,  4*8,  // min_x min_y
    14*8, 10*8}, // max_x max_y
};

struct Enemy enemies_theclimb[] = {
  {21*8, 20*8,  // pos_x pos_y
	        1,  // dir_x
	7*8, 29*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {10*8, 10*8,  // pos_x pos_y
	       -1,  // dir_x
   8*8, 11*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {19*8, 6*8,  // pos_x pos_y
	        1,  // dir_x
   18*8, 25*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_theclimb = {
	2, 20,        // start_x start_y
	28, 6,       // end_x end_y
	21, 18,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	platforms_theclimb,       // platforms
	0,           // num_mines
	NULL,      // mines
	3,           // num_enemies
	enemies_theclimb,        // enemies
	g_Screen34,   // layout
	"THE CLIMB - FABRIZIO LUGLIO", // name
};
