#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>

class Texture {
public:
    GLuint textureID;
    
    Texture(unsigned char* data, int width, int height, bool rgb);
    ~Texture();
    
    void update(unsigned char* data, int width, int height, bool rgb);
    void bind();
};

#endif
