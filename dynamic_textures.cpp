#include "stdafx.h"
#include "tools.h"


//#include <stdio.h> 
//#include <string> 
//#include <cmath>
TTF_Font *gFont = NULL;
int text_id;
static int initz;
bool load_font()
{
	bool success = true;
	//gFont = TTF_OpenFont( "resources\\SeasideResortNF\\SEASRN.ttf", 20 );
	gFont = TTF_OpenFont( "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\UbuntuMono-B.ttf", 20);
	if( gFont == NULL ) {
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() ); success = false;
	}
	return success;
}
int get_text_id()
{
	return text_id;
}
void set_text_id(int id)
{
	text_id = id;
}
bool init_sdl_crap()
{
	bool success = true;
	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG; 
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
	{ printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() ); success = false;
	} 
	//Initialize SDL_ttf
	if( TTF_Init() == -1 ) { 
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() ); success = false;
	}
	if (!load_font())
		success = false;
	if (success)
		initz = true;
	return success;
}
/*unsigned int power_two_floor(unsigned int val) {
  unsigned int power = 2, nextVal = power*2;
  while((nextVal *= 2) <= val)
    power*=2;
  return power*2;
}*/

SDL_Surface * render_text_to_surface(char * string, unsigned char color[3], glm::vec3 position)
{
	//struct t_square * text_object;
//	int Mode = GL_RGB;
//	int tid;
//	GLfloat width;
//	GLfloat height;
	while (!initz)
		init_sdl_crap();
	SDL_Color textColor = {color[0], color[1], color[2] };
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, string, textColor);//textureText.c_str(), textColor );
	return textSurface;
	//if (textSurface == NULL)
	//	return -1;
	//tid = add_cube_wrapper("resources\\ball.tga", position.x,position.y,position.z, 0.0, 0.0, SQUARE, 1.0,BACKGROUND | UI | TEXT_OBJ);
	//text_object = get_squares(tid);
	//if (text_object == NULL)
	//	return -1;
	//glDeleteBuffers(1, &(text_object->vbo_square_vertices));
	//glDeleteTextures(1, &(text_object->texture_id));
	//printf("loading texture, w: %f h: %f\n", textSurface->w/PIXELRATIO, textSurface->h/PIXELRATIO);
	//width = textSurface->w/PIXELRATIO; height = textSurface->h/PIXELRATIO;
	/*GLfloat coord3d[6][3] = {
	  { -width, -height,  0.0}, //bottom left
	  {width, -height,  0.0}, //bottom right
	  {width,  height,  0.0}, //top right
	  {width,  height,  0.0}, //top right
	  {-width,  height,  0.0}, //top left
	  { -width, -height,  0.0} //bottom left
  };*/
 // glGenBuffers(1, &text_object->vbo_square_vertices);
  //glBindBuffer(GL_ARRAY_BUFFER, text_object->vbo_square_vertices);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(coord3d), coord3d, GL_STATIC_DRAW);
 // glBindBuffer(GL_ARRAY_BUFFER, 0);
 // glGenTextures(1, &text_object->texture_id);
 // glBindTexture(GL_TEXTURE_2D, text_object->texture_id);
  //int w = textSurface->w * 2;//power_two_floor(textSurface->w) * 2;
 // int h = textSurface->h * 2;//power_two_floor(textSurface->h) * 2;
  //SDL_SetAlpha(textSurface, 0, 128);
//SDL_Surface * s = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
//SDL_BlitSurface(textSurface, NULL, s, NULL);
//	if(s->format->BytesPerPixel == 4) {
	//	printf("RGBA FORMAT\n");
//	   Mode = GL_RGBA;
//	}
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, Mode, w, h, 0, Mode, GL_UNSIGNED_BYTE, s->pixels);
	
	//if(text_object->texture_id == 0) {
	 //printf("couldn't load this texture '%s'\n", string);
	//}
	//glBindTexture(GL_TEXTURE_2D, 0);
	//SDL_FreeSurface(s);
	//SDL_FreeSurface(textSurface);
	//return tid;
}

void free_SDL()
{
	if (initz) {
		TTF_CloseFont( gFont );
		gFont = NULL;
		TTF_Quit(); IMG_Quit(); SDL_Quit();
	}
}

void set_texture_to_object(SDL_Surface * sdl_texture, SDL_Surface * sdl_normal_texture, d3_object * d3object)
{
	if (d3object == NULL) {
		printf("ERROR: set_texture_to_object object is null\n");
		return;
	}
	if (d3object->texture_id == NULL){
		printf("ERROR: set_texture_to_object texture_id is null\n");
		return;
	}
	if (strncmp(d3object->texture_id->mfilename, "dynamic", strlen("dynamic")) != 0) {
		d3object->texture_id->mfilename = malloc_string("dynamic");
	}
	printf("deleting texture %u\n", d3object->texture_id->texture_id);
	if (d3object->texture_id->texture_id != 0)
		glDeleteTextures(1, &(d3object->texture_id->texture_id));

	if (sdl_normal_texture != NULL && d3object->texture_bump_id == NULL) {
		printf("ERROR: set_texture_to_object texture_bump_id is null\n");
		return;
	}
	SDL_Surface * s = SDL_CreateRGBSurface(0, sdl_texture->w, sdl_texture->h, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	SDL_BlitSurface(sdl_texture, NULL, s, NULL);
	gpu_texture(&(d3object->texture_id->texture_id), (unsigned char * )s->pixels, s->w, s->h);
	printf("created texture %u\n", d3object->texture_id->texture_id);
	SDL_FreeSurface(s);
	if (sdl_normal_texture != NULL)
	{
		printf("deleting normal texture %u\n", d3object->texture_bump_id->texture_id);
		if (d3object->texture_bump_id->texture_id != 0)
			glDeleteTextures(1, &(d3object->texture_bump_id->texture_id));
		s = SDL_CreateRGBSurface(0, sdl_normal_texture->w, sdl_normal_texture->h, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
		SDL_BlitSurface(sdl_normal_texture, NULL, s, NULL);
		gpu_texture(&(d3object->texture_bump_id->texture_id), (unsigned char * )s->pixels, s->w, s->h);
		printf("creating normal texture %u\n", d3object->texture_bump_id->texture_id);
		SDL_FreeSurface(s);
	}
}

void add_surface_to_surface(SDL_Surface * sdl_texture_dest, SDL_Surface * sdl_texture_src, unsigned int position[2])
{
	SDL_Rect destrect;
	destrect.x = position[0]; destrect.y = position[1];
	SDL_BlitSurface(sdl_texture_src, NULL, sdl_texture_dest, &destrect);
}