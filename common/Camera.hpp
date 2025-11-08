#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    void setPosition(glm::vec3 pos);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
};

#endif
