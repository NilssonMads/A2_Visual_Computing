#ifndef SCENE_HPP
#define SCENE_HPP

#include "Object.hpp"
#include "Camera.hpp"
#include <vector>

class Scene {
public:
    Scene();
    ~Scene();
    
    void addObject(Object* obj);
    void render(Camera* camera);
    
private:
    std::vector<Object*> objects;
};

#endif
