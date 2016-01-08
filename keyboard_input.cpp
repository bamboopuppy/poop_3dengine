#include "stdafx.h"
#include "tools.h"

void (*keypress_callbacks[256]) ( unsigned char key_pressed, bool keyup);

void clear_keypress_callbacks()
{
	int i;
	for (i=0; i < 256; i++)
		keypress_callbacks[i] = NULL;
}

void set_keypress_callback(unsigned char key_to_press, void (*function) (unsigned char key_pressed, bool keyup))
{
	keypress_callbacks[tolower((int) key_to_press)] = function;
}

void keypress(unsigned char key, int x, int y)
{
	int lowercase_key = tolower((int) key);
	if (keypress_callbacks[lowercase_key] == NULL)
		return;
	(*keypress_callbacks[lowercase_key])(key, false);
}

void keyup(unsigned char key, int x, int y)
{
	int lowercase_key = tolower((int) key);
	if (keypress_callbacks[lowercase_key] == NULL)
		return;
	(*keypress_callbacks[lowercase_key])(key, true);
}

float leftright = 0.0f;
float updown = 0.0f;
bool noclip = false;
void set_leftright_updown( float new_leftright, float new_updown)
{
	//if (new_leftright != 0.0f)
		leftright = new_leftright;
	//if (new_updown != 0.0f)
		updown = new_updown;
}

void set_noclip(bool new_noclip)
{
	noclip = new_noclip;
}
bool get_noclip()
{
	return noclip;
}
float get_leftright(bool noclip_reset)
{
	float ret = leftright;
	if (noclip && noclip_reset)
		leftright = 0.0f;
	return ret;
}
float get_updown(bool noclip_reset)
{
	float ret = updown;
	if (noclip && noclip_reset)
		updown = 0.0f;
	return ret;
}