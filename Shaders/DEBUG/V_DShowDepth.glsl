#version 430 core

out vec2 TexCoords;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoords;



void main()
{
	gl_Position = vec4(vPos.xyz,1);
	TexCoords = vTexCoords;
}