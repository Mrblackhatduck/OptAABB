#version 430 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D DepthImage;


uniform sampler2D albedo;
uniform sampler2D position;
uniform sampler2D shadowMap;
uniform sampler2D normal;

uniform mat4 LightMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 camPos;

float CalculateShadow(vec4 positionWorld,vec3 Normal)
{
    vec4 positionLightSpace = LightMatrix * positionWorld;
    vec3 positionLight3 = positionLightSpace.xyz / positionLightSpace.w; 
    positionLight3 = positionLight3* .5 + .5;

    float closestDepth = texture(shadowMap, positionLight3.xy).r; 
    
    float currentDepth = positionLight3.z;
    

    vec3 lightForward = normalize(vec3(LightMatrix[2][0], LightMatrix[2][1], LightMatrix[2][2]));
    
    float bias = max(0.00004 * (1.0 - dot(Normal, lightForward)), 0.00004);
    //float bias = .0002f;

    
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

vec4 ScreenToWorld(vec3 point)
{
    float ndc_x = (point.x * 2) -1;
    float ndc_y = (point.y * 2) -1;
    float ndc_z = point.z;

    vec4 NDC = inverse(projectionMatrix)* vec4(ndc_x,ndc_y,ndc_z,1);
    NDC = NDC/NDC.w;
    vec4 worldPoint = inverse(viewMatrix) * NDC;
    
    return worldPoint;
}
float March(vec2 point,float marchSteps,float maxMarchDistance)
{
    float marchUnit = maxMarchDistance/marchSteps;
    vec3 start = (ScreenToWorld(vec3(point,0))).xyz;
    vec3 end = (ScreenToWorld(vec3(point,1))).xyz;

    vec3 direction = normalize(end - start);
    

    vec3 pointWorld = start;
    float value = 0;
    for(int i=0; i<marchSteps; i++)
    {
        pointWorld = start + (direction * marchUnit * i);
        if(CalculateShadow(vec4(pointWorld,1),vec3(1.0))>.1)
            value += .05f;

    }

    return value;
}



void main()
{
	// Shadow map debug
//	float depthValue = texture(DepthImage, TexCoords).r;
//	FragColor = vec4(vec3(depthValue),1);
    vec4 position = texture(position,TexCoords);
    vec3 normal = texture(normal,TexCoords).rgb;
	vec4 col = texture(albedo,TexCoords);
	col = (1-CalculateShadow(position,normal)) * col;
    float marchVal = March(TexCoords,30,20);
    vec3 texCoordNdc = vec3(TexCoords.x - .5);
    float dist = distance (ScreenToWorld(vec3(TexCoords,.3f)).xyz,position.xyz);
    if(marchVal < .1f)
    {
        col += vec4(1,0,0,1);
    }
    
	FragColor = col;
}