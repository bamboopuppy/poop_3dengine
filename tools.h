#include "stdafx.h"
#ifndef TOOLS_H
#define TOOLS_H
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <iterator>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <SOIL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h> 
/*physics */
#include "btBulletDynamicsCommon.h" 
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>


/* DEFINES */
#define MAX_LIGHTS 10
#define PI 3.141592653f
#define NEXT_CYCLE_US 1000000/60
#define LEETERROR 1337
#define MAX_JOINTS 10
#define NEXT_CYCLE 1000/60
/* d3_object FLAGS */
#define D3_OBJECT_STATIC 0x1
#define D3_OBJECT_NORENDER 0x2
#define D3_OBJECT_NOSHADOW 0x4
#define D3_OBJECT_NOLIGHT 0x8
#define D3_OBJECT_NOPHYSICS 0x10
#define D3_OBJECT_SETSTENCIL 0x20

/* collision shapes */
#define COLLISION_CUBE 0x1

int initilize_engine(int argc, char** argv);
glm::vec3 get_camerafront();
glm::vec3 get_camerapos();
/*tools */
char* file_read(const char* filename);
void print_log(GLuint object);

char * malloc_string(char * string1);
void follow_character(struct character_kinematic * charactor);

/*keyboard_input*/
void clear_keypress_callbacks();
void set_keypress_callback(unsigned char key_to_press, void (*function) (unsigned char key_pressed, bool keyup));
void keypress(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
bool get_noclip();
void set_leftright_updown( float new_leftright, float new_updown);
void set_noclip(bool new_noclip);
float get_leftright(bool noclip_reset);
float get_updown(bool noclip_reset);

/*mouse_input*/
void mouse_input(int button, int state, int x, int y);
void mouse_move(int x, int y);
float get_y_rotate();
float get_x_rotate();

/* textures */
struct d3_texture {
	char * mfilename;
	GLuint texture_id;
};
void gpu_texture(GLuint * texture_id, unsigned char * img, int img_width, int img_height);
struct d3_texture * get_texture(char * filename, char * resource);
GLuint create_texture_from_resource(char * resource_filename, char * filename);

/* physics_base */
#define FIXEDCONSTRAINT 0x01

struct physics_joints {
	void * joint[MAX_JOINTS];
	unsigned int type[MAX_JOINTS];
	unsigned int size;
	struct d3_object * connected[MAX_JOINTS];
};
struct t_collision_shape {
	btCollisionShape* shape;
	glm::vec3 dimensions;

};

struct physics_object {
	btRigidBody* fallRigidBody;
	btDefaultMotionState* fallMotionState;
	struct t_collision_shape * collision_shape;
	struct physics_joints joints;
};

struct character_kinematic {
	btKinematicCharacterController* m_character;
	btPairCachingGhostObject* m_ghostObject;
	glm::vec3 old_position;
};
struct character_kinematic * create_character(float width, float height, glm::vec3 location);
btDiscreteDynamicsWorld* get_physics_world();
glm::vec3 get_box_dimensions(std::vector<glm::vec4> * mesh_points, bool center);
struct physics_object * get_physics_object(struct d3_object * object);
void phx_step(float hertz, struct character_kinematic * charactor);
void add_fixed_constraint(btFixedConstraint * fixed, struct d3_object * object1, d3_object * object2);
void phx_init();
void delete_physics_object(struct physics_object * physics2delete);
glm::vec3 get_character_world_transform(struct character_kinematic * charactor);
void transport_character(struct character_kinematic * charactor, glm::vec3 position);
void weld_2_objects(struct d3_object * object1, struct d3_object * object2);
void add_rigid_body(struct physics_object * physics_object_2add);
void remove_rigid_body(struct physics_object * physics_object_2remove);

/* object_points */
struct d3_points {
	char * mwaveformname;
	GLuint vbo_vertices;
	GLuint vbo_normals;
	GLuint vbo_texcoord;
	GLuint vbo_tangents;
	GLuint vbo_bitangents;
	glm::vec3 dimensions;
	unsigned int size;
};
struct d3_points * get_points(char * filename, char * resource);
void load_obj(char * resource_filename, char* filename, std::vector<glm::vec4> * vertices, std::vector<glm::vec3> * normals, std::vector<GLushort> * elements, std::vector<glm::vec2> * texture_points, std::vector<GLushort>  * texture_elements, std::vector<glm::vec3> * tangents, std::vector<glm::vec3> * bitangents);
void delete_points(struct d3_points * points2delete);
void offset_mesh(std::vector<glm::vec4> * list_of_points, glm::vec3 offset);

/* 3dobjects */
struct d3_object {
	int id;
	char * name;
	struct d3_points * points;
	struct d3_texture * texture_id;
	struct d3_texture * texture_bump_id;
	//unsigned int size;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 model_matrix;
	glm::mat4 depthMVP;
	struct physics_object * physics_obj;
	unsigned int flags;
	std::vector<struct light_source*> * affected_lights;
};
struct d3_object * create_object(char * name, char * object_file, char * texture, char * bump_texture, char * resource_file, glm::vec3 position, unsigned int flags, glm::vec3 rotation);
void add_d3object(struct d3_object * created_object);
std::vector<d3_object*> get_d3objects();
struct d3_object * find_object_by_name(char * name);
struct d3_object * find_object_by_id(int id);
void delete_object(struct d3_object * object2delete, bool freemem);
void clear_object_list(bool freemem);

/*lights*/
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
	glm::vec3 spotDirection;
	char * lightid;
	struct d3_object * visual_light;
	bool cast_shadows;
};
struct light_array
{
	//struct light_source * lights[MAX_LIGHTS];
	glm::mat4 light_position[MAX_LIGHTS];
	glm::mat3 light_properties[MAX_LIGHTS];
	int num_lights;
};

