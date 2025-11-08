#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Shader.hpp"
#include <glm/glm.hpp>

class Object {
public:
    Object();
    virtual ~Object();
    
    void setShader(Shader* s);
    virtual void render(const glm::mat4& view, const glm::mat4& projection);
    
protected:
    Shader* shader;
    glm::mat4 modelMatrix;
};

#endif
