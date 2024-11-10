#version 430 core

layout (location = 0) in vec3 position;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;



void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(aPos.xyz,1);
}