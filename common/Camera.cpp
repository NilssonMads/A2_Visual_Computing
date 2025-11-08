#include "Camera.hpp"

Camera::Camera() {
    position = glm::vec3(0, 0, 0);
    target = glm::vec3(0, 0, 0);
    up = glm::vec3(0, 1, 0);
}

void Camera::setPosition(glm::vec3 pos) {
    position = pos;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
}
