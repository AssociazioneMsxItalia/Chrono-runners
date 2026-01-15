extern unsigned char g_Level2CrystalChemMaxPrivrat[];

struct Platform plat_crystalchem[] = {
   {19*8,  6*8,  // pos_x pos_y
      -1,    0,  // dir_x dir_y
    15*8,  6*8,  // min_x min_y
    25*8,  6*8}, // max_x max_y

   {28*8,  8*8,  // pos_x pos_y
       0,    1,  // dir_x dir_y
    28*8,  5*8,  // min_x min_y
    28*8, 14*8}, // max_x max_y
};

struct Enemy enemies_crystalchem[] = {
  {13*8, 14*8,  // pos_x pos_y
			1,  // dir_x
   12*8, 19*8,  // min_x max_x
			1,  // type (0-3)
			0,  // mDX (initialized to 0)
			0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

  {23*8, 17*8,  // pos_x pos_y
		   -1,  // dir_x
   13*8, 29*8,  // min_x max_x
	     	1,  // type (0-3)
	     	0,  // mDX (initialized to 0)
	     	0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX

  {14*8, 21*8,  // pos_x pos_y
		   -1,  // dir_x
    4*8, 24*8,  // min_x max_x
	     	1,  // type (0-3)
	     	0,  // mDX (initialized to 0)
	     	0,  // stunned_timer (initialized to 0)
			0,  // field_state
			0,  // field_timer
			0,  // field_x
			0,  // field_y
			0}, // field_mDX
  };

  struct Level level_crystalchem = {
	28, 21,      // start_x start_y
	12, 4,       // end_x end_y
	5, 10,      // key_x key_y
	0, 0,        // crystal_x crystal_y
	2,           // num_platforms
	plat_crystalchem,       // platforms
	0,           // num_mines
	NULL,        // mines
	3,           // num_enemies
	enemies_crystalchem,        // enemies
	g_Level2CrystalChemMaxPrivrat,   // layout
	"CRYSTALCHEM - MAX PRIVRAT", // name
};
