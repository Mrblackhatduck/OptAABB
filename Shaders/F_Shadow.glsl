#version 430 core
out vec4 FragColor;
in V_OUT
{
    vec4 fragPos;
	vec3 fragNormal;
	vec2 fragTexCoord;
	vec4 fragPosLightSpace;
}v_in;

uniform sampler2D Depthmap;
uniform vec3 lightPos;
uniform vec3 viewPos;

float calculateShadow(vec4 fragPosLightSpace)
{
    // do perspective division
    vec3 projectCoords = (fragPosLightSpace.xyz)  / fragPosLightSpace.z;
    //transform values from [-1,1] to [0,1] like the depth value
    projectCoords = (projectCoords * .5f) + .5f;
    float closestDepth = texture(Depthmap,projectCoords.xy).r;
    float currentDepth = projectCoords.z;
float value = currentDepth > closestDepth? 1.0f : 0.0f;
return value;

}
float calculateLight(vec4 fragPosLightSpace)
{
    
        
return 0;
}
void main()
{
    vec3 color = {0.0f,0.0f,1.0f};
	vec3 fNormal = normalize(v_in.fragNormal);
    vec3 lightDir =normalize(v_in.fragPos.xyz - lightPos);
    float diffuse = max(dot(lightDir,fNormal),0);
    vec3 diffuseCol = color * diffuse;

    float shadow = calculateShadow(v_in.fragPosLightSpace);

    //FragColor = ((1.0 - shadow) * (diffuse)) * color;
    FragColor = vec4(1.0f,1.0f,0.0f,1.0f);
}