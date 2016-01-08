#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"
#define MAX_GRIDS 10

struct grid_of_objects grid_array[MAX_GRIDS];
int active_grid;
glm::vec3 get_world_position_from_grid(int grid[2])
{
	glm::vec3 position = glm::vec3(2.0 * grid[0], 2.0, 2.0* grid[1]);
	return position;
}

void get_grid_position_from_world(int * grid, glm::vec3 position) //grid is always size 2
{
	grid[0] = (int) floor(position.x / 2 + 0.5);
	grid[1] = (int) floor(position.z / 2 + 0.5);
}
bool check_if_valid_position(int position[2])
{
	if (position[0] < 0 || position[0] >= grid_array[active_grid].width || position[1] < 0 || position[1]  >= grid_array[active_grid].height) {
		printf("position is out of bounds for grid???? (%i, %i) grid_width: %i height: %i active_grid:%i\n", position[0], position[1], grid_array[active_grid].width, grid_array[active_grid].height, active_grid);
		return false;
	}
	return true;
}

bool add_world_object_to_grid_vector(int position[2], struct world_object * created_world_object)
{
	/*if (position[0] < 0 || position[0] >= grid_array[active_grid].width || position[1] < 0 || position[1]  >= grid_array[active_grid].height) {
		printf("position is out of bounds for grid???? (%i, %i) grid_width: %i height: %i active_grid:%i\n", position[0], position[1], grid_array[active_grid].width, grid_array[active_grid].height, active_grid);
		return false;
	}*/
	if (!check_if_valid_position(position))
		return false;
	int grid_vector_pos = position[1] * grid_array[active_grid].width + position[0]; //translate 2dimensions into 1dimension vector
	if (grid_array[active_grid].whole_grid[grid_vector_pos] == NULL) {//have to create vector for this spot
		std::vector<world_object*> * new_empty_vector = new std::vector<world_object*>();
		//grid_array[active_grid].whole_grid[grid_vector_pos] = (std::vector<world_object*> *)malloc(sizeof(std::vector<world_object*>));
		grid_array[active_grid].whole_grid[grid_vector_pos] = new_empty_vector;
		if (grid_array[active_grid].whole_grid[grid_vector_pos] == NULL) {
			printf(" couldn't allocate memory for object on grid, that sucks\n");
			return false;
		}
	}
	grid_array[active_grid].whole_grid[grid_vector_pos]->push_back(created_world_object);
	printf("added item to grid (%i,%i)\n", position[0], position[1]);
	return true;
}
void disable_shadows_on_all_lights()
{
	int j;
	for(j = 0; j < grid_array[active_grid].lights->size(); j++)
	{
		(*grid_array[active_grid].lights)[j]->display_light->cast_shadows = false;
	}
}

std::vector<light_source*> * get_lights_from_grid_spot(int * grid)
{
	if (!check_if_valid_position(grid)){
		printf("illegal grid coordinates (%i,%i)\n", grid[0], grid[1]);
		return NULL;
	}
	int i = grid[0] + (grid[1] * grid_array[active_grid].width);
	return grid_array[active_grid].whole_grid_lights[i];
}

