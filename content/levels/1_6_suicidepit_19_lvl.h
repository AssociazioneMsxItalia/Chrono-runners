extern unsigned char g_Screen19[];

struct Platform plat_sp[] = {
   { 1*8, 18*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
     1*8, 14*8,  // min_x min_y
     1*8, 21*8}, // max_x max_y
};

struct Mine mines_suicidepit[] = {
	{7*8,  11*8, TRUE}, // pos_x pos_y
	{15*8, 10*8, TRUE},
    {23*8,  9*8, TRUE},
    {15*8, 17*8, TRUE},
};

struct Enemy enemies_suicidepit[] = {
  {10*8, 16*8,  // pos_x pos_y
	        1,  // dir_x
	8*8, 13*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {17*8, 21*8,  // pos_x pos_y
	       -1,  // dir_x
    7*8, 25*8,  // min_x max_x
	        0,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  {25*8, 14*8,  // pos_x pos_y
	        1,  // dir_x
   21*8, 27*8,  // min_x max_x
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
	2, 9,       // start_x start_y
	28, 21,       // end_x end_y
	27, 8,       // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(plat_sp),  // num_platforms
	plat_sp,        // platforms
	numberof(mines_suicidepit),  // num_mines
	mines_suicidepit,        // mines
	numberof(enemies_suicidepit),  // num_enemies
	enemies_suicidepit,  // enemies
	g_Screen19,   // layout
	"SUICIDE PIT - E. DEL CUCINA", // name
};
