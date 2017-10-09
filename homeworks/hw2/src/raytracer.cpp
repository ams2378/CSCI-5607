/*
*
* Fundamentals of Computer Graphics - CSCI 5607 Fall 2017
* Authod: Adil Sadik
*         sadik.adil@gmail.com
* 
* Description: Homework 2 part 1 
*
*/

//File parsing example using filestream >> operator

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

#include "fileParser.h"
#include "image.h"
#include "dataStructure.h"

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

#define PI 3.1416
#define EN_DEBUG0 
//#define EN_DEBUG 

using namespace std;

CoOrdinate P0, P1, P2, P3; // FIXME 

/*
Intersection FindIntersection(Ray ray, Scene scene) {

  Intersection newCoord;

  return newCoord;
}
*/

/*
  Sphere:
  (X0 + Xd * t - Xc)2 + (Y0 + Yd * t - Yc)2 + (Z0 + Zd * t - Zc)2 = Sr^2

  Origin    : X0, Y0, Z0
  direction : Xd, Yd, Zd
  Radius    : Sr
  Center    : Xc, Yc, Zc

  or A*t^2 + B*t + C = 0
  with: A = Xd^2 + Yd^2 + Zd^2
  B = 2 * (Xd * (X0 - Xc) + Yd * (Y0 - Yc) + Zd * (Z0 - Zc))
  C = (X0 - Xc)^2 + (Y0 - Yc)^2 + (Z0 - Zc)^2 - Sr^2

  t0, t1 = (- B +- sqrt(B^2 - 4*A*C)) / 2*A where t0 is for (-) and t1 is for (+)

  Discriminant : sqrt(B^2 - 4*A*C)

*/

bool discriminant (Ray ray, Scene scene) {

  float X0, Y0, Z0;
  float Xd, Yd, Zd;
  float Xc, Yc, Zc, Sr;

  X0 = ray.origin.x;
  Y0 = ray.origin.y;
  Z0 = ray.origin.z;

  Xd = ray.direction.x;
  Yd = ray.direction.y;
  Zd = ray.direction.z;

  Xc = scene.sphere0.origin.x;
  Yc = scene.sphere0.origin.y;
  Zc = scene.sphere0.origin.z;
  Sr = scene.sphere0.r;

  float A = 1; // Xd*Xd + Yd*Yd + Zd*Zd
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

  if (dis < 0) {
    return false;
  } else { 
    return true;
  }
}

bool FindIntersection(Ray ray, Scene scene) {

  Intersection newCoord;

  if (discriminant(ray, scene)) {
    return true;
  } else {
    return false;
  }
}

// R(t) = R0 + t * Rd , t > 0 with R0 = [X0, Y0, Z0] and Rd = [Xd, Yd, Zd]
Ray ConstructRayThroughPixel (Camera camera, int i, int j, int width, int height) {

  Ray newRay;

  CoOrdinate Px, Py, Pz;

  float offset = .5;

  Py = P0 + (P1-P0) * (j+offset)/height;
  Px = P2 + (P3-P2) * (i+offset)/width;
  Pz = Vec(0, 0, -height / (2* tan(camera.ha)));  // FIXME FIXME 

  CoOrdinate pixelCoord = Px + Py - Pz;

  Vec v = (pixelCoord-camera.origin);
  v.normalize();

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

void /*Image*/ RayCast(Camera camera, Scene scene, int width, int height){ 

  CoOrdinate origin         = camera.origin;
  Vec viewingVec            = camera.direction;
  Vec upVec                 = camera.up; //Vec (0,1,0);
  upVec.normalize();
  viewingVec.normalize();
  Vec upVecCrossViewingVec  = cross(upVec, viewingVec); //Vec (1,0,0); // cross of up vector and viewingVec /?? 


  // Find distance from camera origin to image plane- d
  float d = height / (2* tan(camera.ha));
  // Find half angle on x plane- derived from aspect ratio
  float ha_x = atan (width/(2*d));  

  // Find midpoints at top, bottom, left and right edge of image plane
  // P0 - top
  // P1 - bottom
  // P2 - left
  // P3 - right
  P0 = origin + viewingVec * d + upVec * d * tan(camera.ha);
  P1 = origin + viewingVec * d - upVec * d * tan(camera.ha);  
  P2 = origin + viewingVec * d + upVecCrossViewingVec * d * tan(ha_x);
  P3 = origin + viewingVec * d - upVecCrossViewingVec * d * tan(ha_x);

#ifdef EN_DEBUG0
  printf("------------P0 P1 P2 P3------------------\n");
  printf("d %f \n", d); 
  printf("viewing vector \n");
  viewingVec.print();
  printf("up\n");
  upVec.print();
  printf("cross upvec and viewing vec\n");
  upVecCrossViewingVec.print();
  P0.print();
  P1.print();
  P2.print();
  P3.print();
  printf("------------P0 P1 P2 P3------------------\n");
#endif

  Image* image = new Image(width, height);
  Pixel p0, p1; 
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      p0.r = p0.g = p0.b = 0;
      p1.r = p1.g = p1.b = 100;
      
      Ray ray = ConstructRayThroughPixel(camera, i, j, width, height); 

      if (FindIntersection(ray, scene)) { // intersects
#ifdef EN_DEBUG
        printf("intersect true\n");
#endif
        image->SetPixel(i,j,p0);
      } else {  // does not intersect
#ifdef EN_DEBUG
        printf("intersect false\n");
#endif
        image->SetPixel(i,j,p1);
      }
    }   
  }

  // save image file
  char* filename = "example.bmp";
  image->Write(filename);
  delete image;
  //return image; 
}


int main(int argc, char** argv){
  string line;
  
  //if (argc != 2){ // FIXME
  //   cout << "Usage: ./a.out scenefile\n";
  //   exit(0);
  //}

  int width = 300; //64;
  int height = 300; //48;

  string fileName = argv[1];

  int readStatus = readInputFile(fileName);


  Camera camera;
  camera.origin = CoOrdinate(0,0,0);
  camera.direction = Vec (1,0,-1); // - Z?
  camera.up = Vec (0,1,0); // - Z?

  camera.origin.print();

  camera.ha = 45 * (PI/180);

  Sphere sphere1;

  sphere1.origin = CoOrdinate (120,-100,-height / (2* tan(camera.ha)) - 100 );

  //sphere1.origin.x = sphere1.origin.y = sphere1.origin.z = 100;
  sphere1.r = 50;

  Scene scene;
  scene.sphere0 = sphere1;


  RayCast (camera, scene, width, height);


  return 0;
}



