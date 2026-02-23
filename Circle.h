/*----------------------------------------------------------
* COSC363  Ray Tracer - Assignment 2
*
*  The Circle class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CIRCLE
#define H_CIRCLE

#include <glm/glm.hpp>
#include "SceneObject.h"

class Circle : public SceneObject
{
private:
	glm::vec3 centre_ = glm::vec3(0);   //The vertices of the quad
	glm::vec3 normal_ = glm::vec3(1.0, 0.0, 0.0); 
	float radius_ = 1.0;
	
public:	
	Circle() = default;
	
	Circle(glm::vec3 centre, glm::vec3 normal, float radius) : 
		centre_(centre), normal_(normal), radius_(radius) {}

	bool isInside(glm::vec3 pt);
	
	float intersect(glm::vec3 posn, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 pt);

};

#endif //!H_CIRCLE
