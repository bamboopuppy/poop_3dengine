#include "stdafx.h"
#include "tools.h"

GLuint program;
/* shadow program */
GLuint shadow_program;
GLuint depthMatrixID;
GLuint depth_vertexPosition_modelspaceID;
GLuint FramebufferName;
GLuint depthTexture;
GLuint DepthBiasID;
GLuint ShadowMapID;
GLuint depth_rb;
GLuint m_shadowmap;
unsigned int shadow_width_height = 512;
/*******************/
GLint attribute_coord4d, attribute_v_color,uniform_fade, uniform_mvp, uniform_mytexture, attribute_texcoord, uniform_m, uniform_v, uniform_p;
GLint attribute_v_tangent, attribute_v_bitangent;
GLint uniform_m_3x3_inv_transp, uniform_v_inv;
GLint attribute_v_normal, uniform_normal_map;
GLint uniform_shadow_intensity;
GLint uniform_light_properties[MAX_LIGHTS];
GLint uniform_light_position[MAX_LIGHTS];
//GLint uniform_num_lights;

//monkey
GLuint vbo_monkey_vertices;
GLuint vbo_monkey_normals;
GLuint ibo_monkey_elements;
GLuint vbo_texcoord;
GLuint vbo_tangents;
GLuint vbo_bitangents;
GLuint texture_id; GLuint texture_bump_id;

GLuint gen_buffers( int size, void * data)
{
	GLuint newbuffer;
	glGenBuffers(1, &newbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, newbuffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return newbuffer;
}

GLint get_uniform_position(char * attribute_name, GLuint programz)
{
	GLint uniform;
  uniform = glGetUniformLocation(programz, attribute_name);
  if (uniform_mvp == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", attribute_name);
    return uniform;
  }
  return uniform;
}

int get_shader_locations()
{
	int i;
	for (i = 0; i < MAX_LIGHTS; i++)
  {
	  char uniform[64];
	  sprintf_s(uniform,64,"light_position[%i]", i);
	  uniform_light_position[i] = get_uniform_position(uniform, program);
	  sprintf_s(uniform,64,"light_properties[%i]", i);
	  uniform_light_properties[i] = get_uniform_position(uniform, program);
  }
	depthMatrixID = get_uniform_position("depthMVP", shadow_program);
	DepthBiasID = glGetUniformLocation(program, "DepthBiasMVP");
	ShadowMapID = glGetUniformLocation(program, "shadowMap");
	const char* attribute_name = "vertexPosition_modelspace";
	depth_vertexPosition_modelspaceID = glGetAttribLocation(shadow_program, attribute_name);
	attribute_name = "coord4d";
  attribute_coord4d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord4d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }
  attribute_name = "v_normal";
  attribute_v_normal = glGetAttribLocation(program, attribute_name);
  if (attribute_v_normal == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }

  attribute_name = "v_tangent";
  attribute_v_tangent = glGetAttribLocation(program, attribute_name);
  if (attribute_v_tangent == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }

  /*attribute_name = "v_bitangent";
  attribute_v_bitangent = glGetAttribLocation(program, attribute_name);
  if (attribute_v_bitangent == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }*/

  attribute_name = "texcoord";
  attribute_texcoord = glGetAttribLocation(program, attribute_name);
  if (attribute_texcoord == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }
  uniform_m = get_uniform_position("m", program);
  uniform_v = get_uniform_position("v", program);
  uniform_p = get_uniform_position("p", program);
  uniform_v_inv = get_uniform_position("v_inv", program);
  uniform_shadow_intensity = glGetUniformLocation(program, "shadow_intensity");
  uniform_m_3x3_inv_transp = get_uniform_position("m_3x3_inv_transp", program);
}

