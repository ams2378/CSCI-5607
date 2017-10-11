#ifndef DS_INCLUDED
#define DS_INCLUDED

#include "vector.h"
#include <fstream>
#include <cstring>

#define PI 3.1416

using namespace std;

/*
	Camera :
	Thes specifies the camera parmeters.
	(px, py, pz) Is the POSITION.
	(dx, dy, dz) is the viewing DIRECTION. 
	(ux, uy, uz) is the UP vector. 
	“ha” is one-half of the “height” angle of the viewing frustum. 
*/

struct Camera {
	CoOrdinate 	origin;
	Vec 		direction;
	Vec 		up;
	float		ha;
};

struct  Resolution {
	int height;
	int width;
};

struct Setup {
	Camera 		camera;
	Resolution 	res;
	CoOrdinate 	background;
	string    	outfile;
	int 		max_depth;
};

struct Ray {
	CoOrdinate	origin;
	CoOrdinate	direction;
};


struct Material {
	CoOrdinate 	ambient;
	CoOrdinate 	diffuse;
	CoOrdinate 	specular;
	float 		ns;
	CoOrdinate	transmissive;
	float		ior;

	void print();
};


struct Sphere {
	CoOrdinate	origin;
	float	 	r;

	Material	material;
};


struct Intersection {
	CoOrdinate	intersection;
	int         objectNumber;
	bool		hit;
	float     	t;
};

//typedef CoOrdinate Intersection;


// Light sources

struct DirectionalLight {
	CoOrdinate 	color;
	Vec 		location;

    // Constructor
    DirectionalLight (CoOrdinate x_=CoOrdinate(0,0,0), Vec y_=Vec(0,0,0)) : color(CoOrdinate(x_)), location(Vec(y_)) {}
};

typedef DirectionalLight PointLight;
typedef CoOrdinate AmbientLight;

struct SpotLight {
	CoOrdinate 	color;
	CoOrdinate 	location;
	Vec 		direction;
	float		angle1;
	float		angle2;

	// Constructor
    SpotLight (CoOrdinate x_=CoOrdinate(0,0,0), Vec y_=Vec(0,0,0), Vec z_=Vec(0,0,0), float angle1_=0, float angle2_=0) : 
    color(CoOrdinate(x_)), location(CoOrdinate(y_)), direction(Vec(z_)), angle1(angle1_), angle2(angle2_) {}
};

struct Light {
	DirectionalLight 	dirLight;
	SpotLight			spotLight;
	PointLight			pointLight;
	AmbientLight		ambientLight;
};


struct Scene {
	Sphere 	sphere0[50];
	int     numSpheres;
	Light   light;
};

#endif


