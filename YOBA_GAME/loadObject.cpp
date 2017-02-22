#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include "math/math3d.h"
#include "math/mathgl.h"

#include "loadObject.h"
#include "error_log.h"

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>      
#include <assimp/postprocess.h>

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

void calc_tangent(vec3 pos1, vec3 pos2, vec3 pos3, vec2 uv1, vec2 uv2, vec3 uv3,
	vec3& tangent, vec3& bitangent)
{
	vec3 edge1 = pos2 - pos1;
	vec3 edge2 = pos3 - pos1;
	vec2 deltaUV1 = uv2 - uv1;
	vec2 deltaUV2 = uv3 - uv1;


	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = normalize(bitangent);
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
			for (int i = 0; i < 3; i++)
			{
				//ifs >> buf;
				fscanf(F, "%s", s);
				vt.push_back(strtofloat(s));
			}
		}
		else if (!strcmp(s, "vt"))
		{
			for (int i = 0; i < 2; i++)
			{
				//ifs >> buf;
				fscanf(F, "%s", s);
				uv.push_back(strtofloat(s));
			}
		}
		else if (!strcmp(s, "vn"))
		{
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
		/*
		if (i % 9 == 0)
		{
			vec3 T, B;
			calc_tangent(
				vec3(
					obj->verts[obj->verts.size() - 9],
					obj->verts[obj->verts.size() - 8],
					obj->verts[obj->verts.size() - 7]),
				vec3(
					obj->verts[obj->verts.size() - 6],
					obj->verts[obj->verts.size() - 5],
					obj->verts[obj->verts.size() - 4]),
				vec3(
					obj->verts[obj->verts.size() - 3],
					obj->verts[obj->verts.size() - 2],
					obj->verts[obj->verts.size() - 1]),
				vec2(
					obj->texcoords[obj->texcoords.size() - 6],
					obj->texcoords[obj->texcoords.size() - 5]),
				vec2(
					obj->texcoords[obj->texcoords.size() - 4],
					obj->texcoords[obj->texcoords.size() - 3]),
				vec2(
					obj->texcoords[obj->texcoords.size() - 2],
					obj->texcoords[obj->texcoords.size() - 1]),
				T, B);
			for (int k = 0; k < 3; k++)
			{
				obj->tangent.push_back(T.x);
				obj->tangent.push_back(T.y);
				obj->tangent.push_back(T.z);

				obj->bitangent.push_back(B.x);
				obj->bitangent.push_back(B.y);
				obj->bitangent.push_back(B.z);
			}
		}*/
	}

	obj->vert_num = obj->verts.size() / 3;
	obj->ind_num = obj->indicies.size();
	obj->file = file;
	
	for (int i = 0; i < obj->ind_num; i++)
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

	for (int i = 0; i < obj->vert_num / 3; i++)
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

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file,
		aiProcess_CalcTangentSpace);

	for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++)
	{
		obj->tangent.push_back(scene->mMeshes[0]->mTangents[i].x);
		obj->tangent.push_back(scene->mMeshes[0]->mTangents[i].y);
		obj->tangent.push_back(scene->mMeshes[0]->mTangents[i].z);
		
		obj->bitangent.push_back(scene->mMeshes[0]->mBitangents[i].x);
		obj->bitangent.push_back(scene->mMeshes[0]->mBitangents[i].y);
		obj->bitangent.push_back(scene->mMeshes[0]->mBitangents[i].z);
	}

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