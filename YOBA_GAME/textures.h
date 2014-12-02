#ifndef _TEXTURES_H_INC
#define _TEXTURES_H_INC

#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint get_texture_from_tga(char *name);
GLuint texture_create_depth(GLsizei width, GLsizei height);
GLuint texture_create_cubemap(GLsizei size);

#endif