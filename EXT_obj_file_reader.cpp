#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"
/*
struct object_definition {
	char * name;
	char * mesh;
	char * texture;
	char * normal_map;
	char * resource_file;
	glm::vec3 offset;
};*/


void read_map(char * resource_filename, char * filename)
{
	int filesize;
	unsigned int flags = 0;
	char name[128]; char mesh[128]; char texture[128]; char normal_map[128]; char resource_file[128];
	glm::vec3 offset;
	glm::vec3 rotation;
	int grid[2];
	char * in = get_char_from_file(resource_filename,filename, &filesize);
	if (filesize == 0) {
		printf(" couldn't load object %s from resource %s\n", filename, resource_filename);
	}
	char * line = in;//strtok(in, (const char *)"\n\r");
	//char * line = strtok(in, (const char *)"\r");
    while (line)//getline(in, line))
    {
		//printf("line: %s\n", line);
        if (strncmp(line,"d ",2) == 0) { //definition
			sscanf(line, "d %s %s %s %s %s %f %f %f %u %f %f %f", name, mesh, texture, normal_map,resource_file, &offset.x, &offset.y, &offset.z, &flags, &rotation.x, &rotation.y, &rotation.z);
			if(strncmp("null", resource_file, strlen(resource_file)) == 0)
				add_object_definition(name, mesh, texture, normal_map, resource_filename, offset, flags, rotation); // if resource file is null then it is part of the existing resource file
			else
				add_object_definition(name, mesh, texture, normal_map, (char *)resource_file, offset, flags, rotation);
		}
		else if(strncmp(line,"o ",2) == 0) {
			sscanf(line, "o %s %i %i", name, &grid[0], &grid[1]);
			struct object_definition * object2add = get_definition_by_name(name);
			add_object_to_grid(object2add, grid, glm::vec3(0.0f, 0.0f, 0.0f));
		}
		else if(strncmp(line,"c ",2) == 0) {
			sscanf(line, "c %i %i", &grid[0], &grid[1]);
			glm::vec3 character_position = get_world_position_from_grid(grid);

//			transport_character(character_position);
			struct character_kinematic * new_character = create_character(0.5, 1.0, character_position);
			follow_character(new_character);
		}
		else if(strncmp(line,"lp",2) == 0) {
			sscanf(line, "lp %s %i %i", name, &grid[0], &grid[1]);
			add_point_light(grid, name);
		}
		else if(strncmp(line,"cs",2) == 0) {
			sscanf(line, "cs %s %i %i %s %f %f %f %s", name, &grid[0], &grid[1], mesh, &offset.x, &offset.y, &offset.z, texture);
			add_computer_screen(grid, name, mesh, offset, texture);
		}
		else if(strncmp(line,"gr",2) == 0) {
			sscanf(line, "gr %i %i %i", &flags, &grid[0], &grid[1]);
			create_grid(flags, grid[0], grid[1]);
		}
		while (line[0] != '\n') {
			line++;
			if (line[0] == EOF) {
				line = NULL;
				break;
			}
		}
		if (line)
			line++;
		//line = //strtok(NULL, (const char *)"\n\r");
		//line = strtok(in, (const char *)"\r");
	}


}
