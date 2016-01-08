// 3d_spaceman_sim.cpp : Defines the entry point for the console application.
//
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif
#include "stdafx.h"
#include "tools.h"
//#include <stdio.h>
//#include <stdlib.h>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
//#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
//#include <GL/freeglut.h>

/* more maths stuff */
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
/*textures*/

/// This is a Hello World program for running a basic Bullet physics simulation

int screen_width = 800;
int get_screen_width()
{
	return screen_width;
}
int screen_height = 600;
int get_screen_height()
{
	return screen_height;
}

long int next_time;
glm::mat4 g_view;
glm::mat4 g_projection;
glm::mat4 g_movement;


/*monkey */

/*std::vector<glm::vec4> suzanne_vertices;
std::vector<glm::vec3> suzanne_normals;
std::vector<GLushort> suzanne_elements;
std::vector<GLushort> suzanne_texture_elements;
std::vector<glm::vec2> suzanne_texture_points;
std::vector<glm::vec3> suzanne_tangents;
std::vector<glm::vec3> suzanne_bitangents;
*/
glm::mat4 get_g_view()
{
	return g_view;
}
glm::mat4 get_g_projection()
{
	return g_projection;
}
glm::mat4 get_g_movement()
{
	return g_movement;
}
glm::vec3 camerapos =glm::vec3(0.0, 1.0 , 7.0);
glm::vec3 get_camerapos()
{
	return camerapos;
}
glm::vec3 camerafront;
glm::vec3 get_camerafront()
{
	return camerafront;
}
struct character_kinematic * following_character = NULL;
void follow_character(struct character_kinematic * charactor)
{
	following_character = charactor;
}

void update_view()
{
	//glm::vec3(get_leftright(), 1.0 , -get_updown() + 7.0);
	//glm::vec3 camerafront = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 front;
	front.x = cos((get_y_rotate())) * cos((get_x_rotate()));
	front.y = sin((get_y_rotate()));
	front.z = cos((get_y_rotate())) * sin((get_x_rotate()));
	camerafront = glm::normalize(front);
	glm::vec3 cameraup = glm::vec3(0.0, 1.0, 0.0);
	if (!get_noclip()) {
		camerapos = get_character_world_transform(following_character);
	} else {
		camerapos += (get_updown(false)) * camerafront;
		camerapos += glm::normalize(glm::cross(camerafront, cameraup)) * (get_leftright(false));
	}
	g_view = glm::lookAt(camerapos, camerapos + camerafront, cameraup); 
	//g_view = glm::lookAt(glm::vec3(get_leftright(), get_updown(), 7.0), glm::vec3(get_leftright(), get_updown(), -3.0), glm::vec3(0.0, 1.0, 0.0));
	//g_view = glm::lookAt(glm::vec3(0.0, 0.0, 7.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	//g_view = glm::rotate(glm::mat4(1.0f),get_x_rotate(), glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0f),get_y_rotate(), glm::vec3(1,0,0)) * g_view;
	g_projection = glm::perspective(glm::radians(45.0f), 1.0f*screen_width/screen_height, 0.1f, 50.0f);
	g_movement = glm::translate(glm::mat4(1.0), glm::vec3(0.0,0.0, 0.0));
}



void idle()
{
	int last_time = glutGet(GLUT_ELAPSED_TIME);
	if (last_time < next_time/1000) {
	int sleep = (next_time/1000 - last_time);
	if (sleep > 0 && sleep <= (NEXT_CYCLE_US/1000)) // for 60hz
			Sleep(sleep);
	}
	if (last_time > (next_time/1000) + 32) //we've been missing a few frames so reset the base time value
		next_time = glutGet(GLUT_ELAPSED_TIME)*1000;
	next_time = next_time + NEXT_CYCLE_US;
//	printf("idlinging\n");
	update_view();
	glutPostRedisplay();
	phx_step(60.0f, following_character);
	check_events_due(glutGet(GLUT_ELAPSED_TIME));
//	printf("idlinging done\n");
}
int initilize_engine(int argc, char** argv)
//int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitContextVersion(2,0);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_STENCIL);
  glutInitWindowSize(screen_width, screen_height);
  glutCreateWindow("3d spaceman simulator");
  /* Extension wrangler initialising */
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK)
  {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
  }
  if (!GLEW_VERSION_2_0) {
    fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
    return 1;
  }
  next_time = glutGet(GLUT_ELAPSED_TIME) * 1000 + NEXT_CYCLE_US;
  /* When all init functions run without errors,
  the program can initialise the resources */
  printf("done\nInitializing Window... ");
  phx_init();
  init_character();
  if (init_resources())
  {
    /* We can display it if everything goes OK */
    glutDisplayFunc(onDisplay);
	glutIdleFunc(idle);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyup);
	glutMouseFunc(mouse_input);
	glutMotionFunc(mouse_move);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	// Accept fragment if it closer to the camera than the former one
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glutKeyboardUpFunc(keyboard_up);
//	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	//glutKeyboardFunc(NULL);
	//glutMouseFunc(mousepress);
	//update_view();
	//glutReshapeFunc(onReshape);
	//printf("done\nRunning Game\n");
    //glutMainLoop();
	
  }
  //getchar();
  return 0;
}


