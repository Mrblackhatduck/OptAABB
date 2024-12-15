#include <Texture.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(int Width, int Height, TextureType loadType) :
    width(Width),
    height(Height),
    loadedComponents(3)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    //glActiveTexture(GL_TEXTURE0);
    if (loadType == TextureType::BASIC) 
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0,GL_RGBA, GL_FLOAT, NULL);
        glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }
    if (loadType == TextureType::DEPTH)
    {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
   
    if (loadType == TextureType::FRAMEBUFFER_ATTACHMENT)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    }
    if (loadType == TextureType::FRAMEBUFFER_ATTACHMENT_ALBEDOSPEC) 
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}
//by default the texture loads an (RGBA) 4 comp
Texture::Texture(string& path, TextureType loadType, int requiredComponents) : loadedComponents(0)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* result = stbi_load(path.c_str(), &width, &height, &loadedComponents, requiredComponents);
    if (result == NULL) {
        PRNTF("ERROR : Loading Texture file : [%s] ", path.c_str());
        return;
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, result);
    glGenerateMipmap(GL_TEXTURE_2D);
   
    
    glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::operator uint()const
{
    return id;
}