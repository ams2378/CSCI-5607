
#include "light.h"

/******************************************************************************************/
/* Ambient light 																		  */
/******************************************************************************************/

void AmbientLight::print() const {
	printf("Ambient light color %f, %f, %f \n", color.x(), color.y(), color.z());
}

string AmbientLight::lightType() const {
	string name;
	name = "ambient";
	return name;
}

vec3 AmbientLight::getColor() {
    return color;
}

/******************************************************************************************/
/* Point Light 																			  */
/******************************************************************************************/


void PointLight::print() const {
	printf("Point light color %f, %f, %f \n", color.x(), color.y(), color.z());
	printf("Point light location %f, %f, %f \n", location.x(), location.y(), location.z());
}

string PointLight::lightType() const {
	string name;
	name = "point";
	return name;
}

vec3 PointLight::getColor() {
    return color;
}

vec3 PointLight::getLocation() {
    return location;
}

/******************************************************************************************/
/* Spot light 																			  */
/******************************************************************************************/

void SpotLight::print() const {
	printf("Spot light color %f, %f, %f \n", color.x(), color.y(), color.z());
	printf("Spot light location %f, %f, %f \n", location.x(), location.y(), location.z());
	printf("Spot light location %f, %f, %f \n", direction.x(), direction.y(), direction.z());
	printf("Spot light angle1 and angle2 %f %f \n", angle1, angle2); 
}

string SpotLight::lightType() const {
	string name;
	name = "spot";
	return name;
}

vec3 SpotLight::getColor() {
    return color;
}

vec3 SpotLight::getLocation() {
    return location;
}

vec3 SpotLight::getDirection() {
    return direction;
}

float SpotLight::getAngle1() {
    return angle1;
}

float SpotLight::getAngle2() {
    return angle2;
}