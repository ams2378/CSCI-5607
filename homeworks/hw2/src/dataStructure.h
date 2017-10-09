
#include "vector.h"

/*
typedef CoOrdinate vector;

class CoOrdinate {

	public:
		float x;
		float y;
		float z;

	public:
		CoOrdinate (float m_x, float m_y, float m_z);

		~CoOrdinate ();

		CoOrdinate add (CoOrdinate addand) {
			CoOrdinate newCoord = new (x+addand.x, y+addand.y, z+addand.z);
			return newCoord;
		}

		CoOrdinate sub (CoOrdinate addand) {
			CoOrdinate newCoord = new (x-addand.x, y-addand.y, z-addand.z);
			return newCoord;
		}

		CoOrdinate mult (float mult) {
			CoOrdinate newCoord = new (x*mult, y*mult, z*mult);
			return newCoord;
		}

		void normalize () {
			float val = sqrt(x*x + y*y + z*z);
			x /= val;
			y /= val;
			z /= val;
		}

		float val (CoOrdinate v) {
			float val = sqrt(x*x + y*y + z*z);
			return val;
		}
};
*/

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


struct Ray {
	CoOrdinate	origin;
	CoOrdinate	direction;
};

struct Sphere {
	CoOrdinate	origin;
	float	 	r;
};


struct Scene {
	Sphere	sphere0;
};

typedef CoOrdinate Intersection;