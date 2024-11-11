#version 430 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 TexCoords;
uniform mat4 model;
uniform mat4 viewProj;
uniform mat4 lightSpaceMatrix;
out V_OUT
{
	vec3 fragPos;
	vec3 fragNormal;
	vec2 fragTexCoord;
	vec4 fragPosLightSpace;
}v_out;

void main()
{
	v_out.fragPos = model * vec4(vPos,1);
	v_out.fragNormal = transpose(inverse(mat3(model))) * vNormal;
	v_out.fragPosLightSpace = lightSpaceMatrix * vec4(v_out.fragPos,1);
	fragTexCoord = texCoords;
	gl_Position = viewProj * model * vec4(vPos,1);
}