#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_NORMAL   2

#define FRAG_OUTPUT0 0

uniform struct Material
{
	sampler2D texture;
} material;


in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	color = texture(material.texture, Vert.texcoord);
}
~