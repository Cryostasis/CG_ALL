#ifndef _PHYSICS_H_INC_
#define _PHYSICS_H_INC_

#include "math/math3d.h"
#include "math/mathgl.h"
#include "mesh.h"

enum OBJ_TYPES { OT_UNKNOWN, OT_SPHERE, OT_CUBE };

struct phys_data_t
{
	vec3 gravity;
};

extern phys_data_t phys_data;

class physical_mesh_t : public mesh_t
{
public:
	physical_mesh_t();
	physical_mesh_t(vec3 pos, GLfloat sz, int mater, GLuint tex, g_object *mod, vec3 spd);
	vec3 speed;
	void physics_analize();
};

class bullet_t : public physical_mesh_t
{
public:
	bullet_t();
	bullet_t(vec3 pos, GLfloat sz, int mater, GLuint tex, g_object *mod, vec3 spd, int life);
	bool intercept(mesh_t obj, int type);
	void physics_analize();
	void physics_reset();
	int life;
	int max_life;
};

void bullet_create(bullet_t &bullet, vec3 pos, vec3 spd, int mater, GLuint tex, GLfloat sz, int life);

int intercept_tr_seg(vec3 p1, vec3 p2, vec3 pa, vec3 pb, vec3 pc, vec3 *p);
void init_physics(float gr_x, float gr_y, float gr_z);

#endif