#version 430 core

#define NUM_STEPS 40  // Increased for smoother sampling
#define MAX_DIST 25
#define M_PI 3.14159265359
#define LOCAL_X 8
#define LOCAL_Y 8

layout(local_size_x = LOCAL_X, local_size_y = LOCAL_Y, local_size_z = 1) in;

uniform float time;

uniform vec3 camPosition;
uniform vec3 lightPosition;
uniform mat4 projectionMat;
uniform mat4 viewMatrix;
uniform mat4 lightMatrix;

uniform sampler2D inPosition;
uniform sampler2D inNormal;
uniform sampler2D inDepth;

layout (rgba32f, binding = 0) uniform image2D VolumetricResult;

// Enhanced random function for dithering
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Improved Henyey-Greenstein phase function
float HenyeyGreinstein(vec3 worldPosition)
{
    float g = 0.2f;  // Scattering asymmetry parameter
    vec3 lightDir = normalize(lightPosition - worldPosition);
    vec3 viewDir = normalize(camPosition - worldPosition);
    float costh = dot(lightDir, viewDir);
    
    return (1.0 - g * g) / (4.0 * M_PI * pow(1.0 + g * g - 2.0 * g * costh, 3.0/2.0));
}

// Soft shadow calculation with smoothstep
float CalculateShadow(vec4 positionWorld, vec3 Normal, bool filtered)
{
    vec4 positionLightSpace = lightMatrix * positionWorld;
    vec3 positionLight3 = positionLightSpace.xyz / positionLightSpace.w; 
    positionLight3 = positionLight3 * 0.5 + 0.5;

    float closestDepth = texture(inDepth, positionLight3.xy).r; 
    float currentDepth = positionLight3.z;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - positionWorld.xyz);
    
    // Adaptive bias based on light angle
    float bias = mix(0.005f, 0.0f, dot(normal, -lightDir));
    
    // Percentage-closer filtering (PCF)
    float shadow = 0.0;
    if(filtered){
        vec2 texelSize = 1.0 / textureSize(inDepth, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(inDepth, positionLight3.xy + vec2(x, y) * texelSize).r; 
                shadow += smoothstep(0.0, 1.0, currentDepth - bias > pcfDepth ? 1.0 : 0.0);        
            }    
        }
        shadow /= 9.0;
    }
    
    // Clip shadows beyond light frustum
    if(positionLight3.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

// Screen to world space conversion
vec4 ScreenToWorld(vec3 imgSpace)
{
    vec4 ndc; 
    ndc.x = (imgSpace.x * 2) - 1;
    ndc.y = (imgSpace.y * 2) - 1;
    ndc.z = imgSpace.z;
    ndc.w = 1;
    
    vec4 view = inverse(projectionMat) * ndc;
    view = view / view.w;

    vec4 world = (inverse(viewMatrix) * view);
    
    return world;
}

// Improved Volumetric Raymarching
float March(vec2 point, vec3 Normal, int marchSteps, float maxMarchDistance)
{
    vec3 start = camPosition;
    vec3 end = (ScreenToWorld(vec3(point, 1f))).xyz;

    float rayLength = length(end - start)*2;
    float marchUnit = rayLength / float(marchSteps);
    vec3 direction = normalize(end - start);
    
    // Get potential early exit point
    vec3 endEarlyPos = texture(inPosition, point).xyz;
    float fragLengthInDir = dot(direction, endEarlyPos);

    // Accumulation with improved sampling
    float accumulatedValue = 0.0;
    float accumulationFactor = 1.0 / float(marchSteps);

    for(int i = 0; i < marchSteps; i++)
    {
        // Add randomized offset to break up uniform sampling
        float randomOffset = random(vec2(point.x * i, point.y * i)) * marchUnit;
        vec3 pointWorld = start + (direction * marchUnit * i) + (direction * randomOffset);
        
        // Early exit if we've passed the fragment
        float currentValDot = dot(direction, pointWorld);
        if(currentValDot > fragLengthInDir)
            break;

        // Soft shadow and scattering calculation
        float shadowFactor = CalculateShadow(vec4(pointWorld, 1), Normal, true);
        if(shadowFactor > 0.001f)
        {
            float scatterValue = HenyeyGreinstein(pointWorld);
            accumulatedValue += scatterValue *4;///** accumulationFactor*/ * (1.0 - shadowFactor);
        }
    }

    return accumulatedValue;
}

void main()
{
    // Screen and work group setup
    uint SCR_WIDTH = gl_NumWorkGroups.x * LOCAL_X;
    uint SCR_HEIGHT = gl_NumWorkGroups.y * LOCAL_Y;
    
    uint GlobalX = (gl_WorkGroupID.x * LOCAL_X) + (gl_LocalInvocationID.x % LOCAL_X);
    uint GlobalY = (gl_WorkGroupID.y * LOCAL_Y) + (gl_LocalInvocationID.y % LOCAL_Y);

    ivec2 ItexCoord = ivec2(GlobalX, GlobalY);
    vec2 texCoord = vec2(
        float(ItexCoord.x) / float(SCR_WIDTH),
        float(ItexCoord.y) / float(SCR_HEIGHT)
    );
	
    // Sample world position and normal
    vec4 position_world = texture(inPosition, texCoord);
    vec3 normal = texture(inNormal, texCoord).xyz;

    // Compute volumetric effect
    float result = March(texCoord, normal, NUM_STEPS, MAX_DIST);
	
    // Color accumulation
    vec4 color = vec4(result, result, result, result);

    // Store result
    imageStore(VolumetricResult, ivec2(GlobalX, GlobalY), color);
}
