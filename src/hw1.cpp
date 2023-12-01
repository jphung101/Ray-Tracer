#include "hw1.h"
#include "hw1_scenes.h"
#include "pcg.h"
#include "parallel.h"
using namespace hw1;
Vector3 rayPixel(Image3 img, float x, float y, Vector3 lookfrom, Vector3 lookat, Vector3 up, float vfov){
    Vector3 W = normalize(lookfrom - lookat);
    Vector3 U = normalize(cross(up, W));
    Vector3 V = cross(W, U);

    float fovy = vfov * (M_PI/180);
    float fovxVal = tan(fovy/2.0f) * ((float)img.width/img.height);
    float alpha = fovxVal * (x - img.width/2.0f)/(img.width/2.0f);
    float beta = tan(fovy/2.0f) * (img.height/2.0f - y)/(img.height/2.0f);    

    Vector3f back = normalize((alpha * (Vector3f)U) + (beta * (Vector3f)V) - (Vector3f)W);
    return back;
}

void sphereIntersect(Vector3 ray, Sphere &min, float &minT, Vector3f &tPoint, Vector3 &norm, int scene_id, Vector3 lookfrom){
    float a, b, c, delta;
    float ZERO = 1e-6;
    for(auto sphere: hw1_scenes[scene_id].shapes){
        a = dot(ray, ray);
        b = 2 * dot(ray, lookfrom - sphere.center);
        c = dot(lookfrom - sphere.center, lookfrom - sphere.center) - (sphere.radius * sphere.radius);
        delta = (b * b) - (4 * a * c); //discriminant
        if(delta >= ZERO){
            float tPos = ((-b) + sqrt(delta))/(2 * a);
            float tNeg = ((-b) - sqrt(delta))/(2 * a);  
            float t; 
            if(tNeg == tPos){ //tangent
                t = tNeg;
            }else if((tNeg > ZERO) && (tPos > ZERO)){ //two points; tNeg first as more likely to be negative
                t = std::min(tNeg, tPos);
            }else{ //in->out
                t = tPos;
            }
            if(t < minT && t > ZERO){
                minT = t;
                min = sphere;
                tPoint = Vector3f(lookfrom) + (Vector3f(ray) * t);
                norm = normalize(tPoint - Vector3f(sphere.center));
            }
        }
    }
} 

Vector3 color(int scene_id, Sphere min, Vector3 norm, Vector3 lightDir, PointLight light, Vector3f tPoint){
    Vector3 col = Vector3((Vector3f(hw1_scenes[scene_id].materials[min.material_id].color) * max(float(dot(norm, lightDir)), 0.0f))/float(M_PI)); 
    col *= light.intensity/distance_squared(light.position, Vector3(tPoint));
    return col;
}
 
Vector3 rayTrace(Vector3 &col, int scene_id, Vector3 ray, Sphere &min, float &minT, Vector3f &tPoint, Vector3 &norm, Vector3 lookfrom, int x, int y, int depth){
    sphereIntersect(ray, min, minT, tPoint, norm, scene_id, lookfrom);  
    if(minT != INFINITY){
        if(hw1_scenes[scene_id].materials[min.material_id].type == MaterialType::Diffuse || depth > 4){
            for(PointLight light: hw1_scenes[scene_id].lights){
                Vector3 lightDir = normalize(light.position - Vector3(tPoint));
                if(dot(norm, lightDir) < 0.0f){
                    norm = -norm;
                }
                Sphere shadowMin;
                Vector3f shadowTPoint;
                float shadowMinT = INFINITY;
                Vector3 shadowNorm;
                Vector3 shadowRay = (Vector3)tPoint + lightDir * (Real)0.0001;
                sphereIntersect(lightDir, shadowMin, shadowMinT, shadowTPoint, shadowNorm, scene_id, shadowRay);
                if(shadowMinT == INFINITY || shadowMinT < 0.0001 || shadowMinT > (1-0.0001)*distance(light.position, Vector3(tPoint))){
                    col += color(scene_id, min, norm, lightDir, light, tPoint);
                }
            } 
        }else{
            Vector3 reflect = ray - (dot((Real)2 * ray, norm) * norm);
            Sphere refMin;
            float refMinT = INFINITY;
            Vector3 refNorm = norm;
            Vector3f refTPoint;
            Vector3 refRay = (Vector3)tPoint + reflect * (Real)0.0001;
            col = hw1_scenes[scene_id].materials[min.material_id].color * rayTrace(col, scene_id, reflect, refMin, refMinT, refTPoint, refNorm, refRay, x, y, depth + 1);
        }
    }else{
        col = Vector3f(0.5, 0.5, 0.5);
    }
    return col;
}

