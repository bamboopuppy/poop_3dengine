#include "stdafx.h"
#include "tools.h"

std::vector<shader_struct*> shader_objects;

GLuint create_shader(const char* filename, GLenum type, char * header)
{
  const GLchar* source = file_read(filename);
  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename); perror("");
    return 0;
  }
  GLuint res = glCreateShader(type);
  if (header != NULL) {
	  const GLchar* sources[3] = {
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"
    "	#define GLES2\n",
#else
		"#version 120\n",
	//"#extension GL_EXT_gpu_shader4 : enable\n",
#endif
		header,
		source };
	 glShaderSource(res, 3, sources, NULL);
  } else {
	const GLchar* sources[2] = {
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"
		"#define GLES2\n",
#else
		"#version 120\n",
		//"#extension GL_EXT_gpu_shader4 : enable\n",
#endif
		source };
	glShaderSource(res, 2, sources, NULL);
  }
  free((void*)source);

  glCompileShader(res);
  GLint compile_ok = GL_FALSE;
  glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    fprintf(stderr, "%s:", filename);
    print_log(res);
    glDeleteShader(res);
    return 0;
  }

  return res;
}

GLuint load_shaders(char * v, char * f, char * header)
{
	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	GLuint fs = 0;
    GLuint vs = 0;
	GLuint new_shader;
	if ((vs = create_shader(v, GL_VERTEX_SHADER, NULL))   == 0) return LEETERROR;
	if ((fs = create_shader(f, GL_FRAGMENT_SHADER, header)) == 0) return LEETERROR;
	new_shader = glCreateProgram();
	glAttachShader(new_shader, vs);
	glAttachShader(new_shader, fs);
	glLinkProgram(new_shader);
	glGetProgramiv(new_shader, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
	  fprintf(stderr, "glLinkProgram:");
		print_log(new_shader);
	  return LEETERROR;
	}
	return new_shader;
}
/*
struct shader_struct {
	GLuint shader_program;
	int shadow_lights;
	int noshadow_lights;
	int nospecular_lights;
};*/
GLuint get_shader(int shadow_point_lights, int noshadow_point_lights, int nospecular_point_lights)
{
	/*if (shadow_point_lights + noshadow_point_lights + nospecular_point_lights == 0) { //no lights are lighting this area up
		nospecular_point_lights = 1; // make sure we have a fake light with zero intensity to match this...
	}*/
	unsigned int i;
	//shadow_point_lights = 1;
	for (i = 0; i < shader_objects.size(); i++) {
		if (shader_objects[i]->shadow_lights == shadow_point_lights && shader_objects[i]->noshadow_lights == noshadow_point_lights && shader_objects[i]->nospecular_lights == nospecular_point_lights)
			return shader_objects[i]->shader_program;
	}
	
	int max_lights = shadow_point_lights + noshadow_point_lights + nospecular_point_lights;
	char shader_header[256];
	if (max_lights){
	sprintf(shader_header,"#define MAX_LIGHTS %i\n#define MAX_SHADOW_LIGHTS %i\n#define MAX_NOSHADOW_LIGHTS %i\n#define MAX_SIMPLE_LIGHTS %i\nuniform mat4 light_position[MAX_LIGHTS];\nuniform mat3 light_properties[MAX_LIGHTS];\n",
				shadow_point_lights + noshadow_point_lights + nospecular_point_lights,
				shadow_point_lights, noshadow_point_lights, nospecular_point_lights);
	} else {
		sprintf(shader_header,"#define MAX_LIGHTS %i\n#define MAX_SHADOW_LIGHTS %i\n#define MAX_NOSHADOW_LIGHTS %i\n#define MAX_SIMPLE_LIGHTS %i\nuniform mat4 light_position[1];\nuniform mat3 light_properties[1];\n",
				shadow_point_lights + noshadow_point_lights + nospecular_point_lights,
				shadow_point_lights, noshadow_point_lights, nospecular_point_lights);
	}
	printf("shader header: %s \n\n", shader_header);
	GLuint shader_program = load_shaders("C:\\bin\\january_2016_jam\\3d_spaceman_sim\\triangle.v.glsl", "C:\\bin\\january_2016_jam\\3d_spaceman_sim\\triangle.f2.glsl", shader_header);
	struct shader_struct * new_shader_struct = (struct shader_struct *) malloc(sizeof(struct shader_struct));
	new_shader_struct->noshadow_lights = noshadow_point_lights;
	new_shader_struct->nospecular_lights = nospecular_point_lights;
	new_shader_struct->shadow_lights = shadow_point_lights;
	new_shader_struct->shader_program = shader_program;
	shader_objects.push_back(new_shader_struct);
	return shader_program;
}

GLuint load_shader_for_object(struct d3_object * object)
{
	//printf("load_shader_for_object\n");
	int i = 0;
	GLuint shader_program;
	int num_lights = 0;
	if (object->affected_lights != NULL){
		//printf("affected_lights IS NOT NULL\n");
		num_lights = object->affected_lights->size();
	}
	//printf("num_lights %i, i: %i\n", num_lights, i);
	int shadow_lights = 0;
	int noshadow_lights = 0;
	int nospecular_lights = 0;
	bool cast_shadows = false;
	/* check to see if one of the lights is a shadow casting one */
	for (i=0; i < num_lights; i++)
	{
		//printf("num_lights %i, i: %i\n", num_lights, i);
		if ((*object->affected_lights)[i]->cast_shadows) {
			cast_shadows = true;
			shadow_lights++;
			break;
		}
	}
	noshadow_lights = num_lights - shadow_lights;
	//shadow_lights = 0; /*test */
	shader_program = get_shader(shadow_lights, noshadow_lights, nospecular_lights);
	//printf("shader loaded:(%i, %i, %i)\n", shadow_lights, noshadow_lights, nospecular_lights);
	glUseProgram(shader_program);
	return shader_program;
}