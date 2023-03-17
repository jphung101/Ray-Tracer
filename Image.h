#ifndef IMAGE_H
#define IMAGE_H

#include "Scene.h"
#include "FreeImage.h"

class Ray{
    public:
        glm::vec3 eye, direction; //eye = p0 (origin)
        float t; //when calling intersect, set "t"

        Ray(glm::vec3 eye, glm::vec3 direction);
        Ray();

        static Ray RayThruPixel(Camera camera, int h, int w, int height, int width);
        glm::vec3 intersect(Object object);
};

class Image{
    public:
        Scene scene;

        Image();
        Image(Scene scene);

        //finds and compares t to set the smallest t and ray to pass into lighting
        BYTE* RayTrace(Scene scene); //main RT, returns bytes that can be drawn
        glm::vec3 lighting(Object object, Ray ray, Scene scene);
};
#endif