int init_resources(void)
{
#ifdef GL_ES_VERSION_2_0
	printf("OPENGL 2.0 enabled \n");
#else
	printf("OPENGL 2.1 enabled \n");
#endif
	unsigned int i;
  program = get_shader(1, 0, 0);//load_shaders("C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\triangle.v.glsl", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\triangle.f2.glsl");//glCreateProgram();
  if (program == LEETERROR) {
		printf("couldn't load program\n");  return 0;
  }
  /* shadow program */
	shadow_program = load_shaders("C:\\bin\\january_2016_jam\\3d_spaceman_sim\\DepthRTT.vertexshader", "C:\\bin\\january_2016_jam\\3d_spaceman_sim\\DepthRTT.fragmentshader", NULL);
	if (shadow_program == LEETERROR) {
			printf("couldn't load program\n");  return 0;
	}
	depthMatrixID = get_uniform_position("depthMVP", shadow_program);
	DepthBiasID = glGetUniformLocation(program, "DepthBiasMVP");
	ShadowMapID = glGetUniformLocation(program, "shadowMap");
	const char* attribute_name = "vertexPosition_modelspace";
	depth_vertexPosition_modelspaceID = glGetAttribLocation(shadow_program, attribute_name);
	FramebufferName = 0;
	
	
	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 800, 600, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);//GL_FLOAT, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, 512, 512, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);//GL_FLOAT, 0);//GL_FLOAT, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, 512, 512, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);//GL_FLOAT, 0);//GL_FLOAT, NULL);

	//GLfloat v_bc[] = {1.0f,1.0f,1.0f,1.0f};
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, v_bc);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//_TO_BORDER);
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//does nothing//glGenerateMipmapEXT(GL_TEXTURE_2D);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 512, 512, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadow_width_height, shadow_width_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//GL_UNSIGNED_INT, NULL);


	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 512, 512, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);


	glBindTexture(GL_TEXTURE_2D, 0);
	glGenTextures(1, &m_shadowmap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (i = 0 ; i < 6 ; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, shadow_width_height, shadow_width_height, 0, GL_RED, GL_FLOAT, NULL);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, shadow_width_height, shadow_width_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glGenFramebuffersEXT(1, &FramebufferName);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FramebufferName);
	//DON'T USE//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, depthTexture, 0);//GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
	//don'tuse glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			printf("FBO is okay good\n");
			break;
		default:
			printf("FBO done fucked up\n");
			break;
	}
	//glGenRenderbuffersEXT(1, &depth_rb);
  // glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
   //glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 512, 512);
   //-------------------------
   //Attach depth buffer to FBO
   //glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
	

	// No color output in the bound framebuffer, only depth.
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	//int FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//  if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
	//	  printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	  // switch back to window-system-provided framebuffer
	  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	  glBindTexture(GL_TEXTURE_2D, 0);
  /*****************/
	attribute_name = "coord4d";
  attribute_coord4d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord4d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }
  attribute_name = "v_normal";
  attribute_v_normal = glGetAttribLocation(program, attribute_name);
  if (attribute_v_normal == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }

  attribute_name = "v_tangent";
  attribute_v_tangent = glGetAttribLocation(program, attribute_name);
  if (attribute_v_tangent == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }

  attribute_name = "v_bitangent";
  attribute_v_bitangent = glGetAttribLocation(program, attribute_name);
  if (attribute_v_bitangent == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }

  attribute_name = "texcoord";
  attribute_texcoord = glGetAttribLocation(program, attribute_name);
  if (attribute_texcoord == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    //return 0;
  }
  //int i;
  for (i = 0; i < MAX_LIGHTS; i++)
  {
	  char uniform[64];
	  sprintf_s(uniform,64,"light_position[%i]", i);
	  uniform_light_position[i] = get_uniform_position(uniform, program);
	  sprintf_s(uniform,64,"light_properties[%i]", i);
	  uniform_light_properties[i] = get_uniform_position(uniform, program);
  }
  uniform_m = get_uniform_position("m", program);
  uniform_v = get_uniform_position("v", program);
  uniform_p = get_uniform_position("p", program);
  uniform_v_inv = get_uniform_position("v_inv", program);
  
  
  uniform_m_3x3_inv_transp = get_uniform_position("m_3x3_inv_transp", program);
  
  //uniform_num_lights = get_uniform_position("num_lights", program);
  struct d3_object * created_object = NULL;
  /*front wall*/
  /*for (i = 0; i < 4; i++) {
	 created_object = create_object(malloc_string("some_box"), "boxtextured.obj", "box.png", "box_bump.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\test_objects2.poo", glm::vec3(i * 2.0,-1.1,0.1));
	 add_d3object(created_object);
  }
  for (i = 0; i < 4; i++) {
	created_object = create_object(malloc_string("some_box"), "boxtextured.obj", "box.png", "box_bump.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\test_objects2.poo", glm::vec3(i * 2.0,1.1,0.1));
	add_d3object(created_object);
  }
  for (i = 0; i < 4; i++) {
	created_object = create_object(malloc_string("some_box"), "boxtextured.obj", "box.png", "box_bump.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\test_objects2.poo", glm::vec3(i * 2.0,3.1,0.1));
	add_d3object(created_object);
  }
  */
  /*left wall*/

  /*
  int j;
  for (i = 0; i < 4; i++) {
	  for (j =0; j < 4; j++) {
		created_object = create_object("some_box", "wall.obj", "wall_texture_grey.png", "wall_texture_grey_normal_map.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\wall.poo", glm::vec3(0.1 +(i*2),-1.1,0.1 +(j *2)), D3_OBJECT_STATIC);//i * 2.0 + 2.1));
		add_d3object(created_object);
	  }
  }
  created_object = create_object("some_box", "wall.obj", "wall_texture_grey.png", "wall_texture_grey_normal_map.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\wall.poo", glm::vec3(4.1,35.1,4.1), 0);
  add_d3object(created_object);
  created_object = create_object("some_box", "halfcube.obj", "floor_texture.png", "floor_texture_normal_map.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\wall.poo", glm::vec3(1.1,30.1,4.1), 0);
  add_d3object(created_object);
  created_object = create_object("some_box", "wall.obj", "wall_texture_grey.png", "wall_texture_grey_normal_map.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\wall.poo", glm::vec3(4.1,20.1,1.1), 0);
  add_d3object(created_object);
  */

