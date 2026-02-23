/*==================================================================================
* COSC 363  Computer Graphics - Assignment 2
* Department of Computer Science and Software Engineering, University of Canterbury.
* Yanlong Su
* A basic ray tracer
* The program is based on lab 7 of the course. 
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>

#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Circle.h"
#include "TextureBMP.h"

using namespace std;

const float EDIST = 30.0;
const int NUMDIV = 500;
const int MAX_STEPS = 25;
const float XMIN = -20.0;
const float XMAX = 20.0;
const float YMIN = -20.0;
const float YMAX = 20.0;
const float AMBIENT = 0.2;
const float DIFFUSE = 0.0;
const float SPECULAR = 0.0;
const int MAX_DEPTH = 1;           // Max recursion for AA
const float COLOR_THRESHOLD = 0.4f; // Difference threshold to trigger more samples


float CDR = 3.14159265 / 180.0f;

TextureBMP textureButterfly;
TextureBMP textureEarth;
vector<SceneObject*> sceneObjects;

// Define parameters for box
float boxWidth = 1000;	// direction parallel to X, unit mm
float boxHight = 1000.0;	// direction parallel to Y, unit mm
float boxLength = 2000.0;	// direction parallel to Z, unit mm
glm::vec3 boxPositon(0.0f, 0.0f - boxHight / 2.0f, -1000.0f);	// The centre of the box, unit mm

// Define parameters for table
glm::vec3 tablePositon(boxPositon.x - 250, boxPositon.y, boxPositon.z);	// The centre of the table, unit mm, at y=0
float tableHeight = 150.0f; 
float tableWidth = 400.0f;
float tableLength = 400.0f;
float coeff = 0.3;

// Define parameters for texture
float textureWidth = 400 ;
float textureHeight = 536;
glm::vec3 texturePosition = glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight / 2.0 + 100, boxPositon.z - 500); // Right wall

// Define parameters for picture
float pictureWidth = 416.0f;
float pictureHeight = 536.0f;
glm::vec3 picturePosition = glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight / 2.0 + 100, boxPositon.z - 0);	// Left wall

// Define parameters for spotlight1
glm::vec3 objectSpottedLocation = glm::vec3(boxPositon.x + 0.4f * boxWidth, boxPositon.y + 25.0f, boxPositon.z + 0.075f * boxLength); // the object locationthat to be spoted

// Define common colors
glm::vec3 red(1.0f, 0.0f, 0.0f);
glm::vec3 green(0.0f, 1.0f, 0.0f);
glm::vec3 white(1.0f, 1.0f, 1.0f);
glm::vec3 blue(0.0f, 0.0f, 1.0f);
glm::vec3 yellow(1.0f, 1.0f, 0.0f);
glm::vec3 black(0.0f, 0.0f, 0.0f);
glm::vec3 grey(0.5f, 0.5f, 0.5f);
glm::vec3 silver(0.75f, 0.75f, 0.75f);
glm::vec3 timberColor(0.76f, 0.60f, 0.42f);
glm::vec3 orange(1.0, 0.647, 0.0);
glm::vec3 darkBrown(0.2f, 0.1f, 0.05f);
glm::vec3 brightBrown(0.65f, 0.32f, 0.17f);

//---A helper function to make shadow lighter for transparent and refractive object
glm::vec3 solidShadow(SceneObject *objRayHit) {
	glm::vec3 color = AMBIENT * objRayHit->getColor();
	return color;
}

glm::vec3 lighterShadow(SceneObject *objRayHit, glm::vec3 lightingColor) {
	glm::vec3 ambient = 0.2f * objRayHit->getColor();
	glm::vec3 diffSpec = lightingColor - ambient; //Find the diffuse + specular value
	glm::vec3 finalColor = ambient + 0.6f*diffSpec;
	return finalColor;
}

// For two light sources
glm::vec3 shadow(Ray primaryRay, Ray shadowRay1, Ray shadowRay2, glm::vec3 lightingColor1, glm::vec3 lightingColor2, bool isShadow1, bool isShadow2) {
	glm::vec3 color = glm::vec3(0);

	if (isShadow1 && !isShadow2) {
		if (sceneObjects[shadowRay1.index]->isRefractive() || sceneObjects[shadowRay1.index]->isTransparent()) {
			color = lightingColor2 + lighterShadow(sceneObjects[primaryRay.index], lightingColor1) - 0.2f * sceneObjects[primaryRay.index]->getColor();
		} else {
			color = lightingColor2;
		}
	}

	if (!isShadow1 && isShadow2) {
		if (sceneObjects[shadowRay2.index]->isRefractive() || sceneObjects[shadowRay2.index]->isTransparent()) {
			color = lightingColor1 + lighterShadow(sceneObjects[primaryRay.index], lightingColor2) - 0.2f * sceneObjects[primaryRay.index]->getColor();
		} else {
			color = lightingColor1;
		}
	}

	if (isShadow1 && isShadow2) {
		bool isShadow1Ligher = sceneObjects[shadowRay1.index]->isTransparent() || sceneObjects[shadowRay1.index]->isRefractive();
		bool isShadow2Ligher = sceneObjects[shadowRay2.index]->isTransparent() || sceneObjects[shadowRay2.index]->isRefractive();
		color = solidShadow(sceneObjects[primaryRay.index]);
		if (isShadow1Ligher && isShadow2Ligher) {
			color = color + lighterShadow(sceneObjects[primaryRay.index], lightingColor1) - 0.2f * sceneObjects[primaryRay.index]->getColor() +
							lighterShadow(sceneObjects[primaryRay.index], lightingColor2) - 0.2f * sceneObjects[primaryRay.index]->getColor();
		}
	}
	return color;
}

glm::vec3 applyFog(glm::vec3 color, float distance, glm::vec3 fogColor, float fogStart, float fogEnd) {
	float fogFactor = (distance - fogStart) / (fogEnd - fogStart);
	fogFactor = glm::clamp(fogFactor, 0.0f, 1.0f);
	return (1.0f - fogFactor) * color + fogFactor * fogColor;
}

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step) {
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos1(boxPositon.x + 0.45 * boxWidth, boxPositon.y + boxHight - 200, boxPositon.z + 0.1 * boxLength);					//Light's position
	glm::vec3 lightPos2(boxPositon.x - 0.45 * boxWidth, boxPositon.y + boxHight - 200, boxPositon.z + 0.1 * boxLength);
	glm::vec3 lightColor(0.5);

	glm::vec3 color(0);
	SceneObject* obj;

	ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
	if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found
	//glm::vec3 lightVec = lightPos1 - (ray.hit + 0.001f * obj->normal(ray.hit));
	glm::vec3 lightVec1 = lightPos1 - ray.hit;
	glm::vec3 lightVec2 = lightPos2 - ray.hit;
	glm::vec3 normalVec = obj->normal(ray.hit);

	// //Add patterns or textures
	if (ray.index == 24) {							// Add chequered pattern to object 14, the plane object	of table				
		int squareWidth = 45;
		int iz = floor((ray.hit.z - texturePosition.z) / squareWidth);
		int ix = floor((ray.hit.y - texturePosition.y) / squareWidth);
		if ((iz + ix) % 2 == 0) {
			color = 0.9f * white;
		} else {
			color = 0.1f * white;
		} 
		obj->setColor(color);
	}
	
	// Add code for texture mapping here
	if (ray.index == 25) {
		float a1 = picturePosition.z - pictureWidth / 2.0;
		float b1 = picturePosition.y - pictureHeight / 2.0;
		float a2 = picturePosition.z + pictureWidth / 2.0;
		float b2 = picturePosition.y + pictureHeight / 2.0;

		float texcoords = (ray.hit.z - a1) / (a2 - a1);
		float texcoordt = (ray.hit.y - b1) / (b2 - b1);
		if(texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1) {
			color = textureButterfly.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}
	}

	// Add non-planar texturing on sphere
	if (ray.index == 6) {
		// Find the point coordinate on the sphere
		Sphere* sphere = dynamic_cast<Sphere*>(sceneObjects[ray.index]);
		if (sphere) {
			glm::vec2 uvCoordinate = sphere->getTextureCoordinates(ray.hit);
			float u = uvCoordinate.x;
			float v = uvCoordinate.y;
			color = textureEarth.getColorAt(u, v);
			obj->setColor(color);
		}
	}

	// Ray casting + Phong Lighting + Shadow Ray + multishadows
	Ray shadowRay1(ray.hit + 0.001f * lightVec1, lightVec1);
	Ray shadowRay2(ray.hit + 0.001f * lightVec2, lightVec2);

	shadowRay1.closestPt(sceneObjects);
	shadowRay2.closestPt(sceneObjects);

	// Define flags to contorl flow
	bool isShadow1 = shadowRay1.index > -1 && shadowRay1.dist < glm::length(lightVec1);
	bool isShadow2 = shadowRay2.index > -1 && shadowRay2.dist < glm::length(lightVec2);

	glm::vec3 lightingColor1 = obj->lighting(lightPos1, lightColor, -ray.dir, ray.hit);	// one single light source
	glm::vec3 lightingColor2 = obj->lighting(lightPos2, lightColor, -ray.dir, ray.hit);	// one single light source

	// Handle lighting
	if (isShadow1 || isShadow2) {
		color = shadow(ray, shadowRay1, shadowRay2, lightingColor1, lightingColor2, isShadow1, isShadow2);
	} else {
		color = obj->lighting(lightPos1, lightPos2, lightColor, -ray.dir, ray.hit);
	}

	// Test for transparent and refractive objects
		// Transparency object
	if (obj->isTransparent() && step < MAX_STEPS)
	{
		float rho = obj->getTransparencyCoeff();
		Ray rayTransmitted = Ray(ray.hit, ray.dir);
		rayTransmitted.closestPt(sceneObjects);
		Ray rayExiting = Ray(rayTransmitted.hit, ray.dir);
		color = coeff * color + (1 - coeff) * trace(rayExiting, step + 1);
	}

	// Add reflections
	if (obj->isReflective() && step < MAX_STEPS)
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);
	}

	// Add refraction for sphere object
	if (obj->isRefractive() && step < MAX_STEPS) {	
		float n = sceneObjects[ray.index]->getRefractiveIndex();
		float rho = sceneObjects[ray.index]->getRefractionCoeff();

		glm::vec3 normalVecIn = obj->normal(ray.hit);
		float eta = 1.0f / n;
	
		if (glm::dot(ray.dir, normalVecIn) > 0) {
			normalVecIn = -normalVecIn;
			eta = n;  // Light exiting
		}

		glm::vec3 transmittedVec = glm::refract(ray.dir, normalVecIn, eta);
		if (glm::length(transmittedVec) > 0.0f) {		// Check for total internal reflection
			Ray refrRay(ray.hit, transmittedVec);
			refrRay.closestPt(sceneObjects);
	
			glm::vec3 normalVecOut = obj->normal(refrRay.hit);
			if (glm::dot(transmittedVec, normalVecOut) > 0) {
				normalVecOut = -normalVecOut;
			}
	
			glm::vec3 outgoingVec = glm::refract(transmittedVec, normalVecOut, 1.0f / eta);
			Ray existingRay(refrRay.hit, outgoingVec);
	
			glm::vec3 refractedColor = trace(existingRay, step + 1);
			color = color + (rho * refractedColor);
		}
	}

	// Check spot light
	glm::vec3 spotlightPos = glm::vec3(boxPositon.x - 0.0f * boxWidth, boxPositon.y + boxHight - 50, boxPositon.z + 0.1 * boxLength);
	glm::vec3 spotlightDir = glm::normalize(objectSpottedLocation - spotlightPos);
	glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 0.5f);
	float cutoffAngle = glm::cos(glm::radians(4.0f));
	glm::vec3 lightVec = glm::normalize(spotlightPos - ray.hit); // hitPoint = ray.hit
	float theta = glm::dot(-lightVec, spotlightDir);  // Negative because spotlight direction is toward the scene
	if (theta > cutoffAngle) {
		color = color + obj->lighting(spotlightPos, spotlightColor, -ray.dir, ray.hit) - glm::vec3(0.2f) * obj->getColor();
	}
	
	// Add fog
	float distance = ray.hit.z;
	//glm::vec3 fogColor(0.7f, 0.7f, 0.8f); // Light bluish fog
	glm::vec3 fogColor(1.0f, 1.0f, 1.0f); // fog
	color = applyFog(color, distance, fogColor, boxPositon.z + boxLength / 4.0f, boxPositon.z - boxLength / 2.0f);
		
	return color;
}

void simpleSample(float xp, float yp, float cellX, float cellY, glm::vec3 eye) {
	glm::vec3 dir(xp + 0.5 * cellX - eye.x, yp + 0.5 * cellY - eye.y, -EDIST - eye.z);	//direction of the primary ray
	Ray ray = Ray(eye, dir);
	glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
	glColor3f(col.r, col.g, col.b);
	glVertex2f(xp, yp);				//Draw each cell with its color value
	glVertex2f(xp + cellX, yp);
	glVertex2f(xp + cellX, yp + cellY);
	glVertex2f(xp, yp + cellY);
}

void adaptiveSample(float xp, float yp, float cellX, float cellY, int depth, glm::vec3 eye) {
	// Sample 4 corners
	glm::vec3 dirC1(xp + 0.0 * cellX - eye.x, yp + 0.0 * cellY - eye.y, -EDIST - eye.z);	//direction of the primary ray
	glm::vec3 dirC2(xp + 1.0 * cellX - eye.x, yp + 0.0 * cellY - eye.y, -EDIST - eye.z);	//direction of the primary ray
	glm::vec3 dirC3(xp + 0.0 * cellX - eye.x, yp + 1.0 * cellY - eye.y, -EDIST - eye.z);	//direction of the primary ray
	glm::vec3 dirC4(xp + 1.0 * cellX - eye.x, yp + 1.1 * cellY - eye.y, -EDIST - eye.z);	//direction of the primary ray

	Ray rayC1 = Ray(eye, dirC1);
	Ray rayC2 = Ray(eye, dirC2);
	Ray rayC3 = Ray(eye, dirC3);
	Ray rayC4 = Ray(eye, dirC4);


	glm::vec3 c1 = trace(rayC1, 1);
	glm::vec3 c2 = trace(rayC2, 1);
	glm::vec3 c3 = trace(rayC3, 1);
	glm::vec3 c4 = trace(rayC4, 1);

    // Compute average and variance
    glm::vec3 avgColor = 0.25f * (c1 + c2 + c3 + c4);
    float variance = glm::length(c1 - avgColor)
                   + glm::length(c2 - avgColor)
                   + glm::length(c3 - avgColor)
                   + glm::length(c4 - avgColor);
	if (depth >= MAX_DEPTH || variance < COLOR_THRESHOLD) {
		glColor3f(avgColor.r, avgColor.g, avgColor.b);
		glVertex2f(xp, yp);				//Draw each cell with its color value
		glVertex2f(xp + cellX, yp);
		glVertex2f(xp + cellX, yp + cellY);
		glVertex2f(xp, yp + cellY);		
	} else {
		// Subdivide further: 4 sub-quadrants
		float halfX = cellX / 2.0f;
		float halfY = cellY / 2.0f;

		adaptiveSample(xp, yp, halfX, halfY, depth + 1, eye);
		adaptiveSample(xp + halfX, yp, halfX, halfY, depth + 1, eye);
		adaptiveSample(xp, yp + halfY, halfX, halfY, depth + 1, eye);
		adaptiveSample(xp + halfX, yp + halfY, halfX, halfY, depth + 1, eye);
	}
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display() {
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++) {	//Scan every cell of the image plane
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++) {
			yp = YMIN + j * cellY;
			//simpleSample(xp, yp, cellX, cellY, eye); 
			adaptiveSample(xp, yp, cellX, cellY, 0, eye);
		}
	}
	glEnd();
	glFlush();
}

//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize() {
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

	textureButterfly = TextureBMP("Butterfly.bmp");
	textureEarth = TextureBMP("Earth.bmp");

	glClearColor(0, 0, 0, 1);	    // Colors
	
	// Add box - object index = 0
	Plane *boxLeft = new Plane (glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y, boxPositon.z + boxLength / 2.0), //Point A
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y, boxPositon.z - boxLength / 2.0), //Point B
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z - boxLength / 2.0), //Point C
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z + boxLength / 2.0)); //Point D
	boxLeft->setColor(red);
	boxLeft->setSpecularity(false);
	sceneObjects.push_back(boxLeft);

	//object index = 1
	Plane *boxRight = new Plane (glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y, boxPositon.z + boxLength / 2.0), //Point A
								glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z + boxLength / 2.0), //Point D
								glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z - boxLength / 2.0), //Point C
								glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y, boxPositon.z - boxLength / 2.0)); //Point B
	boxRight->setColor(green);
	boxRight->setSpecularity(false);
	sceneObjects.push_back(boxRight);

	//object index = 2
	Plane *boxTop = new Plane (glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z + boxLength / 2.0), //Point A
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z + boxLength / 2.0), //Point D
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z - boxLength / 2.0), //Point C
								glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z - boxLength / 2.0)); //Point B
	boxTop->setColor(white);
	boxTop->setSpecularity(false);
	sceneObjects.push_back(boxTop);
	
	//object index = 3
	Plane *boxBottom = new Plane (glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y, boxPositon.z + boxLength / 2.0), //Point A
									glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y, boxPositon.z - boxLength / 2.0), //Point B
									glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y, boxPositon.z - boxLength / 2.0), //Point C
									glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y, boxPositon.z + boxLength / 2.0)); //Point D
	boxBottom->setColor(brightBrown);
	boxBottom->setSpecularity(false);
	sceneObjects.push_back(boxBottom);
	
	//object index = 4
	Plane *boxBack = new Plane (glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y, boxPositon.z - boxLength / 2.0), //Point A
								glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z - boxLength / 2.0), //Point B
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z - boxLength / 2.0), //Point C
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y, boxPositon.z - boxLength / 2.0)); //Point D
	boxBack->setColor(yellow);
	boxBack->setSpecularity(false);
	sceneObjects.push_back(boxBack);

	//object index = 5
	Plane *boxFront = new Plane (glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y, boxPositon.z + boxLength / 2.0), //Point A
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y, boxPositon.z + boxLength / 2.0), //Point B
								glm::vec3(boxPositon.x - boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z + boxLength / 2.0), //Point C
								glm::vec3(boxPositon.x + boxWidth / 2.0, boxPositon.y + boxHight, boxPositon.z + boxLength / 2.0)); //Point D
	boxFront->setColor(white);
	boxFront->setSpecularity(false);
	sceneObjects.push_back(boxFront);

	//object index = 6
	glm::vec3 sphere1Location = glm::vec3(boxPositon.x + 400, boxPositon.y + 400, boxPositon.z - 0.15 * boxLength);
	Sphere *sphere1 = new Sphere(sphere1Location, 75.0);
	sphere1->setColor(blue);   //Set colour to blue
	sceneObjects.push_back(sphere1);		 //Add sphere to scene objects
	//Set tranformation
	glm::mat4 transformForSphere1 = glm::mat4(1.0);
	transformForSphere1 = glm::translate(transformForSphere1, sphere1Location);
	transformForSphere1 = glm::rotate(transformForSphere1, glm::radians(210.0f), glm::vec3(0.0, 1.0, 0.0));
	transformForSphere1 = glm::translate(transformForSphere1, -sphere1Location);
	sphere1->setTransform(transformForSphere1);


	//object index = 7
	Cylinder *cylinder1 = new Cylinder(glm::vec3(boxPositon.x + 400, boxPositon.y, boxPositon.z - 0.15 * boxLength), 75.0, 400 - 75);
	cylinder1->setColor(red); 
	cylinder1->setCapped(true);
	sceneObjects.push_back(cylinder1);		 //Add sphere to scene objects

	//object index = 8
	glm::vec3 sphere2Location = glm::vec3(boxPositon.x + 400, boxPositon.y + 200, boxPositon.z - 0 * boxLength);
	Sphere *sphere2 = new Sphere(sphere2Location, 50.0);
	sphere2->setColor(blue);   //Set colour to blue
	sceneObjects.push_back(sphere2);		 //Add sphere to scene objects
	//Set tranformation
	glm::mat4 transformTBD = glm::mat4(1.0);
	transformTBD = glm::translate(transformTBD, sphere2Location);
	transformTBD = glm::scale(transformTBD, glm::vec3(0.5, 1.0, 0.5));
	transformTBD = glm::translate(transformTBD, -sphere2Location);
	sphere2->setTransform(transformTBD);

	//object index = 9
	Cylinder *cylinder2 = new Cylinder(glm::vec3(boxPositon.x + 400, boxPositon.y, boxPositon.z - 0 * boxLength), 50.0, 200 - 50);
	cylinder2->setColor(red);  
	cylinder2->setCapped(true);
	sceneObjects.push_back(cylinder2);		 //Add sphere to scene objects

	// Add a table to the scene
	//object index = 10
	Cylinder *tableLeg1 = new Cylinder(glm::vec3(tablePositon.x - coeff * tableWidth, tablePositon.y, tablePositon.z - coeff * tableLength), 10, tableHeight-1);
	tableLeg1->setColor(red);   //Set colour to blue
	tableLeg1->setCapped(true);
	sceneObjects.push_back(tableLeg1);		 //Add cylinder to scene objects

	//object index = 11
	Cylinder *tableLeg2 = new Cylinder(glm::vec3(tablePositon.x - coeff * tableWidth, tablePositon.y, tablePositon.z + coeff * tableLength), 10, tableHeight-1);
	tableLeg2->setColor(red);   //Set colour to blue
	tableLeg2->setCapped(true);
	sceneObjects.push_back(tableLeg2);		 //Add cylinder to scene objects

	//object index = 12
	Cylinder *tableLeg3 = new Cylinder(glm::vec3(tablePositon.x + coeff * tableWidth, tablePositon.y, tablePositon.z - coeff * tableLength), 10, tableHeight-1);
	tableLeg3->setColor(red);   //Set colour to blue
	tableLeg3->setCapped(true);
	sceneObjects.push_back(tableLeg3);		 //Add cylinder to scene objects

	//object index = 13
	Cylinder *tableLeg4 = new Cylinder(glm::vec3(tablePositon.x + coeff * tableWidth, tablePositon.y, tablePositon.z + coeff * tableLength), 10, tableHeight-1);
	tableLeg4->setColor(red);   //Set colour to blue
	tableLeg4->setCapped(true);
	sceneObjects.push_back(tableLeg4);		 //Add cylinder to scene objects

	//object index = 14
	Plane *tableSurface = new Plane (glm::vec3(tablePositon.x - 0.5 * tableWidth, tablePositon.y + tableHeight, tablePositon.z - 0.5 * tableLength), //Point A
									glm::vec3(tablePositon.x - 0.5 * tableWidth, tablePositon.y + tableHeight, tablePositon.z + 0.5 * tableLength), //Point B
									glm::vec3(tablePositon.x + 0.5 * tableWidth, tablePositon.y + tableHeight, tablePositon.z + 0.5 * tableLength), //Point C
									glm::vec3(tablePositon.x + 0.5 * tableWidth, tablePositon.y + tableHeight, tablePositon.z - 0.5 * tableLength)); //Point D
	tableSurface->setColor(brightBrown);
	sceneObjects.push_back(tableSurface);

	// Add a non transparent and non reflective sylinder on the table
	//object index = 15
	Cylinder *cylinderOnTable = new Cylinder(glm::vec3(tablePositon.x - 0.3 * tableWidth, tablePositon.y + tableHeight, tablePositon.z - 0.3 * tableLength), 30, 200);
	cylinderOnTable->setColor(glm::vec3(0.2, 0.8, 0));  
	sceneObjects.push_back(cylinderOnTable);		 //Add cylinder to scene objects

	// // Add a non transparent and non reflective scone on the table
	//object index = 16
	Cone *coneOnTable = new Cone(glm::vec3(tablePositon.x - 0.35 * tableWidth, tablePositon.y + tableHeight, tablePositon.z - 0.09 * tableLength), 30, 150);
	coneOnTable->setColor(yellow); 
	sceneObjects.push_back(coneOnTable);		 //Add cylinder to scene objects

	//Add a transparent sphere object
	//object index = 17
	Sphere *sphereOnTable = new Sphere(glm::vec3(tablePositon.x - 0.0 * tableWidth, tablePositon.y + tableHeight + 75, tablePositon.z + 0.25 * tableLength), 75.0);
	sphereOnTable->setColor(blue);   //Set colour to blue
	sphereOnTable->setTransparency(true, 0.3);
	sceneObjects.push_back(sphereOnTable);		 //Add sphere to scene objects

	// Add a refractive sphere object
	//object index = 18
	Sphere *sphere3 = new Sphere(glm::vec3(boxPositon.x + 200, boxPositon.y + 500, boxPositon.z - 0 * boxLength), 75.0);
	sphere3->setColor(blue);   //Set colour to blue
	sphere3->setRefractivity(true);
	//sphere3->setRefractivity(true, 0.9, 1.001);
	sceneObjects.push_back(sphere3);		 //Add sphere to scene objects

	//object index = 19
	Cylinder *cylinder3 = new Cylinder(glm::vec3(boxPositon.x + 200, boxPositon.y, boxPositon.z - 0 * boxLength), 25.0, 500 - 75);
	cylinder3->setColor(red);   //Set colour to blue
	cylinder3->setCapped(true);
	sceneObjects.push_back(cylinder3);		 //Add sphere to scene objects

	// Add a refractive sphere object
	//object index = 20
	Sphere *sphere4 = new Sphere(glm::vec3(boxPositon.x, boxPositon.y + 500, boxPositon.z - 0 * boxLength), 75.0);
	sphere4->setColor(black);   //Set colour to blue
	//sphere4->setRefractivity(true);
	sphere4->setRefractivity(true, 0.9, 1.5);
	sceneObjects.push_back(sphere4);		 //Add sphere to scene objects

	//object index = 21
	Cylinder *cylinder4 = new Cylinder(glm::vec3(boxPositon.x, boxPositon.y, boxPositon.z - 0 * boxLength), 25.0, 500 - 75.0);
	cylinder4->setColor(glm::vec3(red));   //Set colour to blue
	cylinder4->setCapped(true);
	sceneObjects.push_back(cylinder4);		 //Add sphere to scene objects>

	// Add a reflective sphere object
	//object index = 22
	Circle *mirror = new Circle(glm::vec3(boxPositon.x - 0.5f * boxWidth + 5, boxPositon.y + 300, boxPositon.z + 0.05f * boxLength), glm::vec3(1.0f, 0.0f, 0.0f), 125.0);
	mirror->setColor(black);   //Set colour
	mirror->setReflectivity(true, 0.9);
	sceneObjects.push_back(mirror);		 //Add sphere to scene objects

	//object index = 23
	Circle *mirrorFrame = new Circle(glm::vec3(boxPositon.x - 0.5f * boxWidth + 5, boxPositon.y + 300, boxPositon.z + 0.05f * boxLength), glm::vec3(1.0f, 0.0f, 0.0f), 140.0);
	mirrorFrame->setColor(timberColor);   //Set colour
	sceneObjects.push_back(mirrorFrame);		 //Add sphere to scene objects

	// Add a rectanglar plane with chequered pattern
	//object index = 24
	Plane *texture = new Plane (glm::vec3(texturePosition.x - 0.1f, texturePosition.y - textureHeight / 2.0f, texturePosition.z - textureWidth / 2.0f), //Point A
								glm::vec3(texturePosition.x - 0.1f, texturePosition.y - textureHeight / 2.0f, texturePosition.z + textureWidth / 2.0f), //Point B
								glm::vec3(texturePosition.x - 0.1f, texturePosition.y + textureHeight / 2.0f, texturePosition.z + textureWidth / 2.0f), //Point C
								glm::vec3(texturePosition.x - 0.1f, texturePosition.y + textureHeight / 2.0f, texturePosition.z - textureWidth / 2.0f)); //Point D
	texture->setColor(white);
	sceneObjects.push_back(texture);

	// Add a rectanglar plane with picture texture
	//object index = 25
	Plane *picture = new Plane (glm::vec3(picturePosition.x - 0.1f, picturePosition.y - pictureHeight / 2.0f, picturePosition.z - pictureWidth / 2.0f), //Point A
								glm::vec3(picturePosition.x - 0.1f, picturePosition.y - pictureHeight / 2.0f, picturePosition.z + pictureWidth / 2.0f), //Point B
								glm::vec3(picturePosition.x - 0.1f, picturePosition.y + pictureHeight / 2.0f, picturePosition.z + pictureWidth / 2.0f), //Point C
								glm::vec3(picturePosition.x - 0.1f, picturePosition.y + pictureHeight / 2.0f, picturePosition.z - pictureWidth / 2.0f)); //Point D
	picture->setColor(white);
	sceneObjects.push_back(picture);

	// Add a sphere on floor for testing multiple shadows, this sphere has a earth texture mapped.
	//object index = 26
	Sphere *earthShpere = new Sphere(glm::vec3(boxPositon.x + 100.0f, boxPositon.y + 50.0f, boxPositon.z + 0.05f * boxLength), 50.0f);
	earthShpere->setColor(white);   //Set colour to blue
	sceneObjects.push_back(earthShpere);		 //Add sphere to scene objects

	// Add a transformed sphere on ground
	//object index = 27
	glm::vec3 ellipsoidLocation  = glm::vec3(objectSpottedLocation);
	Sphere *ellipsoid = new Sphere(ellipsoidLocation, 25.0);
	ellipsoid->setColor(orange);   //Set colour to orange
	sceneObjects.push_back(ellipsoid);		 //Add sphere to scene objects
	//Set tranformation
	glm::mat4 transformForEllipsoid = glm::mat4(1.0);
	transformForEllipsoid = glm::translate(transformForEllipsoid, ellipsoidLocation);
	transformForEllipsoid = glm::rotate(transformForEllipsoid, 45 * CDR, glm::vec3(0.0, 1.0, 0.0));
	transformForEllipsoid = glm::scale(transformForEllipsoid, glm::vec3(1.5, 1.0, 1.5));
	transformForEllipsoid = glm::translate(transformForEllipsoid, -ellipsoidLocation);
	ellipsoid->setTransform(transformForEllipsoid);

}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Raytracing");

	glutDisplayFunc(display);
	initialize();

	glutMainLoop();
	return 0;
}
