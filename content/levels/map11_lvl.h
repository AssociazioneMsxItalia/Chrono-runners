extern unsigned char g_Screen11[];

struct Platform platforms_map11[] = {
   {29*8, 9*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    29*8, 4*8,  // min_x min_y
    29*8, 14*8} // max_x max_y
};

struct Enemy enemies_map11[] = {
  {7*8, 20*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	1*8, 30*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {18*8, 6*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	5*8, 25*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0}, // field_dir
  {24*8, 13*8,  // pos_x pos_y
	        1,  // dir_x  TODO: set direction (-1 or 1)
	18*8, 29*8,  // min_x max_x  TODO: adjust patrol range
	        2,  // type (0-3)
	        0,  // mDX (initialized to 0)
	        0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0,  // field_mDX
			0} // field_dir
};

struct Level level_map11 = {
	1, 20,       // start_x start_y
	29, 20,       // end_x end_y
	1, 12,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	numberof(platforms_map11),  // num_platforms
	platforms_map11,       // platforms
	0,           // num_mines
	NULL,        // mines
	numberof(enemies_map11),  // num_enemies
	enemies_map11,  // enemies
	1,           // key_trigger_enemy (enemy 1 reveals key)
	g_Screen11,   // layout
	"THE FORTRESS - LEONARDO VETTORI",  // name
};
