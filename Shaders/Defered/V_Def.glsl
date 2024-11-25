#version 430 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 pos;
out vec3 norm;
out vec2 texCoord;

void main()
{
	pos = proj * view * model * vec4(vPos,1);
	gl_Position = pos;
	norm = transpose(inverse(mat3(model))) * vNormal;
	texCoord = TexCoords;
}