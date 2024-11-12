#version 430 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
out V_OUT
{
	vec4 fragPos;
	vec3 fragNormal;
	vec2 fragTexCoord;
	vec4 fragPosLightSpace;
}v_out;

void main()
{
	v_out.fragPos = model * vec4(vPos.xyz,1);
	v_out.fragNormal = transpose(inverse(mat3(model))) * vNormal;
	v_out.fragTexCoord = TexCoords;
	v_out.fragPosLightSpace = lightSpaceMatrix * vec4(vPos.xyz,1);
	gl_Position = proj *view * model * vec4(vPos.xyz,1);
}