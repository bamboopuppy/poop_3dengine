#include "stdafx.h"
#include "tools.h"
#include "EXT_tools.h"


void char_moveleft(unsigned char key, bool keyup)
{
	if (!keyup)
		set_leftright_updown( -0.1f, get_updown(false));
	else
		set_leftright_updown( 0.0f, get_updown(false));
}
void char_moveright(unsigned char key, bool keyup)
{
	if (!keyup)
		set_leftright_updown( 0.1f, get_updown(false));
	else
		set_leftright_updown( 0.0f, get_updown(false));
}
void char_moveup(unsigned char key, bool keyup)
{
	if (!keyup)
		set_leftright_updown(get_leftright(false), 0.1f);
	else
		set_leftright_updown(get_leftright(false), 0.0f);
}
void char_movedown(unsigned char key, bool keyup)
{
	if (!keyup)
		set_leftright_updown(get_leftright(false), -0.1f);
	else
		set_leftright_updown(get_leftright(false), 0.0f);
}
void toggle_noclip(unsigned char key, bool keyup)
{
	if(!keyup)
		set_noclip(!get_noclip());
}

void init_keyboard_keys()
{
	clear_keypress_callbacks();
	set_keypress_callback('a', char_moveleft);
	set_keypress_callback('d', char_moveright);
	set_keypress_callback('w', char_moveup);
	set_keypress_callback('s', char_movedown);
	set_keypress_callback('v', toggle_noclip);
}
/*
void keypress(unsigned char key, int x, int y)
{
	struct light_source *  lightitup;
	switch (key) {
	case 'a' : leftright = -1.0f;break;//change_leftright_updown(-0.1, 0.0); break;
	case 'd' : leftright = 1.0f;break;//change_leftright_updown(0.1, 0.0); break;
	case 'w' : updown = 1.0f;break;//change_leftright_updown(0.0, 0.1); break;
	case 's' : updown = -1.0f;break;//change_leftright_updown(0.0, -0.1); break;
	case 'v' : noclip = !noclip;break; //toggle noclip
	case 'k' : 
		lightitup = get_light_obj("test_light1");
		lightitup->position.x += 0.05f;
		lightitup->visual_light->position.x += 0.05f;
		lightitup->visual_light->model_matrix = glm::translate(glm::mat4(1.0), lightitup->visual_light->position);
		break;
	case 'l' :
		lightitup = get_light_obj("test_light1");
		lightitup->position.x -= 0.05f;
		lightitup->visual_light->position.x -= 0.05f;
		lightitup->visual_light->model_matrix = glm::translate(glm::mat4(1.0), lightitup->visual_light->position);
		break;
	case 'o' : 
		lightitup = get_light_obj("test_light1");
		lightitup->position.z += 0.05f;
		lightitup->visual_light->position.z += 0.05f;
		lightitup->visual_light->model_matrix = glm::translate(glm::mat4(1.0), lightitup->visual_light->position);
		break;
	case 'p' :
		lightitup = get_light_obj("test_light1");
		lightitup->position.z -= 0.05f;
		lightitup->visual_light->position.z -= 0.05f;
		lightitup->visual_light->model_matrix = glm::translate(glm::mat4(1.0), lightitup->visual_light->position);
		break;
	}
	//update_view();
}
void keyup(unsigned char key, int x, int y)
{
	switch (key) {
		case 'a' : leftright = 0.0f;break;//change_leftright_updown(-0.1, 0.0); break;
		case 'd' : leftright = 0.0f;break;//change_leftright_updown(0.1, 0.0); break;
		case 'w' : updown = 0.0f;break;//change_leftright_updown(0.0, 0.1); break;
		case 's' : updown =0.0f;break;//change_leftright_updown(0.0, -0.1); break;
	}
}
*/