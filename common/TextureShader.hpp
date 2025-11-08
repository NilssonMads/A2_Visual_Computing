#ifndef TEXTURESHADER_HPP
#define TEXTURESHADER_HPP

#include "Shader.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>

class TextureShader : public Shader {
public:
    TextureShader(const char* vertex_path, const char* fragment_path);
    
    void setTexture(Texture* tex);
    void setMVP(const glm::mat4& mvp);
    
private:
    Texture* texture;
};

#endif
