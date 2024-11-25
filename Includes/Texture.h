#ifndef TEXTURE_H
#define TEXTURE_H
#include <Types.h>

enum class TextureType 
{
    BASIC,
    DEPTH,
    FRAMEBUFFER_ATTACHMENT,
    FRAMEBUFFER_ATTACHMENT_ALBEDOSPEC
};
class Texture
{
    uint id = 0;
    int width, height, loadedComponents;
public:
    Texture(int Width, int Height, TextureType loadType);

    //by default the texture loads an (RGBA) 4 comp
    Texture(string& path, TextureType loadType, int requiredComponents = 4);
    operator uint()const;
    
};

#endif