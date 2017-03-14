#include <cmath>

#include "math/math3d.h"
#include "math/mathgl.h"
#include "physics.h"

#define EPS 1e-5

phys_data_t phys_data;

physical_mesh_t::physical_mesh_t()
{

}

void physical_mesh_t::physics_analize()
{
	speed += phys_data.gravity;
	position += speed;
}

physical_mesh_t::physical_mesh_t(vec3 pos, GLfloat sz, int mater, GLuint tex, g_object *mod, vec3 spd): 
	mesh_t(pos, vec3(sz, sz, sz), mater, tex, -1, -1, mod) 
{
	speed = spd;
}

bullet_t::bullet_t()
{

}

bullet_t::bullet_t(vec3 pos, GLfloat sz, int mater, GLuint tex, g_object *mod, vec3 spd, int life): 
	physical_mesh_t(pos, sz, mater, tex, mod, spd)
{
	this->life = life;
	max_life = life;
}

void bullet_t::physics_analize()
{
	physical_mesh_t::physics_analize();
	life--;
	if (!life)
		visible = false;
}

void bullet_t::physics_reset()
{
	life = max_life;
	visible = true;
}

void bullet_create(bullet_t &bullet, vec3 pos, vec3 spd, int mater, GLuint tex, GLuint tex_n, GLuint tex_s, GLfloat sz, int life)
{
	static bool flag;
	static int  index;

	if (!flag)
		index = reg_object("objects/yoba.obj");
	flag = true;
	if (index == -1)
		return;
	bullet = bullet_t(pos, sz, mater, tex, &objects[index], spd, life);
}

bool bullet_t::intercept(mesh_t obj, int type)
{
	vec3 p;
	switch (type)
	{
	case OT_UNKNOWN:
	{
		for (int i = 0; i < obj.model->ind_num; i += 3)
		{
			if (
				intercept_tr_seg(
					vec3(position.v[0] - size / 2, position.v[1], position.v[2]),
					vec3(position.v[0] + size / 2, position.v[1], position.v[2]),
					vec3(obj.model->verts[obj.model->indicies[i] * 3], obj.model->verts[obj.model->indicies[i] * 3 + 1], obj.model->verts[obj.model->indicies[i] * 3 + 2]),
					vec3(obj.model->verts[obj.model->indicies[i + 1] * 3], obj.model->verts[obj.model->indicies[i + 1] * 3 + 1], obj.model->verts[obj.model->indicies[i + 1] * 3 + 2]),
					vec3(obj.model->verts[obj.model->indicies[i + 2] * 3], obj.model->verts[obj.model->indicies[i + 2] * 3 + 1], obj.model->verts[obj.model->indicies[i + 2] * 3 + 2]), &p)
				|| intercept_tr_seg(
					vec3(position.v[0], position.v[1] - size / 2, position.v[2]),
					vec3(position.v[0], position.v[1] + size / 2, position.v[2]),
					vec3(obj.model->verts[obj.model->indicies[i] * 3], obj.model->verts[obj.model->indicies[i] * 3 + 1], obj.model->verts[obj.model->indicies[i] * 3 + 2]),
					vec3(obj.model->verts[obj.model->indicies[i + 1] * 3], obj.model->verts[obj.model->indicies[i + 1] * 3 + 1], obj.model->verts[obj.model->indicies[i + 1] * 3 + 2]),
					vec3(obj.model->verts[obj.model->indicies[i + 2] * 3], obj.model->verts[obj.model->indicies[i + 2] * 3 + 1], obj.model->verts[obj.model->indicies[i + 2] * 3 + 2]), &p)
				|| intercept_tr_seg(
					vec3(position.v[0], position.v[1], position.v[2] - size / 2),
					vec3(position.v[0], position.v[1], position.v[2] + size / 2),
					vec3(obj.model->verts[obj.model->indicies[i] * 3], obj.model->verts[obj.model->indicies[i] * 3 + 1], obj.model->verts[obj.model->indicies[i] * 3 + 2]),
					vec3(obj.model->verts[obj.model->indicies[i + 1] * 3], obj.model->verts[obj.model->indicies[i + 1] * 3 + 1], obj.model->verts[obj.model->indicies[i + 1] * 3 + 2]),
					vec3(obj.model->verts[obj.model->indicies[i + 2] * 3], obj.model->verts[obj.model->indicies[i + 2] * 3 + 1], obj.model->verts[obj.model->indicies[i + 2] * 3 + 2]), &p))
			return true;
		}
		break;
	}
	case OT_SPHERE:
	{
		return (obj.size + size > distance(obj.position, position));
		break;
	}
	case OT_CUBE:
	{
		break;
	}
	}
	
	return false;
}

