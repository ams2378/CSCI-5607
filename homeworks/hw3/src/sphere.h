#ifndef SPHEREH
#define SPHEREH

#include "object.h"

class Sphere: public Object {
    public:
        virtual Intersection hit(Ray &ray, int objectNumber) const; //(const ray& r, float tmin, float tmax, hit_record& rec) const;
        virtual string objectType() const;
        virtual void print() const;
        virtual void set_material(Material* mat);
        virtual Material* get_material() const;
        virtual void freeMem() const; 
        virtual vec3 getSurfaceNormal(vec3 p, vec3 dir) const;

        virtual vec3 get_center();
        virtual float get_radius();

        Sphere() {}
        Sphere(vec3 cen, float r) : center(cen), radius(r) {};
        
        ~Sphere();

        //virtual void set_material(vec3 ambient, vec3 diffuse, vec3 specular, float ns, vec3 transmissive, float ior);
        vec3 center;
        float radius;
        Material* mat_ptr;
};

#endif