Image3 hw_1_1(const std::vector<std::string> &/*params*/) {
    // Homework 1.1: generate camera rays and output the ray directions
    // The camera is positioned at (0, 0, 0), facing towards (0, 0, -1),
    // with an up vector (0, 1, 0) and a vertical field of view of 90 degree.
    Image3 img(640 /* width */, 480 /* height */);
    Vector3f camEye(0, 0, 0);
    Vector3f camCenter(0, 0, -1);
    Vector3f camUp(0, 1, 0);
    float camFovy = 90;

    Vector3f W = normalize(camEye - camCenter);
    Vector3f U = normalize(cross(camUp, W));
    Vector3f V = cross(W, U);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            float fovy = camFovy * (M_PI/180);
            float fovxVal = tan(fovy/2.0f) * ((float)img.width/img.height);
            float alpha = fovxVal * ((x + 0.5f) - img.width/2.0f)/(img.width/2.0f);
            float beta = tan(fovy/2.0f) * (img.height/2.0f - (y + 0.5f))/(img.height/2.0f);    

            Vector3f back = normalize((alpha * U) + (beta * V) - W);
            img(x, y) = back;
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &/*params*/) {
    // Homework 1.2: intersect the rays generated from hw_1_1
    // with a unit sphere located at (0, 0, -2)

    Image3 img(640 /* width */, 480 /* height */);
    std::vector<std::string> parameters; //not sure
    Image3 temp = hw_1_1(parameters);
    
    Sphere sphere;
    sphere.center = Vector3f(0, 0, -2);
    sphere.radius = 1;

    Vector3 camEye(0, 0, 0);

    float a, b, c;
    float ZERO = 1e-6;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 ray = temp(x, y);
            a = dot(ray, ray);
            b = 2 * dot(ray, camEye - sphere.center);
            c = dot(camEye - sphere.center, camEye - sphere.center) - (sphere.radius * sphere.radius);
            float delta = (b * b) - (4 * a * c); //discriminate
            if(delta >= ZERO){
                float tPos = ((-b) + sqrt(delta))/(2 * a);
                float tNeg = ((-b) - sqrt(delta))/(2 * a);  
                float t; 
                if(tNeg == tPos){ //tangent
                    t = tNeg;
                }else if((tNeg > ZERO) && (tPos > ZERO)){ //two points; tNeg first as more likely to be negative
                    t = std::min(tNeg, tPos);
                }else{ //in->out
                    t = tPos;
                }
                Vector3f tPoint = Vector3f(camEye) + (Vector3f(ray) * t);
                img(x, y) = (normalize(tPoint - Vector3f(sphere.center)) + Vector3f(1, 1, 1))/2.0f;
            }else{
                img(x, y) = Vector3f(0.5, 0.5, 0.5);
            }
        }
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: add camera control to hw_1_2. 
    // We will use a look at transform:
    // The inputs are "lookfrom" (camera position),
    //                "lookat" (target),
    //                and the up vector
    // and the vertical field of view (in degrees).
    // If the user did not specify, fall back to the default
    // values below.
    // If you use the default values, it should render
    // the same image as hw_1_2.

    Vector3 lookfrom = Vector3{0, 0,  0};
    Vector3 lookat   = Vector3{0, 0, -2};
    Vector3 up       = Vector3{0, 1,  0};
    Real    vfov     = 90;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-lookfrom") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            lookfrom = Vector3{x, y, z};
        } else if (params[i] == "-lookat") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            lookat = Vector3{x, y, z};
        } else if (params[i] == "-up") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            up = Vector3{x, y, z};
        } else if (params[i] == "-vfov") {
            vfov = std::stof(params[++i]);
        }
    }

    // avoid unused warnings
    UNUSED(lookfrom);
    UNUSED(lookat);
    UNUSED(up);
    UNUSED(vfov);

    Image3 img(640 /* width */, 480 /* height */);
    std::vector<std::string> parameters; //not sure
    Image3 temp;

    Vector3 W = normalize(lookfrom - lookat);
    Vector3 U = normalize(cross(up, W));
    Vector3 V = cross(W, U);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            float fovy = vfov * (M_PI/180);
            float fovxVal = tan(fovy/2.0f) * ((float)img.width/img.height);
            float alpha = fovxVal * ((x + 0.5f) - img.width/2.0f)/(img.width/2.0f);
            float beta = tan(fovy/2.0f) * (img.height/2.0f - (y + 0.5f))/(img.height/2.0f);    

            Vector3f back = normalize((alpha * (Vector3f)U) + (beta * (Vector3f)V) - (Vector3f)W);
            img(x, y) = back;
        }
    }
    temp = img;
    Sphere sphere;
    sphere.center = Vector3f(0, 0, -2);
    sphere.radius = 1;

    float a, b, c;
    float ZERO = 1e-6;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 ray = temp(x, y);
            a = dot(ray, ray);
            b = 2 * dot(ray, lookfrom - sphere.center);
            c = dot(lookfrom - sphere.center, lookfrom - sphere.center) - (sphere.radius * sphere.radius);
            float delta = (b * b) - (4 * a * c); //discriminate
            if(delta >= ZERO){
                float tPos = ((-b) + sqrt(delta))/(2 * a);
                float tNeg = ((-b) - sqrt(delta))/(2 * a);  
                float t; 
                if(tNeg == tPos){ //tangent
                    t = tNeg;
                }else if((tNeg > ZERO) && (tPos > ZERO)){ //two points; tNeg first as more likely to be negative
                    t = std::min(tNeg, tPos);
                }else{ //in->out
                    t = tPos;
                }
                Vector3f tPoint = Vector3f(lookfrom) + (Vector3f(ray) * t);
                img(x, y) = (normalize(tPoint - Vector3f(sphere.center)) + Vector3f(1, 1, 1))/2.0f;
            }else{
                img(x, y) = Vector3f(0.5, 0.5, 0.5);
            }
        }
    }

    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render the scenes defined in hw1_scenes.h
    // output their diffuse color directly.
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    UNUSED(scene_id); // avoid unused warning
    // Your scene is hw1_scenes[scene_id]

    Vector3 lookfrom = Vector3{0, 0,  0};
    Vector3 lookat   = Vector3{0, 0, -1};
    Vector3 up       = Vector3{0, 1,  0};
    Real    vfov     = 45;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-lookfrom") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            lookfrom = Vector3{x, y, z};
        } else if (params[i] == "-lookat") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            lookat = Vector3{x, y, z};
        } else if (params[i] == "-up") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            up = Vector3{x, y, z};
        } else if (params[i] == "-vfov") {
            vfov = std::stof(params[++i]);
        }
    }

    // avoid unused warnings
    UNUSED(lookfrom);
    UNUSED(lookat);
    UNUSED(up);
    UNUSED(vfov);

    Image3 img(640 /* width */, 480 /* height */);
    std::vector<std::string> parameters; //not sure
    Image3 temp;

    Vector3 W = normalize(lookfrom - lookat);
    Vector3 U = normalize(cross(up, W));
    Vector3 V = cross(W, U);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            float fovy = vfov * (M_PI/180);
            float fovxVal = tan(fovy/2.0f) * ((float)img.width/img.height);
            float alpha = fovxVal * ((x + 0.5f) - img.width/2.0f)/(img.width/2.0f);
            float beta = tan(fovy/2.0f) * (img.height/2.0f - (y + 0.5f))/(img.height/2.0f);    

            Vector3f back = normalize((alpha * (Vector3f)U) + (beta * (Vector3f)V) - (Vector3f)W);
            img(x, y) = back;
        }
    }
    temp = img;

    float a, b, c;
    float ZERO = 1e-6;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 ray = temp(x, y);
            Sphere min;
            float minT = INFINITY;
            for(auto sphere: hw1_scenes[scene_id].shapes){
                a = dot(ray, ray);
                b = 2 * dot(ray, lookfrom - sphere.center);
                c = dot(lookfrom - sphere.center, lookfrom - sphere.center) - (sphere.radius * sphere.radius);
                float delta = (b * b) - (4 * a * c); //discriminate

                if(delta >= ZERO){
                    float tPos = ((-b) + sqrt(delta))/(2 * a);
                    float tNeg = ((-b) - sqrt(delta))/(2 * a);  
                    float t; 
                    if(tNeg == tPos){ //tangent
                        t = tNeg;
                    }else if((tNeg > ZERO) && (tPos > ZERO)){ //two points; tNeg first as more likely to be negative
                        t = std::min(tNeg, tPos);
                    }else{ //in->out
                        t = tPos;
                    }
                    if(t < minT && t > ZERO){
                        minT = t;
                        min = sphere;
                    }
                }
            }
            if(minT != INFINITY){
                img(x, y) = hw1_scenes[scene_id].materials[min.material_id].color;
            }else{
                img(x, y) = Vector3f(0.5, 0.5, 0.5);
            }
        }
    }

    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: render the scenes defined in hw1_scenes.h,
    // light them using the point lights in the scene.
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    UNUSED(scene_id); // avoid unused warning
    // Your scene is hw1_scenes[scene_id]

    Vector3 lookfrom = Vector3{0, 0,  0};
    Vector3 lookat   = Vector3{0, 0, -1};
    Vector3 up       = Vector3{0, 1,  0};
    Real    vfov     = 45;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-lookfrom") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            lookfrom = Vector3{x, y, z};
        } else if (params[i] == "-lookat") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            lookat = Vector3{x, y, z};
        } else if (params[i] == "-up") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            Real z = std::stof(params[++i]);
            up = Vector3{x, y, z};
        } else if (params[i] == "-vfov") {
            vfov = std::stof(params[++i]);
        }
    }

    // avoid unused warnings
    UNUSED(lookfrom);
    UNUSED(lookat);
    UNUSED(up);
    UNUSED(vfov);

    Image3 img(640 /* width */, 480 /* height */);
    std::vector<std::string> parameters; //not sure
    Image3 temp;
    Vector3 W = normalize(lookfrom - lookat);
    Vector3 U = normalize(cross(up, W));
    Vector3 V = cross(W, U);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            float fovy = vfov * (M_PI/180);
            float fovxVal = tan(fovy/2.0f) * ((float)img.width/img.height);
            float alpha = fovxVal * ((x + 0.5f) - img.width/2.0f)/(img.width/2.0f);
            float beta = tan(fovy/2.0f) * (img.height/2.0f - (y + 0.5f))/(img.height/2.0f);    

            Vector3f back = normalize((alpha * (Vector3f)U) + (beta * (Vector3f)V) - (Vector3f)W);
            img(x, y) = back;
        }
    }
    temp = img;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 ray = temp(x, y);
            Sphere min;
            float minT = INFINITY;
            Vector3f tPoint;
            Vector3 norm;
            sphereIntersect(ray, min, minT, tPoint, norm, scene_id, lookfrom);
            img(x, y) = Vector3f(0.0, 0.0, 0.0);
            if(minT != INFINITY){
                for(PointLight light: hw1_scenes[scene_id].lights){
                    Vector3 lightDir = normalize(light.position - Vector3(tPoint));
                    if(dot(norm, lightDir) < 0.0f){
                        norm = -norm;
                    }
                    Sphere shadowMin;
                    Vector3f shadowTPoint;
                    float shadowMinT = INFINITY;
                    Vector3 shadowNorm;
                    Vector3 shadowRay = (Vector3)tPoint + lightDir * (Real)0.0001;
                    sphereIntersect(lightDir, shadowMin, shadowMinT, shadowTPoint, shadowNorm, scene_id, shadowRay);
                    if(shadowMinT > minT){
                        img(x, y) += (Vector3((Vector3f(hw1_scenes[scene_id].materials[min.material_id].color) * max(float(dot(norm, lightDir)), 0.0f))/float(M_PI))) * (light.intensity/distance_squared(light.position, Vector3(tPoint)));
                    }
                }
            }else{
                img(x, y) = Vector3f(0.5, 0.5, 0.5);
            }
            
        }
    }
    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: add antialiasing to homework 1.5
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = 0;
    int spp = 64;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-spp") {
            spp = std::stoi(params[++i]);
        } else {
            scene_id = std::stoi(params[i]);
        }
    }

    UNUSED(scene_id); // avoid unused warning
    UNUSED(spp); // avoid unused warning

    Image3 img(160 /* width */, 120 /* height */);

    Image3 temp;
    Camera cam = hw1_scenes[scene_id].camera;
    pcg32_state rng = init_pcg32();
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 ray;
            Sphere min;
            float minT = INFINITY;
            Vector3f tPoint;
            Vector3 norm;
            img(x, y) = Vector3f(0.0, 0.0, 0.0);
            for(int s = 0; s < spp; s++){
                double rand;
                rand = next_pcg32_real<Real>(rng);
                ray = rayPixel(img, x + rand, y + rand, cam.lookfrom, cam.lookat, cam.up, cam.vfov);
                sphereIntersect(ray, min, minT, tPoint, norm, scene_id, cam.lookfrom);
                if(minT != INFINITY){
                    for(PointLight light: hw1_scenes[scene_id].lights){
                        Vector3 lightDir = normalize(light.position - Vector3(tPoint));
                        if(dot(norm, lightDir) < 0.0f){
                            norm = -norm;
                        }
                        Sphere shadowMin;
                        Vector3f shadowTPoint;
                        float shadowMinT = INFINITY;
                        Vector3 shadowNorm;
                        Vector3 shadowRay = (Vector3)tPoint + lightDir * (Real)0.0001;
                        sphereIntersect(lightDir, shadowMin, shadowMinT, shadowTPoint, shadowNorm, scene_id, shadowRay);
                        if(shadowMinT > minT){
                            img(x, y) += color(scene_id, min, norm, lightDir, light, tPoint);
                        }
                    }
                }else{
                    img(x, y) = Vector3f(0.5, 0.5, 0.5);
                }
            }
            if(minT != INFINITY){
                img(x, y) = img(x, y)/(Real)spp;
            }
        }
    }

    return img;
}

