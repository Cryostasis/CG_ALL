#ifndef _GRAPHOBJ_H_INC_
#define _GRAPHOBJ_H_INC_

#define VERT_POSITION  0
#define VERT_TEXCOORD  1
#define VERT_NORMAL    2

#define FRAG_OUTPUT0   0

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
	mesh_t(vec3 pos, GLfloat sz, int mater, GLuint tex, g_object *mod);
	mat4 get_model_mat();
	void render(GLuint program, camera_t &camera);
	void render_normals(GLuint program, camera_t &camera, int mat);
	void render_with_norms(GLuint program, camera_t &camera, int mat);
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
	GLuint VBO[4];
	GLuint N_VAO;
	GLuint N_VBO[2];
	int vert_cnt;
	int ind_cnt;
	vec3 position;
	vec3 rotation_angle;
	mat4 rotation;
	mat4 scale;
	GLfloat size;
	int material;
	GLuint texture;
	bool visible;
	bool physics;
	mat4 aux_matrix;
	g_object *model;
};

extern GLuint norm_tex;

void mesh_create_cube(mesh_t &obj, vec3 position, GLfloat scale, int material, GLuint tex);
void mesh_create_sphere(mesh_t &obj, vec3 position, GLfloat scale, int material, GLuint tex);
void mesh_create_from_file(char *file, mesh_t &obj, vec3 position, GLfloat scale, int material, GLuint tex);

#endif