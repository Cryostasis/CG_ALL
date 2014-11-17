#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_NORMAL   2

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

uniform struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
} light;

out Vertex 
{
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir;
	vec3  viewDir;
	float distance;
} Vert;

void main(void)
{
	vec4 vertex   = transform.model * vec4(position, 1.0);

	vec4 lightDir = light.position - vertex;

	Vert.texcoord = texcoord;
	Vert.normal   = transform.normal * normal;
	Vert.lightDir = vec3(lightDir);
	Vert.viewDir  = transform.viewPosition - vec3(vertex);
	Vert.distance = length(lightDir);

	gl_Position = transform.viewProjection * vertex;
}
~