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
   // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(Depthmap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
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
    // calculate shadow
    float shadow = calculateShadow(v_in.fragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
    //FragColor = ((1.0 - shadow) /** (diffuse)*/) * color;
    //FragColor = vec4(1.0f,1.0f,5.0f,1.0f);
}