#pragma once
#ifndef PRIMATIVES_H
#define PRIMATIVES_H
#include <Drawables.h>

class Cube: public Drawable
{
    unsigned int vao = 0;
    unsigned int vbo;
    mat4 modelMat;
    void Draw(Shader* shader);
    operator mat4&();
};


#endif