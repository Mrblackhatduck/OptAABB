#ifndef SHADER_H
#define SHADER_H



#include <string>
#include <Types.h>




class Shader
{
public:
    unsigned int ID;
    
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const char* computeShader);
    void use();
   
    
    void setBool(const std::string& name, bool value) const;
    
    void setInt(const std::string& name, int value) const;
    
    void setFloat(const std::string& name, float value) const;
   
    void setMat4(const std::string& name, const mat4& matrix4)const ;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& val)const;
    operator unsigned int()const 
    {
        return ID;
    }

private:
   
    void checkCompileErrors(unsigned int shader, std::string type);
    
};
#endif