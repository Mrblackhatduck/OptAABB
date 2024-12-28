#version 430 core

#define NUM_STEPS 16
#define MAX_DIST 30 
#define M_PI 3.14159265359
layout(local_size_x = 1, local_size_y =1, local_size_z = 1) in;


uniform vec3 camPosition;
uniform vec3 lightPosition;
uniform mat4 projectionMat;
uniform mat4 viewMatrix;
uniform mat4 lightMatrix;

uniform sampler2D inPosition;
uniform sampler2D inNormal;
uniform sampler2D inDepth;

layout (rgba32f, binding =0) uniform image2D VolumetricResult;

float HenyeyGreinstein(vec3 worldPosition)
{
    float g = .2f;
    float costh = dot(normalize(worldPosition -lightPosition), normalize(worldPosition - camPosition));
    return (1.0 - g * g) / (4.0 * M_PI * pow(1.0 + g * g - 2.0 * g * costh, 3.0/2.0));
}

float CalculateShadow(vec4 positionWorld,vec3 Normal,bool filtered)
{
    //Normal = normalize(Normal);
    vec4 positionLightSpace = lightMatrix * positionWorld;
    
    vec3 positionLight3 = positionLightSpace.xyz / positionLightSpace.w; 
    // to ndc
    positionLight3 = positionLight3* .5 + .5;

    float closestDepth = texture(inDepth, positionLight3.xy).r; 
    
    float currentDepth = positionLight3.z;
    
    
    vec3 lightVec = normalize(vec3(lightMatrix[3]) - positionWorld.xyz);
    
 
float shadow = 0.0;
if(filtered){
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - positionWorld.xyz);
    //float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    float bias = mix(0.005f ,0.0f,dot(Normal, -lightDir));
      
    vec2 texelSize = 1.0 / textureSize(inDepth, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(inDepth, positionLight3.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
}

    if(positionLight3.z > 1.0)
        shadow = 0.0;
        
    return shadow;
    
}

vec4 ScreenToWorld(vec3 imgSpace)
{
	vec4 ndc; 
	ndc.x = (imgSpace.x*2) - 1;
	ndc.y = (imgSpace.y*2) - 1;
	ndc.z = imgSpace.z;
	ndc.w = 1;
	
	vec4 view = inverse(projectionMat) * ndc;
	view = view / view.w;

	vec4 world = (inverse(viewMatrix) * view);
	
	return vec4(world);
//    float ndc_x = (imgSpace.x * 2) -1;
//    float ndc_y = (imgSpace.y * 2) -1;
//    float ndc_z = imgSpace.z;
//
//    vec4 NDC = inverse(projectionMat)* vec4(ndc_x,ndc_y,ndc_z,1);
//    NDC = NDC/NDC.w;
//    vec4 worldPoint = inverse(viewMatrix) * NDC;
//    
//    return worldPoint;
}


float March(vec2 point,vec3 Normal,float marchSteps,float maxMarchDistance)
{
    vec3 start = (ScreenToWorld(vec3(point,0))).xyz;
    vec3 end = (ScreenToWorld(vec3(point,1))).xyz;
    //float marchUnit = maxMarchDistance/marchSteps;
    float marchUnit = length(vec3(end-start))/marchSteps;
    vec3 direction = normalize(end - start);
    

    vec3 pointWorld = start;
    float value = 0;
    for(int i=0; i<marchSteps; i++)
    {
        pointWorld = start + (direction * marchUnit * i);
        if(CalculateShadow(vec4(pointWorld,1),Normal,true)>.01f)
            value += HenyeyGreinstein(pointWorld);

    }

    return value;
}


void main()
{

	
	uint GlobalX = gl_GlobalInvocationID.x;
	uint GlobalY = gl_GlobalInvocationID.y;

    ivec2 ItexCoord = ivec2(GlobalX,GlobalY);
    vec2 texCoord = vec2(0);
    texCoord.x = float(ItexCoord.x) / float(gl_NumWorkGroups.x);
    texCoord.y = float(ItexCoord.y) / float(gl_NumWorkGroups.y);


	
	vec4 position_world = texture(inPosition,texCoord);
	vec3 normal = texture(inNormal, texCoord).xyz;
    float shadow = CalculateShadow(position_world,normal,true);

	//vec3 ViewDirection = normalize(start_position_world.xyz - camPosition);
    
	
    float result = March(texCoord,normal,16,10);
	
    vec4 color = vec4(0.0f);
    color = color * (1-shadow);
    if(result < .5f)
//    {
        
        //imageStore(VolumetricResult,ivec2(gl_GlobalInvocationID.xy),vec4*shadow);
        color += vec4(.5f+  result,.5f+ result,.5f + result,    result);
    //}
    
        imageStore(VolumetricResult,ivec2(gl_GlobalInvocationID.xy),color);
        memoryBarrierImage();
//    barrier();
//	
//    atomicAdd(pixelVal,int(start_position_world.x)*225);
//	
//	barrier();
//
//
//	if(gl_LocalInvocationID.z == 0)
//	{
//		//vec4 finalVal = vec4(val/225);
//		vec4 finalVal = vec4(pixelVal/225);
//        
//		imageStore(VolumetricResult,ivec2(gl_GlobalInvocationID.xy),finalVal);
//	}
}
