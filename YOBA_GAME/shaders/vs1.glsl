#version 330 core

uniform mat4 resultMatrix;

in vec3 position;
in vec3 color;

out vec3 fragmentColor;

void main(void)
{
	gl_Position   = resultMatrix * vec4(position, 1.0);
	fragmentColor = color;
}
~