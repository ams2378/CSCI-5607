
// Vec
#ifndef VEC
#define VEC

#include <stdint.h>
#include <math.h> 
#include <stdio.h>  

struct Vec
{
    // Data
    float x;
    float y;
    float z;

    // Constructor
    Vec (float x_=0, float y_=0, float z_=0) : x(x_), y(y_), z(z_) {}

    void normalize();
    void print();

};

Vec operator+ (const Vec& p, const Vec& q);

Vec operator- (const Vec& p, const Vec& q);

Vec operator* (const Vec& p, double f);

Vec operator/ (const Vec& p, double f);

Vec cross (const Vec& p, const Vec& q);

typedef Vec CoOrdinate;


#endif