#include "Object.h"

using namespace std;
using namespace glm;

Object::Object(){

}
Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c){
    shape = triangle;
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
}

vec3 Triangle::getNorm(vec3 point){

}

Sphere::Sphere(vec3 coordinates, float radius){
    shape = sphere;
    this->coordinates = coordinates;
    this->radius = radius;
}

vec3 Sphere::getNorm(vec3 point){

}
