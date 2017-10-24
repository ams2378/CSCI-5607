
#ifndef LIGHT
#define LIGHT

#include <iostream>
#include <string>

#include "vec3.h"

using namespace std;

// Lights
// 1. directional
// 2. point
// 3. spot
// 4. ambient

// Abstruct base class
class Light
{
    public:

        virtual string lightType() const = 0;
        virtual void print() const = 0;  

        virtual vec3 getLocation() 		{return vec3(0,0,0);}
        virtual vec3 getColor()			{return vec3(0,0,0);}
        virtual vec3 getDirection()		{return vec3(0,0,0);}
        virtual float getAngle1()  		{return 0;}
        virtual float getAngle2() 		{return 0;}

};

/******************************************************************************************/
/* Ambient light 																		  */
/******************************************************************************************/

class AmbientLight: public Light {
    public:
        virtual string lightType() const;
        virtual void print() const;
        virtual vec3 getColor();

        AmbientLight() {}
        AmbientLight(vec3 col) : color(col) {};
        
        ~AmbientLight() {};

        //rgb color
        vec3 color;
};


/******************************************************************************************/
/* Point Light 																			  */
/******************************************************************************************/

class PointLight: public Light {
    public:
        virtual string lightType() const;
        virtual void print() const;
        virtual vec3 getLocation();
        virtual vec3 getColor();

        PointLight() {}
        PointLight(vec3 col, vec3 loc) : color(col), location(loc) {};
        
        ~PointLight() {};

        //rgb color
        vec3 color;
        vec3 location;
};


/******************************************************************************************/
/* Directional light																	  */
/******************************************************************************************/

typedef PointLight DirectionalLight;


/******************************************************************************************/
/* Spot light 																			  */
/******************************************************************************************/

class SpotLight: public Light {
    public:
        virtual string lightType() const;
        virtual void print() const;
        virtual vec3 getLocation();
        virtual vec3 getColor();
        virtual vec3 getDirection();
        virtual float getAngle1();
        virtual float getAngle2();

        SpotLight() {}
        SpotLight(vec3 col, vec3 loc, vec3 dir, float a1, float a2) : color(col), location(loc), direction(dir), angle1(a1), angle2(a2) {};
        
        ~SpotLight() {};

        //rgb color
        vec3 color;
        vec3 location;
        vec3 direction;
        float angle1;
        float angle2;
};


#endif










