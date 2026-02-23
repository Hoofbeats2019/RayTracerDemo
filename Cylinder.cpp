/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cylinder class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include <iostream>
#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray. 
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir) {
	// ax^2 + bx + c = 0
	float a = dir.x * dir.x + dir.z * dir.z;
	float b = 2 * (dir.x * (p0.x - centre.x) + dir.z * (p0.z - centre.z));
	float c = pow(p0.x - centre.x, 2) + pow(p0.z - centre.z, 2) - pow(radius, 2);
	float delta = b * b - 4 * a * c;

	if(delta < 0.001) return -1.0;    //includes zero and negative values

	float t1 = (-b - sqrt(delta)) / (2 * a);
	float t2 = (-b + sqrt(delta)) / (2 * a);

	glm::vec3 p1(p0 + t1 * dir);
	glm::vec3 p2(p0 + t2 * dir);

	float upperH = centre.y + height;
	float lowerH = centre.y;

	if (p1.y > upperH && (p2.y <= upperH && p2.y >= lowerH)) {
		if (isCapped()) {
			return (centre.y + height - p0.y) / dir.y;
		} else {
			return t2;
		}
	} else if (p1.y >= lowerH && p1.y <= upperH && t1 >= 0) {
		return t1;
	} else {
		return -1;
	}
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p) {
	float upperH = centre.y + height;
	float lowerH = centre.y;
	
	if (isCapped() && std::abs(p.y - upperH) < 0.001) {
		return glm::vec3(0, 1, 0);
	} else {
		glm::vec3 centreH(centre.x, p.y, centre.z);
		glm::vec3 n = p - centreH;
		n = glm::normalize(n);
		return n;
	}
}

bool Cylinder::isCapped() {
	return capped_;
}

void Cylinder::setCapped(bool flag) {
	capped_ = flag;
}