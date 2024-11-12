#pragma once
#ifndef DRAWABLE_H
#define DRAWABLE_H
#include <Types.h>
#include <Shader.h>
class Drawable
{
public:
    mat4 Transform;
    virtual void Draw(Shader* shader) = 0;
    virtual operator mat4&() = 0;
};



struct DrawCall
{
public:
    DrawCall():shader(0) {}
    DrawCall(Shader&& _shader):shader(std::move(_shader))
    {}
    Shader shader;
    virtual void Draw(vector<Drawable*>& drawables)
    {
        for each (Drawable* var in drawables)
        {
            var->Draw(&shader);
        }
    }
};

struct DrawCallDepth :public DrawCall 

{
public:
    int size_x,size_y;
    mat4* LightMatrix;
    uint fbo, depthTexture;
    Shader depthShader;
    vec3 lightPos;
    DrawCallDepth(int sizeX, int sizeY, mat4* lightTransform, Shader&& color, Shader&& Depth,vec3 lightPosition)
        :DrawCall(std::forward<Shader>(color)),
        size_x(sizeX),
        size_y(sizeY),
        LightMatrix(lightTransform),
        depthShader(Depth),
        lightPos(lightPosition)
    {
        glGenTextures(1,&depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_DEPTH_COMPONENT,
            size_x, sizeY,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        //no color needed
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Draw(vector<Drawable*>& drawables)override 
    {
        glViewport(0, 0, size_x, size_y);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_DEPTH_BUFFER_BIT); 
        GLuint modelMatLocation = glGetUniformLocation(depthShader.ID, "model");
        mat4 temp;
        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", (*LightMatrix));
        for (auto drw : drawables)
        {
            temp = *(drw);
            glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, VAL_PTR(temp));
            drw->Draw(&depthShader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, depthTexture);
        shader.setMat4("lightSpaceMatrix",*(LightMatrix));
        for (auto drw : drawables) 
        {
            temp = *(drw);
            glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, VAL_PTR(temp));
            drw->Draw(&shader);
        }
    }
};

class RenderPass
{
    //std::unique_ptr<Shader> 

    Shader shader;
};



#endif // !DRAWABLE_H
