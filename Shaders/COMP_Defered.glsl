#version 430 core


layout (local_size_x = 1, local_size_y =1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D result;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;
uniform sampler2D depth;

uniform sampler2D shadowMap;
uniform mat4 LightMatrix;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 camPos;

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

vec3 ScreenToWorldRay(vec3 screenSpacePos)
{
    // Step 1: Convert from screen space to normalized device coordinates (NDC)
    float x_ndc = (2.0f * screenSpacePos.x) - 1.0f;
    float y_ndc = 1.0f - (2.0f * screenSpacePos.y) ;
    float z_ndc = screenSpacePos.z;
    vec4 clipSpacePos = vec4(x_ndc, y_ndc, 1, 1.0f);

    mat4 toWorld = inverse(projectionMatrix * viewMatrix);
    vec4 from = toWorld * vec4(clipSpacePos.xy, -1.0f, 1.0f);
    vec4 to = toWorld * vec4(clipSpacePos.xy, 1.0f, 1.0f);

    from /= from.w;
    to /= to.w; 
    return vec3(to - from);
}
vec4 ScreenToWorld(vec3 point)
{
    float ndc_x = (point.x * 2) -1;
    float ndc_y = 1 - (point.y * 2) ;
    float ndc_z = point.z;

    mat4 toWorld = inverse(projectionMatrix* viewMatrix);
    vec4 worldPoint = toWorld * vec4(ndc_x,ndc_y,ndc_z,1);
    
    return worldPoint;
}
float March(vec2 point,float marchSteps,float maxMarchDistance)
{
    float marchUnit = maxMarchDistance/marchSteps;
    vec3 start = (ScreenToWorld(vec3(point,-1))).xyz;
    vec3 end = camPos;//(ScreenToWorld(vec3(point,1))).xyz;

    vec3 direction = normalize(end - start);
    

    vec3 pointWorld = start;
    float value = 0;
    for(int i=0; i<marchSteps; i++)
    {
        pointWorld = start + (direction * marchUnit * i);
        if(CalculateShadow(vec4(pointWorld,1),vec3(1))>.7)
            value += .01f;

    }

    return value;
}

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 coordF ;
    coordF.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    coordF.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	vec4 col =  texture(albedo, coordF);
    vec4 fragPosition = texture(position,coordF);

    col = col * (1-CalculateShadow(fragPosition,texture(normal,coordF).xyz));
    float marchVal = (March(coordF,5,30));
    if(marchVal > .1f)
    {
        col = /*col +*/ vec4(1,0,0,1);
    }
//    float dist = distance((ScreenToWorld(vec3(coordF,1))).xyz,vec3(1));
//    if(dist < 1.5)
//        col += vec4(.5,0,0,.1);
    imageStore(result, coord, col);
}

