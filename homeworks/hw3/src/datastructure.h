#ifndef DS_INCLUDED
#define DS_INCLUDED

#include "vector.h"
#include <fstream>
#include <cstring>
#include <vector>

#include "light.h"
#include "vec3.h"
#include "object.h"

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


typedef vector<Object*> Scene;
typedef vector<Light*> LightArray;

struct Camera {
	vec3 	origin;
	vec3 		direction;
	vec3 		up;
	float		ha;
};

struct  Resolution {
	int height;
	int width;
};

struct Setup {
	Camera 		camera;
	Resolution 	res;
	vec3 		background;
	string    	outfile;
	int 		max_depth;
	LightArray  light;
};

#endif


