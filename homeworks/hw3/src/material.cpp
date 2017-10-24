
#include "material.h"

void Material::print() {
	printf("Ambient light %f, %f, %f \n", ambient.x(), ambient.y(), ambient.z()); 
	printf("Diffuse light %f, %f, %f \n", diffuse.x(), diffuse.y(), diffuse.z()); 
	printf("Specular light %f, %f, %f \n", specular.x(), specular.y(), specular.z()); 
	printf("transmissive light %f, %f, %f \n", transmissive.x(), transmissive.y(), transmissive.z()); 
	printf("ns , ior %f %f \n", ns, ior);
}