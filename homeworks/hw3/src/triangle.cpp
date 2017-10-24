

#include "triangle.h"

Intersection Triangle::hit(Ray& ray, int objectNumber) const {

    Intersection intersect;
    intersect.hit = false;

    float gm, bt;
    vec3 dir = ray.direction;
    vec3 loc = ray.origin;

    float a = v0.x() - v1.x();
    float b = v0.y() - v1.y();
    float c = v0.z() - v1.z();
    float d = v0.x() - v2.x();
    float e = v0.y() - v2.y();
    float f = v0.z() - v2.z();
    float g = dir.x();
    float h = dir.y();
    float i = dir.z();
    float j = v0.x() - loc.x();
    float k = v0.y() - loc.y();
    float l = v0.z() - loc.z();
    float m = e*i - h*f;
    float n = g*f - d*i;
    float o = d*h - e*g;
    float p = a*k - j*b;
    float q = j*c - a*l;
    float r = b*l - k*c;
    float s = a*m + b*n + c*o;

    float t = -(f*p + e*q + d*r)/s;
    gm = (i*p + h*q + g*r)/s;
    bt = (j*m + k*n + l*o)/s;

    if (t < 0)                      return intersect; 
    if (!(gm >= 0 && gm <= 1))      return intersect; 
    if (!(bt >= 0 && bt <= 1-gm))   return intersect;

    intersect.hit = true;
    intersect.t = t;
    intersect.objectNumber = objectNumber; 
    intersect.intersection = ray.origin + ray.direction*t; 
    return intersect;
}

string Triangle::objectType() const {
    string name;
    name = "triangle";
    return name;
}

void Triangle::print() const {
    printf("Vertex v0 %f, %f, %f \n", v0.x(), v0.y(), v0.z());
    printf("Vertex v1 %f, %f, %f \n", v1.x(), v1.y(), v1.z());
    printf("Vertex v2 %f, %f, %f \n", v2.x(), v2.y(), v2.z());    
    if (has_normal)
        printf("Normal present\n");
    else
        printf("Normal not present\n");
    printf("Normal n0 %f, %f, %f \n", n0.x(), n0.y(), n0.z());
    printf("Normal n1 %f, %f, %f \n", n1.x(), n1.y(), n1.z());
    printf("Normal n2 %f, %f, %f \n", n2.x(), n2.y(), n2.z());     
    mat_ptr->print();
}


void Triangle::set_material(Material* mat) {
    mat_ptr = mat;
}

Material* Triangle::get_material() const {
    return mat_ptr;
}

vec3 Triangle::get_vertex0() {
    return v0;
}

vec3 Triangle::get_vertex1() {
    return v1;
}

vec3 Triangle::get_vertex2() {
    return v2;
}

vec3 Triangle::get_normal0() {
    return n0;
}

vec3 Triangle::get_normal1() {
    return n1;
}

vec3 Triangle::get_normal2() {
    return n2;
}

vec3 Triangle::getSurfaceNormal(vec3 p, vec3 dir) const {
    vec3 N;
    vec3 e0 = v1 - v0;
    vec3 e1 = v2 - v1;
    float invertN = 1;
    e0.make_unit_vector();    
    e1.make_unit_vector();
    N = cross(e0, e1);
    N.make_unit_vector();

    float nDotDir = dot(dir, N);

    if (nDotDir > 0)
        invertN = -1;
    return N * invertN;
}

Triangle::~Triangle() {
    mat_ptr->~Material();
}

void Triangle::freeMem() const {
    Triangle::~Triangle();
}

