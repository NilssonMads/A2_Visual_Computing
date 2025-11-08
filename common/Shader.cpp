#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertex_path, const char* fragment_path) {
    programID = loadShaders(vertex_path, fragment_path);
}

Shader::~Shader() {
    glDeleteProgram(programID);
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

GLuint Shader::loadShaders(const char* vertex_path, const char* fragment_path) {
    // Read vertex shader
    std::string vertexCode;
    std::ifstream vShaderFile(vertex_path);
    if (!vShaderFile.is_open()) {
        std::cerr << "Failed to open vertex shader: " << vertex_path << std::endl;
        return 0;
    }
    std::stringstream vShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    vertexCode = vShaderStream.str();
    vShaderFile.close();

    // Read fragment shader
    std::string fragmentCode;
    std::ifstream fShaderFile(fragment_path);
    if (!fShaderFile.is_open()) {
        std::cerr << "Failed to open fragment shader: " << fragment_path << std::endl;
        return 0;
    }
    std::stringstream fShaderStream;
    fShaderStream << fShaderFile.rdbuf();
    fragmentCode = fShaderStream.str();
    fShaderFile.close();

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    
    // Check vertex shader
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    
    // Check fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Link program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check linking
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
