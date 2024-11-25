#version 430 core


layout (local_size_x = 1, local_size_y =1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D result;

uniform sampler2D albedo;
uniform sampler2D position;
uniform sampler2D depth;

ivec2  value;

void main()
{
    vec4 valuex = vec4(0.0, 0.0, 0.0, 1.0);
    
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 coordF ;
    coordF.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    coordF.y = float(texelCoord.y)/(gl_NumWorkGroups.y);

        
	vec4 col = texture(albedo, coordF);
    imageStore(result, coord, col);

	//imageStore(result,texelCoord, texture(albedo,texelCoord));
}

