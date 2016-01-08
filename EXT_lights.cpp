#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"


/*struct light_object {
	struct light_source * display_light;
	int grid_location[2];
	struct d3_object * visual_light;
};*/
bool check_if_light_change_required( int * new_grid)
{
	struct light_source * old_shadow_light = get_shadow_light();
	std::vector<light_source*> * grid_lights_vector = get_lights_from_grid_spot(new_grid);
	if(grid_lights_vector == NULL || grid_lights_vector->size() == 0) {
		printf("no lights here so leave shadows the same, I guess\n");
		return false;
	}
	struct light_source * new_shadow_light = (*grid_lights_vector)[0];
	if ((void*) new_shadow_light == (void*) old_shadow_light) {
		printf("new shadow light is same as old so leave it alone\n");
		return false;
	}
	return true;
}
void change_shadow_light_to_brightest(int * new_grid)
{
	//struct light_source * old_shadow_light = get_shadow_light();
	std::vector<light_source*> * grid_lights_vector = get_lights_from_grid_spot(new_grid);
	if(grid_lights_vector == NULL || grid_lights_vector->size() == 0) {
		printf("no lights here so leave shadows the same, I guess\n");
		return;
	}
	struct light_source * new_shadow_light = (*grid_lights_vector)[0];
	//if ((void*) new_shadow_light == (void*) old_shadow_light) {
	//	printf("new shadow light is same as old so leave it alone\n");
	//	return;
	//}
	disable_shadows_on_all_lights();
	printf("setting new shadow light\n");
	set_shadow_light(new_shadow_light);
}

void add_point_light(int grid[2], char * name)
{
	glm::vec3 light_position = get_world_position_from_grid(grid);
	struct object_definition * object2add = get_definition_by_name(name);
	struct light_source * new_light_source = add_light(glm::vec4(light_position.x + object2add->offset.x, light_position.y + object2add->offset.y -0.05, light_position.z + object2add->offset.z, 1.0), glm::vec4(1.0,  1.0,  1.0, 1.0), glm::vec4(1.0,  1.0,  1.0, 1.0), 0.0, 0.0, 1.0, 180.0, 0.0, glm::vec3(1.0, 1.0, 0.0), "test_light1");
	struct world_object * new_world_object = add_object_to_grid(object2add, grid, glm::vec3(0.0f, 0.0f, 0.0f));
	struct d3_object * added_object = new_world_object->visual_object;
	added_object->flags = added_object->flags | D3_OBJECT_NOSHADOW | D3_OBJECT_STATIC;
	struct light_object * new_light_object = (struct light_object*) malloc(sizeof(struct light_object));
	new_light_object->visual_light = new_world_object;
	new_light_object->grid_location[0] = grid[0]; new_light_object->grid_location[1] = grid[1];
	new_light_object->display_light = new_light_source;
	//struct d3_object * created_object = NULL;
	//created_object = create_object(malloc_string("some_light"), "small_sphere.obj", "light_sphere.png", "light_sphere_normal_map.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\wall.poo", glm::vec3(position.x, position.y, position.z), D3_OBJECT_NOSHADOW | D3_OBJECT_STATIC);
	//new_light->visual_light = created_object;
	//add_d3object(created_object);
	disable_shadows_on_all_lights();
	new_light_source->cast_shadows = true;
	set_shadow_light(new_light_source);
	add_light_object_to_grid(new_light_object);
}

float check_if_light_affects_object(struct world_object * object, struct light_object * light)
{
	struct light_source * lightsource = light->display_light;
	float light_distance = 2.0f * get_grid_distance_two_objects(object, light->visual_light);// 2.0 since each grid is actually 2 units long
	if (light_distance == 0.0f)
		light_distance = 1.0f;
		//return 1.0f;
	float attenuation = (1.0f / (lightsource->constantAttenuation + lightsource->linearAttenuation * light_distance + lightsource->quadraticAttenuation * light_distance * light_distance));
	printf("distance: %f attenuation: %f grid(%i,%i)\n", light_distance, attenuation, object->grid_location[0], object->grid_location[1]);
	//if (attenuation < LIGHT_AFFECT_THRESHOLD)
	//	return false;
	//return true;
	return attenuation;
}