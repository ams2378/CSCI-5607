#ifndef OBJECT
#define OBJECT 

#include <iostream>
#include <string>

//#include "ray.h"
#include "vec3.h"
#include "material.h"
//#include "datastructure.h"

using namespace std;

struct hit_record
{
    float t;  
    vec3 p;
    vec3 normal; 
//    Material *mat_ptr; // FIXME
};

struct Intersection {
	vec3	intersection;
	int     objectNumber;
	bool	hit;
	float   t;
};


struct Ray {
	vec3	origin;
	vec3	direction;
};

class Object  {
    public:

    	// Pure viftual functions
    	// implement hit/miss math
        virtual Intersection hit(Ray &ray, int objectNumber) const = 0; //const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
        // returns object type- either triangle or sphere
        virtual string objectType() const = 0;     
        // print object parameters (i.e. vertex, material properties etc)
        virtual void print() const = 0;  
        // set material properties
        virtual void set_material(Material* mat) = 0; 
        // return pointer to the object storing material properties
        virtual Material* get_material() const = 0;
        // cleanup memory
        virtual void freeMem() const = 0; 
        virtual vec3 getSurfaceNormal(vec3 p, vec3 dir) const = 0;

        // non pure method- based class will implement these based on their type (i.e. sphere or triangle)
        // return center of the sphere
        virtual vec3 get_center()  { return vec3(0,0,0); };
        // return radius of sphere
        virtual float get_radius() { return 0;};
        // return three vertices of a triangle   
        virtual vec3 get_vertex0() { return vec3(0,0,0); };
        virtual vec3 get_vertex1() { return vec3(0,0,0); };
        virtual vec3 get_vertex2() { return vec3(0,0,0); };
        // return normal to three vertices of a triangle 
        virtual vec3 get_normal0() { return vec3(0,0,0); };
        virtual vec3 get_normal1() { return vec3(0,0,0); };
        virtual vec3 get_normal2() { return vec3(0,0,0); }; 

};


#endif




