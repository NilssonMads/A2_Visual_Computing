#include "Object.hpp"

Object::Object() : shader(nullptr) {
    modelMatrix = glm::mat4(1.0f);
}

Object::~Object() {}

void Object::setShader(Shader* s) {
    shader = s;
}

void Object::render(const glm::mat4& view, const glm::mat4& projection) {
    // Base implementation - override in derived classes
}
