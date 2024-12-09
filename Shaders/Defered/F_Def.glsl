#version 430 core

in vec4 WorldPos;
in vec3 norm;
in vec2 texCoord;

layout (location = 0) out vec4 position;
layout (location = 1) out vec3 Normals;
layout (location = 2) out vec4 Albedo;


uniform vec4 BaseColor;
void main()
{
position = WorldPos;
Normals = (norm);
Albedo  = BaseColor;
}