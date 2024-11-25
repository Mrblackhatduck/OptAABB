#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform sampler2D inDepth;
layout(rgba32f, binding = 0) uniform image2D imgOutput;


void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    value.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    value.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	value = vec4(5.0, 0.0, 0.0, 1.0);
    //value.r = 
//    if(texelCoord.x > .5f && texelCoord.y>.5f)
//        {
//        value.b = 1.0f;
//        }
//    if(texelCoord.x > .5f && texelCoord.y<.5f)
//        {
//        value.r = 1.0f;
//        }
//    if(texelCoord.x < .5f && texelCoord.y<.5f)
//        {
//        value.g = 1.0f;
//        }
    imageStore(imgOutput, texelCoord, value);
}