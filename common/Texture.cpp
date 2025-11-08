#include "Texture.hpp"

Texture::Texture(unsigned char* data, int width, int height, bool rgb) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    GLenum format = rgb ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureID);
}

void Texture::update(unsigned char* data, int width, int height, bool rgb) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum format = rgb ? GL_RGB : GL_RGBA;
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, textureID);
}
