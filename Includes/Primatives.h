#pragma once
#ifndef PRIMATIVES_H
#define PRIMATIVES_H
#include <Drawables.h>

class Cube: public IDrawable
{
    unsigned int vao = 0;
    unsigned int vbo;
    void Draw(Shader* shader);
    
};


#endif