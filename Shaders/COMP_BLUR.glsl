
#version 430 core
#define LOCAL_X 8
#define LOCAL_Y 8

layout(local_size_x = LOCAL_X, local_size_y =LOCAL_Y, local_size_z = 1) in;


/*layout(rgba32f, binding = 0)*/ uniform sampler2D inputImage;
layout(rgba32f, binding = 1) uniform image2D outputImage;

///----------------

 //vec2 pos;                    // screen position <-1,+1>
//out vec4 gl_FragColor;          // fragment output color
//uniform sampler2D txr;          // texture to blur
float xs,ys;            // texture resolution
 float r;                // blur radius
 int axis;




void main() {
    r = 10.0f;
    uint SCR_WIDTH = gl_NumWorkGroups.x * LOCAL_X;
    uint SCR_HEIGHT = gl_NumWorkGroups.y * LOCAL_Y;
    
    
    ivec2 pixelCoords;
    uint X = (gl_WorkGroupID.x * LOCAL_X) + (gl_LocalInvocationID.x % LOCAL_X);
    uint Y = (gl_WorkGroupID.y * LOCAL_Y) + ((gl_LocalInvocationID.y) % LOCAL_Y);
    pixelCoords = ivec2(X,Y);
    ivec2 screenRes = ivec2(gl_NumWorkGroups.x * LOCAL_X, gl_NumWorkGroups.y* LOCAL_Y);
    float u =  float(pixelCoords.x) / float(screenRes.x);
    float v =  float(pixelCoords.y) / float(screenRes.y);

    
    vec2 UV  =vec2(u, v);
  //-------------------------

    xs = float (screenRes.x);
    ys = float (screenRes.y);
    
    float x,y,rr=r*r,d,w,w0;
    vec2 p= UV;
    vec4 col=vec4(0.0,0.0,0.0,0.0);
    
    for(axis = 0; axis < 2; axis ++){
    w0=0.5135/pow(r,0.96);
    if (axis==0) 
        for (d=1.0/xs,x=-r,p.x+=x*d;x<=r;x++,p.x+=d)
        { 
            w=w0*exp((-x*x)/(2.0*rr));
            col+= texture(inputImage,p) * w;
        }
    if (axis==1) 
        for (d=1.0/ys,y=-r,p.y+=y*d;y<=r;y++,p.y+=d)
        { 
            w=w0*exp((-y*y)/(2.0*rr)); 
            col+=texture(inputImage,p)*w; 
            }
            }




        imageStore(outputImage, pixelCoords, col);

}