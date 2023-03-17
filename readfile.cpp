#include "readfile.h"


void matransform(stack<glm::mat4> &transfstack,  float* values){
    glm::mat4 transform = transfstack.top(); 
    glm::vec4 valvec = glm::vec4(values[0],values[1],values[2],values[3]); 
    glm::vec4 newval = transform * valvec; 
    for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}
void rightmultiply(const glm::mat4 & M, stack<glm::mat4> &transfstack){
    glm::mat4 &T = transfstack.top(); 
    T = T * M; 
}
bool readvals(stringstream &s, const int numvals, float* values){
    for (int i = 0; i < numvals; i++){
        s >> values[i]; 
        if (s.fail()){
        cout << "Failed reading value " << i << " will skip\n"; 
        return false;
        }
    }
    return true; 
}
string readfile(const char* filename, Scene scene){
    string back = "";
    string str, cmd;
    ifstream in;
    in.open(filename);
    if(in.is_open()){
        stack<glm::mat4> transfstack;
        transfstack.push(glm::mat4(1.0));

        vector<glm::vec3> vertexBuffer; //holds all vertices
        vector<glm::vec3> vertexBufferNorm;

        Attenuation tempAttenuation;
        glm::vec3 tempDiffuse, tempSpecular, tempEmission, tempAmbient;
        float tempShininess;



        getline(in, str);
        while(in){
            if((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')){ //ignore comments/blanks
                stringstream s(str);
                s >> cmd;
                float values[10]; //max 10 param
                if(cmd == "size"){
                    if(readvals(s, 2, values)){
                        scene.size.x = values[0];
                        scene.size.y = values[1];
                    }
                }else if(cmd == "maxdepth"){
                    if(readvals(s, 1, values)){
                        scene.maxDepth = values[0];
                    }
                }else if(cmd == "output"){ //we are putting output in Image/Main
                    if(readvals(s, 0, values)){
                        // scene. = values[0];
                    }
                }else if(cmd == "camera"){
                    if(readvals(s, 10, values)){
                        glm::vec3 eye = glm::vec3(values[0], values[1], values[2]);
                        glm::vec3 center = glm::vec3(values[3], values[4], values[5]);
                        glm::vec3 up = glm::vec3(values[6], values[7], values[8]);
                        Camera cam = Camera(eye, center, up, values[9]);
                        scene.camera = cam;
                    }
                }else if(cmd == "sphere"){ 
                    if(readvals(s, 4, values)){
                        glm::vec3 coor = glm::vec3(values[0], values[1], values[2]);
                        Sphere sp = Sphere(coor, values[3]);
                        sp.diffuse = tempDiffuse;
                        sp.specular = tempSpecular;
                        sp.emission = tempEmission;
                        sp.ambient = tempAmbient;
                        sp.shininess = tempShininess;
                        sp.transform = transfstack.top();
                        scene.objects.push_back(sp);
                    }
                }else if(cmd == "maxverts"){ //ignore for now
                    if(readvals(s, 1, values)){
                        // scene. = values[0];
                    }
                }else if(cmd == "maxvertnorms"){ //ignore for now
                    if(readvals(s, 1, values)){
                        // scene. = values[0];
                    }
                }else if(cmd == "vertex"){ 
                    if(readvals(s, 3, values)){
                        glm::vec3 coor = glm::vec3(values[0], values[1], values[2]);
                        vertexBuffer.push_back(coor);
                    }
                }else if(cmd == "vertexnormal"){ 
                    if(readvals(s, 6, values)){
                        glm::vec3 coor = glm::vec3(values[0], values[1], values[2]);
                        glm::vec3 coorNorm = glm::vec3(values[3], values[4], values[5]);
                        vertexBufferNorm.push_back(coor); //every EVEN index (x)
                        vertexBufferNorm.push_back(coorNorm); //every ODD index (x+1)
                    }
                }else if(cmd == "tri"){ 
                    if(readvals(s, 3, values)){
                        glm::vec3 a = vertexBuffer[values[0]];
                        glm::vec3 b = vertexBuffer[values[1]];
                        glm::vec3 c = vertexBuffer[values[2]];
                        Triangle tri = Triangle(a, b, c);
                        tri.diffuse = tempDiffuse;
                        tri.specular = tempSpecular;
                        tri.emission = tempEmission;
                        tri.ambient = tempAmbient;
                        tri.shininess = tempShininess;
                        tri.transform = transfstack.top();
                        scene.objects.push_back(tri);
                    }
                }else if(cmd == "trinormal"){ //might not need
                    if(readvals(s, 3, values)){
                        //need transformation stack
                        glm::vec3 a = vertexBufferNorm[values[0] * 2];
                        glm::vec3 aNorm = vertexBufferNorm[(values[0] * 2) + 1];
                        glm::vec3 b = vertexBufferNorm[values[1] * 2];
                        glm::vec3 bNorm = vertexBufferNorm[(values[1] * 2) + 1];
                        glm::vec3 c = vertexBufferNorm[values[2] * 2];
                        glm::vec3 cNorm = vertexBufferNorm[(values[2] * 2) + 1];
                        Triangle tri = Triangle(a, b, c);
                        tri.verticesNorm[0] = aNorm;
                        tri.verticesNorm[1] = bNorm;
                        tri.verticesNorm[2] = cNorm;
                        tri.diffuse = tempDiffuse;
                        tri.specular = tempSpecular;
                        tri.emission = tempEmission;
                        tri.ambient = tempAmbient;
                        tri.shininess = tempShininess;
                        tri.transform = transfstack.top();
                        scene.objects.push_back(Triangle(a, b, c));
                    }
                }else if(cmd == "translate"){ 
                    if(readvals(s, 3, values)){
                        glm::mat4 trans = Transform::translate(values[0], values[1], values[2]);
                        rightmultiply(trans, transfstack);
                    }
                }else if(cmd == "rotate"){ 
                    if(readvals(s, 4, values)){
                        glm::vec3 axis = glm::vec3(values[0], values[1], values[2]);
                        glm::mat4 rot = mat4(Transform::rotate(values[3], axis));
                        rightmultiply(rot, transfstack);
                    }
                }else if(cmd == "scale"){ 
                    if(readvals(s, 3, values)){
                        glm::mat4 scale = Transform::scale(values[0], values[1], values[2]);
                        rightmultiply(scale, transfstack);
                    }
                }else if(cmd == "pushTransform"){
                    transfstack.push(transfstack.top()); 
                }else if(cmd == "popTransform"){
                    if(transfstack.size() <= 1){
                    cerr << "Stack has no elements.  Cannot Pop\n"; 
                    }else{
                        transfstack.pop(); 
                    }
                }else if(cmd == "directional"){ 
                    if(readvals(s, 6, values)){
                        glm::vec3 dir = glm::vec3(values[0], values[1], values[2]);
                        glm::vec3 col = glm::vec3(values[3], values[4], values[5]);
                        Light light = Light(dir, col);
                        light.type = Light::directional;
                        scene.lights.push_back(light);
                    }
                }else if(cmd == "point"){ 
                    if(readvals(s, 6, values)){
                        glm::vec3 point = glm::vec3(values[0], values[1], values[2]);
                        glm::vec3 col = glm::vec3(values[3], values[4], values[5]);
                        Light light = Light(point, col);
                        light.type = Light::point;
                        light.attenuation = tempAttenuation;
                        scene.lights.push_back(light);
                    }
                }else if(cmd == "attenuation"){ 
                    if(readvals(s, 3, values)){
                        Attenuation atten;
                        atten.constant = values[0];
                        atten.linear = values[1];
                        atten.quadratic = values[2];
                        tempAttenuation = atten;
                    }
                }else if(cmd == "ambient"){ 
                    if(readvals(s, 3, values)){
                        tempAmbient = glm::vec3(values[0], values[1], values[2]);
                    }
                }else if(cmd == "diffuse"){ 
                    if(readvals(s, 3, values)){
                        tempDiffuse = glm::vec3(values[0], values[1], values[2]);
                    }
                }else if(cmd == "specular"){ 
                    if(readvals(s, 3, values)){
                        tempSpecular = glm::vec3(values[0], values[1], values[2]);
                    }
                }else if(cmd == "emission"){ 
                    if(readvals(s, 3, values)){
                        tempEmission = glm::vec3(values[0], values[1], values[2]);
                    }
                }else if(cmd == "shininess"){ 
                    if(readvals(s, 1, values)){
                        tempShininess = values[0];
                    }
                }else if(cmd == "output"){
                    if(readvals(s, 1, values)){
                        back = values[0];
                    }
                }
            }
            getline (in, str); 
        }
    }else{
        cerr << "Unable to Open Input Data File " << filename << "\n"; 
    throw 2; 
  }
  return back;
}

