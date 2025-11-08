#include "TextureShader.hpp"
#include <glm/gtc/type_ptr.hpp>

TextureShader::TextureShader(const char* vertex_path, const char* fragment_path)
    : Shader(vertex_path, fragment_path), texture(nullptr) {}

void TextureShader::setTexture(Texture* tex) {
    texture = tex;
    if (texture) {
        use();
        texture->bind();
        setInt("textureSampler", 0);
    }
}

void TextureShader::setMVP(const glm::mat4& mvp) {
    use();
    GLuint mvpID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));
}
