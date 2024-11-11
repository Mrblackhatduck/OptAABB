#version 430 core
out vec3 FragColor;
in V_OUT
{
    vec3 fragPos;
	vec3 fragNormal;
	vec2 fragTexCoord;
	vec4 fragPosLightSpace;
}v_in;

uniform sampler2D Depthmap;
uniform vec3 lightPos;
uniform vec3 viewPos;

float calculateShadow(){}
float calculateLight(mat4 fragPosLightSpace){}
void main()
{
    vec3 color = {0.0f,0.0f,1.0f};
	vec3 fNormal = normalize(v_in.fragNormal);
    vec3 lightDir =normalize(v_in.fragPos - lightPos);
    float diffuse = max(dot(lightDir,fNormal),0);
    float diffuseCol = color * diffuse;

    float shadow = calculateShadow(v_in.fragPosLightSpace);

    FragColor = ((1.0 - shadow) * (diffuse)) * color;
}