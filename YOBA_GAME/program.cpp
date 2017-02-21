#include "error_log.h"
#include "glerrors.h"
#include "files.h"
#include "light.h"
#include "camera.h"
#include "program.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

GLuint Program;
GLuint depth_program;
GLuint normals_program;
GLuint quad_program;
GLuint line_program;
GLuint text_program;
GLuint cube_depth_program;

GLint use_tex_loc;

char* read_shader_f(char *file)
{
	int len;
	char *buf;

	len = load_file(file, false, false, &buf);

	if (!len)
		return NULL;

	for (int i = 0; i < len; i++)
	if (buf[i] == '~')
	{
		buf[i] = 0;
		break;
	}

	return buf;
}

int check_shader(GLuint shader, char *name)
{
	int   infologLen = 0;
	int   charsWritten = 0;
	char *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		printf("Shader '%s' log: %s\n", name, infoLog);
		delete[] infoLog;
		return ST_SHADER_ERROR;
	}
	return ST_OK;
}

int check_program(GLuint program)
{
	int   infologLen = 0;
	int   charsWritten = 0;
	char *infoLog;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		glGetProgramInfoLog(program, infologLen, &charsWritten, infoLog);
		printf("Program log: %s\n", infoLog);
		delete[] infoLog;
		return ST_SHADER_ERROR;
	}
	return ST_OK;
}

int check_attrunif(GLint* d, GLchar* name)
{
	char *buf = new char[50];
	if (*d == -1)
	{
		sprintf(buf, "Can't find '%s'\n", name);
		raise_error(buf, false, true);
		delete[] buf;
		return ST_SHADER_ERROR;
	}
	delete[] buf;
	return ST_OK;
}

int get_attrib_loc(GLint* attrib, GLuint prog, GLchar* name)
{
	*attrib = glGetAttribLocation(prog, name);
	return check_attrunif(attrib, name);
}

int get_unif_loc(GLint* unif, GLuint prog, GLchar* name)
{
	*unif = glGetUniformLocation(prog, name);
	return check_attrunif(unif, name);
}

int init_shaders(char *p_v, char* p_f, GLuint &prog)
{
	GLuint vs, fs;

	char *vs_code = read_shader_f(p_v);
	char *fs_code = read_shader_f(p_f);

	if (!vs_code || !fs_code)
	{
		raise_error("Error read shaders \n", false, true);
		return ST_SHADER_ERROR;
	}

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_code, NULL);
	glCompileShader(vs);
	int err = check_shader(vs, p_v);
	if (err)
		return err;

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_code, NULL);
	glCompileShader(fs);
	err = check_shader(fs, p_f);
	if (err)
		return err;

	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glLinkProgram(prog);
	err = check_program(prog);
	if (err)
		return err;

	delete[] vs_code;
	delete[] fs_code;

	if (check_GL_error())
		return ST_SHADER_ERROR;
	return ST_OK;
}

int init_programs()
{
	int err = init_shaders("shaders/vs5.glsl", "shaders/fs5.glsl", Program);
	if (err)
		return err;

	get_unif_loc(&use_tex_loc, Program, "use_tex");

	get_unif_loc(&material_locs.texture_loc, Program, "material.texture");
	
	get_unif_loc(&material_locs.tex_n_loc, Program, "material.tex_n");
	get_unif_loc(&material_locs.tex_s_loc, Program, "material.tex_s");

	get_unif_loc(&material_locs.ambient_loc, Program, "material.ambient");
	get_unif_loc(&material_locs.diffuse_loc, Program, "material.diffuse");
	get_unif_loc(&material_locs.specular_loc, Program, "material.specular");
	get_unif_loc(&material_locs.emission_loc, Program, "material.emission");
	get_unif_loc(&material_locs.shininess_loc, Program, "material.shininess");

	get_unif_loc(&transform_locs.model, Program, "transform.model");
	get_unif_loc(&transform_locs.viewProjection, Program, "transform.viewProjection");
	get_unif_loc(&transform_locs.normal, Program, "transform.normal");
	get_unif_loc(&transform_locs.viewPosition, Program, "transform.viewPosition");

	err = init_shaders("shaders/shad_v.glsl", "shaders/shad_f.glsl", depth_program);
	if (err)
		return err;

	err = init_shaders("shaders/shad_cube_v.glsl", "shaders/shad_cube_f.glsl", cube_depth_program);
	if (err)
		return err;

	err = init_shaders("shaders/line_v.glsl", "shaders/line_f.glsl", line_program);
	if (err)
		return err;

	err = init_shaders("shaders/text_v.glsl", "shaders/text_f.glsl", text_program);
	if (err)
		return err;

	return ST_OK;
}