#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glad/glad.h>

class Shader {
public:
    GLuint programID;
    
    Shader(const char* vertex_path, const char* fragment_path);
    virtual ~Shader();
    
    void use();
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    
protected:
    GLuint loadShaders(const char* vertex_path, const char* fragment_path);
};

#endif
