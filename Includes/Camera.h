#ifndef CAMERA_H
#define CAMERA_H
#include <Types.h>
#include <glm/glm/gtc/matrix_transform.hpp>
class Camera
{
    public:
    int SCR_Height, SCR_Width;
    enum Projection{ortho, perspect};
    Projection Projection;
    vec3 Position;
    vec3 Forward;
    vec3 Up;
    mat4 Transform;

    Camera()
    :
    Position(vec3(0.0f,0.0f,0.0f)),
    Forward(vec3(0.0f,0.0f,1.0f)),
    Up(vec3(0.0f,1.0f,0.0f)),
    Transform(mat4(1.0f)),
    Projection(perspect)
    {}
    
    Camera(int screenWidth, int screenHeight)
    :
    Position(vec3(0.0f,0.0f,-3.0f)),
    Forward(vec3(0.0f,0.0f,-1.0f)),
    Up(vec3(0.0f,1.0f,0.0f)),
    Transform(mat4(1.0f)),
    Projection(perspect),
    SCR_Width(screenWidth),
    SCR_Height(screenHeight)
    {}

    void ViewPortCallback(int width, int height)
    {
        SCR_Height = height;
        SCR_Width = width;
    }
    
    mat4 GetViewProj()
    {
        if (Projection == perspect)
                {
                    return glm::perspective(glm::radians(45.0f),((float)SCR_Width/(float)SCR_Height),.1f,100.0f) * glm::lookAt(Position,Forward,Up);
                }else            
                    return glm::ortho(0.0f,(float)SCR_Width,0.0f,(float)SCR_Height,.1f,1000.0f) * glm::lookAt(Position,Forward,Up);

    }
    operator mat4()
    
    {
       if (Projection == perspect)
                {
                    return glm::perspective(glm::radians(45.0f),((float)SCR_Width/(float)SCR_Height),.1f,100.0f) * glm::lookAt(Position,Forward,Up);
                }else            
                    return glm::ortho(0.0f,(float)SCR_Width,0.0f,(float)SCR_Height,.1f,100.0f) * glm::lookAt(Position,Forward,Up);

        
    }    

};

#endif