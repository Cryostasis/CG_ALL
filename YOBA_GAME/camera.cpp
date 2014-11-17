#include "math/math3d.h"
#include "math/mathgl.h"
#include "camera.h"
#include "CG_mat.h"
#define _USE_MATH_DEFINES
#include <cmath>

transform_loc_t transform_locs;

camera_t::camera_t()
{
}

camera_t::camera_t(float x, float y, float z)
{
	position = vec3(x, y, z);
	projection = mat4_identity;
	rotation = vec3(0, 0, 0);
}

void camera_t::calc_perspective(float fov, float aspect, float zNear, float zFar)
{
	projection = GLPerspective(fov, aspect, zNear, zFar);
}

void camera_t::calc_orto(float left, float right, float bottom, float top, float zNear, float zFar)
{
	projection = GLOrthographic(left, right, bottom, top, zNear, zFar);
}

void camera_t::rotate(float x, float y, float z)
{
	rotation.v[0] = rotation.v[0] + x;
	rotation.v[1] = rotation.v[1] + y;

	if (rotation.v[0] < -M_PI / 2)
		rotation.v[0] = -M_PI / 2;
	if (rotation.v[0] > M_PI / 2)
		rotation.v[0] = M_PI / 2;
}

extern int ololo;
int ololo = 3;

mat4 camera_t::get_light_matrix()
{
	static const mat4 bias(
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
	mat4 buf = GLrotate(rotation);

	mat4 view = GLrotate(rotation) * GLTranslation(-position);
	mat4 viewProjection = bias *  projection * view;

	return transpose(viewProjection);
}

void camera_t::move_to(float x, float y, float z)
{
	position = vec3(x, y, z);
}

void camera_t::move_to(vec3 vec)
{
	move_to(vec.v[0], vec.v[1], vec.v[2]);
}

void camera_t::move(float x, float y, float z)
{
	vec4 buf = transpose(GLrotate(rotation)) * vec4(x, y, z, 1.0);
	vec3 move = vec3(buf.v[0], buf.v[1], buf.v[2]);

	position += move;
}

void camera_t::look_at(const vec3 &position, const vec3 &center, const vec3 &up)
{
	rotation = GLToEulerRad(GLLookAt(position, center, up));
	this->position = position;
}

void camera_t::setup(GLuint program, const mat4 &model)
{
	static mat4 view;
	static mat4 viewProjection;
	static mat3 normal;

	static mat4 modelViewProjection;

	view =					GLrotate(rotation) * GLTranslation(-position);
	viewProjection =		projection * view;
	normal =				mat3(transpose(inverse(model)));

	modelViewProjection =	viewProjection * model;

	if (program == 3)
	{
		glUniformMatrix4fv(transform_locs.model, 1, GL_TRUE, model.m);
		glUniformMatrix4fv(transform_locs.viewProjection, 1, GL_TRUE, viewProjection.m);
		glUniformMatrix3fv(transform_locs.normal, 1, GL_TRUE, normal.m);
		glUniform3fv(transform_locs.viewPosition, 1, position.v);
	}

	GLuint loc = glGetUniformLocation(program, "transform.modelViewProjection");
	glUniformMatrix4fv(loc, 1, GL_TRUE, modelViewProjection.m);
}
