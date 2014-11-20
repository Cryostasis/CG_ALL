#version 330 core

#define VERT_POSITION 0

layout(location = VERT_POSITION) in vec3 position;

uniform struct Transform
{
	mat4 modelViewProjection;
} transform;

void main(void)
{
	gl_Position = transform.modelViewProjection * vec4(position, 1.0);
}
~