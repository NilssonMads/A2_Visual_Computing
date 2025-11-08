#include "Scene.hpp"

Scene::Scene() {}

Scene::~Scene() {
    for (auto obj : objects) {
        delete obj;
    }
}

void Scene::addObject(Object* obj) {
    objects.push_back(obj);
}

void Scene::render(Camera* camera) {
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();
    
    for (auto obj : objects) {
        obj->render(view, projection);
    }
}
