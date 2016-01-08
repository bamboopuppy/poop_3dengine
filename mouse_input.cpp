#include "stdafx.h"
#include "tools.h"

#define MOUSE_DOWN 0
#define MOUSE_UP 1
#define LEFT_MOUSE 0
#define RIGHT_MOUSE 2
#define MIDDLE_MOUSE 1

struct mouse_position {
	int x;
	int y;
	int button;
	int state;
};
struct mouse_position last_position;

float x_rotate = (-PI/2);
float y_rotate = 0.0;
void mouse_input(int button, int state, int x, int y)
{
	unsigned char pixelrgb[4];
	unsigned int distance;
	//printf("mouse button: %i state: %i position (%i,%i)\n", button, state, x, y);
	last_position.button = button;
	last_position.state = state;
	last_position.x = x;
	last_position.y = y;
	if (button == LEFT_MOUSE && state == MOUSE_DOWN)
		ray_test(glm::vec2(x,y), glm::vec2(get_screen_width(), get_screen_height()));
	else if( button == MIDDLE_MOUSE && state == MOUSE_DOWN) {
		glReadPixels(x, get_screen_height() - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)pixelrgb);
		glReadPixels( x, get_screen_height() - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_INT, &distance );
		printf("(%u, %u, %u, %u) Distance: %i\n", pixelrgb[0], pixelrgb[1], pixelrgb[2], pixelrgb[3], distance);
	}
}

void mouse_move(int x, int y)
{
	if (last_position.button != RIGHT_MOUSE &&  last_position.state != MOUSE_DOWN)
		return;
	int deltax = x - last_position.x;
	int deltay = last_position.y - y;
	x_rotate = x_rotate + (deltax * 0.005f);
	y_rotate = y_rotate + (deltay * 0.005f);
	last_position.x = x;
	last_position.y = y;
	if (x_rotate > (2*PI))
		x_rotate = x_rotate - (2 *PI);
	if (y_rotate > (2*PI))
		y_rotate = y_rotate - (2 *PI);
	if (x_rotate < (2*PI))
		x_rotate = x_rotate + (2 *PI);
	if (y_rotate < (2*PI))
		y_rotate = y_rotate + (2 *PI);
	//printf("mouse move: (%i,%i)\n", x, y);
}

float get_x_rotate()
{
	return x_rotate;
}
float get_y_rotate()
{
	return y_rotate;
}