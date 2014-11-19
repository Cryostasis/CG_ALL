#version 330 core
#extension GL_ARB_explicit_uniform_location: enable

#define FRAG_OUTPUT0 0
#define USE_TEX 5
#define LINE_COLOR 6

layout(location = FRAG_OUTPUT0)		out vec4 color;
layout(location = USE_TEX)			uniform int use_tex;
layout(location = LINE_COLOR)		uniform vec3 ln_color;

void main(void)
{
	if (use_tex == 0)
		color = vec4(ln_color, 1.0);
	else
		color = vec4(1.0, 1.0, 1.0, 1.0);
}
~