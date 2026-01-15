extern unsigned char g_Level1SuicidePitEmanueleDelCucina[];

struct Mine mines_suicidepit[] = {
	{7*8,  12*8, TRUE}, // pos_x pos_y
	{15*8, 10*8, TRUE},
    {23*8,  8*8, TRUE},
    {7*8,  18*8, TRUE},
    {15*8, 16*8, TRUE},
    {23*8, 14*8, TRUE},
};

struct Enemy enemies_suicidepit[] = {
  {11*8, 16*8,  // pos_x pos_y
	        1,  // dir_x
	9*8, 13*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {18*8, 14*8,  // pos_x pos_y
	       -1,  // dir_x
   17*8, 21*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {26*8, 12*8,  // pos_x pos_y
	        1,  // dir_x
   25*8, 29*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
};

struct Level level_suicidepit = {
	2, 10,       // start_x start_y
	2, 21,       // end_x end_y
	29, 9,       // key_x key_y
	0, 0,        // crystal_x crystal_y
	0,           // num_platforms
	NULL,        // platforms
	6,           // num_mines
	mines_suicidepit,        // mines
	3,           // num_enemies
	enemies_suicidepit,  // enemies
	g_Level1SuicidePitEmanueleDelCucina,   // layout
	"SUICIDE PIT - E. DEL CUCINA", // name
};
