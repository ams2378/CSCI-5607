

#include "vector.h"
#include <math.h> 
#include <stdio.h>  

Vec operator+ (const Vec& p, const Vec& q)
{
    return Vec(p.x+q.x, p.y+q.y, p.z+q.z);
}

Vec operator- (const Vec& p, const Vec& q)
{
    return Vec(p.x-q.x, p.y-q.y, p.z-q.z);
}

Vec operator* (const Vec& p, double t)
{
    return Vec(p.x*t, p.y*t, p.z*t);
}

Vec operator/ (const Vec& p, double t)
{
    return Vec(p.x/t, p.y/t, p.z/t);
}

Vec cross (const Vec& p, const Vec& q) {

/*
    uvi = u2 * v3 - v2 * u3;
    uvj = v1 * u3 - u1 * v3;
    uvk = u1 * v2 - v1 * u2;
*/

	float uvi, uvj, uvk;
    
    uvi = p.y * q.z - q.y * p.z;
    uvj = q.x * p.z - p.x * q.z;
    uvk = p.x * q.y - q.x * p.y;

    return Vec(uvi, uvj, uvk);
}


Vec dot (const Vec& p, const Vec& q) {

	/*
		a · b = ax × bx + ay × by
	*/

	float dotProduct = p.x * q.x + p.y * q.y + p.z * q.z;

    return dotProduct;
}

void Vec::normalize(){
	float val = sqrt(x*x + y*y + z*z);
	x = x/val;
	y = y/val;
	z = z/val;
}

void Vec::print(){
	printf("X Y Z: %f, %f, %f\n", x,y,z);
}