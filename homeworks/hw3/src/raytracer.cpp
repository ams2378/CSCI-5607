/*
*
* Fundamentals of Computer Graphics - CSCI 5607 Fall 2017
* Authod: Adil Sadik
*         sadik.adil@gmail.com
* 
* Description: Homework 2 part 1 
*
*/

#ifndef RAYTRACER
#define RAYTRACER

#include <math.h>
#include <float.h>
#include <cmath>
#include <iomanip>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vec3.h"
#include "fileParser.h"
#include "image.h"
#include "datastructure.h"

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

//#define PI 3.1416
#define EN_DEBUG0 
//#define LIGHT_OFF
//#define EN_DEBUG 

using namespace std;

vec3 P0, P1, P2, P3; // FIXME  global variable- not good!


float clamp (float p) {
  return (p < 0 ? 0 : (p > 255 ? 255 : p));
}

Intersection IntersectMath (Ray&  ray, Scene& scene, int i) {

  float X0, Y0, Z0;       // Origin
  float Xd, Yd, Zd;       // Direction
  float Xc, Yc, Zc, Sr;   // Sphere center and redius
  float Xi, Yi, Zi;       // Intersection point on sphere

  X0 = ray.origin.x();
  Y0 = ray.origin.y();
  Z0 = ray.origin.z();

  Xd = ray.direction.x();
  Yd = ray.direction.y();
  Zd = ray.direction.z();

  Xc = scene[i]->get_center().x();
  Yc = scene[i]->get_center().y();
  Zc = scene[i]->get_center().z();
  Sr = scene[i]->get_radius();

  float A = Xd*Xd + Yd*Yd + Zd*Zd;
  float B = 2 * (Xd * (X0 - Xc) + Yd * (Y0 - Yc) + Zd * (Z0 - Zc));
  float C = (X0 - Xc)*(X0 - Xc) + (Y0 - Yc)*(Y0 - Yc) + (Z0 - Zc)*(Z0 - Zc) - Sr*Sr;

  float dis = B*B - 4*A*C;

  float t0 = (-B - sqrt(dis))/(2*A);
  float t1 = (-B + sqrt(dis))/(2*A);

#ifdef EN_DEBUG
  printf("ray origin X Y Z %f %f %f\n", X0, Y0, Z0);
  printf("ray direction X Y Z %f %f %f\n", Xd, Yd, Zd);
  printf("sphere Xc Yc Zc Sr %f %f %f %f\n", Xc, Yc, Zc, Sr);
  printf("dis %f \n", dis);
#endif

  // Find the smallest t
  float t = t0 < t1 ? t0 : t1;

  // find intersection point
  Xi = X0 + Xd * t;
  Yi = Y0 + Yd * t;
  Zi = Z0 + Zd * t;

  Intersection intersect;
  intersect.intersection = vec3 (Xi, Yi, Zi);
  intersect.objectNumber = i; 
  intersect.t = t;

  if (t >= 0) {
    intersect.hit = true;
  } else { 
    intersect.hit = false;
  }

  return intersect;
}

// Find intersection with the objects 
Intersection FindIntersection(Ray& ray, Scene& scene) {

    Intersection t, currentIntersect;
    currentIntersect.hit  = false;
    float currentDistance = std::numeric_limits<float>::max();

    for (int i=0; i<scene.size(); i++) {
      t = scene[i]->hit(ray, i); 

      if (t.hit) {

        if (t.t < currentDistance) {
          currentIntersect.intersection = vec3(t.intersection.x(), t.intersection.y(), t.intersection.z());
          currentIntersect.objectNumber = t.objectNumber;
          currentIntersect.hit = t.hit;
          currentIntersect.t = t.t;
          currentDistance = t.t;
        } 
      }  
    }

    return currentIntersect;
}


