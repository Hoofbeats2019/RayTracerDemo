/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include <iostream>
#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray. 
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 dir) {
	// ax^2 + bx + c = 0
	float a = pow(dir.x, 2) + pow(dir.z, 2) - pow(radius / height * dir.y, 2);
	float b = 2 * (dir.x * (p0.x - centre.x) + dir.z * (p0.z - centre.z) + pow(radius / height, 2) * dir.y * (height - (p0.y - centre.y)));
	float c = pow(p0.x - centre.x, 2) + pow(p0.z - centre.z, 2) - pow(radius / height, 2) * pow(height - (p0.y - centre.y), 2);
	float delta = b * b - 4 * a * c;

	if(delta < 0.001) return -1.0;    //includes zero and negative values

	float t1 = (-b - sqrt(delta)) / (2 * a) - 0.1;	//0.01 to avoid shadow acne
	float t2 = (-b + sqrt(delta)) / (2 * a);

	glm::vec3 p1(p0 + t1 * dir);
	glm::vec3 p2(p0 + t2 * dir);

	float upperH = centre.y + height;
	float lowerH = centre.y;

	if (p1.y >= lowerH && p1.y <= upperH && t1 > 0) {
		return t1;
	} else if ((p1.y < lowerH || p1.y > upperH) && (p2.y >= lowerH && p2.y <= upperH)) {
		return t2;
	} else {
		return -1;
	}
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sylinder.
*/
glm::vec3 Cone::normal(glm::vec3 p) {
	glm::vec3 apex = centre + glm::vec3(0, height, 0);  // Apex is top of cone
    glm::vec3 v = p - apex;  // Vector from apex to point

    // Compute the slope of the cone side
    float k = radius / height;

    // Adjust Y to make the vector perpendicular to the cone surface
    float r = sqrt(v.x * v.x + v.z * v.z);
    v.y = k * r;

    glm::vec3 n = glm::normalize(v);
    return n;
	// float cita = atan(radius / height);
	// glm::vec3 apex(centre.x, centre.y + height, centre.z);
	// float lengthPointApex = glm::length(apex - p);
	// glm::vec3 centreR(centre.x, apex.y - lengthPointApex / cos(cita), centre.z);
	// glm::vec3 n = glm::vec3(p - centreR);
	// n = glm::normalize(n);
	// return n;
}
