/*--------------------------------------------------------------
* COSC363  Ray Tracer
*
*  The SceneObject class
*  This is a generic type for storing objects in the scene
*  Sphere, Plane etc. must be defined as subclasses of SceneObject.
*  Being an abstract class, this class cannot be instantiated.
-----------------------------------------------------------------*/

#include "SceneObject.h"

glm::vec3 SceneObject::getColor() {
	return color_;
}

// Single light source
glm::vec3 SceneObject::lighting(glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewVec, glm::vec3 hit) {
    float ambientTerm = 0.2f;
	float specularTerm = 0.0f;
    float specularStrength = 0.5f;
	
    glm::vec3 normalVec = normal(hit);
    glm::vec3 lightVec = glm::normalize(lightPos - hit);
    glm::vec3 viewDir = glm::normalize(viewVec);

	float lDotn = glm::max(glm::dot(lightVec, normalVec), 0.0f);

    if (spec_) {
        glm::vec3 reflVec = glm::reflect(-lightVec, normalVec);
        float rDotv = glm::max(glm::dot(reflVec, viewDir), 0.0f);
        specularTerm = pow(rDotv, shin_);
    }

	glm::vec3 ambient = ambientTerm * color_;
    glm::vec3 diffuse = lDotn * color_ * lightColor;
    glm::vec3 specular = specularTerm * specularStrength * lightColor;
	glm::vec3 finalColor = ambient + diffuse + specular;

	return finalColor;
}

// Double light sources
glm::vec3 SceneObject::lighting(glm::vec3 lightPos1, glm::vec3 lightPos2, glm::vec3 lightColor, glm::vec3 viewVec, glm::vec3 hit) {
	float ambientTerm = 0.2;
	float specularStrength = 0.25f;

    float spec1 = 0.0f;
    float spec2 = 0.0f;

    glm::vec3 normalVec = normal(hit);
    glm::vec3 viewDir = glm::normalize(viewVec);

    glm::vec3 lightVec1 = glm::normalize(lightPos1 - hit);
    glm::vec3 lightVec2 = glm::normalize(lightPos2 - hit);

	float lDotn1 = glm::max(glm::dot(lightVec1, normalVec), 0.0f);
    float lDotn2 = glm::max(glm::dot(lightVec2, normalVec), 0.0f);

	if (spec_) {
        glm::vec3 reflVec1 = glm::reflect(-lightVec1, normalVec);
        glm::vec3 reflVec2 = glm::reflect(-lightVec2, normalVec);

        float rDotv1 = glm::max(glm::dot(reflVec1, viewDir), 0.0f);
        float rDotv2 = glm::max(glm::dot(reflVec2, viewDir), 0.0f);

        spec1 = pow(rDotv1, shin_);
        spec2 = pow(rDotv2, shin_);
	}

	glm::vec3 diffuse = (lDotn1 + lDotn2) * color_ * lightColor;
    glm::vec3 specular = (spec1 + spec2) * specularStrength * lightColor;
    glm::vec3 ambient = ambientTerm * color_;

	glm::vec3 finalColor = ambient + diffuse + specular;
	return finalColor;
}

float SceneObject::getReflectionCoeff() {
	return reflc_;
}

float SceneObject::getRefractionCoeff() {
	return refrc_;
}

float SceneObject::getTransparencyCoeff() {
	return tranc_;
}

float SceneObject::getRefractiveIndex() {
	return refri_;
}

float SceneObject::getShininess() {
	return shin_;
}

bool SceneObject::isReflective() {
	return refl_;
}

bool SceneObject::isRefractive() {
	return refr_;
}


bool SceneObject::isSpecular() {
	return spec_;
}


bool SceneObject::isTransparent() {
	return tran_;
}

void SceneObject::setColor(glm::vec3 col) {
	color_ = col;
}

void SceneObject::setReflectivity(bool flag) {
	refl_ = flag;
}

void SceneObject::setReflectivity(bool flag, float refl_coeff) {
	refl_ = flag;
	reflc_ = refl_coeff;
}

void SceneObject::setRefractivity(bool flag) {
	refr_ = flag;
}

void SceneObject::setRefractivity(bool flag, float refr_coeff, float refr_index) {
	refr_ = flag;
	refrc_ = refr_coeff;
	refri_ = refr_index;
}

void SceneObject::setShininess(float shininess) {
	shin_ = shininess;
}

void SceneObject::setSpecularity(bool flag) {
	spec_ = flag;
}

void SceneObject::setTransparency(bool flag) {
	tran_ = flag;
}

void SceneObject::setTransparency(bool flag, float tran_coeff) {
	tran_ = flag;
	tranc_ = tran_coeff;
}