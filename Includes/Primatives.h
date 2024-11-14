#pragma once
#ifndef PRIMATIVES_H
#define PRIMATIVES_H
#include <Drawables.h>

#define CREATE_MAT4_OPERATOR(type) type::operator mat4& ()\
{\
  return Transform;\
}\

class Quad : public Drawable 
{
public:
    
    uint quadVAO = 0;
    uint quadVBO;
    operator mat4& ();
    void Draw(Shader* shader);
    
};


class Cube: public Drawable
{
public:
    Cube();
    
    unsigned int vao = 0;
    unsigned int vbo;
    void Draw(Shader* shader);
    operator mat4&();
};

class Plane : public Drawable 
{
public:
    uint vao;
    uint vbo;
    Plane();
    void Draw(Shader* shader);
};

#endif