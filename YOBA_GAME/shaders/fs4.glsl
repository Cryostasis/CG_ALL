#version 430 core

#define FRAG_OUTPUT0 0

#define MAX_P_LIGHTS 3
#define MAX_D_LIGHTS 3
#define MAX_S_LIGHTS 3

uniform int use_tex;

uniform pLight
{
	int  pLight_num;
	vec4 pLight_position[MAX_P_LIGHTS];
	vec4 pLight_ambient[MAX_P_LIGHTS];
	vec4 pLight_diffuse[MAX_P_LIGHTS];
	vec4 pLight_specular[MAX_P_LIGHTS];
	vec3 pLight_attenuation[MAX_P_LIGHTS];
};

uniform dLight
{
	int  dLight_num;
	vec4 dLight_direction[MAX_D_LIGHTS];
	vec4 dLight_ambient[MAX_D_LIGHTS];
	vec4 dLight_diffuse[MAX_D_LIGHTS];
	vec4 dLight_specular[MAX_D_LIGHTS];
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
};

uniform struct Material
{
	sampler2D texture;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
} material;

in Vertex 
{
	vec2  texcoord;
	vec3  normal;
	vec3  viewDir;
	vec3  pLightDir[MAX_P_LIGHTS];
	float pDistance[MAX_P_LIGHTS];
	vec3  dLightDir[MAX_D_LIGHTS];
	float dDistance[MAX_D_LIGHTS];
	vec3  sLightDir[MAX_S_LIGHTS];
	float sDistance[MAX_S_LIGHTS];
	float sCutoff[MAX_S_LIGHTS];
	float sExponent[MAX_S_LIGHTS];
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	vec3 normal   = normalize(Vert.normal);
	vec3 viewDir  = normalize(Vert.viewDir);
	color = material.emission;

	for (int i = 0; i < pLight_num; i++)
	{
		vec3 lightDir = normalize(Vert.pLightDir[i]);
		float attenuation = 1.0 / (pLight_attenuation[i].x +
			pLight_attenuation[i].y * Vert.pDistance[i] +
			pLight_attenuation[i].z * Vert.pDistance[i] * Vert.pDistance[i]);

		color += material.ambient * pLight_ambient[i] * attenuation;

		float NdotL = max(dot(normal, lightDir), 0.0);
		color += material.diffuse * pLight_diffuse[i] * NdotL * attenuation;

		float RdotVpow = pow(max(dot(reflect(-viewDir, normal), lightDir), 0.0), material.shininess);
		color += material.specular * pLight_specular[i] * RdotVpow * attenuation;


	}
	for (int i = 0; i < dLight_num; i++)
	{
		vec3 lightDir = normalize(Vert.dLightDir[i]);

		color += material.ambient * dLight_ambient[i];

		float NdotL = max(dot(normal, lightDir), 0.0);
		color += material.diffuse * dLight_diffuse[i] * NdotL;

		float RdotVpow = pow(max(dot(reflect(-viewDir, normal), lightDir), 0.0), material.shininess);
		color += material.specular * dLight_specular[i] * RdotVpow;
	}
	for (int i = 0; i < sLight_num; i++)
	{
		vec3 lightDir = normalize(Vert.sLightDir[i]);

		float spotEffect = dot(normalize(vec3(sLight_direction[i])), -lightDir);
		float spot       = float(spotEffect > sLight_cutoff[i]);

		spotEffect = max(pow(spotEffect, sLight_exponent[i]), 0.0);

		float attenuation = spot * spotEffect / (sLight_attenuation[i].x +
			sLight_attenuation[i].y * Vert.sDistance[i] +
			sLight_attenuation[i].z * Vert.sDistance[i] * Vert.sDistance[i]);

		color += material.ambient * sLight_ambient[i] * attenuation;

		float NdotL = max(dot(normal, lightDir), 0.0);
		color += material.diffuse * sLight_diffuse[i] * NdotL * attenuation;

		float RdotVpow = pow(max(dot(reflect(-viewDir, normal), lightDir), 0.0), material.shininess);
		color += material.specular * sLight_specular[i] * RdotVpow * attenuation;
	}

	if (use_tex == 1)
		color *= vec4(0.5, 0.5, 0.5, 1.0) * 2;
	else
		color *= texture(material.texture, Vert.texcoord);
}
~