/*
  for (i = 0; i < 4; i++) {
	created_object = create_object(malloc_string("some_box"), "boxtextured.obj", "box.png", "box_bump.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\test_objects2.poo", glm::vec3(0.1,1.1,i * 2.0 + 2.1));
	add_d3object(created_object);
  }
  for (i = 0; i < 4; i++) {
	created_object = create_object(malloc_string("some_box"), "boxtextured.obj", "box.png", "box_bump.png", "C:\\bin\\3d_spaceman_sim\\vc++_proj\\3d_spaceman_sim\\test_objects2.poo", glm::vec3(0.1,3.1,i * 2.0 + 2.1));
	add_d3object(created_object);
  }
  */
  /* add light */   //0.0, 1.0, 4.0, 0.0
  //add_light(glm::vec4 position, glm::vec4 diffuse, glm::vec4 specular, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, float spotCutoff, float spotExponent, glm::vec3 spotDirection, char * lightid);
  
  //add_light(glm::vec4(6.0, 4.0,  -4.0, 1.0), glm::vec4(1.0,  1.0,  1.0, 1.0), glm::vec4(1.0,  1.0,  1.0, 1.0), 1.0, 0.0, 0.0, 180.0, 0.0, glm::vec3(0.0, -0.1, 1.0), "test_light2");
  return 1;
}

//glm::mat4 depthMVP;
glm::mat4 depthProjectionMatrix;
glm::mat4 depthViewMatrix;
bool skip = false;

#define NUM_OF_LAYERS 6
struct CameraDirection
{
    GLenum cubemapface;
    glm::vec3 target;
    glm::vec3 up;
};

struct CameraDirection gCameraDirections[NUM_OF_LAYERS] =
{
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    //{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
    //{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
};