// R(t) = R0 + t * Rd , t > 0 with R0 = [X0, Y0, Z0] and Rd = [Xd, Yd, Zd]
Ray ConstructRayThroughPixel (Camera camera, int i, int j, int width, int height) {

  Ray newRay;
  vec3 Px1, Px2;

  // Assuming sample point is in the middle of the pixel
  float offset = .5;

  /*
    Interpolate pixel co-ordinate (i,j) from image plane to world space
    using P0, P1, P2, P3
   
    P0 - top left
    P1 - top right
    P2 - bottom left
    P3 - bottom right

       -----> i
         P1     Px1           P1
         - - - - x- - - - - - - 
      |  |                    |
      |  |                    |  
    j |  |       x pixelCoord |    
      v  |                    |   
         - - - - x- - - - - - -   
        P2      Px2           P3

    First find Px1 and Px2 and then interpolate pixelCoord
  */

  Px1 = P1 + (P0-P1) * (i+offset)/width;
  Px2 = P3 + (P2-P3) * (i+offset)/width;

  vec3 pixelCoord = Px1 + (Px2-Px1) * (j+offset)/height;

  // Direction- from origin to pixel location
  vec3 v = (pixelCoord-camera.origin);
  // Normalize the direction to make math simple
  v.make_unit_vector();

  // Set up Ray origin and direction
  newRay.origin = camera.origin;
  newRay.direction = v;

#ifdef EN_DEBUG
  printf("ConstructRayThroughPixel i j ============================== %d %d\n", i, j);
  pixelCoord.print();
  printf("vector ray direction\n");
  v.print();
  printf("\n");
#endif

  return newRay;
}


vec3 ambientColor(vec3 lightColor, vec3 materialColor) {

    float r, g, b;

    return vec3(materialColor.x() * lightColor.x(), 
                materialColor.y() * lightColor.y(), 
                materialColor.z() * lightColor.z());

}

inline float clamp(const float &lo, const float &hi, const float &v) { 
  return std::max(lo, std::min(hi, v)); 
}


vec3 reflect(const vec3 &I, const vec3 &N)
{
    return I - 2 * dot(I, N) * N;
}

