#include "Image.h"

Ray::Ray(glm::vec3 eye, glm::vec3 direction){
    this->eye = eye;
    this->direction = direction;
}

static Ray RayThruPixel(Camera camera, int h, int w, int height, int width){
    Ray back;
    glm::vec3 W = glm::normalize(camera.eye - camera.center);
    glm::vec3 U = glm::normalize(glm::cross(camera.up, W));
    glm::vec3 V = glm::cross(W, U);

    float fovy = camera.fovy * (M_PI/180);
    float fovxVal = tan(fovy/2) * (width/height);
    float alpha = fovxVal * (w - width/2)/(width/2);
    float beta = tan(fovy/2) * (height/2 - h)/(height/2);

    back.eye = camera.eye;
    back.direction = glm::normalize((alpha * U) + (beta * V) - W);
    return back;
}
glm::vec3 Ray::intersect(Object object){
    glm::vec3 back = glm::vec3(INFINITY, INFINITY, INFINITY); //default (no hit)
    float a, b, c; //both alpha/beta/gamma and a/b/c
    
    if(object.shape == Object::triangle){
        Triangle* triangle = dynamic_cast<Triangle*>(&object); //downcasts Object to Sphere
        glm::vec3 normal = glm::normalize(glm::cross(triangle->vertices[2] - triangle->vertices[0], triangle->vertices[1] - triangle->vertices[0]));
        if(glm::dot(direction, normal) != ZERO){ //hit
            float t = glm::dot(triangle->vertices[0], normal)/glm::dot(direction, normal); //A maybe triangle->vertices[0] - eye
            if(t > ZERO){ //object is in front of camera
                glm::vec3 apNorm = (glm::cross(normal, triangle->vertices[2] - triangle->vertices[1])) *
                    1.0f/(glm::dot((glm::cross(normal, triangle->vertices[2] - triangle->vertices[1])), triangle->vertices[0] - triangle->vertices[2]));
                float apW = glm::dot(-apNorm, triangle->vertices[2]);
                glm::vec3 bpNorm = (glm::cross(normal, triangle->vertices[0] - triangle->vertices[2]))/
                    glm::dot((glm::cross(normal, triangle->vertices[0] - triangle->vertices[2])), triangle->vertices[1] - triangle->vertices[0]);
                float bpW = glm::dot(-bpNorm, triangle->vertices[0]);
                
                glm::vec3 P = eye + (direction * t);
                float alpha = glm::dot(apNorm, P) + apW;
                float beta = glm::dot(bpNorm, P) + bpW;
                float gamma = 1 - alpha - beta;

                if(alpha >= 0 && beta >= 0 && gamma >=0){
                    back = P;
                }
            }
        }
    }else{
        Sphere* sphere = dynamic_cast<Sphere*>(&object); //downcasts Object to Sphere
        a = glm::dot(direction, direction);
        b = 2 * glm::dot(direction, eye - sphere->coordinates);
        c = glm::dot(eye - sphere->coordinates, eye - sphere->coordinates);
        float delta = (b * b) - (4 * a * (c - (sphere->radius * sphere->radius))); //discriminate

        if(delta >= ZERO){ //have intersection
            float tPos = ((-b) + sqrt(delta))/(2 * a);
            float tNeg = ((-b) - sqrt(delta))/(2 * a);

            if(tNeg == tPos){ //tangent
                t = tNeg;
            }else if((tNeg > ZERO) && (tPos > ZERO)){ //two points; tNeg first as more likely to be negative
                t = min(tNeg, tPos);
            }else{ //in->out
                t = tPos;
            }
            back = eye + (direction * t);
        }
    }
    return back;
}

Image::Image(Scene scene){
    this->scene = scene;
}
 
