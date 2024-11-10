#version 430 core
layout (location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 viewProj;

void main()
{
	gl_Position = viewProj * model * vec4(vPos,1);
}