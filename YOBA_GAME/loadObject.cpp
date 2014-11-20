#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "loadObject.h"
#include "error_log.h"

using namespace std;

int obj_count = 0;
g_object objects[MAX_OBJECTS];

inline float strtofloat(char *s)
{
	int i = 0;
	float res = 0;
	int flag = 1;
	if (s[0] == '-')
	{
		i++;
		flag = -1;
	}
	while (s[i] != 0 && s[i] != '.')
		res = res * 10 + (int)(s[i++] - '0');
	i++;
	int buf = 10;
	while (s[i] != 0)
	{
		res += (float)(s[i] - '0') / (buf);
		buf *= 10;
		i++;
	}
	return res * flag;
}

void save_bin_aux_f(vector<GLfloat> &v, FILE *F)
{
	int buf = v.size();
	fwrite(&buf, sizeof(int), 1, F);
	fwrite(&v[0], sizeof(GLfloat), buf, F);
}

void save_bin_aux_u(vector<GLuint> &v, FILE *F)
{
	int buf = v.size();
	fwrite(&buf, sizeof(int), 1, F);
	fwrite(&v[0], sizeof(GLuint), buf, F);
}

void save_bin(g_object *obj, FILE *F)
{
	int buf = BIN_FORMAT_VER;
	fwrite(&buf, sizeof(int), 1, F);

	save_bin_aux_f(obj->verts, F);
	save_bin_aux_f(obj->texcoords, F);
	save_bin_aux_f(obj->normals, F);
	save_bin_aux_u(obj->indicies, F);
	save_bin_aux_f(obj->N_verts, F);
	save_bin_aux_u(obj->N_indicies, F);
	save_bin_aux_f(obj->P_verts, F);
	save_bin_aux_u(obj->P_indicies, F);
}

void read_bin_aux_f(vector<GLfloat> &v, FILE *F)
{
	int buf;
	fread(&buf, sizeof(int), 1, F);
	v.resize(buf);
	if (buf == 5760)
	{
		int i = 5;
		i++;
	}
	fread(&v[0], sizeof(GLfloat), buf, F);
}

void read_bin_aux_u(vector<GLuint> &v, FILE *F)
{
	int buf;
	fread(&buf, sizeof(int), 1, F);
	v.resize(buf);
	fread(&v[0], sizeof(GLuint), buf, F);
}

int load_bin(g_object *obj, FILE *F)
{
	int buf;
	fread(&buf, sizeof(int), 1, F);
	if (buf != BIN_FORMAT_VER)
		return LS_FAIL;

	read_bin_aux_f(obj->verts, F);
	read_bin_aux_f(obj->texcoords, F);
	read_bin_aux_f(obj->normals, F);
	read_bin_aux_u(obj->indicies, F);
	read_bin_aux_f(obj->N_verts, F);
	read_bin_aux_u(obj->N_indicies, F);
	read_bin_aux_f(obj->P_verts, F);
	read_bin_aux_u(obj->P_indicies, F);

	obj->vert_num = obj->verts.size() / 3;
	obj->ind_num = obj->indicies.size();
	return LS_OK;
}

