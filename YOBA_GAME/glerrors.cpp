#include "glerrors.h"
#include "error_log.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>

int check_GL_error()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		const GLubyte *c = gluErrorString(err);
		char *buf = new char[100];
		sprintf(buf, "OpenGl error: %s", (char*)c);
		raise_error(buf, false, true);
		delete[] buf;
		return ST_GL_ERROR;
	}
	return ST_OK;
}