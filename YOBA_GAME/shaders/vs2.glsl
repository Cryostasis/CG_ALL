#version 330 core

uniform mat4 resultMatrix;

in vec3 position;

out vec2 fragTexcoord;

void main(void)
{
	gl_Position   = resultMatrix * vec4(position, 1.0);
	fragTexcoord = texcoord;
}
~