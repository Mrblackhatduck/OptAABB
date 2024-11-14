#version 430 core
out vec4 FragColor;
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

float calculateShadow(vec4 fragPosLightSpace)
{
    // do perspective division
    vec3 projectCoords = (fragPosLightSpace.xyz)  / fragPosLightSpace.z;
    //transform values from [-1,1] to [0,1] like the depth value
    projectCoords = (projectCoords * .5f) + .5f;
    float currentDepth = projectCoords.z;
     vec3 normal = normalize(v_in.fragNormal);
    vec3 lightDir = normalize(lightPos - v_in.fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(Depthmap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(Depthmap, projectCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projectCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
float calculateLight(vec4 fragPosLightSpace)
{
    
        
return 0;
}
void main()
{
   
    vec3 color = {0.0f,0.0f,1.0f};
	vec3 normal = normalize(v_in.fragNormal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - v_in.fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - v_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    float shadow = calculateShadow(v_in.fragPosLightSpace);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    FragColor = vec4(lighting,1);
    //FragColor = ((1.0 - shadow) /** (diffuse)*/) * color;
    //FragColor = vec4(1.0f,1.0f,5.0f,1.0f);
}