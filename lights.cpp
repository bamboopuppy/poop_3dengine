#include "stdafx.h"
#include "tools.h"
//#include <vector>

/*
struct light_source
{
	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float spotCutoff;
	float spotExponent;
	vec3 spotDirection;
	char * lightid;
};

struct light_array
{
	glm::mat4 light_position[MAX_LIGHTS];
	glm::mat3 light_properties[MAX_LIGHTS];
	int num_lights;
}

*/




std::vector<light_source*> lights;
/* MEMORY CREATED:
STRUCT light_source
char * lightid
*/
struct light_source * add_light(glm::vec4 position, glm::vec4 diffuse, glm::vec4 specular, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, float spotCutoff, float spotExponent, glm::vec3 spotDirection, char * lightid)
{
	char * new_light_name;
	struct light_source * new_light = (struct light_source *)malloc(sizeof(struct light_source));
	if (lightid != NULL)
		new_light_name = malloc_string(lightid);
	else {
		new_light_name = NULL;
		printf("LIGHT NAME IS NULL\n");
	}
	new_light->lightid = new_light_name;
	new_light->position = position;
	new_light->diffuse = diffuse;
	new_light->specular = specular;
	new_light->constantAttenuation = constantAttenuation;
	new_light->linearAttenuation = linearAttenuation;
	new_light->quadraticAttenuation = quadraticAttenuation;
	new_light->spotCutoff = spotCutoff;
	new_light->spotExponent = spotExponent;
	new_light->spotDirection = glm::normalize(spotDirection);
	
	lights.push_back(new_light);
	return new_light;
}

struct light_array get_lights(struct d3_object * object)
{
	int i;
	struct light_array new_light_array;
	if (object->affected_lights == NULL)
	{
		new_light_array.num_lights = 0;
		return new_light_array;
	}
	new_light_array.num_lights = object->affected_lights->size();//lights.size();
	int shadow_light_index = -1;
	/* check if a light is a shadow light...*/
	for (i = 0; i < new_light_array.num_lights; i++)
	{
		if (i >= MAX_LIGHTS)
			break;
		if((*object->affected_lights)[i]->cast_shadows){
			struct light_source * lighters = (*object->affected_lights)[i];
			new_light_array.light_position[0][0] = glm::vec4(lighters->position);
			new_light_array.light_position[0][1] = glm::vec4(lighters->diffuse);
			new_light_array.light_position[0][2] = glm::vec4(lighters->specular);
			new_light_array.light_position[0][3] = glm::vec4(lighters->spotDirection, 0.0);
			new_light_array.light_properties[0][0] = glm::vec3(lighters->constantAttenuation, lighters->linearAttenuation, lighters->quadraticAttenuation);
			new_light_array.light_properties[0][1] = glm::vec3(lighters->spotCutoff, lighters->spotExponent, 0.0);
			new_light_array.light_properties[0][2] = glm::vec3(0.0, 0.0, 0.0);
			shadow_light_index = i;
		}
	}
	int array_index = 0;
	if (shadow_light_index != -1)
		array_index = 1;
	//printf("num lights: %i shadow_light_index:%i\n", new_light_array.num_lights, shadow_light_index);
	for (i = 0; i < new_light_array.num_lights; i++) {
		if (i >= MAX_LIGHTS)
			break;
		if (i == shadow_light_index)
			continue;
		struct light_source * lighters = (*object->affected_lights)[i];
		new_light_array.light_position[array_index][0] = glm::vec4(lighters->position);
		new_light_array.light_position[array_index][1] = glm::vec4(lighters->diffuse);
		new_light_array.light_position[array_index][2] = glm::vec4(lighters->specular);
		new_light_array.light_position[array_index][3] = glm::vec4(lighters->spotDirection, 0.0);
		new_light_array.light_properties[array_index][0] = glm::vec3(lighters->constantAttenuation, lighters->linearAttenuation, lighters->quadraticAttenuation);
		new_light_array.light_properties[array_index][1] = glm::vec3(lighters->spotCutoff, lighters->spotExponent, 0.0);
		new_light_array.light_properties[array_index][2] = glm::vec3(0.0, 0.0, 0.0);
		array_index++;

	}
	return new_light_array;
}

struct light_source * get_light_obj(char * id)
{
	unsigned int i;
	for (i = 0; i < lights.size(); i++) {
		if (strncmp(lights[i]->lightid, id, strlen(id) + 1) == 0) {
			return lights[i];
		}
	}
	return NULL;
}