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
	vec3 fragPos;
	vec3 fragNormal;
	vec2 fragTexCoord;
	vec4 fragPosLightSpace;
}v_out;

void main()
{
/*
	 vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
*/
	v_out.fragPos = vec3(model * vec4(vPos,1));
	v_out.fragNormal = transpose(inverse(mat3(model))) * vNormal;
	v_out.fragTexCoord = TexCoords;
	v_out.fragPosLightSpace = lightSpaceMatrix  * vec4(v_out.fragPos,1);
	gl_Position = proj *view * model * vec4(vPos,1.0);
}