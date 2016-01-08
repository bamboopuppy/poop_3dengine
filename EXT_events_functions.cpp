#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"
void fade_out_shadows(struct t_event *eventer)
{
	if (eventer->destroy)
	{
		if (eventer->data != NULL)
			free(eventer->data);
		return;
	}
	float * shadow_intensity = (float*)(eventer->data);
	(*shadow_intensity) -= 0.05f;
	if (*shadow_intensity < -1.0f){
		destroy_event(eventer);
		return;
	}
	set_shadow_intensity(abs(*shadow_intensity));
	eventer->time_to_run += NEXT_CYCLE * 1;
}
void fade_in_shadows(struct t_event *eventer)
{
	if (eventer->destroy)
	{
		if (eventer->data != NULL)
			free(eventer->data);
		return;
	}
	float * shadow_intensity = (float*)(eventer->data);
	(*shadow_intensity) += 0.05f;
	if (*shadow_intensity > 1.0f){
		destroy_event(eventer);
		return;
	}
	set_shadow_intensity(*shadow_intensity);
	eventer->time_to_run += NEXT_CYCLE * 1;
}
void change_shadow_to_new_grid(struct t_event *eventer)
{
	if (eventer->destroy)
	{
		if (eventer->data != NULL)
			free(eventer->data);
		return;
	}
	int * new_grid = (int*) eventer->data;
	change_shadow_light_to_brightest(new_grid);
	destroy_event(eventer);
}
/*
void flash_red(struct t_event * eventer)
{
	if (eventer->destroy) {
		return;
	}
	glm::vec3 new_color(1.0,0.0,0.0);
	set_color_adjust(new_color);
	add_event(remove_flash_red, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE *4), NULL);
	destroy_event(eventer->position);
}
void remove_flash_red(struct t_event * eventer)
{
	if (eventer->destroy) {
		glm::vec3 new_color(0.0,0.0,0.0);
		set_color_adjust(new_color);
		return;
	}
	glm::vec3 new_color(0.0,0.0,0.0);
	set_color_adjust(new_color);
	destroy_event(eventer->position);
}

void fade_out(struct t_event * eventer)
{
	if (eventer->destroy) {
		glm::vec3 new_color(1.0,1.0,1.0);
		set_color_adjust(new_color);
		return;
	}
	glm::vec3 * color = (glm::vec3 *) eventer->data;
	if (color->x >= 1.0) {
		free(eventer->data);
		destroy_event(eventer->position);
		return;
	}
	color->x += 0.01;
	color->y = color->x;
	color->z = color->x;
	set_color_adjust(*color);
	set_pause(true);
}
void fade_in(struct t_event * eventer)
{
	if (eventer->destroy) {
		glm::vec3 new_color(0.0,0.0,0.0);
		set_color_adjust(new_color);
		return;
	}
	glm::vec3 * color = (glm::vec3 *) eventer->data;
	if (color->x <= 0.0) {
		free(eventer->data);
		destroy_event(eventer->position);
		set_pause(false);
		return;
	}
	color->x -= 0.01;
	color->y = color->x;
	color->z = color->x;
	set_color_adjust(*color);
}
void set_pickupable(struct t_event *eventer)
{
	if (eventer->destroy) {
		return;
	}
	struct t_square * object = (struct t_square *) eventer->data;
	object->flags = object->flags & ~NOPICKUP;
	destroy_event(eventer->position);
}

void set_obj_charactor_col(struct t_event *eventer)
{
	if (eventer->destroy) {
		return;
	}
	b2Filter filter;
	struct t_square * object = (struct t_square *) eventer->data;
	for (b2ContactEdge* edge = object->body->GetContactList(); edge; edge = edge->next) {
	  if (edge->contact->IsTouching()){
		  eventer->time_to_run += NEXT_CYCLE;
		  return;
	  }
	}
	for ( b2Fixture* f = object->body->GetFixtureList(); f; f = f->GetNext() ) {
	   filter = f->GetFilterData();
	   filter.categoryBits = filter.categoryBits | COL_CHARACTER;
	   filter.maskBits = filter.maskBits | COL_CHARACTER;//COL_DEACTIVATED;
	   filter.groupIndex = filter.groupIndex | COL_CHARACTER;
	   f->SetFilterData( (const b2Filter)filter );
	}
	destroy_event(eventer->position);
}
void set_global_dead_2(struct t_event *eventer)
{
	if (eventer->destroy) {
		set_global_dead(2);
		return;
	}
	destroy_event(eventer->position);
}
void delete_square(struct t_event *eventer)
{
	int * square = (int *) eventer->data;
	if (*square == 0) {
		printf(" trying to delete square 0???\n");
	}
	if (eventer->destroy) {
		free_square(*square);
		*square = 0;
		return;
	}
	free_square(*square);
	//free(eventer->data);
	destroy_event(eventer->position);
}
int first_time = 0;
void move_elevator(struct t_event *eventer)
{
	if (eventer->destroy) {
		stop_train();
		first_time = 0;
		return;
	}
	if (first_time == 0){
		play_train();
		first_time = 1;
	}

	int * orig_time = (int *) eventer->data;
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int elevator_int = get_elevator_id();
	struct t_square * elevator_obj =  get_squares(elevator_int);
	elevator_obj->body->ApplyLinearImpulse( b2Vec2(0,-1.0), elevator_obj->body->GetWorldCenter(), true );
	//GLfloat angular = elevator_obj->object->body->GetAngularVelocity();
	b2Vec2 position1 = elevator_obj->body->GetPosition();
	position1.x = 0.0;
	elevator_obj->body->SetTransform(position1,0.0);
	//if (angular > 0.0 || angular < 0.0)
	//elevator_obj->y_pos -= 0.01 *((current_time - *orig_time)/1000);
	if (position1.y < -105.0) {
		printf("elevator hit bottom\n");
		b2Vec2 vel = elevator_obj->body->GetLinearVelocity();
		vel.x = 0.0; vel.y = -0.01;
		elevator_obj->body->SetLinearVelocity(vel);
		elevator_obj->body->SetAngularVelocity(0);
		free(eventer->data);
		stop_train();
		destroy_event(eventer->position);
		return;
	}
	struct t_square * character = get_character_obj();
	b2Vec2 position2 = character->body->GetPosition();
	float distance = Distance(position1.x, position1.y, position2.x, position2.y);
	int vol = 128 - (int) distance *3;
	int cur_vol = get_train_volume();
	if (vol != cur_vol)
		set_train_volume(vol);
	eventer->time_to_run += NEXT_CYCLE * 1;
}
static int asteroid_id;
static int crack_id;

void satelite_colision(struct t_event *eventer)
{
	if (eventer->destroy) {
		asteroid_id = 0;
		crack_id = 0;
		return;
	}
	if (asteroid_id == 0)
		asteroid_id = add_cube_wrapper("resources\\asteroid.tga", 5.0, 0.0,-9.9,-45.0, 0.0, SQUARE, 0.0, BACKGROUND|FIXED | NOCLIP);
	struct t_square * asteroid_obj =  get_squares(asteroid_id);
	//printf("asteroid z_pos1 : %f\n", asteroid_obj->z_pos);
	if (asteroid_obj == NULL) {
		destroy_event(eventer->position);
		return;
	}
	if (asteroid_obj->z_pos < -0.7 && asteroid_obj->z_pos > -0.9)
		play_glass_shatter();
	if (asteroid_obj->z_pos > -0.4){
		
		if (crack_id == 0){
			crack_id = add_cube_wrapper("resources\\glass_crack.tga", 5.0, 0.0,-0.2, 0.0, 0.0, SQUARE, 0.0, BACKGROUND|FIXED | NOCLIP);
			play_wind();
			add_event(pull_objects, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE), NULL);
		}
		asteroid_obj->y_pos += 0.01;
		asteroid_obj->x_pos += 0.01;
		eventer->time_to_run += NEXT_CYCLE * 2;
		if (asteroid_obj->y_pos > 12.0){
			free_square(asteroid_id);
			destroy_event(eventer->position);
			return;
		}
		return;
	}
	asteroid_obj->z_pos += 0.1;
	//printf("asteroid z_pos2 : %f\n", asteroid_obj->z_pos);
	eventer->time_to_run += NEXT_CYCLE * 2;
}
int text1_id = 0;
int text2_id = 0;
int the_end_id = 0;
int t_the_end_id = 0;
int t_text2_id = 0;
int t_text1_id = 0;
void winner(struct t_event *eventer)
{
	if (eventer->destroy) {
		text1_id = 0;
		text2_id = 0;
		the_end_id = 0;
		t_the_end_id = 0;
		t_text2_id = 0;
		t_text1_id = 0;
		return;
	}
	struct t_square * character = get_character_obj();
	b2Vec2 vel(3.0,0.0);
	b2Vec2 position = character->body->GetPosition();
	if (position.x > 60 && t_the_end_id ==0)
	{
		t_the_end_id = 1;
		set_pause_time(true);
		glm::vec3 colors(0.0,0.0,200.0);
		glm::vec3 positiontext;
		positiontext.x = 0.0;//position1.x;
		positiontext.y = 2.0;//position1.y;
		positiontext.z = 8.1;
		the_end_id = render_text_to_surface("THE END", colors, positiontext);
		//printf("the_end_id = %i\n", the_end_id);
		//int * id = (int *) malloc(sizeof(int));
		//*id = the_end_id;
		//printf("id: %i\n", *id);
		if (the_end_id > 0)
			add_event(delete_square, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE * 140), (void *)&the_end_id);
	}
	if (position.x > 70 && t_text1_id == 0)
	{
		t_text1_id = 1;
		//clear_inventory();
		text1_id = add_cube_wrapper("resources\\credits1.tga", 0.0,0.0,1.0,0.0, 0.0, SQUARE, 0.0, FIXED | NOCLIP| BACKGROUND | UI);
		//int * id = (int *) malloc(sizeof(int));
		//*id = text1_id;
		add_event(delete_square, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE * 280), (void *)&text1_id);
	}
	if (position.x > 90 && t_text2_id == 0)
	{
		t_text2_id = 1;
		text2_id = add_cube_wrapper("resources\\credits2.tga", 0.0,0.0,1.0,0.0, 0.0, SQUARE, 0.0, FIXED | NOCLIP| BACKGROUND | UI);
		//int * id = (int *) malloc(sizeof(int));
		//*id = text2_id;
		add_event(delete_square, glutGet(GLUT_ELAPSED_TIME) + (NEXT_CYCLE * 280), (void *)&text2_id);
	}
	if (position.x > 110)
	{
		destroy_event(eventer->position);
		set_level_number(get_level_number() +1);
		stop_music();
		character_die();
		return;
	}
		character->body->SetLinearVelocity(vel);
	//destroy_event(eventer->position);
	eventer->time_to_run += NEXT_CYCLE * 2;
}
int time_id = -1;
unsigned int time_ms = 0;
unsigned int last_time = 0;
char * convert_ms_to_string(unsigned int time)
{
	int milliseconds = time % 1000;
	int seconds = (time/1000) % 60;
	int minutes = (time/1000)/60;
	char * s_time = (char *) malloc(32); //make 32 character string
	sprintf(s_time,"%02i:%02i.%03i",minutes,seconds,milliseconds);
	return s_time;
}
unsigned int get_last_time()
{
	return last_time;
}
bool pause_time = false;
void set_pause_time(bool set_pause)
{
	pause_time = set_pause;
}
void update_timer(struct t_event *eventer)
{
		if (eventer->destroy) {
			free_square(time_id);
			//printf("setting last_time %u to time_ms %u\n", last_time, time_ms);
			last_time = time_ms;
			time_id = -1;
			time_ms = 0;
			return;
		}
		glm::vec3 colors(250.0,100.0,50.0);
		glm::vec3 positiontext;
		positiontext.x = 8.0;//position1.x;
		positiontext.y = 6.5;//position1.y;
		positiontext.z = 1.1;
		char * malloc_text = convert_ms_to_string(time_ms);
		free_square(time_id);
		time_id = render_text_to_surface(malloc_text, colors, positiontext);
		if (!get_pause() && !pause_time)
			time_ms += NEXT_CYCLE;
		eventer->time_to_run += NEXT_CYCLE;
		free(malloc_text);
		
}
	*/