void rebuild_grid_lighting(int gridid)
{
	int position[2];
	struct world_object some_object;
	unsigned int i, j, k;
	int light_array_size = grid_array[active_grid].lights->size();
	int actual_light_size = 0;
	float * attenuation_array = (float *) malloc(sizeof(float) * light_array_size);
	struct light_source ** light_source_array = (struct light_source **) malloc(sizeof(light_source *) * light_array_size);
	for(i = 0; i < grid_array[active_grid].width * grid_array[active_grid].height; i++)
	{
		if (grid_array[active_grid].whole_grid[i] == NULL || grid_array[active_grid].whole_grid[i]->size() == 0)
			continue; // there's no objects there so why bother calculating lighting?
		actual_light_size = 0;
		some_object.grid_location[0] = i % grid_array[active_grid].width;
		some_object.grid_location[1] = i / grid_array[active_grid].width;
		if (grid_array[active_grid].whole_grid_lights[i] != NULL)
			grid_array[active_grid].whole_grid_lights[i]->clear();
		for(j = 0; j < grid_array[active_grid].lights->size(); j++)
		{
			float attenuation = check_if_light_affects_object(&some_object, (*grid_array[active_grid].lights)[j]);
			if (attenuation == 1.0)
				printf("attenuation is 1.0\n");
			if (attenuation > LIGHT_AFFECT_THRESHOLD)
			{
				if (grid_array[active_grid].whole_grid_lights[i] == NULL)
					grid_array[active_grid].whole_grid_lights[i] = new std::vector<light_source*>();
				if (grid_array[active_grid].whole_grid_lights[i] == NULL) {
					printf("couldn't allocate grid light info while rebuilding grid_lighting, god damn\n");\
					free(attenuation_array); free(light_source_array);
					return;
				}
				attenuation_array[actual_light_size] = attenuation;
				light_source_array[actual_light_size] = (*grid_array[active_grid].lights)[j]->display_light;
				actual_light_size++;
			}
		}
		if(actual_light_size) //if there are lights with affect on the grid we should sort them and add them to the grid light stuff
		{
			if (i == grid_array[active_grid].width * 3 + 1)
				printf("we are at the interesting marker\n");
			int index_maximum;// = 0;
			float attenuation_maximum;
			for(j = 0; j < actual_light_size; j++){
				index_maximum = 0;
				attenuation_maximum = attenuation_array[0];
				for(k = 1; k < actual_light_size; k++) {
					if (attenuation_array[k] > attenuation_maximum) {
						index_maximum = k;
						attenuation_maximum = attenuation_array[k];
					}
				}
				grid_array[active_grid].whole_grid_lights[i]->push_back(light_source_array[index_maximum]);
				attenuation_array[index_maximum] = 0.0;
			}
		}
		for(j = 0; j < grid_array[active_grid].whole_grid[i]->size(); j++) // add the list of lights to all the objects in the grid location
		{
			(*grid_array[active_grid].whole_grid[i])[j]->visual_object->affected_lights = grid_array[active_grid].whole_grid_lights[i];
		}

	}
	free(attenuation_array); free(light_source_array);
}

struct world_object * add_object_to_grid(struct object_definition * object2add, int position[2], glm::vec3 offset)
{
	struct d3_object * created_object = NULL;
	struct world_object * created_world_object = NULL;
	glm::vec3 world_position = get_world_position_from_grid(position);
	created_object = create_object(object2add->name, object2add->mesh, object2add->texture, object2add->normal_map, object2add->resource_file,
				glm::vec3(world_position.x +object2add->offset.x + offset.x ,world_position.y +object2add->offset.y + offset.y, world_position.z +object2add->offset.z + offset.z), object2add->flags, object2add->rotation);
	created_world_object = (struct world_object *) malloc(sizeof(struct world_object));
	created_world_object->visual_object = created_object;
	created_world_object->definition = object2add;
	created_world_object->world_object_id = created_object->id;
	created_world_object->grid_location[0] = position[0]; created_world_object->grid_location[1] = position[1];
	add_world_object_to_grid_vector(position, created_world_object);
	//add_d3object(created_object);
	return created_world_object;
	//printf("adding object %s to %i, %i\n", object2add->name, position[0], position[1]);
}

bool add_light_object_to_grid(struct light_object * created_light_object)
{
	if (!check_if_valid_position(created_light_object->grid_location))
		return false;
	grid_array[active_grid].lights->push_back(created_light_object);
	return true;
}

void delete_grid(int grid_id)
{
	printf("deleting grid, butt this does nothing...\n");
	/* loop this to delete I guess */
}

