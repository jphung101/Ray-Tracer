#include "Scene.h"

Camera::Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fovy){
    this->eye = eye;
    this->center = center;
    this->up = up;
    this->fovy = fovy;
}

Light::Light(glm::vec3 coordinate, glm::vec3 color){
    this->coordinate = coordinate;
    this->color = color;
}

Scene::Scene(){

};