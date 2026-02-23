/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray. 
*/
float Sphere::intersect(glm::vec3 p0, glm::vec3 dir) {
	p0 = transform * glm::vec4(p0, 1.0);
	dir = transform * glm::vec4(dir, 0.0);
	float tScale = glm::length(dir);
	dir = glm::normalize(dir);

	glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)
	float b = glm::dot(dir, vdif);
	float len = glm::length(vdif);
	float c = len*len - radius*radius;
	float delta = b*b - c;

	if(delta < 0.001) return -1.0;    //includes zero and negative values

	float t1 = -b - sqrt(delta);
	float t2 = -b + sqrt(delta);

	t1 /= tScale;
	t2 /= tScale;

	if (t1 < 0)
	{
		return (t2 > 0) ? t2 : -1;
	}
	else return t1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Sphere::normal(glm::vec3 p) {
	p = transform * glm::vec4(p, 1.0);
	glm::vec3 n = p - center;
	n = normalTransform * glm::vec4(n, 0.0);
	n = glm::normalize(n);
	return n;
}

void Sphere::setTransform(glm::mat4 transformPassed) {
	transform = glm::inverse(transformPassed);
	normalTransform = glm::transpose(transform);
}

glm::vec3 Sphere::getCentre() {
	return center;
}

glm::vec2 Sphere::getTextureCoordinates(glm::vec3 pointOnSphere) {
	// Calculate spherical coordinates
	glm::vec4 pointTransformed = transform * glm::vec4(pointOnSphere, 1);		//Apply tranformation
	glm::vec3 point = glm::vec3(pointTransformed.x, pointTransformed.y, pointTransformed.z);
	glm::vec3 dVec = point - center;
	dVec = glm::normalize(dVec);

	float theta = -atan2(dVec.z, dVec.x); // azimuthal angle (-π to π)
	float phi = asin(dVec.y);    // polar angle (-π/2 to π/2)

	// Map spherical coordinates to texture coordinates (u, v)
	float u = 0.5f + theta / (2 * M_PI);    // map [0, 2π] -> [0, 1]
	float v = 0.5f + phi / (M_PI);          // map [-π/2, π/2] -> [0, 1]

	return glm::vec2(u, v);
}

