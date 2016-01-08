#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"

int main(int argc, char** argv)
{
	initilize_engine(argc, argv);
	init_keyboard_keys();
	read_xml_doc("C:\\bin\\january_2016_jam\\3d_spaceman_sim\\data.xml");
	read_map("C:\\bin\\january_2016_jam\\3d_spaceman_sim\\wall.poo", "map.txt");
	ext_init_character();
	int position[2] = {1,1};
	clear_and_send_grid_to_engine(0, position, 30, 30);
	rebuild_grid_lighting(0);
	glutMainLoop();
	getchar();
	return 0;
}