#include "math/math3d.h"
#include "math/mathgl.h"
#include "glerrors.h"
#include "light.h"
#include "mesh.h"
#include "loadObject.h"
#include "program.h"
#include "light.h"

#include <math.h>

#define _USE_MATH_DEFINES

GLuint norm_tex;

mesh_t::mesh_t()
{

}

mesh_t::mesh_t(vec3 pos, vec3 scale, int mater, GLuint tex, 
	GLuint texn, GLuint texs, g_object *mod)
{
	vert_cnt = mod->vert_num;
	ind_cnt = mod->ind_num;

	position = pos;
	rotation = mat4_identity;
	rotation_angle = vec3(0, 0, 0);
	this->scale = GLScale(scale);
	size = fmax(fmax(scale.v[0], scale.v[1]), scale.v[2]);
	material = mater;
	texture = tex;
	tex_n = texn;
	tex_s = texs;
	visible = true;
	physics = false;
	model = mod;
	aux_matrix = mat4_identity;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(4, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vert_cnt * sizeof(GLfloat) * 3, &model->verts[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERT_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, vert_cnt * sizeof(GLfloat) * 2, &model->texcoords[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERT_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, vert_cnt * sizeof(GLfloat) * 3, &model->normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(VERT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERT_NORMAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_cnt * sizeof(GLuint), &model->indicies[0], GL_STATIC_DRAW);


	glGenVertexArrays(1, &N_VAO);
	glBindVertexArray(N_VAO);

	glGenBuffers(2, N_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, N_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vert_cnt * 2 * sizeof(GLfloat) * 3, &model->N_verts[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERT_POSITION);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, N_VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_cnt * 2 * sizeof(GLuint), &model->N_indicies[0], GL_STATIC_DRAW);


	glGenVertexArrays(1, &P_VAO);
	glBindVertexArray(P_VAO);

	glGenBuffers(2, P_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, P_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vert_cnt * 2 * sizeof(GLfloat) * 3, &model->P_verts[0], GL_STATIC_DRAW);

	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERT_POSITION);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, P_VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_cnt * 2 * sizeof(GLuint), &model->P_indicies[0], GL_STATIC_DRAW);

	check_GL_error();
}

mat4 mesh_t::get_model_mat()
{
	return aux_matrix * GLTranslation(position) * rotation * scale;
}

void mesh_t::render(GLuint program, camera_t &camera)
{
	camera.setup(program, get_model_mat());
	material_setup(program, material);

	if (program == Program)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(material_locs.texture_loc, 0);

		if (tex_n != -1 && tex_s != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_n);
			glUniform1i(material_locs.tex_n_loc, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_s);
			glUniform1i(material_locs.tex_s_loc, 0);
		}
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, ind_cnt, GL_UNSIGNED_INT, NULL);
}

void mesh_t::render_normals(GLuint program, camera_t &camera)
{
	camera.setup(program, get_model_mat());
	//material_setup(program, mat);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, norm_tex);
	//glUniform1i(material_locs.texture_loc, 0);

	vec3 buf = vec3(1.0, 0.0, 0.0);

	glUniform3fv(LINE_COLOR_LOC, 1, buf.v);

	glBindVertexArray(N_VAO);
	glDrawElements(GL_LINES, ind_cnt * 2, GL_UNSIGNED_INT, NULL);
}

void mesh_t::render_pol_mesh(GLuint program, camera_t &camera)
{
	camera.setup(program, get_model_mat());

	glUniform3fv(LINE_COLOR_LOC, 1, &vec3(0.0, 1.0, 0.0).v[0]);

	glBindVertexArray(P_VAO);
	glDrawElements(GL_LINES, ind_cnt * 2, GL_UNSIGNED_INT, NULL);
}

void mesh_t::render_with_norms(GLuint program, camera_t &camera)
{
	render(program, camera);
	render_normals(program, camera);
}

void mesh_t::rotate(GLfloat x, GLfloat y, GLfloat z)
{
	rotation_angle += vec3(x, y, z);
	rotation = GLrotate(rotation_angle);
}

void mesh_t::rotate(vec3 vec)
{
	rotate(vec.v[0], vec.v[1], vec.v[2]);
}

void mesh_t::rotate_strict(GLfloat x, GLfloat y, GLfloat z)
{
	rotation_angle = vec3(x, y, z);
	rotation = GLrotate(rotation_angle);
}

void mesh_t::rotate_strict(vec3 vec)
{
	rotate_strict(vec.v[0], vec.v[1], vec.v[2]);
}

void mesh_t::rotate_around_y(vec3 point, GLfloat angle)
{
	vec3 pos = position - point;
	position = point + vec3(
		GLrotate(0.0, angle, 0.0) *
		vec4(pos.v[0], pos.v[1], pos.v[2], 1.0));
}

void mesh_t::rotate_around_y_central(vec3 point, GLfloat angle_out, GLfloat angle_in)
{
	rotate_around_y(point, angle_out);
	rotate(0.0, angle_in, 0.0);
}

void mesh_t::move(GLfloat x, GLfloat y, GLfloat z)
{
	position = position + vec3(x, y, z);
}

void mesh_t::move(vec3 v)
{
	position = position + v;
}

void mesh_t::move_to(GLfloat x, GLfloat y, GLfloat z)
{
	position = vec3(x, y, z);
}

void mesh_t::move_to(vec3 v)
{
	position = v;
}

void mesh_t::set_size(float sz)
{
	size = sz;
	scale = GLScale(sz, sz, sz);
}

void mesh_t::look_at(vec3 pos, vec3 target, vec3 up)
{
	position = pos;
	rotate_strict(GLToEulerRad(transpose(GLLookAt(pos, target, up))));
}

void mesh_t::look_at_dir(vec3 pos, vec3 dir, vec3 up)
{
	position = pos;
	rotation = GLLookAt(pos, pos + dir, up);
}

void mesh_create_cube(mesh_t &obj, vec3 position, GLfloat scale, int material, 
	GLuint tex, GLuint texn, GLuint texs)
{
	static bool flag;
	static int  index;

	if (!flag)
		index = reg_object("objects/cube.obj");
	flag = true;
	if (index == -1)
		return;
	obj = mesh_t(position, vec3(scale, scale, scale), material, tex, texn, texs, &objects[index]);
}

void mesh_create_sphere(mesh_t &obj, vec3 position, GLfloat scale, int material, GLuint tex)
{
	static bool flag;
	static int  index;

	if (!flag)
		index = reg_object("objects/sphere.obj");
	flag = true;
	if (index == -1)
		return;
	obj = mesh_t(position, vec3(scale, scale, scale), material, tex, -1, -1, &objects[index]);
}

void mesh_create_from_file(char *file, mesh_t &obj, vec3 position, GLfloat scale, int material, GLuint tex)
{
	int index = reg_object(file);
	if (index == -1)
		return;
	obj = mesh_t(position, vec3(scale, scale, scale), material, tex, -1, -1, &objects[index]);
}

void mesh_create_from_clone(mesh_t &obj, g_object clone, vec3 position, GLfloat scale, GLuint tex)
{
	obj = mesh_t(position, vec3(scale, scale, scale), 0, tex, -1, -1, &clone);
}