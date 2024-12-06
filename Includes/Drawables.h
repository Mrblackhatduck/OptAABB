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
    //virtual operator mat4&() = 0;
};



struct DrawCall
{
public:
    DrawCall():shader(nullptr) {}
    DrawCall(Shader* _shader):shader(_shader)
    {}
    Shader* shader;
    virtual void Draw(vector<Drawable*>& drawables)
    {
        for each (Drawable* var in drawables)
        {
            shader->use();
            shader->setMat4("transform", var->Transform);
            var->Draw(shader);
        }
    }
};

struct DrawCallDepth :public DrawCall 

{
public:
    int size_x,size_y;
    mat4* LightMatrix;
    uint fbo, depthTexture;
    Shader* depthShader;
    vec3 lightPos;
    uint screenWidth,screenHeight;
    DrawCallDepth(int sizeX, int sizeY, mat4* lightTransform, Shader* color, Shader* Depth,vec3 lightPosition)
        :DrawCall(color),
        size_x(sizeX),
        size_y(sizeY),
        LightMatrix(lightTransform),
        depthShader(Depth),
        lightPos(lightPosition)
    {
        glGenFramebuffers(1, &fbo);
        glGenTextures(1,&depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_DEPTH_COMPONENT,
            size_x, size_y,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
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
        
        //glClearColor(0.1f, 0.1f, 0.0f, 0.0f);
        glClear( GL_DEPTH_BUFFER_BIT);



        //GLuint modelMatLocation = glGetUniformLocation(depthShader.ID, "model");
        //glCullFace(GL_FRONT);
        
        depthShader->use();
        depthShader->setMat4("lightSpaceMatrix", (*LightMatrix));
        //depthShader->setMat4();
        for (auto drw : drawables)
        {
            //temp = (drw)->Transform;
            //depthShader.setMat4("transform", drw->Transform);
            depthShader->setMat4("model",drw->Transform);
            drw->Draw(depthShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glViewport(0, 0, screenWidth, screenHeight);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //
        ////glCullFace(GL_BACK);
        //
        //shader->use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, depthTexture);
        //shader->setMat4("lightSpaceMatrix",*(LightMatrix));
        ////shader.setMat4("mode");
        //for (auto drw : drawables) 
        //{
        //    //temp = (drw)->Transform;
        //    shader->setMat4("model", drw->Transform);
        //    //glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, VAL_PTR(temp));
        //    drw->Draw(shader);
        //}
    }
};

class DebugDepthCall:public DrawCall
{
public:
    uint DepthImageHandle;
    DebugDepthCall(Shader* shader, uint depthImage = -1):DrawCall(shader), DepthImageHandle(depthImage)
    {}
    
 
    void Draw(vector<Drawable*>& drawables)override 
    {
        shader->use();
        //shader->setInt("Depth")
        (drawables[0])->Draw(shader);
    }
};
class RenderPass
{
    //std::unique_ptr<Shader> 

    Shader shader;
};



#endif // !DRAWABLE_H
