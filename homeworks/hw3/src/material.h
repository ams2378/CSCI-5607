
#ifndef MATERIAL
#define MATERIAL 

#include "vec3.h"

class Material  {
    
public:
	vec3 	ambient;
	vec3 	diffuse;
	vec3 	specular;
	float 		ns;
	vec3	transmissive;
	float		ior;

    Material() {}
    Material (vec3 a, vec3 d, vec3 s, vec3 t, float n, float i) { 
    	ambient = a; 
    	diffuse = d; 
    	specular = s;
    	transmissive = t;
    	ns = n;
    	ior = i;
    }

	void print();
	~Material() {};

};

#endif