void create_grid(int grid_id, int width, int height)
{
	int i;
	if (grid_id < 0 || grid_id >= MAX_GRIDS) {
		printf("grid_id %i is out of bounds of the maximum amount of grids: %i\n", grid_id, MAX_GRIDS);
	}
	active_grid = grid_id;
	if (grid_array[grid_id].width != 0)
		delete_grid(grid_id);
	grid_array[grid_id].width = width;
	grid_array[grid_id].height = height;
	grid_array[grid_id].grid_id = grid_id;
	grid_array[grid_id].whole_grid = (std::vector<world_object*> **) malloc(sizeof(std::vector<world_object*> **) * width * height);
	grid_array[grid_id].whole_grid_lights = (std::vector<light_source*> **) malloc(sizeof(std::vector<light_source*> **) * width * height);
	grid_array[grid_id].lights = new std::vector<light_object*>();
	printf("sizeof: %i, total_size: %i\n", sizeof(std::vector<world_object*> **), sizeof(std::vector<world_object*> **) * width * height);
	for(i = 0; i < (width * height); i++) {
		grid_array[grid_id].whole_grid[i] = NULL;
		grid_array[grid_id].whole_grid_lights[i] = NULL;
	}
}

void clear_and_send_grid_to_engine(int grid_id, int position[2], int width, int height)
{
	int i,j;
	unsigned int k;
	int grid_vector_pos;
	active_grid = grid_id;
	struct grid_of_objects * current_grid = &grid_array[grid_id];
	clear_object_list(false); //clear the list in the engine
	int width_bounds[2] = {position[0] - width/2, position[0] + width/2};
	int height_bounds[2] = {position[1] -height/2, position[1] + height/2};
	if (width_bounds[0] < 0) width_bounds[0] = 0; //correct bounds if they fall out of range
	if (width_bounds[1] > current_grid->width) width_bounds[1] = current_grid->width -1;
	if (height_bounds[0] < 0) height_bounds[0] = 0; //correct bounds if they fall out of range
	if (height_bounds[1] > current_grid->height) height_bounds[1] = current_grid->height -1;
	printf("height_bounds(%i,%i) width_bounds(%i,%i)\n", height_bounds[0], height_bounds[1], width_bounds[0], width_bounds[1]);
	for (i = height_bounds[0]; i <= height_bounds[1]; i++)
	{
		for (j = width_bounds[0]; j <= width_bounds[1]; j++)
		{
			 grid_vector_pos = i * current_grid->width + j;
			 if (current_grid->whole_grid[grid_vector_pos] == NULL){ 
				 //printf("nothing on grid(%i,%i)\n", j, i);
				 continue; // nothing in that grid location so continue
			 }
			 for (k = 0; k < current_grid->whole_grid[grid_vector_pos]->size(); k++) {
				 //printf("adding objects to engine...%i\n", k);
				 add_d3object((*current_grid->whole_grid[grid_vector_pos])[k]->visual_object); // this adds the visual object to the engine for displaying
				 //add_rigid_body((*current_grid->whole_grid[grid_vector_pos])[k]->visual_object->physics_obj); // this adds the physics object to the engine
			 }


		}
	}

}

float get_grid_distance_two_objects(struct world_object* obj1, struct world_object*  obj2)
{
	int obj1_position[2] = {obj1->grid_location[0], obj1->grid_location[1]};
	int obj2_position[2] = {obj2->grid_location[0], obj2->grid_location[1]};
	float deltas[2] = {abs(obj1_position[0]-obj2_position[0]), abs(obj1_position[1]-obj2_position[1])};
	float distance = sqrt((float)(deltas[0] * deltas[0] + deltas[1] + deltas[1]));
	return distance;
}

float get_grid_distance_two_points(int obj1_position[2], int obj2_position[2])
{
	//int obj1_position[2] = {obj1->grid_location[0], obj1->grid_location[1]};
	//int obj2_position[2] = {obj2->grid_location[0], obj2->grid_location[1]};
	float deltas[2] = {abs(obj1_position[0]-obj2_position[0]), abs(obj1_position[1]-obj2_position[1])};
	float distance = sqrt((float)(deltas[0] * deltas[0] + deltas[1] + deltas[1]));
	return distance;
}