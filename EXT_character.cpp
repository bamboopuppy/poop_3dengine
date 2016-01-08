#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"

int character_grid_position[2] = {0,0};

void fade_in_and_out_new_shadows(int * new_grid)
{
	float * new_data = (float *) malloc(sizeof(float));
	*new_data = 1.0;
	add_event(fade_out_shadows, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE *1), new_data);
	int * new_grid_malloc = (int *) malloc(sizeof(int) * 2);
	new_grid_malloc[0] = new_grid[0]; new_grid_malloc[1] = new_grid[1];
	add_event(change_shadow_to_new_grid, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE *20), new_grid_malloc);
}
void character_changed_grid(int * new_grid)
{
	
	if (check_if_light_change_required(new_grid))
		fade_in_and_out_new_shadows(new_grid);
	//change_shadow_light_to_brightest(new_grid);
}

void character_moved_callback(glm::vec3 position)
{
	int grid[2];
	get_grid_position_from_world(grid, position);
	printf("character moved (%f, %f, %f) GRID: (%i, %i)\n",position.x, position.y, position.z, grid[0], grid[1]);
	if(grid[0] != character_grid_position[0] || grid[1] != character_grid_position[1])
	{
		character_changed_grid(grid);
	}
	character_grid_position[0] = grid[0]; character_grid_position[1] = grid[1];
}

void ext_init_character()
{
	register_character_move_callback(character_moved_callback);
}