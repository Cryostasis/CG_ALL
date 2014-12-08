#version 330 core
#extension GL_ARB_explicit_uniform_location: enable

#define P_LIGHT_POS 8

in vec3 WorldPos;

layout(location = P_LIGHT_POS) uniform vec3 pLight_pos;
          
out float FragColor;
        
void main()
{
    vec3 LightToVertex = WorldPos - pLight_pos;
    float LightToPixelDistance = length(LightToVertex);
    FragColor = LightToPixelDistance;
}
~