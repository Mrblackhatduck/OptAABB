#pragma once
#ifndef DRAWABLE_H
#define DRAWABLE_H
#include <Types.h>
#include <Shader.h>
class IDrawable
{
public:
    virtual void Draw(Shader* shader) = 0;
};



struct DrawCall
{
public:
    DrawCall():shader(0) {}
    DrawCall(Shader&& _shader):shader(std::move(_shader))
    {}
    Shader shader;
    void Draw(vector<IDrawable*>& drawables)
    {
        for each (IDrawable* var in drawables)
        {
            var->Draw(&shader);
        }
    }
};

class RenderPass
{
    //std::unique_ptr<Shader> 

    Shader shader;
};



#endif // !DRAWABLE_H