int intercept_tr_seg(vec3 p1, vec3 p2, vec3 pa, vec3 pb, vec3 pc, vec3 *p)
{
	double d;
	double a1, a2, a3;
	double total, denom, mu;
	vec3 n, pa1, pa2, pa3;

	n.v[0] = (pb.v[1] - pa.v[1]) * (pc.v[2] - pa.v[2]) - (pb.v[2] - pa.v[2]) * (pc.v[1] - pa.v[1]);
	n.v[1] = (pb.v[2] - pa.v[2]) * (pc.v[0] - pa.v[0]) - (pb.v[0] - pa.v[0]) * (pc.v[2] - pa.v[2]);
	n.v[2] = (pb.v[0] - pa.v[0]) * (pc.v[1] - pa.v[1]) - (pb.v[1] - pa.v[1]) * (pc.v[0] - pa.v[0]);
	normalize(n);
	d = -n.v[0] * pa.v[0] - n.v[1] * pa.v[1] - n.v[2] * pa.v[2];

	denom = n.v[0] * (p2.v[0] - p1.v[0]) + n.v[1] * (p2.v[1] - p1.v[1]) + n.v[2] * (p2.v[2] - p1.v[2]);
	if (fabs(denom) < EPS) 
		return false;
	mu = -(d + n.v[0] * p1.v[0] + n.v[1] * p1.v[1] + n.v[2] * p1.v[2]) / denom;
	p->v[0] = p1.v[0] + mu * (p2.v[0] - p1.v[0]);
	p->v[1] = p1.v[1] + mu * (p2.v[1] - p1.v[1]);
	p->v[2] = p1.v[2] + mu * (p2.v[2] - p1.v[2]);
	if (mu < 0 || mu > 1) 
		return false;

	pa1.v[0] = pa.v[0] - p->v[0];
	pa1.v[1] = pa.v[1] - p->v[1];
	pa1.v[2] = pa.v[2] - p->v[2];
	normalize(pa1);
	pa2.v[0] = pb.v[0] - p->v[0];
	pa2.v[1] = pb.v[1] - p->v[1];
	pa2.v[2] = pb.v[2] - p->v[2];
	normalize(pa2);
	pa3.v[0] = pc.v[0] - p->v[0];
	pa3.v[1] = pc.v[1] - p->v[1];
	pa3.v[2] = pc.v[2] - p->v[2];
	normalize(pa3);
	a1 = pa1.v[0] * pa2.v[0] + pa1.v[1] * pa2.v[1] + pa1.v[2] * pa2.v[2];
	a2 = pa2.v[0] * pa3.v[0] + pa2.v[1] * pa3.v[1] + pa2.v[2] * pa3.v[2];
	a3 = pa3.v[0] * pa1.v[0] + pa3.v[1] * pa1.v[1] + pa3.v[2] * pa1.v[2];
	total = acosf(a1) + acosf(a2) + acosf(a3);
	if (fabs(total - 360) > EPS)
		return false;

	return true;
}

void init_physics(float gr_x, float gr_y, float gr_z)
{
	phys_data.gravity = vec3(gr_x, gr_y, gr_z);
}