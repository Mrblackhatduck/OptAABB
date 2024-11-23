#ifndef TEXTURE_H
#define TEXTURE_H
#include <Types.h>

class Texture
{
    uint id = 0;
    int width, height, loadedComponents;
public:
    Texture(int Width, int Height);

    //by default the texture loads an (RGBA) 4 comp
    Texture(string& path, int requiredComponents = 4); 
    operator uint()const;
    
};

#endif