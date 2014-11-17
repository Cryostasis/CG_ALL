#include "error_log.h"

#include <stdio.h>
#include <stdlib.h>

int load_file(char *name, bool binary, bool strict, char **buffer)
{
	FILE *f_in;
	int size, readed;
	char *buf = new char[100];

	const char mode[] = { 'r', binary ? 'b' : 't', '\0' };

	if ((f_in = fopen(name, mode)) == NULL)
	{
		sprintf(buf, "Error in opening file '%s'\n");
		raise_error(buf, false, true);
		return 0;
	}

	fseek(f_in, 0, SEEK_END);
	size = (int)ftell(f_in);
	rewind(f_in);

	if (!size)
	{
		sprintf(buf, "Empty file '%s'\n");
		raise_error(buf, false, true);
		fclose(f_in);
		return 0;
	}

	*buffer = new char[size];

	readed = fread(*buffer, 1, size, f_in);

	fclose(f_in);

	if (strict && readed != size)
	{
		sprintf(buf, "Error in reading file '%s'\n");
		raise_error(buf, false, true);
		delete[] * buffer;
		return 0;
	}
	delete[] buf;

	return size;
}