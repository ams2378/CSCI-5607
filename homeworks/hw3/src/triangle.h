#ifndef TRIANGLE
#define TRIANGLE

#include "object.h"

class Triangle: public Object  {

    public:

        virtual Intersection hit(Ray &ray, int objectNumber) const; //(const ray& r, float tmin, float tmax, hit_record& rec) const;
        virtual string objectType() const;
        virtual void print() const;
        virtual void set_material(Material* mat);
        virtual Material* get_material() const;


        // Triangle specific methods
        virtual vec3 get_vertex0();
        virtual vec3 get_vertex1();
        virtual vec3 get_vertex2();
        virtual vec3 get_normal0();
        virtual vec3 get_normal1();
        virtual vec3 get_normal2();
        virtual void freeMem() const;   
        virtual vec3 getSurfaceNormal(vec3 p, vec3 dir) const;

        Triangle() {}
        Triangle(vec3 vtx0, vec3 vtx1, vec3 vtx2, vec3 nr0, vec3 nr1, vec3 nr2, bool has_n) : 
        v0(vtx0), v1(vtx1), v2(vtx2), n0(nr0), n1(nr1), n2(nr2), has_normal(has_n) {};
        ~Triangle ();

        vec3 v0;
        vec3 v1;
        vec3 v2;
        vec3 n0;
        vec3 n1;
        vec3 n2;
        //float tu;
        //float tv;
        bool has_normal;
        Material* mat_ptr;
};

#endif
