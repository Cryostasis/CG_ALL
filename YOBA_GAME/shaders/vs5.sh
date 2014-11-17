#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_NORMAL   2

#define MAX_P_LIGHTS 3
#define MAX_D_LIGHTS 3
#define MAX_S_LIGHTS 3

layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;

uniform struct Transform
{ 
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
} transform;

uniform pLight
{
	int  pLight_num;
	vec4 pLight_position[MAX_P_LIGHTS];
	vec4 pLight_ambient[MAX_P_LIGHTS];
	vec4 pLight_diffuse[MAX_P_LIGHTS];
	vec4 pLight_specular[MAX_P_LIGHTS];
	vec3 pLight_attenuation[MAX_P_LIGHTS];
	mat4 pLight_mat[MAX_P_LIGHTS];
};

uniform dLight
{
	int  dLight_num;
	vec4 dLight_direction[MAX_D_LIGHTS];
	vec4 dLight_ambient[MAX_D_LIGHTS];
	vec4 dLight_diffuse[MAX_D_LIGHTS];
	vec4 dLight_specular[MAX_D_LIGHTS];
	mat4 dLight_mat[MAX_D_LIGHTS];
};

uniform sLight
{
	int   sLight_num;
	vec4  sLight_position[MAX_S_LIGHTS];
	vec4  sLight_direction[MAX_S_LIGHTS];
	vec4  sLight_ambient[MAX_S_LIGHTS];
	vec4  sLight_diffuse[MAX_S_LIGHTS];
	vec4  sLight_specular[MAX_S_LIGHTS];
	vec3  sLight_attenuation[MAX_S_LIGHTS];
	float sLight_cutoff[MAX_S_LIGHTS];
	float sLight_exponent[MAX_S_LIGHTS];
	mat4  sLight_mat[MAX_S_LIGHTS];
};

out Vertex 
{
	vec2  texcoord;
	vec3  normal;
	vec3  viewDir;

	vec3  pLightDir[MAX_P_LIGHTS];
	float pDistance[MAX_P_LIGHTS];
	vec4  pSmcoord [MAX_P_LIGHTS];

	vec3  dLightDir[MAX_D_LIGHTS];
	float dDistance[MAX_D_LIGHTS];
	vec4  dSmcoord [MAX_D_LIGHTS];

	vec3  sLightDir[MAX_S_LIGHTS];
	float sDistance[MAX_S_LIGHTS];
	vec4  sSmcoord [MAX_S_LIGHTS];
	vec4  qsSmcoord [MAX_S_LIGHTS];
} Vert;

void main(void)
{
	vec4 vertex   = transform.model * vec4(position, 1.0);

	Vert.texcoord = texcoord;
	Vert.normal   = transform.normal * normal;
	Vert.viewDir  = transform.viewPosition - vec3(vertex);

	for (int i = 0; i < MAX_P_LIGHTS; i++)
	{
		if (i > pLight_num - 1)
			break;
		vec4 lightDir = pLight_position[i] - vertex;
		Vert.pLightDir[i] = vec3(lightDir);
		Vert.pDistance[i] = length(lightDir);
	}
	Vert.pSmcoord[0] = pLight_mat[0] * vertex;

	for (int i = 0; i < MAX_D_LIGHTS; i++)
	{
		if (i > dLight_num - 1)
			break;
		vec4 lightDir = dLight_direction[i];
		Vert.dLightDir[i] = vec3(lightDir);
		Vert.dDistance[i] = length(lightDir);
	}
	Vert.dSmcoord[0] = dLight_mat[0] * vertex;

	for (int i = 0; i < MAX_S_LIGHTS; i++)
	{
		if (i > sLight_num - 1)
			break;
		vec4 lightDir = sLight_position[i] - vertex;
		Vert.sLightDir[i] = vec3(lightDir);
		Vert.sDistance[i] = length(lightDir);
	}
	Vert.sSmcoord[0] = sLight_mat[0] * vertex;

	gl_Position = transform.viewProjection * vertex;
}
~