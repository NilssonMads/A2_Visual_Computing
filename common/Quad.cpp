#include "Quad.hpp"
#include "TextureShader.hpp"
#include <glm/gtc/matrix_transform.hpp>

Quad::Quad(float aspectRatio) {
    vertexCount = 6;
    
    float width = aspectRatio;
    float height = 1.0f;
    
    GLfloat vertices[] = {
        -width, -height, 0.0f,
         width, -height, 0.0f,
         width,  height, 0.0f,
         width,  height, 0.0f,
        -width,  height, 0.0f,
        -width, -height, 0.0f
    };
    
    GLfloat uvs[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };
    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
}

Quad::~Quad() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
}

void Quad::render(const glm::mat4& view, const glm::mat4& projection) {
    if (shader) {
        shader->use();
        
        TextureShader* texShader = dynamic_cast<TextureShader*>(shader);
        if (texShader) {
            glm::mat4 mvp = projection * view * modelMatrix;
            texShader->setMVP(mvp);
        }
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
}
