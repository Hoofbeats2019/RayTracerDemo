/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CONE
#define H_CONE
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple cone located at (xc, yc, zc)
 * with the specified radius and height
 */
class Cone : public SceneObject {
private:
	glm::vec3 centre = glm::vec3(0);
	float radius = 1.0f;
	float height = 1.0f;

public:
	Cone() {};  //Default constructor creates a unit sylinder
	Cone(glm::vec3 c, float r, float h) : centre(c), radius(r), height(h) {}
	float intersect(glm::vec3 p0, glm::vec3 dir);
	glm::vec3 normal(glm::vec3 p);
};

#endif //!H_CONE