void cubemap_setup(struct CameraDirection selection)
{
	
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, selection.cubemapface, m_shadowmap, 0);
	//glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, selection.cubemapface, m_shadowmap, 0);
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            printf("shadow fbo is fucked in the butt\n");
        }
	//glDrawBuffer(GL_DEPTH_ATTACHMENT_EXT);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

}
struct light_source * shadow_light;
void set_shadow_light(struct light_source *  lightitup)
{
	shadow_light = lightitup;
	shadow_light->cast_shadows = true;
	//shadow_light->cast_shadows = true;
}
struct light_source * get_shadow_light()
{
	return shadow_light;
}
float shadow_intensity = 1.0;
void set_shadow_intensity(float intensity)
{
	shadow_intensity = intensity;
}
void shadow_spot_map()
{
	if (skip){ // do shadows every other frame
		skip = false;
		//return;
	}
	skip = true;
	unsigned int j, i;
	i = 0;
	// Render to our framebuffer
	glUseProgram(shadow_program);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FramebufferName);
	//cubemap_setup(gCameraDirections[0]);
	
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FramebufferName);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	
	glViewport(0,0,shadow_width_height,shadow_width_height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	// Use our shader
	
	struct light_source *  lightitup = shadow_light;//get_light_obj("test_light1");
	
	glm::vec3 lightInvDir = glm::vec3(-lightitup->spotDirection.x,-lightitup->spotDirection.y, -lightitup->spotDirection.z);
	glm::vec3 lightPos(lightitup->position.x, lightitup->position.y, lightitup->position.z);
	depthProjectionMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 50.0f);//*get_screen_width()/get_screen_height(), 0.1f, 50.0f);//glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
	//depthProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 50.0f);//*get_screen_width()/get_screen_height(), 0.1f, 50.0f);//glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
	//glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
	//for (i=0; i < NUM_OF_LAYERS; i++) {
		//cubemap_setup(gCameraDirections[i]);
		//glClearColor(1.0, 1.0, 1.0, 1.0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT);
		
		depthViewMatrix = glm::lookAt(lightPos, lightPos+gCameraDirections[i].target, gCameraDirections[i].up);
		//calcViewMatrix( ??? instead
		std::vector<d3_object*> display_objects = get_d3objects();
		glEnableVertexAttribArray(depth_vertexPosition_modelspaceID);
		for (j = 0; j < display_objects.size(); j++)
		{
			d3_object * display_object = display_objects[j];
			if ((display_object->flags & D3_OBJECT_NORENDER) || (display_object->flags & D3_OBJECT_NOSHADOW))
				continue;
			glm::mat4 depthModelMatrix;
			depthModelMatrix = display_object->model_matrix;
			//display_object->depthMVP = biasMatrix * depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			display_object->depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &display_object->depthMVP[0][0]);
			glUniformMatrix4fv(get_uniform_position("model", shadow_program), 1, GL_FALSE, &depthModelMatrix[0][0]);
			glUniformMatrix4fv(get_uniform_position("view", shadow_program), 1, GL_FALSE, &depthViewMatrix[0][0]);
			glUniformMatrix4fv(get_uniform_position("projection", shadow_program), 1, GL_FALSE, &depthProjectionMatrix[0][0]);
			glUniform3fv(get_uniform_position("lightposition", shadow_program), 1, &lightPos[0]);// GL_FALSE, &lightPos[0]);
	
			// 1rst attribute buffer : vertices
			
			glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_vertices);
			//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				depth_vertexPosition_modelspaceID,  // The attribute we want to configure
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			glDrawArrays(GL_TRIANGLES, 0, display_object->points->size);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	
//	}
	//glutSwapBuffers();
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 512, 512);
	//glFlush();
	glDisableVertexAttribArray(depth_vertexPosition_modelspaceID);

	/*GLfloat *pixels = new GLfloat[512 * 512];
	glReadPixels(0, 0, 512, 512,GL_DEPTH_COMPONENT,GL_FLOAT, pixels);
	for (i = 0; i < 512*512; i++)
	{
		printf("%0.2f ", pixels[i]);
	}
	*/
	glBindTexture(GL_TEXTURE_2D, 0);


	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//unsigned int baderror = glGetError();
	//printf("glGetError: %u\n", baderror);
}

