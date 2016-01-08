#include "stdafx.h"
#include "tools.h"

/*
struct d3_points {
	char * mwaveformname;
	GLuint vbo_vertices;
	GLuint vbo_normals;
	GLuint vbo_texcoord;
	GLuint vbo_tangents;
	Gluint vbo_bitangents;
	unsigned int size;
};*/
std::vector<d3_points*> points;

std::vector<glm::vec4> suzanne_vertices;
std::vector<glm::vec3> suzanne_normals;
std::vector<GLushort> suzanne_elements;
std::vector<GLushort> suzanne_texture_elements;
std::vector<glm::vec2> suzanne_texture_points;
std::vector<glm::vec3> suzanne_tangents;
std::vector<glm::vec3> suzanne_bitangents;

void offset_mesh(std::vector<glm::vec4> * list_of_points, glm::vec3 offset)
{
	unsigned int i;
	for (i = 1; i < list_of_points->size(); i++)
	{
		(*list_of_points)[i].x += offset.x;
		(*list_of_points)[i].y += offset.y;
		(*list_of_points)[i].z += offset.z;
	}
}

struct d3_points * get_points(char * filename, char * resource)
{
	unsigned int i;
	for (i = 0; i < points.size(); i++) {
		if (strncmp(filename, points[i]->mwaveformname, strlen(filename)) == 0)
			return points[i];
	}
	suzanne_vertices.clear();
	suzanne_normals.clear();
	suzanne_elements.clear();
	suzanne_texture_elements.clear();
	suzanne_texture_points.clear();
	suzanne_tangents.clear();
	suzanne_bitangents.clear();
	//points not found in database so add it
	struct d3_points * new_points = (struct d3_points*)malloc(sizeof(struct d3_points));
	load_obj(resource, filename, &suzanne_vertices, &suzanne_normals, &suzanne_elements, &suzanne_texture_points, &suzanne_texture_elements, &suzanne_tangents, &suzanne_bitangents);
	new_points->dimensions = get_box_dimensions(&suzanne_vertices, true);
	new_points->vbo_vertices = gen_buffers(sizeof suzanne_vertices[0] * suzanne_vertices.size(), suzanne_vertices.data());
	new_points->vbo_normals = gen_buffers(sizeof suzanne_normals[0] * suzanne_normals.size(), suzanne_normals.data());
	new_points->vbo_tangents = gen_buffers(sizeof suzanne_tangents[0] * suzanne_tangents.size(), suzanne_tangents.data());
	new_points->vbo_bitangents = gen_buffers(sizeof suzanne_bitangents[0] * suzanne_bitangents.size(), suzanne_bitangents.data());
	new_points->vbo_texcoord = gen_buffers(sizeof suzanne_texture_points[0] * suzanne_texture_points.size(), suzanne_texture_points.data());
	new_points->size = suzanne_vertices.size();
	new_points->mwaveformname = malloc_string(filename);
	//new_points->mesh_points = std::vector<glm::vec4>();
	//new_points->mesh_points.reserve(suzanne_vertices.size());
	//new_points->mesh_points = suzanne_vertices;
	//opy_vectors(new_points->mesh_points, suzanne_vertices);
	points.push_back(new_points);
	return new_points;
}

