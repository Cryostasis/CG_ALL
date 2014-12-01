#include "light.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "textures.h"
#include "error_log.h"
#include "camera.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

p_light_t p_data;
d_light_t d_data;
s_light_t s_data;

material_loc_t material_locs;
material_t *materials;

int material_cnt;

void light_t::add(vec4 amb, vec4 diff, vec4 spec, int window[2], int shad_size)
{
	cnt++;
	ambient.push_back(amb);
	diffuse.push_back(diff);
	specular.push_back(spec);
	depth_tex.push_back(texture_create_depth(window[0] * shad_size, window[1] * shad_size));
	depthFBO.push_back(-1);

	GLenum fboStatus;

	//glGenFramebuffersEXT(1, &depthFBO[cnt - 1]);
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depthFBO[cnt - 1]);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex[cnt - 1], 0);

	glGenFramebuffers(1, &depthFBO[cnt - 1]);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO[cnt - 1]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex[cnt - 1], 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		raise_error("Framebuffer error ", false, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void p_light_t::add(vec4 pos, vec4 amb, vec4 diff, vec4 spec, vec3 att, int window[2], int shad_size)
{
	light_t::add(amb, diff, spec, window, shad_size);
	position.push_back(pos);
	attenuation.push_back(vec4(att));

	camera.push_back(camera_t());
	camera[cnt - 1].move_to(position[cnt - 1]);
	camera[cnt - 1].calc_perspective(360.0, 16.0 / 9.0, 0.05, 20.0);

	mat.push_back(camera[cnt - 1].get_light_matrix());
}

void d_light_t::add(vec4 pos, vec4 amb, vec4 diff, vec4 spec, int window[2], int shad_size)
{
	light_t::add(amb, diff, spec, window, shad_size);
	direction.push_back(pos);

	camera.push_back(camera_t());
	camera[cnt - 1].look_at(direction[cnt - 1], -direction[cnt - 1], vec3_y);
	camera[cnt - 1].calc_orto(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);

	mat.push_back(camera[cnt - 1].get_light_matrix());
}

void s_light_t::add(vec4 pos, vec4 dir, vec4 amb, vec4 diff, vec4 spec, vec3 att, GLfloat cut, GLfloat exp, int window[2], int shad_size)
{
	light_t::add(amb, diff, spec, window, shad_size);
	direction.push_back(dir);
	position.push_back(pos);
	attenuation.push_back(vec4(att));
	cutoff.push_back(cut);
	exponent.push_back(exp);

	camera.push_back(camera_t());
	camera[cnt - 1].look_at(position[cnt - 1], position[cnt - 1] + direction[cnt - 1], vec3_y);
	camera[cnt - 1].calc_perspective(acosf(cutoff[cnt - 1]) * 180.0 / M_PI, 1.0, 0.05, 20.0);

	mat.push_back(camera[cnt - 1].get_light_matrix());
}

void p_light_t::setup(GLuint program)
{
	const int block_sz = 7;
	const char *block_name = "pLight";
	static const char *p_light_names[block_sz] =
	{
		"pLight_num",
		"pLight_position",
		"pLight_ambient",
		"pLight_diffuse",
		"pLight_specular",
		"pLight_attenuation",
		"pLight_mat"
	};

	static GLuint index[block_sz];
	static int    offset[block_sz];
	static GLuint blockIndex;
	static int    blockSize;
	static bool   init = false;
	static GLuint buffer;

	if (!init)
	{
		init = true;
		blockIndex = glGetUniformBlockIndex(program, block_name);
		glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
		glGetUniformIndices(program, block_sz, p_light_names, index);
		glGetActiveUniformsiv(program, block_sz, index, GL_UNIFORM_OFFSET, offset);
		
		char *ch = new char[blockSize];
		memset(ch, 0, blockSize);

		glGenBuffersARB(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, blockSize, ch, GL_STREAM_DRAW);
	}
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffer);
	glUniformBlockBinding(program, blockIndex, 0);

	char *ptr = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	memcpy(ptr + offset[0], &cnt, sizeof(int));
	if (!cnt)
	{
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		return;
	}
	memcpy(ptr + offset[1], position[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[2], ambient[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[3], diffuse[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[4], specular[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[5], attenuation[0].v, cnt * 4 * sizeof(GLfloat));

	memcpy(ptr + offset[6], mat[0].m, cnt * 4 * 4 * sizeof(GLfloat));


	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void d_light_t::setup(GLuint program)
{
	const int block_sz = 6;
	const char *block_name = "dLight";
	static const char *d_light_names[block_sz] =
	{
		"dLight_num",
		"dLight_direction",
		"dLight_ambient",
		"dLight_diffuse",
		"dLight_specular",
		"dLight_mat"
	};

	static GLuint index[block_sz];
	static int    offset[block_sz];
	static GLuint blockIndex;
	static int    blockSize;
	static bool   init = false;
	static GLuint buffer;

	if (!init)
	{
		init = true;
		blockIndex = glGetUniformBlockIndex(program, block_name);
		glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
		glGetUniformIndices(program, block_sz, d_light_names, index);
		glGetActiveUniformsiv(program, block_sz, index, GL_UNIFORM_OFFSET, offset);

		char *ch = new char[blockSize];
		memset(ch, 0, blockSize);

		glGenBuffersARB(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, blockSize, ch, GL_STREAM_DRAW);
	}

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, buffer);
	glUniformBlockBinding(program, blockIndex, 1);
	char *ptr = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	memcpy(ptr + offset[0], &cnt, sizeof(int));
	if (!cnt)
	{
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		return;
	}
	memcpy(ptr + offset[1], direction[0].v,	cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[2], ambient[0].v,	cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[3], diffuse[0].v,	cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[4], specular[0].v,	cnt * 4 * sizeof(GLfloat));

	memcpy(ptr + offset[5], mat[0].m, cnt * 4 * 4 * sizeof(GLfloat));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void s_light_t::setup(GLuint program)
{
	const int block_sz = 10;
	const char *block_name = "sLight";
	static const char *s_light_names[block_sz] =
	{
		"sLight_num",
		"sLight_position",
		"sLight_direction",
		"sLight_ambient",
		"sLight_diffuse",
		"sLight_specular",
		"sLight_attenuation",
		"sLight_cutoff",
		"sLight_exponent",
		"sLight_mat"
	};

	static GLuint index[block_sz];
	static int    offset[block_sz];
	static GLuint blockIndex;
	static int    blockSize;
	static bool   init = false;
	static GLuint buffer;

	if (!init)
	{
		init = true;
		blockIndex = glGetUniformBlockIndex(program, block_name);
		glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
		glGetUniformIndices(program, block_sz, s_light_names, index);
		glGetActiveUniformsiv(program, block_sz, index, GL_UNIFORM_OFFSET, offset);

		char *ch = new char[blockSize];
		memset(ch, 0, blockSize);

		glGenBuffersARB(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, blockSize, ch, GL_STREAM_DRAW);
	}

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, buffer);
	glUniformBlockBinding(program, blockIndex, 2);
	char *ptr = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	
	memcpy(ptr + offset[0], &cnt, sizeof(int));
	if (!cnt)
	{
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		return;
	}
	memcpy(ptr + offset[1], position[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[2], direction[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[3], ambient[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[4], diffuse[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[5], specular[0].v,	 cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[6], attenuation[0].v, cnt * 4 * sizeof(GLfloat));
	memcpy(ptr + offset[7], &cutoff[0],		 cnt * sizeof(GLfloat));
	memcpy(ptr + offset[8], &exponent[0],	 cnt * sizeof(GLfloat));

	memcpy(ptr + offset[9], mat[0].m, cnt * 4 * 4 * sizeof(GLfloat));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

bool flg_p_l = true, flg_d_l = true, flg_s_l = true;

bool toggle_point_setup()
{
	static int cnt;
	int buf;
	buf = p_data.cnt;
	p_data.cnt = cnt;
	cnt = buf;
	return flg_p_l = !flg_p_l;
}

bool toggle_direct_setup()
{
	static int cnt;
	int buf;
	buf = d_data.cnt;
	d_data.cnt = cnt;
	cnt = buf;
	return flg_d_l = !flg_d_l;
}

bool toggle_spot_setup()
{
	static int cnt;
	int buf;
	buf = s_data.cnt;
	s_data.cnt = cnt;
	cnt = buf;
	return flg_s_l = !flg_s_l;
}

void setup_lights(GLuint program)
{
	static bool init;
	//if (init)
	//	return;
	init = true;
	p_data.setup(program);
	d_data.setup(program);
	s_data.setup(program);
}

void material_setup(GLuint program, int material)
{
	if (program == 3)
	{
		glUniform4fv(material_locs.ambient_loc, 1, materials[material].ambient.v);
		glUniform4fv(material_locs.diffuse_loc, 1, materials[material].diffuse.v);
		glUniform4fv(material_locs.specular_loc, 1, materials[material].specular.v);
		glUniform4fv(material_locs.emission_loc, 1, materials[material].emission.v);
		glUniform1fv(material_locs.shininess_loc, 1, &materials[material].shininess);
	}
}

int material_create(vec4 ambient, vec4 diffuse, vec4 specular, vec4 emission, GLfloat shininess)
{
	material_cnt++;
	materials = (material_t*)realloc(materials, material_cnt * sizeof(material_t));
	materials[material_cnt - 1].ambient = ambient;
	materials[material_cnt - 1].diffuse = diffuse;
	materials[material_cnt - 1].specular = specular;
	materials[material_cnt - 1].emission = emission;
	materials[material_cnt - 1].shininess = shininess;
	return material_cnt - 1;
}