void shadow_cube_map()
{
	if (skip){ // do shadows every other frame
		skip = false;
		//return;
	}
	skip = true;
	unsigned int j, i;
	// Render to our framebuffer
	glUseProgram(shadow_program);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FramebufferName);
	cubemap_setup(gCameraDirections[0]);
	
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FramebufferName);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	
	
	glViewport(0,0,shadow_width_height,shadow_width_height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	// Use our shader
	
	struct light_source *  lightitup = shadow_light;//get_light_obj("test_light1");
	
	glm::vec3 lightInvDir = glm::vec3(-lightitup->spotDirection.x,-lightitup->spotDirection.y, -lightitup->spotDirection.z);
	glm::vec3 lightPos(lightitup->position.x, lightitup->position.y, lightitup->position.z);
	//depthProjectionMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 50.0f);//*get_screen_width()/get_screen_height(), 0.1f, 50.0f);//glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
	depthProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 50.0f);//*get_screen_width()/get_screen_height(), 0.1f, 50.0f);//glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
	glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	//depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
	for (i=0; i < NUM_OF_LAYERS; i++) {
		cubemap_setup(gCameraDirections[i]);
		//glClearColor(1.0, 1.0, 1.0, 1.0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT);
		
		depthViewMatrix = glm::lookAt(lightPos, lightPos+gCameraDirections[i].target, gCameraDirections[i].up);
		//calcViewMatrix( ??? instead
		std::vector<d3_object*> display_objects = get_d3objects();
		glEnableVertexAttribArray(depth_vertexPosition_modelspaceID);
		for (j = 0; j < display_objects.size(); j++)
		{
			d3_object * display_object = display_objects[j];
			if ((display_object->flags & D3_OBJECT_NORENDER) || (display_object->flags & D3_OBJECT_NOSHADOW))
				continue;
			glm::mat4 depthModelMatrix;
			depthModelMatrix = display_object->model_matrix;
			//display_object->depthMVP = biasMatrix * depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			display_object->depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &display_object->depthMVP[0][0]);
			glUniformMatrix4fv(get_uniform_position("model", shadow_program), 1, GL_FALSE, &depthModelMatrix[0][0]);
			glUniformMatrix4fv(get_uniform_position("view", shadow_program), 1, GL_FALSE, &depthViewMatrix[0][0]);
			glUniformMatrix4fv(get_uniform_position("projection", shadow_program), 1, GL_FALSE, &depthProjectionMatrix[0][0]);
			glUniform3fv(get_uniform_position("lightposition", shadow_program), 1, &lightPos[0]);// GL_FALSE, &lightPos[0]);
	
			// 1rst attribute buffer : vertices
			
			glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_vertices);
			//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				depth_vertexPosition_modelspaceID,  // The attribute we want to configure
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			glDrawArrays(GL_TRIANGLES, 0, display_object->points->size);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	
	}
	//glutSwapBuffers();
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 512, 512);
	//glFlush();
	glDisableVertexAttribArray(depth_vertexPosition_modelspaceID);

	/*GLfloat *pixels = new GLfloat[512 * 512];
	glReadPixels(0, 0, 512, 512,GL_DEPTH_COMPONENT,GL_FLOAT, pixels);
	for (i = 0; i < 512*512; i++)
	{
		printf("%0.2f ", pixels[i]);
	}
	*/
	glBindTexture(GL_TEXTURE_2D, 0);


	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//unsigned int baderror = glGetError();
	//printf("glGetError: %u\n", baderror);
}

