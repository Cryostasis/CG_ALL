#ifndef _LOADOBJECT_H_INC_
#define _LOADOBJECT_H_INC_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <string>

#define MAX_OBJECTS 15

struct g_object
{
	int vert_num;
	int ind_num;
	std::vector<GLfloat> verts;
	std::vector<GLfloat> texcoords;
	std::vector<GLfloat> normals;
	std::vector<GLuint>  indicies;

	std::vector<GLfloat> N_verts;
	std::vector<GLuint>  N_indicies;

	std::string file;
};

extern int obj_count;
extern g_object objects[MAX_OBJECTS];

//void load_object(g_object *obj, char *file);
int reg_object(char *file);

#endif