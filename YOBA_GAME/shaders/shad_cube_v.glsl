#version 330 core

#define VERT_POSITION 0

layout(location = VERT_POSITION) in vec3 position;

uniform struct Transform
{
	mat4 modelViewProjection;
	mat4 modelView;
} transform;

out vec3 WorldPos;

void main(void)
{
    vec4 vert = vec4(position, 1.0);
    gl_Position = transform.modelViewProjection * vert;
    WorldPos = (transform.modelView * vert).xyz;    
}
~