Image3 hw_1_7(const std::vector<std::string> &params) {
    // Homework 1.7: add mirror materials to homework 1.6
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = 0;
    int spp = 64;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-spp") {
            spp = std::stoi(params[++i]);
        } else {
            scene_id = std::stoi(params[i]);
        }
    }

    UNUSED(scene_id); // avoid unused warning
    UNUSED(spp); // avoid unused warning
    // Your scene is hw1_scenes[scene_id]

    Image3 img(160 /* width */, 120 /* height */);
    // Image3 img(320, 240);
    // Image3 img(640 /* width */, 480 /* height */);

    Image3 temp;
    Camera cam = hw1_scenes[scene_id].camera;
    pcg32_state rng = init_pcg32();
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 ray;
            Sphere min;
            float minT = INFINITY;
            Vector3f tPoint;
            Vector3 norm;
            Vector3 col = Vector3f(0.0, 0.0, 0.0);
            double rand;
            for(int s = 0; s < spp; s++){
                col = Vector3f(0.0, 0.0, 0.0);
                rand = next_pcg32_real<Real>(rng);
                ray = rayPixel(img, x + rand, y + rand, cam.lookfrom, cam.lookat, cam.up, cam.vfov);
                img(x, y) += rayTrace(col, scene_id, ray, min, minT, tPoint, norm, cam.lookfrom, x, y, 0);
            }
            img(x, y) /= (double)spp;   
        }
    }
    return img;
}

