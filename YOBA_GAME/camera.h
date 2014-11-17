#ifndef _CAMERA_H_INCLUDED_
#define _CAMERA_H_INCLUDED_

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "math/math3d.h"
#include "math/mathgl.h"

class camera_t
{
public:
	camera_t();
	camera_t(float x, float y, float z);
	void calc_perspective(float fov, float aspect, float zNear, float zFar);
	void calc_orto(float left, float right, float bottom, float top, float zNear, float zFar);
	void rotate(float x, float y, float z);
	void move(float x, float y, float z);
	void move_to(float x, float y, float z);
	void move_to(vec3 vec);
	mat4 get_light_matrix();
	void camera_t::look_at(const vec3 &position, const vec3 &center, const vec3 &up);
	void setup(GLuint program, const mat4 &model);
	vec3 position;
	vec3 rotation;
	mat4 projection;
};

struct transform_loc_t
{
	GLint model;
	GLint viewProjection;
	GLint normal;
	GLint viewPosition;
};

extern transform_loc_t transform_locs;

#endif