BYTE* Image::RayTrace(Scene scene){
    int size = 3 * scene.size.x * scene.size.y;
    BYTE* back = new BYTE[size]; //3 bytes per pixel
    Object hit;
    Ray ray;

    ray.t = INFINITY;
    for(int height = 0; scene.size.y; height++){
        for(int width = 0; scene.size.x; width++){
            for(Object object: scene.objects){
                Ray tempRay = Ray::RayThruPixel(scene.camera, height, width, scene.size.y, scene.size.x);
                //transforms ray
                glm::vec4 tempEye = glm::inverse(object.transform) * glm::vec4(tempRay.eye, 1.0);
                glm::vec4 tempDir = glm::inverse(object.transform) * glm::vec4(tempRay.direction, 1.0);
                tempRay.eye = tempEye;
                tempRay.direction = tempDir;

                glm::vec3 intersection = tempRay.intersect(object);
                if(intersection != glm::vec3(INFINITY, INFINITY, INFINITY)){
                    if(tempRay.t < ray.t){ //find min
                        hit = object;
                        ray = tempRay;
                    }
                }
            }
            if(ray.t != INFINITY){ //might need to add ambient and emission
                glm::vec3 color = lighting(hit, ray, scene);
                back[3 * (int)(height * scene.size.x + width)] = color.x; //not sure if this is right
                back[3 * (int)(height * scene.size.x + width) + 1] = color.y;
                back[3 * (int)(height * scene.size.x + width) + 2] = color.z;
            }
        }
    }
    return back;
}
//use the contents of this link for derive the lighting equation: https://cseweb.ucsd.edu//~viscomp/classes/cse167/wi23/slides/lecture16.pdf
glm::vec3 lighting(Object object, Ray ray, Scene scene){
    glm::vec3 back;
    glm::vec3 normal;

    back = object.ambient;
    back += object.emission;
    
    if(object.shape == Object::triangle){
        Triangle* triangle = dynamic_cast<Triangle*>(&object); //downcasts Object to Sphere
        normal = glm::normalize(glm::cross(triangle->vertices[2] - triangle->vertices[0], triangle->vertices[1] - triangle->vertices[0]));
    }else{
        Sphere* sphere = dynamic_cast<Sphere*>(&object); //downcasts Object to Sphere
        normal = glm::normalize(ray.eye - sphere->coordinates);
    }


    for(Light light: scene.lights){
        glm::vec3 L = light.color;
        glm::vec3 Direction = glm::normalize(light.coordinate);
        if(light.type == Light::point){
            Direction = glm::normalize(light.coordinate - ray.intersect(object));
            float d = ray.t; // distance
            float atten = light.attenuation.constant + light.attenuation.linear * d + light.attenuation.quadratic * d * d;
            L = L/atten;
        }
        glm::vec3 Diffuse = object.diffuse * max(glm::dot(Diffuse, normal), 0.0f);
        glm::vec3 Half = glm::normalize(Direction - glm::normalize(ray.direction));
        glm::vec3 Specular = object.specular * pow(max(glm::dot(Half, normal), 0.0f), object.shininess);
        
        back += L * (Diffuse + Specular);
    }


    // glm::vec3 back;
    // glm::vec3 normal;
    // if(object.shape == Object::triangle){
    //     Triangle* triangle = dynamic_cast<Triangle*>(&object); //downcasts Object to Sphere
    //     normal = glm::normalize(glm::cross(triangle->vertices[2] - triangle->vertices[0], triangle->vertices[1] - triangle->vertices[0]));
    // }else{
    //     Sphere* sphere = dynamic_cast<Sphere*>(&object); //downcasts Object to Sphere
    //     normal = glm::normalize(ray.eye - sphere->coordinates);
    // }
    // for(Light light: scene.lights){
    //     glm::vec3 L = glm::normalize(light.coordinates - ray.eye);
    //     glm::vec3 V = glm::normalize(scene.camera.eye - ray.eye);
    //     glm::vec3 R = glm::normalize(2 * glm::dot(L, normal) * normal - L);
    //     float diffuse = glm::dot(L, normal);
    //     float specular = glm::dot(R, V);
    //     if(diffuse < ZERO){
    //         diffuse = ZERO;
    //     }
    //     if(specular < ZERO){
    //         specular = ZERO;
    //     }
    //     specular = pow(specular, object.shininess);
    //     back += (object.ambient * light.ambient) + (object.diffuse * light.diffuse * diffuse) + (object.specular * light.specular * specular);
    // }
    // return back;
}