Image3 hw_1_8(const std::vector<std::string> &params) {
    // Homework 1.8: parallelize HW 1.7
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = 0;
    int spp = 64;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-spp") {
            spp = std::stoi(params[++i]);
        } else {
            scene_id = std::stoi(params[i]);
        }
    }

    UNUSED(scene_id); // avoid unused warning
    UNUSED(spp); // avoid unused warning
    // Your scene is hw1_scenes[scene_id]

    // Image3 img(160 /* width */, 120 /* height */);
    Image3 img(320, 240);
    // Image3 img(640 /* width */, 480 /* height */);
    // Image3 img(1280, 960);

    Camera cam = hw1_scenes[scene_id].camera;
    constexpr int tile_size = 16;
    int num_tiles_x = (img.width + tile_size - 1) / tile_size;
    int num_tiles_y = (img.height + tile_size - 1) / tile_size;
    parallel_for([&](const Vector2i &tile) {
        int x0 = tile[0] * tile_size;
        int x1 = std::min(x0 + tile_size, img.width);
        int y0 = tile[1] * tile_size;
        int y1 = std::min(y0 + tile_size, img.height);
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                Vector3 ray;
                Sphere min = hw1_scenes[scene_id].shapes[0];
                float minT = INFINITY;
                Vector3f tPoint = Vector3f(0.0, 0.0, 0.0);
                Vector3 norm = Vector3(0.0, 0.0, 0.0);
                double rand;
                pcg32_state rng = init_pcg32();
                Vector3 col = Vector3f(0.0, 0.0, 0.0);
                for(int s = 0; s < spp; s++){
                    col = Vector3f(0.0, 0.0, 0.0);
                    rand = next_pcg32_real<Real>(rng);
                    ray = rayPixel(img, x + rand, y + rand, cam.lookfrom, cam.lookat, cam.up, cam.vfov);
                    img(x, y) += rayTrace(col, scene_id, ray, min, minT, tPoint, norm, cam.lookfrom, x, y, 0);
                }
                img(x, y) /= (double)spp;
            }
        }
    }, Vector2i(num_tiles_x, num_tiles_y));
    return img;
}
