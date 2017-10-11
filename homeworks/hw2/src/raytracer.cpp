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
#include <assert.h>

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

CoOrdinate P0, P1, P2, P3; // FIXME  global variable- not good!


float clamp (float p) {
  return (p < 0 ? 0 : (p > 255 ? 255 : p));
}

/*

  Ref: https://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm

  Intersection FindIntersection(Ray ray, Scene scene) : steps

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

  Intersection point:
  Ri = [xi, yi, zi] = [x0 + xd * ti ,  y0 + yd * ti,  z0 + zd * ti]

*/

Intersection IntersectMath (Ray&  ray, Scene& scene, int i) {

  float X0, Y0, Z0;       // Origin
  float Xd, Yd, Zd;       // Direction
  float Xc, Yc, Zc, Sr;   // Sphere center and redius
  float Xi, Yi, Zi;       // Intersection point on sphere

  X0 = ray.origin.x;
  Y0 = ray.origin.y;
  Z0 = ray.origin.z;

  Xd = ray.direction.x;
  Yd = ray.direction.y;
  Zd = ray.direction.z;

  Xc = scene.sphere0[i].origin.x;
  Yc = scene.sphere0[i].origin.y;
  Zc = scene.sphere0[i].origin.z;
  Sr = scene.sphere0[i].r;

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
  intersect.intersection = CoOrdinate (Xi, Yi, Zi);;
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
Intersection FindIntersection(Ray ray, Scene scene) {

    Intersection t, currentIntersect;
    currentIntersect.hit  = false;
    float currentDistance = std::numeric_limits<float>::max();

    for (int i=0; i<scene.numSpheres; i++) {
      t = IntersectMath (ray, scene, i);

      if (t.hit) {
        // hit-> compare with previous hit
        if (t.t < currentDistance) {
          currentIntersect.intersection = CoOrdinate(t.intersection.x, t.intersection.y, t.intersection.z);
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
  CoOrdinate Px1, Px2;

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

  CoOrdinate pixelCoord = Px1 + (Px2-Px1) * (j+offset)/height;

  // Direction- from origin to pixel location
  Vec v = (pixelCoord-camera.origin);
  // Normalize the direction to make math simple
  v.normalize();

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


CoOrdinate ambientColor(CoOrdinate lightColor, CoOrdinate materialColor) {

    float r, g, b;

    return CoOrdinate(materialColor.x * lightColor.x, 
                      materialColor.y * lightColor.y, 
                      materialColor.z * lightColor.z);

}

//  (kd*dotProduct*R, kd*dotProduct*G, kd*dotProduct*B)
CoOrdinate diffuseColor(CoOrdinate lightColor, CoOrdinate materialColor, float dotProduct) {

  float r, g, b;

  r = lightColor.x * dotProduct * materialColor.x; 
  g = lightColor.y * dotProduct * materialColor.y; 
  b = lightColor.z * dotProduct * materialColor.z; 

  return CoOrdinate(r,g,b);

}

// ks*lightSourceColor*surfaceColor*(RDotV^n)
CoOrdinate specularColor (CoOrdinate lightColor, CoOrdinate materialColor, float ns, float vDotR) {

  float r, g, b;

  r = lightColor.x * materialColor.x * pow(vDotR, ns); 
  g = lightColor.y * materialColor.y * pow(vDotR, ns);
  b = lightColor.z * materialColor.z * pow(vDotR, ns);

  return CoOrdinate(r,g,b);
}

// Return color of the pixel
Pixel getColor(Intersection& intersect, Scene& scene, Camera& camera) {

  Pixel p; // FIXME
  CoOrdinate ambient, diffuse, specular, finalColor;
  Vec intersectToLight, surfaceNormal, v, r;
  float vDotR, surfaceNormalDotIntersectToLight;

  int objectNumber = intersect.objectNumber;

  // Ambient
  ambient = ambientColor(scene.light.ambientLight, scene.sphere0[objectNumber].material.ambient);

  // Diffuse shading
  intersectToLight = scene.light.pointLight.location - intersect.intersection;
  intersectToLight.normalize(); // L

  surfaceNormal = intersect.intersection - scene.sphere0[objectNumber].origin;
  surfaceNormal.normalize(); // N

  surfaceNormalDotIntersectToLight = dot(surfaceNormal, intersectToLight); 
  surfaceNormalDotIntersectToLight = clamp(surfaceNormalDotIntersectToLight); 
  diffuse = diffuseColor(scene.light.pointLight.color, scene.sphere0[objectNumber].material.diffuse, surfaceNormalDotIntersectToLight);

  // Specular
  // R =  2(n.v)n - v
  v = intersect.intersection - camera.origin;
  v.normalize();
  r = surfaceNormal * 2 * (dot(surfaceNormal, v)) - v;
  vDotR = dot (v, r);

  specular  = specularColor(scene.light.pointLight.color, scene.sphere0[0].material.specular, scene.sphere0[0].material.ns, vDotR);

  // add ambient, diffuse and specular color to get final pixel color
  finalColor = ambient + diffuse + specular; 

  // unbias
  p.r = clamp(finalColor.x * 255);
  p.g = clamp(finalColor.y * 255);
  p.b = clamp(finalColor.z * 255);

#ifdef EN_DEBUG
  printf("Point light color and location \n");
  scene.light.pointLight.color.print();
  scene.light.pointLight.location.print();
  printf("Sphere origin \n");
  scene.sphere0[objectNumber].origin.print();
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

  return p;

}

void /*Image*/ RayCast(Setup setup, Scene scene, int width, int height){ 

  CoOrdinate origin         = setup.camera.origin;
  Vec viewingVec            = setup.camera.direction;
  Vec upVec                 = setup.camera.up; 
  upVec.normalize();
  viewingVec.normalize();
  Vec upVecCrossViewingVec  = cross(upVec, viewingVec); 


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
  printf("------------P0 P1 P2 P3------------------\n");
#endif

  Image* image = new Image(width, height);

  Pixel background;

  // set background
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      background.r = setup.background.x * 255;
      background.g = setup.background.y * 255;
      background.b = setup.background.z * 255;
      image->SetPixel(i,j,background);
    }
  }

  Pixel pixelColor; // FIXME
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      
      // Calculate the equation of ray
      Ray ray = ConstructRayThroughPixel(setup.camera, i, j, width, height); 
      // Find intersection using the ray equation and object in scene
      Intersection intersect = FindIntersection(ray, scene);
      // Shading- get color
      pixelColor = getColor(intersect, scene, setup.camera);
      //printf("final pixel color %d %d %d \n", (int)pixelColor.r, (int)pixelColor.g, (int)pixelColor.b);
      // Update the pixel color if it hit an object
      if (intersect.hit) 
        image->SetPixel(i,j,pixelColor);      

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
  
  //if (argc != 2){ // FIXME
  //   cout << "Usage: ./a.out scenefile\n";
  //   exit(0);
  //}

  Sphere sphere1;
  sphere1.origin = CoOrdinate(3, 1.5, 0); //   CoOrdinate (0,0,-height / (2* tan(setup.camera.ha)) - 10 );
  sphere1.r = 1.25;

  Scene scene;

  string fileName = argv[1];

  Setup setup = readInputFile(fileName, scene);

  int width = setup.res.height; //64;
  int height = setup.res.width; //48;

  printf("number of spheres on screen %d \n", scene.numSpheres);

  for (int i=0; i< scene.numSpheres; i++){
      printf("sphere # %d \n", i);
      printf("sphere origin \n");
      scene.sphere0[i].origin.print();
      printf("sphere radius %f \n", scene.sphere0[i].r);
      printf("Material\n");
      scene.sphere0[i].material.print();
  }

  RayCast (setup, scene, width, height);

/*
  Image* image2 = new Image(width, height);
  Pixel background;
  // set background
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      background.r = 100;
      background.g = 0;
      background.b = 0;
      image2->SetPixel(i,j,background);
    }
  } 

  char* filename = "example2.bmp";
  image2->Write(filename);
  delete image2;
*/

  return 0;
}