//float color_change = 0.0f;
void onDisplay()
{
	int error = 0;
	unsigned int i = 0;
	unsigned int j = 0;
//	printf("displaying\n");
	shadow_cube_map();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glUseProgram(program);
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glGenerateMipmapEXT(GL_TEXTURE_2D);
	glViewport(0,0,get_screen_width(),get_screen_height()); 
	
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	//glEnable(GL_STENCIL_TEST);
//	glEnable(GL_PROGRAMMABLE_STENCIL_WRITE);
	glClearDepth(1.0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
	glCullFace(GL_BACK);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	//glClear( GL_STENCIL_BUFFER_BIT) ;
	
	//color_change += 0.001f;
	//if (color_change >= 1.0)
	//	color_change = 0.0;
	//glClearStencil(255);
	//printf("display objects size: %i\n", display_objects.size());

	glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

	
	//glEnableVertexAttribArray(attribute_v_bitangent);
	std::vector<d3_object*> display_objects = get_d3objects();
	for (j = 0; j < display_objects.size(); j++)
	{
		
		//printf("loop j%i\n", j);
		d3_object * display_object = display_objects[j];//j
		if (display_object->flags & D3_OBJECT_NORENDER)
			continue;
		GLuint new_program = load_shader_for_object(display_object);
		if (new_program != program) {
			program = new_program;
			get_shader_locations();
		}
			glEnableVertexAttribArray(attribute_coord4d);
			glEnableVertexAttribArray(attribute_texcoord);
			glEnableVertexAttribArray(attribute_v_normal);
			glEnableVertexAttribArray(attribute_v_tangent);
		
		//glEnableVertexAttribArray(attribute_coord4d);
		 //glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		 /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
		 glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_vertices);
		 glVertexAttribPointer(
		   attribute_coord4d, // attribute
		   4,                 // number of elements per vertex, here (x,y)
		   GL_FLOAT,          // the type of each element
		   GL_FALSE,          // take our values as-is
		   0,//sizeof(struct attributes),                 // no extra data between each position
		   0//triangle_vertices  // pointer to the C array
		 );

		//glEnableVertexAttribArray(attribute_v_normal);
		glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_normals);
		glVertexAttribPointer(
			attribute_v_normal, // attribute
			3,                  // number of elements per vertex, here (x,y,z)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		);

		//glEnableVertexAttribArray(attribute_v_tangent);
		glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_tangents);
		glVertexAttribPointer(
			attribute_v_tangent, // attribute
			3,                  // number of elements per vertex, here (x,y,z)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		);

		//glEnableVertexAttribArray(attribute_v_bitangent);
		/*
		glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_bitangents);
		glVertexAttribPointer(
			attribute_v_bitangent, // attribute
			3,                  // number of elements per vertex, here (x,y,z)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		);
		*/
		//glEnableVertexAttribArray(attribute_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, display_object->points->vbo_texcoord);
		glVertexAttribPointer(attribute_texcoord, // attribute
		  2,                 // number of elements per vertex, here (r,g,b)
		  GL_FLOAT,          // the type of each element
		  GL_FALSE,          // take our values as-is
		  0,//sizeof(struct attributes),                 // no extra data between each position
		  0//(GLvoid*) (3 * sizeof(GLfloat))                  // offset of first element
		);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, display_object->texture_id->texture_id);
		uniform_mytexture = glGetUniformLocation(program, "mytexture");
		glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, display_object->texture_bump_id->texture_id);
		uniform_normal_map = glGetUniformLocation(program, "normalmap");//"normalmap");
		glUniform1i(uniform_normal_map, /*GL_TEXTURE*/2);
		/*glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		ShadowMapID = glGetUniformLocation(program, "shadowMap");
		glUniform1i(ShadowMapID, 4); */
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
		ShadowMapID = glGetUniformLocation(program, "gShadowMap");
		glUniform1i(ShadowMapID, 4);

		if (display_object->flags & D3_OBJECT_NOLIGHT){
			glUniform1i(glGetUniformLocation(program, "nolight"), 1);
			//printf("no light activated\n");
		}
		else
			glUniform1i(glGetUniformLocation(program, "nolight"), 0);

		if (display_object->flags & D3_OBJECT_SETSTENCIL){
			glUniform1i(glGetUniformLocation(program, "setstencil"), 1);
		}
		else
			glUniform1i(glGetUniformLocation(program, "setstencil"), 0);
		glm::mat4 g_m = display_object->model_matrix;//glm::translate(glm::mat4(1.0), display_object->position);
		//printf("g_m position i: %i (%f, %f, %f)\n", j, display_object->position.x, display_object->position.y, display_object->position.z);
		glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(g_m));
		glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(get_g_view()));
		glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(get_g_projection()));

		glm::mat4 depthBiasMVP = biasMatrix*display_object->depthMVP; //shadows
		//glm::mat4 depthBiasMVP = display_object->depthMVP; //shadows
		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]); //shadows

		glUniform1f(uniform_shadow_intensity,shadow_intensity);
		glm::mat3 pre_g_m = glm::mat3(g_m);
		glm::mat3 pre_g_m2 = glm::inverse(pre_g_m);
		glm::mat3 m_3x3_inv_transp = glm::transpose(pre_g_m2);
		//glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(g_m)));//g_view*g_m)));//pretranslate));
		//glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(g_view*g_m)));//g_view*g_m)));//pretranslate));
		glm::mat4 v_inv = glm::inverse(get_g_view());
		glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));
		glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));//glm::mat4(1)));//m_3x3_inv_transp));
		struct light_array new_light_array= get_lights(display_object);
		//printf("lights number: %i\n", new_light_array.num_lights);
		for (i = 0; i < (unsigned int)new_light_array.num_lights; i++) {
			glUniformMatrix4fv(uniform_light_position[i], 1, GL_FALSE, glm::value_ptr(new_light_array.light_position[i]));
			glUniformMatrix3fv(uniform_light_properties[i], 1, GL_FALSE, glm::value_ptr(new_light_array.light_properties[i]));
		}
		//glUniform1i(uniform_num_lights, new_light_array.num_lights);

		/*glm::mat4 ModelViewMatrix = g_view * g_m;
		glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
		glUniformMatrix3fv(glGetUniformLocation(program, "MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);//glm::value_ptr(localSurface2World0)); 
		glGetUniformLocation(program, "normalmap");*/
		
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, display_object->ibo_elements);
		//int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		//glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		glDrawArrays(GL_TRIANGLES, 0, display_object->points->size);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(attribute_coord4d);
		glDisableVertexAttribArray(attribute_texcoord);
		glDisableVertexAttribArray(attribute_v_normal);
		glDisableVertexAttribArray(attribute_v_tangent);
	}
	
	//glDisableVertexAttribArray(attribute_v_bitangent);
	glutSwapBuffers();

//printf("displaying done\n");
}