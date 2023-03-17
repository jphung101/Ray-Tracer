#ifndef SCENE_H
#define SCENE_H

#include <vector> //main way to hold everything, maybe use unordered_map?
#include <stack> //for matrix stacks

#include "Object.h"

using namespace std;

class Camera{
    public:
        glm::vec3 eye, center, up;
        float fovy;

        Camera();
        Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fovy);
};

struct Attenuation{
    float constant, linear, quadratic;
};
class Light{ //could make subclasses, but seems too complicated, especailly after Object
    public:
        enum Type{directional, point};
        Type type;

        glm::vec3 coordinate, color;        
        Attenuation attenuation;

        Light();
        Light(glm::vec3 coordinate, glm::vec3 color);
};

class Scene{
    public:
        glm::vec2 size; //width x height
        int maxDepth = 5;
        //output file handled in main

        Camera camera; //might make a vector to see multiple cameras

        //The line below generates this error: ./Scene.h:44:16: error: use of undeclared identifier 'Object' vector<Object> objects;, please fix it
        vector<Object> objects;
        vector<Light> lights;

        Scene();
        // int maxVerts, maxVertNorms; //might not need as I am using vectors, so size != relevant 

};


#endif

