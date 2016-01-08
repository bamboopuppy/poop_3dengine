#include "stdafx.h"
#include "tools.h"
/*
struct d3_texture {
	char * mfilename;
	GLuint texture_id;
}*/
std::vector<d3_texture*> textures;

struct d3_texture * get_texture(char * filename, char * resource)
{
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {
		if (strncmp(filename, "dynamic", strlen("dynamic")) == 0) {
			break;
		}
		if (strncmp(filename, textures[i]->mfilename, strlen(filename)) == 0)
			return textures[i];
	}
	//texture not found in database so add it
	struct d3_texture * new_texture = (d3_texture*)malloc(sizeof(struct d3_texture));
	new_texture->mfilename = malloc_string(filename);
	new_texture->texture_id = create_texture_from_resource(resource, filename);
	textures.push_back(new_texture);
	return new_texture;
}

void delete_d3_texture(struct d3_texture * texture2delete)
{
	unsigned int i;
	for (i = 0; i < textures.size(); i++) {
		if (strncmp(texture2delete->mfilename, textures[i]->mfilename, strlen(texture2delete->mfilename)) == 0)
			textures.erase(textures.begin() + i);
	}
	glDeleteTextures(1, &texture2delete->texture_id);
	free(texture2delete);
}

void gpu_texture(GLuint * texture_id, unsigned char * img, int img_width, int img_height)
{
	glGenTextures(1, texture_id);
	glBindTexture(GL_TEXTURE_2D, *texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glBindTexture(GL_TEXTURE_2D, 0);
}
GLuint create_texture_from_resource(char * resource_filename, char * filename)
{
	/*if (resource_filename == NULL || filename == NULL) {
		printf("resource filenames are NULL??\n");
		return -1;
	}
	GLuint texture_id;
	char * mallocimg;
	int img_height, img_width, comp;
	ResourceFileReader readTest(resource_filename);
	readTest.read();
	mallocimg = readTest.getFile(filename); //mallocimg = MEMORY
	*/
	GLuint texture_id;
	int img_height, img_width, comp;
	int filesize;

	if (strncmp(filename, "dynamic", strlen("dynamic")) == 0) {
		return 0;
	}
	char * mallocimg = get_char_from_file(resource_filename, filename, &filesize);
	if ( filesize == 0) {
		printf("couldn't get texture file %s from resource file %s\n", filename, resource_filename);
		return -1;
	}
	unsigned char * img = SOIL_load_image_from_memory ( (const unsigned char *const)mallocimg, filesize, &img_width, &img_height, &comp, SOIL_LOAD_RGBA);
	//char * img = SOIL_load_image("C:\\bin\\3d_spaceman_sim\\box.png", &img_width, &img_height, &comp, SOIL_LOAD_RGBA);
	gpu_texture(&texture_id, img, img_width, img_height);
	printf("loaded texture %u from file %s\n", texture_id, filename);
	SOIL_free_image_data(img);
	delete mallocimg;
	return texture_id;
}
