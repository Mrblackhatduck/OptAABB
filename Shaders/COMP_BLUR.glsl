
#version 430 core
#define LOCAL_X 16
#define LOCAL_Y 16
layout(local_size_x = LOCAL_X, local_size_y = LOCAL_Y, local_size_z = 1) in;


layout(rgba32f,binding = 0) uniform image2D inputImage;
layout(rgba32f, binding = 1) uniform image2D outputImage;



void main() {
  
    ivec2 pixelCoords;
    uint x = (gl_GlobalInvocationID.x * LOCAL_X) + (gl_LocalInvocationID.x) % LOCAL_X;
    uint y = (gl_GlobalInvocationID.y * LOCAL_Y) + (gl_LocalInvocationID.y) % LOCAL_Y;
    pixelCoords = ivec2(x,y);
    
    imageStore(outputImage, pixelCoords, vec4(1,1,1,1));
}
