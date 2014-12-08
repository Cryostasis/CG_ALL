#ifndef _ERROR_LOG_H_INC
#define _ERROR_LOG_H_INC

#include "disable_warnings.h"

#include <string>

enum STATUS { ST_OK, ST_GLEW_INIT_ERROR, ST_GL_ERROR, ST_GLUT_INIT_ERROR, ST_SHADER_ERROR, ST_BUFFER_INIT_ERROR, ST_UNKNOWN_ERROR, ST_TEXTURE_ERROR, ST_TEXTURE_INIT_ERROR };

struct err_log;

char *error_to_string(int err);
void raise_error(char* info, bool stop, bool print);
void print_log();

#endif