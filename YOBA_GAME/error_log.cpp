#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include "error_log.h"

struct err_log
{
	char **data;
	int sz;
} Log;

char *error_to_string(int err)
{
	switch (err)
	{
	case ST_OK:					{return "ST_OK";}
	case ST_GLEW_INIT_ERROR:	{return "ST_GLEW_INIT_ERROR";}
	case ST_GL_ERROR:			{return "ST_GL_ERROR";}
	case ST_GLUT_INIT_ERROR:	{return "ST_GLUT_INIT_ERROR";}
	case ST_SHADER_ERROR:		{return "ST_SHADER_ERROR";}
	case ST_BUFFER_INIT_ERROR:	{return "ST_BUFFER_INIT_ERROR";}
	default:					{return "UNKNOWN";}
	}
}

void halt()
{
	//print_log();
	system("pause");
	exit(ST_UNKNOWN_ERROR);
}

#define STR_RESERVED 20

void raise_error(char *info, bool stop, bool print)
{
	char *buf = (char*)malloc(sizeof(char)* (STR_RESERVED + strlen(info)));
	Log.sz++;
	sprintf(buf, "%s%d%s%s", "Error #", Log.sz, ": ", info);
	Log.data = (char**)realloc(Log.data, Log.sz * sizeof(int));
	Log.data[Log.sz - 1] = buf;
	if (print)
		printf("%s\n", buf);
	if (stop)
		halt();
	free(buf);
}

void print_log()
{
	if (Log.sz)
		printf(">> %d errors in log:\n", Log.sz);
	else
		printf(">> No errors in log\n");
	for (int i = 0; i < Log.sz; i++)
		printf("%s\n", Log.data[i]);
	printf("\n");
}