vec3 refract(const vec3 &I, const vec3 &N, const float &ior)
{
    float cosi = clamp(-1, 1, dot(I, N));
    float etai = 1, etat = ior;
    vec3 n = N;
    if (cosi < 0) { 
      cosi = -cosi; 
    } else { 
      std::swap(etai, etat); 
      n= -N; 
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? vec3(0,0,0) : eta * I + (eta * cosi - sqrtf(k)) * n;
}

// Return color of the pixel
vec3 getColor(Intersection& intersect, Setup& setup, Scene& scene, Ray& ray, int depth) {

  if (depth > setup.max_depth) {
      return setup.background;
  }

  vec3 ambient, diffuse, specular, finalColor;
  vec3 intersectToLight, surfaceNormal, v, r;
  float vDotR, surfaceNormalDotIntersectToLight;
  float spotLightTheta;

  float distance = intersect.t;
  int objectNumber = intersect.objectNumber;
  float bias = 0.00001;
  vec3 hitColor = setup.background;
  bool enableSpot = 0;

  vec3 hitPoint = intersect.intersection;
  vec3 dir = ray.direction; 
  //vec3 N = scene[objectNumber]->getSurfaceNormal(hitPoint, dir); 

  vec3 diffuseColor = vec3(0,0,0), specularColor = vec3(0,0,0);

  vec3 N = scene[objectNumber]->getSurfaceNormal(hitPoint, dir); 
  vec3 shadowPointOrig = (dot(dir, N) < 0) ? hitPoint + N * bias : hitPoint - N * bias;

  string lightTypeAmbient = "ambient";

  for (int i = 0; i < setup.light.size(); ++i) {
    if (setup.light[i]->lightType() != lightTypeAmbient) {
      Intersection t;
      Ray hitPointToLight; 
      vec3 lightDir = setup.light[i]->getLocation() - hitPoint;    
      // square of the distance between hitPoint and the light
      float lightDistance2 = dot(lightDir, lightDir);
      lightDir.make_unit_vector();
      float LdotN = std::max(0.f, dot(lightDir, N));

      // Create a new ray from the hitpoint towards light
      hitPointToLight.origin    = shadowPointOrig;
      hitPointToLight.direction = lightDir;
      t = FindIntersection (hitPointToLight, scene);
      bool inShadow = t.hit && t.t * t.t < lightDistance2;

      if (setup.light[i]->lightType() == "spot") {  // Spot light
        vec3 spotLightDir = setup.light[i]->getDirection();
        spotLightDir.make_unit_vector();
        spotLightTheta = acos(dot(-1*lightDir, spotLightDir)) * 180/PI;

        if (!isnan(spotLightTheta) && (spotLightTheta < setup.light[i]->getAngle1() || spotLightTheta < setup.light[i]->getAngle2())) {
          enableSpot = 1;
        }
      } else {
        enableSpot = 1;
      }

      vec3 reflectionDirection = reflect(-lightDir, N);

      diffuseColor  += enableSpot * (1 - inShadow) * setup.light[i]->getColor() * LdotN * scene[objectNumber]->get_material()->diffuse * (1/(lightDistance2));
      specularColor += enableSpot * (1 - inShadow) * powf(std::max(0.f, -dot(reflectionDirection, dir)), scene[objectNumber]->get_material()->ns) * setup.light[i]->getColor() * scene[objectNumber]->get_material()->specular * (1/(lightDistance2));
    } else {
      //printf("Working on ambient\n");
      ambient = ambientColor(setup.light[i]->getColor(), scene[objectNumber]->get_material()->ambient);  
    }
  }
 
  hitColor = ambient + diffuseColor + specularColor; 

  vec3 reflectionDirection = reflect(dir, N);
  reflectionDirection.make_unit_vector();
  vec3 refractionDirection = refract(dir, N, scene[objectNumber]->get_material()->ior);
  refractionDirection.make_unit_vector();
  vec3 reflectionRayOrig = (dot(reflectionDirection, N) < 0) ? hitPoint - N * bias : hitPoint + N * bias;
  vec3 refractionRayOrig = (dot(refractionDirection, N) < 0) ? hitPoint - N * bias : hitPoint + N * bias;

  Ray reflectRay, refractRay;
  reflectRay.origin     = reflectionRayOrig;
  reflectRay.direction  = reflectionDirection;
  refractRay.origin     = refractionRayOrig;
  refractRay.direction  = refractionDirection;

  Intersection reflectT, refractT;

  reflectT = FindIntersection (reflectRay, scene);
  refractT = FindIntersection (refractRay, scene);

  vec3 reflectionColor = setup.background;
  vec3 refractionColor = setup.background;
  
  if (reflectT.hit) 
    reflectionColor = getColor(reflectT, setup, scene, reflectRay, depth+1);
  else
    return hitColor;
  
  hitColor += reflectionColor * scene[objectNumber]->get_material()->specular;
  
  if (refractT.hit) 
    refractionColor = getColor(refractT, setup, scene, reflectRay, depth+1);
  else
    return hitColor;
  
  hitColor += refractionColor * scene[objectNumber]->get_material()->transmissive;
  
#ifdef EN_DEBUG
  printf("Point light color and location \n");
  light.pointLight.color.print();
  light.pointLight.location.print();
  printf("Sphere origin \n");
  scene[objectNumber]->get_center().print();
  printf("surfaceNormal\n");
  surfaceNormal.print();
  if (intersect.hit) printf("HIT\n");
  printf("surfaceNormalDotIntersectToLight %f \n", surfaceNormalDotIntersectToLight);

  printf("ambient color\n");
  ambient.print();

  printf("diffuse color\n");
  diffuse.print(); 

  printf("final color \n");
  finalColor.print(); 
#endif

#ifdef LIGHT_OFF
  p.r = p.g = p.b = 100;
#endif
  
  return hitColor;

}


void RayCast(Setup& setup, Scene& scene, int width, int height){ 

  vec3 origin         = setup.camera.origin;
  vec3 viewingVec      = setup.camera.direction;
  vec3 upVec           = setup.camera.up; 
  upVec.make_unit_vector();
  viewingVec.make_unit_vector();
  vec3 upVecCrossViewingVec  = cross(upVec, viewingVec); 

  // Find distance from camera origin to image plane- d
  float d = height / (2* tan(setup.camera.ha));
  // Find half angle on x plane- derived from aspect ratio
  float ha_x = atan (width/(2*d));  

  /*
    Find four corners
   
    P0 - top left
    P1 - top right
    P2 - bottom left
    P3 - bottom right

     P1               P1
      - - - - - - - - - 
      |               |
      |               |  
      |               |    
      |               |   
      - - - - - - - - -   
     P2               P3
  */

  P0 = origin + viewingVec * d + upVecCrossViewingVec * d * tan(ha_x) + upVec * d * tan(setup.camera.ha);
  P1 = origin + viewingVec * d - upVecCrossViewingVec * d * tan(ha_x) + upVec * d * tan(setup.camera.ha);
  P2 = origin + viewingVec * d + upVecCrossViewingVec * d * tan(ha_x) - upVec * d * tan(setup.camera.ha);
  P3 = origin + viewingVec * d - upVecCrossViewingVec * d * tan(ha_x) - upVec * d * tan(setup.camera.ha);

#ifdef EN_DEBUG0
  printf("------------P0 P1 P2 P3------------------\n");
  printf("max_depth %d \n", setup.max_depth);
  printf("camera Origin               : "); 
  setup.camera.origin.print();
  printf("camera direction            : ");
  setup.camera.direction.print();
  printf("up                          : ");
  upVec.print();
  printf("viewing vector              : ");
  viewingVec.print();
  printf("half angle                  : %f\n", setup.camera.ha * (180/PI));
  printf("width height                : %d %d\n", width, height);
  printf("distance - d                : %f\n", d); 
  printf("background color            : ");
  setup.background.print();
  printf("cross upvec and viewing vec : ");
  upVecCrossViewingVec.print();
  printf("topleft corner              : ");
  P0.print();
  printf("topright corner             : ");
  P1.print();
  printf("bottom left corner          : ");
  P2.print();
  printf("bottom right corner         : ");
  P3.print();
  printf("num of lights %d \n", (int)setup.light.size());
  for (int i=0; i<setup.light.size(); i++) {
     setup.light[i]->print();
  }
  printf("------------P0 P1 P2 P3------------------\n");
#endif

  Image* image = new Image(width, height);

  Pixel background;

  // set background
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      background.r = setup.background.x() * 255;
      background.g = setup.background.y() * 255;
      background.b = setup.background.z() * 255;
      image->SetPixel(i,j,background);
    }
  }

  Pixel pixelColor; 
  vec3 hitColor;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      Ray ray = ConstructRayThroughPixel(setup.camera, i, j, width, height); 

