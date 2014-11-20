#version 330 core
#extension GL_ARB_explicit_uniform_location: enable

#define FRAG_OUTPUT0	0
#define USE_TEX			5
#define LINE_COLOR		6
#define FONT_TEX		7

layout(location = FRAG_OUTPUT0)		out vec4 color;
layout(location = USE_TEX)			uniform int use_tex;
layout(location = LINE_COLOR)		uniform vec4 ln_color;
layout(location = FONT_TEX)		    uniform sampler2D tex;

in Vert 
{	
	vec2 texcrd;
};

void main(void)
{
	color = ln_color * texture(tex, texcrd);
}
~