#ifndef READFILE_H
#define READFILE_H
#include <iostream>
#include <fstream> //read and write to file (ifstream + ofstream)
#include <sstream> //read and write to strings
#include <string>

#include "Object.h"
#include "Scene.h"
#include "Transform.h"

void matransform (stack<glm::mat4> &transfstack,  float* values);
void rightmultiply (const glm::mat4 & M, stack<glm::mat4> &transfstack);
bool readvals (stringstream &s, const int numvals, float* values);
string readfile (const char* filename, Scene scene);


#endif

