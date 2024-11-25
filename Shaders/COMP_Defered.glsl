#version 430 core


layout (local_size_x = 1, local_size_y =1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D result;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;
uniform sampler2D depth;

uniform sampler2D shadowMap;
uniform mat4 LightMatrix;


ivec2  value;




float CalculateShadow(vec4 positionWorld,vec3 Normal)
{
    vec4 positionLightSpace = LightMatrix * positionWorld;
    vec3 positionLight3 = positionLightSpace.xyz / positionLightSpace.w; 
    positionLight3 = positionLight3* .5 + .5;

    float closestDepth = texture(shadowMap, positionLight3.xy).r; 
    
    float currentDepth = positionLight3.z;
    

    vec3 lightForward = normalize(vec3(LightMatrix[2][0], LightMatrix[2][1], LightMatrix[2][2]));
    float bias = max(0.05 * (1.0 - dot(Normal, lightForward)), 0.0005);

    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, positionLight3.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(positionLight3.z > 1.0)
        shadow = 0.0;

    return shadow;
    
}


float isInVolume(vec2 coord)
{
    return 0;
}
void main()
{
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 coordF ;
    coordF.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    coordF.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	vec4 col = texture(albedo, coordF);
    vec4 fragPosition = texture(position,coordF);

    col = col * (1-CalculateShadow(fragPosition,texture(normal,coordF).xyz));
    //col = col * 0.5f;
        
    imageStore(result, coord, col);
}