void load_obj(char * resource_filename, char* filename, std::vector<glm::vec4> * vertices, std::vector<glm::vec3> * normals, std::vector<GLushort> * elements, std::vector<glm::vec2> * texture_points, std::vector<GLushort>  * texture_elements, std::vector<glm::vec3> * tangents, std::vector<glm::vec3> * bitangents)
{
    /*std::ifstream in(filename, std::ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << std::endl; exit(1);
    } */
	int filesize;
	char * in = get_char_from_file(resource_filename,filename, &filesize);
	if (filesize == 0) {
		printf(" couldn't load object %s from resource %s\n", filename, resource_filename);
	}

	in[filesize-1] = '\0';
	std::vector<glm::vec4> vertices_long;
	std::vector<glm::vec2> texture_points_long;
	std::vector<glm::vec4> vertices_test;
	std::vector<glm::vec2> texture_points_test;
    //std::string line;
	char * line = strtok(in, (const char *)"\n\r");
    while (line)//getline(in, line))
    {
		//printf("%s\n", line);
        if (strncmp(line,"v ",2) == 0)//line.substr(0,2) == "v ")
        {
            //std::istringstream s(line.substr(2));
			glm::vec4 v;
			sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            // s >> v.x; s >> v.y; s >> v.z; 
			v.w = 1.0f;
            vertices_test.push_back(v);
			//printf("vertices (%f %f %f)\n", v.x, v.y, v.z);
        }
        else if (strncmp(line,"f ",2) == 0)
        {
            unsigned int a,b,c;
			unsigned int fa,fb,fc;
			//sscanf(line.c_str(),"f %i/%i %i/%i %i/%i", &a,&fa, &b,&fb, &c,&fc);
			sscanf(line,"f %i/%i %i/%i %i/%i", &a,&fa, &b,&fb, &c,&fc);
			//printf("List of thigns %i/%i %i/%i %i/%i\n",a,fa,b,fb,c,fc);
            a--; b--; c--; fa--; fb--; fc--;
           //elements.push_back(a); elements.push_back(b); elements.push_back(c);
		   //texture_elements.push_back(fa); texture_elements.push_back(fb); texture_elements.push_back(fc);
		   texture_points->push_back(texture_points_test[fa]); texture_points->push_back(texture_points_test[fb]); texture_points->push_back(texture_points_test[fc]);
		   vertices->push_back(vertices_test[a]); vertices->push_back(vertices_test[b]); vertices->push_back(vertices_test[c]);
		   //vertices2[fa] = vertices[a]; vertices2[fb] = vertices[b]; vertices2[fc] = vertices[c]; // make the vertices index the same as the texture index

        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
		else if (strncmp(line,"vt",2) == 0) //texture coords
		{
			glm::vec2 fv;
			//sscanf(line.c_str(), "vt %f %f", &fv.x, &fv.y);
			sscanf(line, "vt %f %f", &fv.x, &fv.y);
			fv.y = 1 - fv.y;
			texture_points_test.push_back(fv);
			//vertices2.push_back(glm::vec4(0.0,0.0,0.0,0.0));
			//printf("texture coords (%f, %f)\n", fv.x, fv.y);
		}
        else
        {
            /* ignoring this line */
        }
		line = strtok(NULL, (const char *)"\n\r");
    }
	//vertices.resize(vertices2.size(), glm::vec4(0.0, 0.0, 0.0, 0.0) );
	//vertices= vertices2; // make these vertices adjusted to texture stuff
    //normals.resize(vertices_long.size(), glm::vec3(0.0, 0.0, 0.0));
    for (unsigned int i = 0; i < vertices->size(); i+=3)
    {
        GLushort ia = i;
        GLushort ib = i+1;
        GLushort ic = i+2;
		glm::vec3 vertex1 = glm::vec3((*vertices)[ia]);
		glm::vec3 vertex2 = glm::vec3((*vertices)[ib]);
		glm::vec3 vertex3 = glm::vec3((*vertices)[ic]);
        glm::vec3 normal = glm::normalize(glm::cross(
        vertex2 - vertex1,
        vertex3 - vertex1));
		normals->push_back(normal); normals->push_back(normal); normals->push_back(normal);


        //normals[ia] = normals[ib] = normals[ic] = normal;
		//normals[i] = normals[i+1] = normals[i+2] = normal;
		//printf("Normal = %i:%f, %i:%f, %i:%f\n", ia, normal.x, ib, normal.y, ic,  normal.z);
		glm::vec3 & v0 = vertex1;
		glm::vec3 & v1 = vertex2;
		glm::vec3 & v2 = vertex3;
		glm::vec2 & uv0 = glm::vec2((*texture_points)[ia]);
		glm::vec2 & uv1 = glm::vec2((*texture_points)[ib]);
		glm::vec2 & uv2 = glm::vec2((*texture_points)[ic]);

		glm::vec3 deltaPos1 = v1-v0;
		glm::vec3 deltaPos2 = v2-v0;
		glm::vec2 deltaUV1 = uv1-uv0;
		glm::vec2 deltaUV2 = uv2-uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r);
		glm::vec3 bitangent = glm::normalize((deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r);
		tangents->push_back(tangent); tangents->push_back(tangent); tangents->push_back(tangent);
		bitangents->push_back(bitangent); bitangents->push_back(bitangent); bitangents->push_back(bitangent);

    }
	// create tangents...
	// It's actually very complicated to make tangents and I don't know what this shit does
	//tangents.resize(vertices_long.size(), glm::vec3(1.0, 1.0, 1.0));
	//bitangents.resize(vertices_long.size(), glm::vec3(1.0, 1.0, 1.0));
  //  for (int i = 0; i < vertices->size(); i+=3)
  //  {
        //GLushort ia = i;
        //GLushort ib = i+1;
        //GLushort ic = i+2;
       // glm::vec3 tangent;
		
		//printf("tangent: (%f, %f, %f)\n", tangent.x, tangent.y, tangent.z);
 
    
		//glm::vec3 vecNormal = normals[ia];
		//tangent = glm::vec3((c3c1b * v2v1.x - c2c1b * v3v1.x), (c3c1b * v2v1.y - c2c1b * v3v1.y), (c3c1b * v2v1.z - c2c1b * v3v1.z));
		//tangent = glm::vec3((c3c1b * v2v1.y - c2c1b * v3v1.y),(c3c1b * v2v1.z - c2c1b * v3v1.z) , (c3c1b * v2v1.x - c2c1b * v3v1.x));
		/*tangent.x = glm::vec3(vertices[ib]).x - glm::vec3(vertices[ia]).x;
		tangent.y = glm::vec3(vertices[ib]).y - glm::vec3(vertices[ia]).y;
		tangent.z = glm::vec3(vertices[ib]).z - glm::vec3(vertices[ia]).z;
		*/
		//tangents[ia] = tangents[ib] = tangents[ic] = tangent;
		//printf("tangents = x:%f, y:%f, z:%f\n", tangent.x, tangent.y, tangent.z);
  //  }
	//vertices = vertices_long;
	//texture_points = texture_points_long;
	//in.close();
	delete in;
}

void delete_points(struct d3_points * points2delete)
{
	unsigned int i;
	for (i = 0; i < points.size(); i++) {
		if (strncmp(points2delete->mwaveformname, points[i]->mwaveformname, strlen(points2delete->mwaveformname)) == 0)
			points.erase(points.begin() + i);//return points[i];
	}
	free(points2delete->mwaveformname);
	glDeleteBuffers(1, &points2delete->vbo_vertices);
	glDeleteBuffers(1, &points2delete->vbo_normals);
	glDeleteBuffers(1, &points2delete->vbo_texcoord);
	glDeleteBuffers(1, &points2delete->vbo_tangents);
	glDeleteBuffers(1, &points2delete->vbo_bitangents);
	free(points2delete);
}