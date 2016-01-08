#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"

std::vector<object_definition*> object_definitions;

struct object_definition * add_object_definition(char * name, char * mesh, char * texture, char * normal_map, char * resource_file, glm::vec3 offset, unsigned int flags, glm::vec3 rotation)
{
	unsigned int i;
	for (i = 0; i < object_definitions.size(); i++) {
		if (strncmp(name, object_definitions[i]->name, strlen(name)) == 0)
			return object_definitions[i];
	}
	struct object_definition * new_definition = (struct object_definition*) malloc(sizeof(struct object_definition));
	new_definition->name = malloc_string(name); new_definition->mesh = malloc_string(mesh);
	new_definition->texture = malloc_string(texture); new_definition->normal_map = malloc_string(normal_map);
	new_definition->resource_file = malloc_string(resource_file);
	new_definition->offset = offset;
	new_definition->flags = flags;
	new_definition->rotation = rotation;
	object_definitions.push_back(new_definition);
	return new_definition;
}

struct object_definition * get_definition_by_name(char * name)
{
	unsigned int i;
	for (i = 0; i < object_definitions.size(); i++) {
		if (strncmp(name, object_definitions[i]->name, strlen(name)) == 0)
			return object_definitions[i];
	}
	printf("couldn't find object name: %s\n", name);
	return NULL;
}