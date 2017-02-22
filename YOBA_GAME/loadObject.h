#ifndef _LOADOBJECT_H_INC_
#define _LOADOBJECT_H_INC_

#include "disable_warnings.h"

#define LOAD_BIN 0

#define BIN_FORMAT_VER 1

enum LOAD_STATE {LS_OK, LS_FAIL};

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
	std::vector<GLfloat>  tangent;
	std::vector<GLfloat>  bitangent;

	std::vector<GLfloat> N_verts;
	std::vector<GLuint>  N_indicies;

	std::vector<GLfloat> P_verts;
	std::vector<GLuint>  P_indicies;

	std::string file;
};

extern int obj_count;
extern g_object objects[MAX_OBJECTS];

//void load_object(g_object *obj, char *file);
int reg_object(char *file);
g_object clone_obj(char *file);

#endif