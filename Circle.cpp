/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Circle class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Circle.h"
#include <math.h>

/**
* Circle's intersection method.  The input is a ray (p0, dir).
*/
float Circle::intersect(glm::vec3 p0, glm::vec3 dir) {
	glm::vec3 n = normal(p0);
	glm::vec3 vdif = centre_ - p0;
	float d_dot_n = glm::dot(dir, n);
	if(fabs(d_dot_n) < 1.e-4) return -1;   //Ray parallel to the Circle

    float t = glm::dot(vdif, n)/d_dot_n;
	if(t < 0) return -1;

	glm::vec3 q = p0 + dir*t; //Point of intersection
	if( isInside(q) ) return t; //Inside the Circle
	else return -1; //Outside

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Circle.
*/
glm::vec3 Circle::normal(glm::vec3 p) {
	return glm::normalize(normal_);
}

/**
* 
* Checks if a point q is inside the current polygon
* See slide Lec09-Slide 33
*/
bool Circle::isInside(glm::vec3 q) {
	glm::vec3 centreQVec = q - centre_;
	if (glm::length(centreQVec) < radius_) {
		return true;
	}
	return false;
}