struct light_source *add_light(glm::vec4 position, glm::vec4 diffuse, glm::vec4 specular, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, float spotCutoff, float spotExponent, glm::vec3 spotDirection, char * lightid);
struct light_array get_lights(struct d3_object * object);
struct light_source * get_light_obj(char * id);

/* resource_file_adder */
char * get_char_from_file(char * resource_filename, char * filename, int * filesize);

/* onDisplay */
void onDisplay();
int init_resources(void);
GLuint gen_buffers( int size, void * data);
void set_shadow_light(struct light_source *  lightitup);
struct light_source * get_shadow_light();
void set_shadow_intensity(float intensity);

/* 3d_spaceman_sim */
glm::mat4 get_g_view();
glm::mat4 get_g_projection();
glm::mat4 get_g_movement();
int get_screen_height();
int get_screen_width();

/* bullet_raycast */
void ray_test(glm::vec2 Mouse, glm::vec2 screen_size);


/* dynamic_textures */
SDL_Surface * render_text_to_surface(char * string, unsigned char color[3], glm::vec3 position);
void set_texture_to_object(SDL_Surface * sdl_texture, SDL_Surface * sdl_normal_texture, d3_object * d3object);
void add_surface_to_surface(SDL_Surface * sdl_texture_dest, SDL_Surface * sdl_texture_src, unsigned int position[2]);

/* XML parser */
void read_xml_doc(char * filename);
const char * get_xml_element_of_element(char * element, char * of_element);
const char * get_xml_element_of_element_of_element(char * element, char * of_element, char * root_element);
int get_xml_element_attribute(char * element, char * attribute);

/* character */
bool check_if_character_moved(struct character_kinematic * charactor);
void register_character_move_callback(void (*character_move_callback_fn) (glm::vec3 position));
void init_character();
void move_player_character(struct character_kinematic * charactor);

/* shader generator */
struct shader_struct {
	GLuint shader_program;
	int shadow_lights;
	int noshadow_lights;
	int nospecular_lights;
};
GLuint load_shaders(char * v, char * f, char * header);
GLuint create_shader(const char* filename, GLenum type, char * header);
GLuint get_shader(int shadow_point_lights, int noshadow_point_lights, int nospecular_point_lights);
GLuint load_shader_for_object(struct d3_object * object);

/* events */
struct t_event {
	void (*function) (struct t_event *);
	unsigned int time_to_run;
	void * data;
	int position;
	bool destroy;
};
void destroy_event(struct t_event * event2delete);
void check_events_due(unsigned int cur_time);
void add_event(void (*function) (struct t_event *), int time_to_run, void * data);
void clear_events();
#endif