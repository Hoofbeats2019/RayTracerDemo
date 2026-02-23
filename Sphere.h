/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_SPHERE
#define H_SPHERE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Sphere : public SceneObject {
private:
	glm::vec3 center = glm::vec3(0);
	float radius = 1;
	glm::mat4 transform = glm::mat4(1);
	glm::mat4 normalTransform = glm::mat4(1);

public:
	Sphere() {};  //Default constructor creates a unit sphere

	Sphere(glm::vec3 c, float r) : center(c), radius(r) {}

	float intersect(glm::vec3 p0, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

	void setTransform(glm::mat4 transformPassed);

	glm::vec3 getCentre();
	glm::vec2 getTextureCoordinates(glm::vec3 pointOnSphere);
};

#endif //!H_SPHERE
