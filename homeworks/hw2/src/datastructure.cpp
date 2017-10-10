

#include "datastructure.h"
#include <math.h> 
#include <stdio.h> 

void Material::print() {
	printf("Ambient light "); 
	ambient.print(); 
    printf("Diffuse light "); 
	diffuse.print(); 
    printf("Specular light "); 
	specular.print(); 
	printf("transmissive light "); 
	transmissive.print(); 
	printf("ns , ior %f %f \n", ns, ior);
}

/*
Intersection operator= (const Intersection& q)
{
	intersection p;

	p.intersection.x = q.intersection.x;
	p.intersection.y = q.intersection.y;
	p.intersection.z = q.intersection.z;
	p.objectNumber = q.objectNumber;
	p.hit = q.hit;
	p.t = q.t;
    

    return p;
}
*/