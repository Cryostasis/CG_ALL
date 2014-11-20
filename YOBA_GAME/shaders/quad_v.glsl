#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_NORMAL   2
layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;

uniform struct Transform
{
	mat4 modelViewProjection;
} transform;

out Vertex
{
	vec2 texcoord;
} Vert;

void main(void)
{
	Vert.texcoord = texcoord;

	gl_Position = transform.modelViewProjection * vec4(position, 1.0);
}
~