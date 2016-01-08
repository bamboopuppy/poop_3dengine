#include "stdafx.h"
#include "tools.h"

/*
struct 3d_object {
	int id;
	char * name;
	struct d3_points * points;
	GLuint texture_id; GLuint texture_bump_id;
	glm::vec3 position;
};*/
std::vector<d3_object*> d3objects;


static int new_id = 0;
struct d3_object * create_object(char * name, char * object_file, char * texture, char * bump_texture, char * resource_file, glm::vec3 position, unsigned int flags, glm::vec3 rotation)
{
	struct d3_object * newobject = (struct d3_object*) malloc(sizeof(struct d3_object));
	//load_obj(resource_file, object_file, &suzanne_vertices, &suzanne_normals, &suzanne_elements, &suzanne_texture_points, &suzanne_texture_elements, &suzanne_tangents, &suzanne_bitangents);

	newobject->points = get_points(object_file, resource_file);
	//newobject->ibo_elements = gen_buffers(sizeof suzanne_texture_elements[0] * suzanne_texture_elements.size(), suzanne_texture_elements.data());
	newobject->texture_id = get_texture(texture, resource_file);//create_texture_from_resource(resource_file, texture);
	newobject->texture_bump_id = get_texture(bump_texture, resource_file);//create_texture_from_resource(resource_file, bump_texture);
	newobject->position = position;
	newobject->model_matrix = glm::translate(glm::mat4(1.0), position);
	//printf("rotation xyz :(%f,%f,%f)\n", rotation.x, rotation.y, rotation.z);
	if (rotation.x > 0.0f)
		newobject->model_matrix = glm::rotate(newobject->model_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	if (rotation.y > 0.0f)
		newobject->model_matrix = glm::rotate(newobject->model_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	if (rotation.z > 0.0f)
		newobject->model_matrix = glm::rotate(newobject->model_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	newobject->name = malloc_string(name);
	newobject->id = new_id; new_id++; //increment id
	newobject->flags = flags;
	newobject->rotation = rotation;
	newobject->affected_lights = NULL;
	if (newobject->flags & D3_OBJECT_NOPHYSICS)
		newobject->physics_obj = NULL;
	else
		newobject->physics_obj = get_physics_object(newobject);
	
	//printf("vertex %i, normals %i, tangents %i %s %s %s %s\n", newobject->vbo_vertices, newobject->vbo_normals, newobject->vbo_tangents, object_file, texture, bump_texture, resource_file);
	
	return newobject;
}

struct d3_object * find_object_by_name(char * name)
{
	unsigned int i;
	for (i = 0; i < d3objects.size(); i++) {
		if (strncmp(name, d3objects[i]->name, strlen(name)) == 0)
			return d3objects[i];
	}
	return NULL;
}

struct d3_object * find_object_by_id(int id)
{
	unsigned int i;
	for (i = 0; i < d3objects.size(); i++) {
		if (d3objects[i]->id == id)
			return d3objects[i];
	}
	return NULL;
}

void add_d3object (struct d3_object * created_object)
{
	d3objects.push_back(created_object);
	add_rigid_body(created_object->physics_obj); // this adds the physics object to the engine
}

std::vector<d3_object*> get_d3objects()
{
	return d3objects;
}

void delete_object(struct d3_object * object2delete, bool freemem)
{
	unsigned int i;
	for (i = 0; i < d3objects.size(); i++) {
		if (d3objects[i]->id == object2delete->id)
			d3objects.erase(d3objects.begin() + i);
	}
	if (freemem) {
		free(object2delete->name);
		if(object2delete->physics_obj)
			delete_physics_object(object2delete->physics_obj);
		free(object2delete);
	}
}

void clear_object_list(bool freemem)
{
//	unsigned int i;
	struct d3_object * object2delete;
	while (d3objects.size() > 0) {
		if (freemem) {
			object2delete = d3objects[d3objects.size() -1];
			free(object2delete->name);
			if(object2delete->physics_obj)
				delete_physics_object(object2delete->physics_obj);
			free(object2delete);
		}
		d3objects.erase(d3objects.end());
	}
}