#ifndef _LIGHT_H_INC_
#define _LIGHT_H_INC_

#include "disable_warnings.h"

#include "math/math3d.h"
#include "math/mathgl.h"
#include "camera.h"

#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

class light_t
{
public:
	virtual void add(vec4 amb, vec4 diff, vec4 spec, int window[2], int shad_size);

	std::vector<vec4> ambient;
	std::vector<vec4> diffuse;
	std::vector<vec4> specular;
	std::vector<mat4> mat;
	std::vector<GLuint> depth_tex;
	int cnt;

	std::vector<GLuint> depthFBO;
	std::vector<camera_t> camera;
};

class p_light_t : public light_t
{
public:
	virtual void add(vec4 pos, vec4 amb, vec4 diff, vec4 spec, vec3 att, int window[2], int shad_size);
	virtual void init_cubemap(GLuint sz);
	virtual void p_light_t::bind_to_face(int ind, int face);
	void setup(GLuint program);

	std::vector<GLuint> cubemap;
	std::vector<vec4> position;
	std::vector<vec4> attenuation;
};

class d_light_t : public light_t
{
public:
	virtual void add(vec4 pos, vec4 amb, vec4 diff, vec4 spec, int window[2], int shad_size);
	void setup(GLuint program);
	std::vector<vec4> direction;
};

class s_light_t : public p_light_t
{
public:
	virtual void add(vec4 pos, vec4 dir, vec4 amb, vec4 diff, vec4 spec, vec3 att, GLfloat cut, GLfloat exp, int window[2], int shad_size);
	void setup(GLuint program);
	std::vector<vec4> direction;
	std::vector<GLfloat> cutoff;
	std::vector<GLfloat> exponent;
};

extern p_light_t p_data;
extern d_light_t d_data;
extern s_light_t s_data;

struct material_t
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 emission;
	float shininess;
};

struct material_loc_t
{
	GLint texture_loc;
	GLint ambient_loc;
	GLint diffuse_loc;
	GLint specular_loc;
	GLint emission_loc;
	GLint shininess_loc;
};

bool toggle_point_setup();
bool toggle_direct_setup();
bool toggle_spot_setup();

void setup_lights(GLuint program);
void material_setup(GLuint program, int material);

extern material_loc_t material_locs;
extern material_t *materials;

int material_create(
	vec4 ambient, vec4 diffuse, vec4 specular, vec4 emission, GLfloat shininess);

#endif