void load_object(g_object *obj, char *file)
{
	//ifstream ifs(file, ifstream::in);
	//freopen(file, "r", stdin);

	FILE *F;

	char *File = new char[100];
	File[0] = 0;
	strcat(File, file);
	strcat(File, "bin");

	if (LOAD_BIN)
		if ((F = fopen(File, "rb")) && load_bin(obj, F) == LS_OK)
		{
			obj->file = file;
			return;
		}

	F = fopen(file, "rt");

	if (!F)
		raise_error("Can't load .obj file", true, true);

	vector<GLfloat> vt;
	vector<GLfloat> uv;
	vector<GLfloat> ns;
	vector<GLuint>  ind;

	char *s = new char[10000];
	while (!feof(F))
	{
		//ifs >> s;
		fscanf(F, "%s", s);
		if (!strcmp(s, "v"))
		{
			GLfloat buf;
			double dbuf;
			for (int i = 0; i < 3; i++)
			{
				//ifs >> buf;
				fscanf(F, "%s", s);
				vt.push_back(strtofloat(s));
			}
		}
		else if (!strcmp(s, "vt"))
		{
			GLfloat buf;
			for (int i = 0; i < 2; i++)
			{
				//ifs >> buf;
				fscanf(F, "%s", s);
				uv.push_back(strtofloat(s));
			}
		}
		else if (!strcmp(s, "vn"))
		{
			GLfloat buf;
			for (int i = 0; i < 3; i++)
			{
				//ifs >> buf;
				fscanf(F, "%s", s);
				ns.push_back(strtofloat(s));
			}
		}
		else if (!strcmp(s, "f"))
		{
			//getline(ifs, s);
			GLuint buf[9];

			fscanf(F, "%u/%u/%u %u/%u/%u %u/%u/%u ", 
				&buf[0], &buf[1], &buf[2], 
				&buf[3], &buf[4], &buf[5], 
				&buf[6], &buf[7], &buf[8]);
			ind.push_back(buf[0] - 1);
			ind.push_back(buf[1] - 1);
			ind.push_back(buf[2] - 1);
			ind.push_back(buf[3] - 1);
			ind.push_back(buf[4] - 1);
			ind.push_back(buf[5] - 1);
			ind.push_back(buf[6] - 1);
			ind.push_back(buf[7] - 1);
			ind.push_back(buf[8] - 1);
		}
	}
	size_t i = 0;
	while (i < ind.size())
	{
		obj->indicies.push_back(i / 3);

		obj->verts.push_back(vt[ind[i] * 3	  ]);
		obj->verts.push_back(vt[ind[i] * 3 + 1]);
		obj->verts.push_back(vt[ind[i] * 3 + 2]);
		i++;

		obj->texcoords.push_back(uv[ind[i] * 2    ]);
		obj->texcoords.push_back(uv[ind[i] * 2 + 1]);
		i++;

		obj->normals.push_back(ns[ind[i] * 3	]);
		obj->normals.push_back(ns[ind[i] * 3 + 1]);
		obj->normals.push_back(ns[ind[i] * 3 + 2]);
		i++;
	}

	obj->vert_num = obj->verts.size() / 3;
	obj->ind_num = obj->indicies.size();
	obj->file = file;
	
	for (i = 0; i < obj->ind_num; i++)
	{
		obj->N_verts.push_back(obj->verts[i * 3]);
		obj->N_verts.push_back(obj->verts[i * 3 + 1]);
		obj->N_verts.push_back(obj->verts[i * 3 + 2]);

		obj->N_verts.push_back(obj->verts[i * 3]     + obj->normals[i * 3]     / 10);
		obj->N_verts.push_back(obj->verts[i * 3 + 1] + obj->normals[i * 3 + 1] / 10);
		obj->N_verts.push_back(obj->verts[i * 3 + 2] + obj->normals[i * 3 + 2] / 10);

		obj->N_indicies.push_back(i * 2);
		obj->N_indicies.push_back(i * 2 + 1);
	}

	for (i = 0; i < obj->vert_num / 3; i++)
	{
		obj->P_verts.push_back(obj->verts[i * 9	   ]);
		obj->P_verts.push_back(obj->verts[i * 9 + 1]);
		obj->P_verts.push_back(obj->verts[i * 9 + 2]);

		obj->P_verts.push_back(obj->verts[i * 9 + 3]);
		obj->P_verts.push_back(obj->verts[i * 9 + 4]);
		obj->P_verts.push_back(obj->verts[i * 9 + 5]);


		obj->P_verts.push_back(obj->verts[i * 9 + 3]);
		obj->P_verts.push_back(obj->verts[i * 9 + 4]);
		obj->P_verts.push_back(obj->verts[i * 9 + 5]);
						
		obj->P_verts.push_back(obj->verts[i * 9 + 6]);
		obj->P_verts.push_back(obj->verts[i * 9 + 7]);
		obj->P_verts.push_back(obj->verts[i * 9 + 8]);


		obj->P_verts.push_back(obj->verts[i * 9 + 6]);
		obj->P_verts.push_back(obj->verts[i * 9 + 7]);
		obj->P_verts.push_back(obj->verts[i * 9 + 8]);

		obj->P_verts.push_back(obj->verts[i * 9]);
		obj->P_verts.push_back(obj->verts[i * 9 + 1]);
		obj->P_verts.push_back(obj->verts[i * 9 + 2]);
			 
		obj->P_indicies.push_back(i * 6);
		obj->P_indicies.push_back(i * 6 + 1);
		obj->P_indicies.push_back(i * 6 + 2);
		obj->P_indicies.push_back(i * 6 + 3);
		obj->P_indicies.push_back(i * 6 + 4);
		obj->P_indicies.push_back(i * 6 + 5);
	}

	fclose(F);

	F = fopen(File, "wb");
	save_bin(obj, F);

	delete[] s;
	delete[] File;
}

int reg_object(char *file)
{
	for (int i = 0; i < obj_count; i++)
		if (objects[i].file == file)
			return i;
	load_object(&objects[obj_count], file);
	obj_count++;
	return obj_count - 1;
}

g_object clone_obj(char *file)
{
	int num = reg_object(file);
	g_object res = objects[num];
	return res;
}