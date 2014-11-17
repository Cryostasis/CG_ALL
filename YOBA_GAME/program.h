#ifndef _SHADERS_H_INCLUDED_
#define _SHADERS_H_INCLUDED_

#include <GL/glew.h>
#include <GL/freeglut.h>

char* read_shader_f(char *file);
int init_programs();

extern GLuint Program;
extern GLuint depth_program;
extern GLuint quad_program;

extern GLint use_tex_loc;

#endif