#ifdef EN_DEBUG
  printf("------------ RAY ------------------\n");
  printf("Ray origin "); ray.origin.print();
  printf("Ray direction "); ray.direction.print();
#endif

      // Find intersection using the ray equation and object in scene
      Intersection intersect = FindIntersection(ray, scene);

      if (intersect.hit) {
        hitColor = getColor(intersect, setup, scene, ray, 0);
        pixelColor.r = clamp(hitColor.x() * 255);
        pixelColor.g = clamp(hitColor.y() * 255);
        pixelColor.b = clamp(hitColor.z() * 255);

        //pixelColor = PixelRandom();
        image->SetPixel(i,j,pixelColor);  
      }

#ifdef EN_DEBUG
      if (intersect.hit) { // intersects
        printf("intersect true\n");
      } else {  // does not intersect
        printf("intersect false\n");
      }
#endif
    }
  }

  // save image file
  char* filename = new char[setup.outfile.length() + 1];
  std::strcpy(filename, setup.outfile.c_str());
  image->Write(filename); //filename);
  delete image;
  delete[] filename;
}


int main(int argc, char** argv){
  string line;

  Scene scene;

  string fileName = argv[1];

  Setup setup;

  readInputFile (fileName, scene, setup);

  printf("############ num of lights %d \n", (int)setup.light.size());

  int width = setup.res.height; //64;
  int height = setup.res.width; //48;

  printf("number of objects on screen %d \n", (int)scene.size());

  string sp = "sphere";
  string tr = "triangle";

  for (int i=0; i< scene.size(); i++){
      if (scene[i]->objectType() == sp) 
        printf(" ||||||||||||||||||||||||| object %d is a sphere \n", i);
      else
        printf(" ||||||||||||||||||||||||| object %d is a triangle \n", i);
        scene[i]->print();
  }

  RayCast (setup, scene, width, height);

  // cleanup call destructors 
  for (int i=0; i< scene.size(); i++){
      scene[i]->freeMem();
  }

  return 0;
}


#endif

