/**
 * Store all the file's contents in memory, useful to pass shaders
 * source code to OpenGL
 */
/* Problem:
 *  We should close the input file before the return NULL; statements but this would lead to a lot of repetition (DRY)
 *   -you could solve this by using goto or by abusing switch/for/while + break or by building an if else mess
 *  better solution: let the user handle the File: char* file_read(const FILE* input)
*/
#include "stdafx.h"
#include "tools.h"

/* Using the GLUT library for the base windowing setup */

char* file_read(const char* filename)
{
  FILE* input = NULL;// = fopen(filename, "rb");
  errno_t errorCode = fopen_s(&input,filename,"rb");
  if(input == NULL) return NULL;
  
  if(fseek(input, 0, SEEK_END) == -1) return NULL;
  long size = ftell(input);
  if(size == -1) return NULL;
  if(fseek(input, 0, SEEK_SET) == -1) return NULL;
  	
  /*if using c-compiler: dont cast malloc's return value*/
  char *content = (char*) malloc( (size_t) size +1  ); 
  if(content == NULL) return NULL;
  
  fread(content, 1, (size_t)size, input);
  if(ferror(input)) {
    free(content);
    return NULL;
  }

  fclose(input);
  content[size] = '\0';
  return content;
}

void print_log(GLuint object)
{
  GLint log_length = 0;
  if (glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if (glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "printlog: Not a shader or a program\n");
    return;
  }

  char* log = (char*)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  fprintf(stderr, "%s", log);
  free(log);
}
/**
 * Compile the shader from file 'filename', with error handling
 */
/*
GLuint create_shader(const char* filename, GLenum type)
{
  const GLchar* source = file_read(filename);
  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename); perror("");
    return 0;
  }
  GLuint res = glCreateShader(type);
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
*/
char * malloc_string(char * string1)
{
	char * new_string = (char *) malloc(strlen(string1) +  1);
	strncpy(new_string, string1, strlen(string1));
	new_string[strlen(string1)] = '\0'; // create new string and null terminate it
	return new_string;
}