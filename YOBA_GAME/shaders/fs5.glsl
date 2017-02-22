#version 330 core
//#extension GL_EXT_texture_array: enable
#extension GL_ARB_explicit_uniform_location: enable

#define FRAG_OUTPUT0 0
#define USE_TEX 5
#define USE_NM	6
#define USE_PM	7

#define MAX_P_LIGHTS 3
#define MAX_D_LIGHTS 3
#define MAX_S_LIGHTS 3

#define NORMAL_MAPPING_DEPTH 0.5

layout(location = USE_TEX) uniform int use_tex;
layout(location = USE_NM) uniform int useNormalMapping;
layout(location = USE_PM) uniform int useParallaxMapping;

uniform samplerCube pLight_depth_tex;
uniform sampler2D dLight_depth_tex;
uniform sampler2D sLight_depth_tex;

//uniform sampler2DArrayShadow    pLight_depth_tex;
//uniform sampler2DArrayShadow    dLight_depth_tex;
//uniform sampler2DArrayShadow    sLight_depth_tex;

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

uniform struct Material
{
	sampler2D texture;
	sampler2D tex_n;
	sampler2D tex_s;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
} material;

in Vertex 
{
	vec2  texcoord;

	vec3  viewTangent;
	vec3  lightTangent;
	vec3  fragTangent;
	mat3  TBN;

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
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

bool calc_cube_shadow(int ind)
{
    float SampledDistance = texture(pLight_depth_tex, Vert.pLightDir[ind]).r;

    float Distance = length(Vert.pLightDir[ind]);

	/*if (SampledDistance == 1)
	{
		color = vec4(1);
		return true;
	}
	else
		return false;*/

    if (Distance <= SampledDistance + 0.0005)
        return false;
    else
        return true; 
}

#define HEIGHT_SCALE 0.1

vec2 ParallaxMapping1(vec2 texCoords, vec3 viewDir)
{ 
	//vec3 h_map = texture(material.tex_s, texCoords).xyz;
	//float height = (h_map.x + h_map.y + h_map.z) / 3.0;    
    
	
	float height = texture(material.tex_s, texCoords).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * HEIGHT_SCALE);
    return texCoords - p;    
} 

vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir)
{
	const float numLayers = 20;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * HEIGHT_SCALE; 
    vec2 deltaTexCoords = P / numLayers;

	vec2  currentTexCoords     = texCoords;
	vec3 h_map = texture(material.tex_s, currentTexCoords).xyz;
	float currentDepthMapValue = (h_map.x + h_map.y + h_map.z) / 3.0; 	
	//float currentDepthMapValue = texture(material.tex_s, currentTexCoords).r;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		h_map = texture(material.tex_s, currentTexCoords).xyz;
		currentDepthMapValue = (h_map.x + h_map.y + h_map.z) / 3.0; 
		//currentDepthMapValue = texture(material.tex_s, currentTexCoords).r;  
		currentLayerDepth += layerDepth;  
	}

	return currentTexCoords;
}

vec2 ParallaxMapping3(vec2 texCoords, vec3 viewDir)
{
	const float numLayers = 20;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * HEIGHT_SCALE; 
    vec2 deltaTexCoords = P / numLayers;

	vec2  currentTexCoords     = texCoords;
	vec3 h_map = texture(material.tex_s, currentTexCoords).xyz;
	float currentDepthMapValue = (h_map.x + h_map.y + h_map.z) / 3.0; 	
	//float currentDepthMapValue = texture(material.tex_s, currentTexCoords).r;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		h_map = texture(material.tex_s, currentTexCoords).xyz;
		currentDepthMapValue = (h_map.x + h_map.y + h_map.z) / 3.0; 
		//currentDepthMapValue = texture(material.tex_s, currentTexCoords).r;  
		currentLayerDepth += layerDepth;  
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	h_map = texture(material.tex_s, prevTexCoords).xyz;
	float beforeDepth = (h_map.x + h_map.y + h_map.z) / 3.0 - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

void main(void)
{
	vec3 normal;
	if (useNormalMapping == 1)
	{
		normal = texture(material.tex_n, Vert.texcoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(normal + vec3(0.0, 0.0, 1.0));
		normal = normalize(transpose(Vert.TBN) * normal);
	}
	else
		normal   = normalize(Vert.normal);

	vec3 viewDir  = normalize(Vert.viewDir);
	color = material.emission;

	const float bias = 0.0005;

	for (int i = 0; i < pLight_num; i++)
	{
		//if (calc_cube_shadow(i))
		//	return;
		//	continue;

		vec3 lightDir = normalize(Vert.pLightDir[i]);
		float attenuation = 1.0 / (pLight_attenuation[i].x +
			pLight_attenuation[i].y * Vert.pDistance[i] +
			pLight_attenuation[i].z * Vert.pDistance[i] * Vert.pDistance[i]);

		color += material.ambient * pLight_ambient[i] * attenuation;

		float NdotL = max(dot(normal, lightDir), 0.0);
		color += material.diffuse * pLight_diffuse[i] * NdotL * attenuation;

		float RdotVpow = max(pow(max(dot(reflect(-viewDir, normal), lightDir), 0.0), material.shininess), 0.0);
		color += material.specular * pLight_specular[i] * RdotVpow * attenuation;
	}
	for (int i = 0; i < dLight_num; i++)
	{
		if (texture(dLight_depth_tex, Vert.dSmcoord[i].xy).z + bias <=  Vert.dSmcoord[i].z)
			continue;
		vec3 lightDir = normalize(Vert.dLightDir[i]);
		color += material.ambient * dLight_ambient[i];

		float NdotL = max(dot(normal, lightDir), 0.0);
		color += material.diffuse * dLight_diffuse[i] * NdotL;

		float RdotVpow = max(pow(max(dot(reflect(-viewDir, normal), lightDir), 0.0), material.shininess), 0.0);
		color += material.specular * dLight_specular[i] * RdotVpow;
	}
	for (int i = 0; i < sLight_num; i++)
	{
		if (texture(sLight_depth_tex, (Vert.sSmcoord[i].xy / Vert.sSmcoord[i].w) ).z  
			<  (Vert.sSmcoord[i].z - bias) / Vert.sSmcoord[i].w )
			continue;

		/*if (textureProj(sLight_depth_tex, Vert.sSmcoord[i].xyw).z  <
			(Vert.sSmcoord[i].z - bias) / Vert.sSmcoord[i].w)
			continue;*/

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

		float RdotVpow = max(pow(max(dot(reflect(-viewDir, normal), lightDir), 0.0), material.shininess), 0.0);
		color += material.specular * sLight_specular[i] * RdotVpow * attenuation;
	}

	vec3 viewDir_PM   = normalize(Vert.viewTangent - Vert.fragTangent);
    vec2 texCoords = ParallaxMapping3(Vert.texcoord, viewDir_PM);
	
	if (use_tex == 1)
		color *= vec4(1.0, 1.0, 1.0, 1.0);
	else
		color *= texture(material.texture, texCoords);
}
~