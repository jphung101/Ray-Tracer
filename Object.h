#ifndef OBJECT_H
#define OBJECT_H


#include "glm-master/glm/glm.hpp"
#include "glm-master/glm/ext.hpp" //perspective, translate, rotate

// #include "Scene.h"

#define ZERO 1e-6

using namespace std;
// using namespace glm; //stopped working for some reason


class Object{
    public:
        enum Shape {triangle, sphere};
        Shape shape;
        glm::vec3 diffuse, specular, emission, ambient;
        float shininess;

        glm::mat4 transform;

        Object();

        virtual glm::vec3 getNorm(glm::vec3 point); //included so `dynamic_cast<Sphere*>(&object)` could work
        // vec4 coordinates; //sphere: 4th coord = radius, not sure about W-coordinate
        // Object(Shape shape, vec4 coordinates, vec3 diffuse, vec3 specular, vec3 emission, vec3 ambient, float shininess); 
};
class Triangle: public Object{
    public:
        glm::vec3 vertices[3], verticesNorm[3];

        // Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 aNorm, glm::vec3 bNorm, glm::vec3 cNorm);
        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

        glm::vec3 getNorm(glm::vec3 point); //used for lighting

};
class Sphere: public Object{
    public:
        glm::vec3 coordinates;
        float radius;

        Sphere(glm::vec3 coordinates, float radius); //same name, need to use this.[name]

        glm::vec3 getNorm(glm::vec3 point);
};

#endif

