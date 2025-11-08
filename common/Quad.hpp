#ifndef QUAD_HPP
#define QUAD_HPP

#include "Object.hpp"
#include <glad/glad.h>

class Quad : public Object {
public:
    Quad(float aspectRatio);
    ~Quad();
    
    void render(const glm::mat4& view, const glm::mat4& projection) override;
    
private:
    GLuint vertexBuffer;
    GLuint uvBuffer;
    int vertexCount;
};

#endif
