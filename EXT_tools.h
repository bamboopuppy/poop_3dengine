#include "stdafx.h"
#define LIGHT_AFFECT_THRESHOLD 0.02

/* object_definitions */
struct object_definition {
	char * name;
	char * mesh;
	char * texture;
	char * normal_map;
	char * resource_file;
	unsigned int flags;
	glm::vec3 offset;
	glm::vec3 rotation;
};
struct object_definition * add_object_definition(char * name, char * mesh, char * texture, char * normal_map, char * resource_file, glm::vec3 offset, unsigned int flags, glm::vec3 rotation);
struct object_definition * get_definition_by_name(char * name);
/* objects */
struct world_object {
	struct object_definition * definition;
	struct d3_object * visual_object;
	int world_object_id;
	int grid_location[2];
};

/* lights */
struct light_object {
	struct light_source * display_light;
	int grid_location[2];
	struct world_object * visual_light;
};
bool check_if_light_change_required( int * new_grid);
void add_point_light(int grid[2], char * name);
float check_if_light_affects_object(struct world_object * object, struct light_object * light);
void change_shadow_light_to_brightest(int * new_grid);

/* grid */

struct grid_of_objects {
	int grid_id;
	int width;
	int height;
	/* need array of object info here */
	std::vector<world_object*> ** whole_grid;
	std::vector<light_object*> * lights;
	std::vector<light_source*> ** whole_grid_lights;
};
void disable_shadows_on_all_lights();
std::vector<light_source*> * get_lights_from_grid_spot(int * grid);
void rebuild_grid_lighting(int gridid);
void create_grid(int grid_id, int width, int height);
struct world_object * add_object_to_grid(struct object_definition * object2add, int position[2], glm::vec3 offset);
glm::vec3 get_world_position_from_grid(int grid[2]);
void clear_and_send_grid_to_engine(int grid_id, int position[2], int width, int height);
float get_grid_distance_two_objects(struct world_object* obj1, struct world_object*  obj2);
float get_grid_distance_two_points(int obj1_position[2], int obj2_position[2]);
bool add_light_object_to_grid(struct light_object * created_light_object);
void get_grid_position_from_world(int * grid, glm::vec3 position);

/* obj_file_reader */
void read_map(char * resource_filename, char * filename);

/* computer_screen */
void add_computer_screen(int grid[2], char * name, char * screen_name, glm::vec3 screen_offset, char * xml_info);

/* keyboard_input */
void char_moveleft(unsigned char key, bool keyup);
void char_moveright(unsigned char key, bool keyup);
void char_moveup(unsigned char key, bool keyup);
void char_movedown(unsigned char key, bool keyup);
void toggle_noclip(unsigned char key, bool keyup);
void init_keyboard_keys();

/* character */
void ext_init_character();
void character_moved_callback(glm::vec3 position);

/* event functions */
void fade_out_shadows(struct t_event *eventer);
void fade_in_shadows(struct t_event *eventer);
void change_shadow_to_new_grid(struct t_event *eventer);