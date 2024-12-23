#version 430 core
#define M_PI 3.14159265359
out vec4 FragColor;
in vec2 TexCoords;


uniform sampler2D DepthImage;
uniform sampler2D albedo;
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D shadowMap;

uniform mat4 LightMatrix;
uniform vec3 LightForward;
uniform vec3 LightPos;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 camPos;

float CalculateShadow(vec4 positionWorld,vec3 Normal,bool filtered)
{
    Normal = normalize(Normal);
    vec4 positionLightSpace = LightMatrix * positionWorld;
    vec3 positionLight3 = positionLightSpace.xyz / positionLightSpace.w; 
    positionLight3 = positionLight3* .5 + .5;

    float closestDepth = texture(shadowMap, positionLight3.xy).r; 
    
    float currentDepth = positionLight3.z;
    
    
    vec3 lightVec = normalize(LightPos - positionWorld.xyz);
    
 
float shadow = 0.0;
if(filtered){
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(LightPos - positionWorld.xyz);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
      
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, positionLight3.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
}else
//    {
//        shadow
//    }
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(positionLight3.z > 1.0)
        shadow = 0.0;
        
    return shadow;
    
}

float Shlick(float k, float costh)
{
    return (1.0 - k * k) / (4.0 * M_PI * pow(1.0 + k * costh, 2.0));
}
float computeScattering(vec3 worldPosition)
{
    //vec3 viewPos = viewMatrix[3].xyz;
    //vec3 lightPos = LightMatrix[3].xyz;
    float g = .5f;
    float costh = dot(normalize(worldPosition -LightPos), normalize(worldPosition - camPos));
    //return (1.0 - k * k) / (4.0 * M_PI * pow(1.0 + k * costh, 2.0));
    
    return (1.0 - g * g) / (4.0 * M_PI * pow(1.0 + g * g - 2.0 * g * costh, 3.0/2.0));
  
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
        if(CalculateShadow(vec4(pointWorld,1),texture(normal,TexCoords).rgb,true)>.01f)
            value += computeScattering(pointWorld);

    }

    return value;
}


vec3 CalculateLight(vec3 fragPos, vec3 normal, vec3 lightPos)
{
    vec3 lightDir = normalize(fragPos - lightPos);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * vec3(0.9f,0.9f,0.9f);

    return diffuse;
}
void main()
{
	// Shadow map debug
//	float depthValue = texture(DepthImage, TexCoords).r;
//	FragColor = vec4(vec3(depthValue),1);



    vec4 position = texture(position,TexCoords);
    vec3 _normal = texture(normal,TexCoords).rgb;
	vec4 col = texture(albedo,TexCoords);
	col = (1-CalculateShadow(position,_normal,true)) * col;
    float marchVal = March(TexCoords,60,10);
        
    
//    col.x = _normal.x;
//    col.y = _normal.y;
//    col.z = _normal.z;
    if(marchVal < .7f)
    {
        
        col += vec4(.4f - 1.5f*marchVal,.4f -1.5f*marchVal,.4f - 1.5f* marchVal,1.5f*marchVal);
    }
    
	FragColor = col; //* vec4(CalculateLight(position.rgb,_normal,LightPos.rgb),1);
}