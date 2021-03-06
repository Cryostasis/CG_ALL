#ifndef _GRAPHOBJ_H_INC_
#define _GRAPHOBJ_H_INC_

#include "disable_warnings.h"

#define VERT_POSITION	0
#define VERT_TEXCOORD	1
#define VERT_NORMAL		2
#define VERT_TANGENT	3
#define VERT_BITANGENT	4

#define FRAG_OUTPUT0	0

#define USE_TEX_LOC		5
#define USE_NM_LOC		6
#define USE_PM_LOC		7

#define LINE_COLOR_LOC  6
#define FONT_TEX		7

#define NO_TEXTURE		-1

#include "light.h"
#include "camera.h"
#include "loadObject.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "math/math3d.h"
#include "math/mathgl.h"

struct vertex_t
{
	GLfloat position[3];
	GLfloat texcoord[2];
	GLfloat normal[3];
};

class mesh_t
{
public:
	mesh_t();
	mesh_t(vec3 pos, vec3 scale, int mater, GLuint tex, GLuint texn , GLuint texs, g_object *mod);
	mat4 get_model_mat();
	void render(GLuint program, camera_t &camera);
	void render_normals(GLuint program, camera_t &camera);
	void render_pol_mesh(GLuint program, camera_t &camera);
	void render_with_norms(GLuint program, camera_t &camera);
	void rotate(GLfloat x, GLfloat y, GLfloat z);
	void rotate(vec3 vec);
	void rotate_strict(GLfloat x, GLfloat y, GLfloat z);
	void rotate_strict(vec3 vec);
	void rotate_around_y(vec3 point, GLfloat angle);
	void rotate_around_y_central(vec3 point, GLfloat angle_out, GLfloat angle_in);
	void move(GLfloat x, GLfloat y, GLfloat z);
	void move(vec3 v);
	void move_to(GLfloat x, GLfloat y, GLfloat z);
	void move_to(vec3 v);
	void look_at(vec3 pos, vec3 target, vec3 up);
	void look_at_dir(vec3 pos, vec3 dir, vec3 up);
	void set_size(float sz);

	GLuint VAO;
	GLuint VBO[6];
	GLuint N_VAO;
	GLuint N_VBO[2];
	GLuint P_VAO;
	GLuint P_VBO[2];

	int vert_cnt;
	int ind_cnt;
	vec3 position;
	vec3 rotation_angle;
	mat4 rotation;
	mat4 scale;
	vec3 scl;
	GLfloat size;
	int material;
	GLuint texture;
	GLuint tex_n;
	GLuint tex_s;
	bool visible;
	bool physics;
	mat4 aux_matrix;
	g_object *model;
};

extern GLuint norm_tex;

void mesh_create_cube(mesh_t &obj, vec3 position, GLfloat scale, int material, 
	GLuint tex, GLuint texn, GLuint texs);
void mesh_create_sphere(mesh_t &obj, vec3 position, GLfloat scale, int material, 
	GLuint tex, GLuint texn, GLuint texs);
void mesh_create_from_file(char *file, mesh_t &obj, vec3 position, GLfloat scale, int material, 
	GLuint tex, GLuint texn, GLuint texs);

#endif