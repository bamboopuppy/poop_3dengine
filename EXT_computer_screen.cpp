#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"

#define BASE_BLUE 24
void add_computer_screen(int grid[2], char * name, char * screen_name, glm::vec3 screen_offset, char * xml_info)
{
	//float empty_offset[3] = {0.0f, 0.0f, 0.0f};
	printf("searching xml for %s\n", xml_info);
	int number_of_lines = get_xml_element_attribute(xml_info, "lines");
	printf("number of lines: %i\n", number_of_lines);

	int i, error;
	char element_id[6];
	int height_width = 128;

	struct object_definition * object2add = get_definition_by_name(name);
	struct world_object * new_world_object = add_object_to_grid(object2add, grid, glm::vec3(0.0f, 0.0f, 0.0f));
	struct d3_object * computer_case = new_world_object->visual_object;
	object2add = get_definition_by_name(screen_name);
	new_world_object = add_object_to_grid(object2add, grid, screen_offset);
	struct d3_object * computerscreen = new_world_object->visual_object;

	unsigned int position[2] = {height_width/2, 64};
	SDL_Rect stencil_rect;
	glm::vec3 light_position = get_world_position_from_grid(grid);
	
	weld_2_objects(computer_case, computerscreen);

	unsigned char color[3] = {25,255,255};
	SDL_Surface * bigsurface = SDL_CreateRGBSurface(0, height_width, height_width, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	SDL_Surface * stencil_surface = SDL_CreateRGBSurface(0, height_width, height_width, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	SDL_FillRect(bigsurface, NULL ,0xff001418+(BASE_BLUE<< 16));
	SDL_FillRect(stencil_surface, NULL, 0x008080ff);
	SDL_Surface * new_surface;
	for (i=0; i < number_of_lines; i++)
	{
		sprintf(element_id,"line%i", i+1);
		const char * new_text = get_xml_element_of_element_of_element("text", element_id, xml_info);
		printf("for line %i, text: %s\n", i+1, new_text);
		color[0] = (BASE_BLUE + 1 + i); //adjust the text blue color
		unsigned int background_color = 0xff001418 + ((BASE_BLUE + i + 1) << 16); // adjust the background blue color
		new_surface = render_text_to_surface((char*)new_text, color, glm::vec3(0.0f, 0.0f, 0.0f));
		int test_negative = ( ((height_width *(i+1))/number_of_lines)) - new_surface->h;
		if (test_negative < 0 || test_negative > height_width) {
			printf("text falling out of bounds on computer screen: %i\n", test_negative);
			continue;
		}
		position[1] = (unsigned int) test_negative;//(height_width * ((i+1)/number_of_lines)) - new_surface->h;
		stencil_rect.x = position[0]; stencil_rect.y = position[1];
		stencil_rect.w = new_surface->w; stencil_rect.h = new_surface->h;
		error = SDL_FillRect(bigsurface, &stencil_rect, background_color);
		if (error)
			printf("some bullshit error while filling rectangle with colors\n");
		add_surface_to_surface(bigsurface, new_surface ,position);
		SDL_FreeSurface(new_surface);
	}
	set_texture_to_object(bigsurface, stencil_surface, computerscreen);
	//SDL_FreeSurface(new_surface);
	//DL_FreeSurface(new_surface2);
	SDL_FreeSurface(bigsurface);
	SDL_FreeSurface(stencil_surface);
}