#include <Texture.h>
#include <glad/glad.h>
#include <stb_image.h>

Texture::Texture(int Width, int Height) :
    width(Width),
    height(Height)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


}
//by default the texture loads an (RGBA) 4 comp
Texture::Texture(string& path, int requiredComponents) : loadedComponents(0)
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