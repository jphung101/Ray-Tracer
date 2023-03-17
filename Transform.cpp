// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
	mat3 back;
	float rad = glm::radians(degrees);
	vec3 axisNorm = normalize(axis);

	back = cos(rad) * mat3(1.0f);
	back += (1 - cos(rad)) * mat3(axisNorm[0] * axisNorm[0], axisNorm[0] * axisNorm[1], axisNorm[0] * axisNorm[2], 
                                axisNorm[0] * axisNorm[1], axisNorm[1] * axisNorm[1], axisNorm[1] * axisNorm[2], 
                                axisNorm[0] * axisNorm[2], axisNorm[1] * axisNorm[2], axisNorm[2] * axisNorm[2]);
	back += sin(rad) * mat3(0.0f, axisNorm[2], -axisNorm[1],
                          -axisNorm[2], 0.0f, axisNorm[0],
                          axisNorm[1], -axisNorm[0], 0.0f);
	return back;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
  eye = rotate(degrees, up) * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
  vec3 axis = normalize(glm::cross(up, eye));
	eye = rotate(-degrees, axis) * eye; 
	up = glm::cross(eye, axis);
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
	vec3 w = normalize(eye);
	vec3 u = normalize(glm::cross(up, w));
	vec3 v = normalize(glm::cross(w, u));

	mat4 back = mat4(u[0], v[0], w[0], 0.0f,
                  u[1], v[1], w[1], 0.0f,
                  u[2], v[2], w[2], 0.0f,
                  -glm::dot(u, eye), -glm::dot(v, eye), -glm::dot(w, eye), 1.0f); 
	return back;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  mat4 back;
  float rad = glm::radians(fovy)/2;
  float d = 1.0f/tan(rad);

  back = mat4(d/aspect, 0.0f, 0.0f, 0.0f,
              0.0f, d, 0.0f, 0.0f, 
              0.0f, 0.0f, -(zFar + zNear)/(zFar - zNear), -1.0f,
              0.0f, 0.0f, -(2* zFar* zNear)/(zFar - zNear), 0.0f);
  return back;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
  mat4 back = mat4(sx, 0.0f, 0.0f, 0.0f,
                  0.0f, sy, 0.0f, 0.0f,
                  0.0f, 0.0f, sz, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);
  return back;


}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
  mat4 back = mat4(1.0f, 0.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 1.0f, 0.0f,
                  tx, ty, tz, 1.0f);
  return back;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
