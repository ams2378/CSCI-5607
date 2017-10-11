//File parsing example using filestream >> operator

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>

#include "datastructure.h"

using namespace std;

Setup& readInputFile (string fileName, Scene& scene) {

  string line;

  // open the file containing the scene description
  ifstream input(fileName);

  // check for errors in opening the file
  if(input.fail()){
    cout << "Can't open file '" << fileName << "'" << endl;
    assert(input.fail() == false);
  }
  
  // determine the file size (this is optional -- feel free to delete the 6 lines below)
  streampos begin,end;
  begin = input.tellg();
  input.seekg(0, ios::end);
  end = input.tellg();
  cout << "File '" << fileName << "' is: " << (end-begin) << " bytes long.\n\n";
  input.seekg(0, ios::beg);

  Setup setup;
  int width, height;
  Material material;
  PointLight pointLight;
  DirectionalLight dirLight;
  AmbientLight ambientLight;
  SpotLight spotLight;
  string outFile;
  
  width = 480;
  height = 640;

  int num_sphere = 0;
  int max_depth = 5;

  // Defaults
  outFile = "raytraced.bmp";

  //0 0 0 1 1 1 0 0 0 5 0 0 0 1
  material.ambient = CoOrdinate(0, 0, 0);
  material.diffuse = CoOrdinate(1, 1, 1);
  material.specular = CoOrdinate(0, 0, 0);
  material.transmissive = CoOrdinate(5, 0, 0);
  material.ior = 0;
  material.ns = 1;  

  ambientLight = AmbientLight(CoOrdinate(0, 0, 0));
  setup.max_depth = max_depth;

  // default width and height
  setup.res.width = width;
  setup.res.height = height;

  // 0 0 0 0 0 1 0 1 0 45
  setup.camera.origin = CoOrdinate(0, 0, 0);
  setup.camera.direction = Vec(0, 0, 1);
  setup.camera.up = Vec(0, 1, 0);
  setup.camera.ha = 45 * (PI/180);

  scene.light.dirLight      = dirLight;
  scene.light.spotLight     = spotLight;
  scene.light.ambientLight  = ambientLight;
  scene.light.pointLight    = pointLight;

  setup.background = CoOrdinate(0, 0, 0);
  setup.outfile = outFile;
  
  //Loop through reading each line
  string command;
  while(input >> command) { //Read first word in the line (i.e., the command type)
    
    if (command[0] == '#'){
      getline(input, line); //skip rest of line
      cout << "Skipping comment: " << command  << line <<  endl;
      continue;
    }   

    if (command == "camera"){ //If the command is a sphere command
       float px, py, pz, dx, dy, dz, ux, uy, uz, ha;
       input >> px >> py >> pz >> dx >> dy >> dz >> ux >> uy >> uz >> ha;
       setup.camera.origin = CoOrdinate(px, py, pz);
       setup.camera.direction = Vec(dx, dy, dz);
       setup.camera.up = Vec(ux, uy, uz);
       setup.camera.ha = ha * (PI/180);
       printf("Camera  position (%f,%f,%f) \n",px, py, pz);
       printf("Camera  direction (%f,%f,%f) \n",dx, dy, dz);
       printf("Camera  UP vector and half angle (%f,%f,%f,%f) \n",ux, uy, uz, ha);
    }
    else if (command == "resolution") {
       input >> width >> height;
       setup.res.width = width;
       setup.res.height = height;
       assert(width > 0);
       assert(height > 0);
       printf("Width and Height (%d,%d)n",width, height);
    }
    else if (command == "sphere"){ //If the command is a sphere command
       float x,y,z,r;
       input >> x >> y >> z >> r;
       scene.sphere0[num_sphere].origin = CoOrdinate(x, y, z);
       scene.sphere0[num_sphere].r = r;
       scene.sphere0[num_sphere].material = material;
       num_sphere++;
       scene.numSpheres = num_sphere;
       printf("Sphere as position (%f,%f,%f) with radius %f\n",x,y,z,r);
    }
    else if (command == "background"){ //If the command is a background command
       float r,g,b;
       input >> r >> g >> b;
       setup.background = CoOrdinate(r,g,b);
       printf("Background color of (%f,%f,%f)\n",r,g,b);
    }
    else if (command == "material"){ //If the command is a background command
       float ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior;
       input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> ns >> tr >> tg >> tb >> ior;
       material.ambient = CoOrdinate(ar, ag, ab);
       material.diffuse = CoOrdinate(dr, dg, dg);
       material.specular = CoOrdinate(sr, sg, sb);
       material.transmissive = CoOrdinate(tr, tg, tb);
       material.ior = ior;
       material.ns = ns;
       //material.print();
       printf("material color of (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n",ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior);
    }
    else if (command == "output_image"){ //If the command is an output_image command
       input >> outFile;
       //setup.outfile = outFile;
       //char* filename = (char*)outFile.c_str();
       setup.outfile = outFile;
       printf("Render to file named: %s\n", outFile.c_str());
    }    
    else if (command == "directional_light") { 
       float r, g, b, x, y, z;
       input >> r >> g >> b >> x >> y >> z;
       dirLight = DirectionalLight(CoOrdinate(r, g, b), Vec(x, y, z));
       printf("Directional Light %f, %f, %f, %f, %f, %f:\n", r, g, b, x, y, z);
       scene.light.dirLight = dirLight;
    }
    else if (command == "point_light") { 
       float r, g, b, x, y, z;
       input >> r >> g >> b >> x >> y >> z;
       pointLight = PointLight(CoOrdinate(r, g, b), Vec(x, y, z));
       printf("Point Light %f, %f, %f, %f, %f, %f:\n", r, g, b, x, y, z);
       scene.light.pointLight    = pointLight;
    }
    else if (command == "ambient_light") { 
       float r, g, b;
       input >> r >> g >> b;
       ambientLight = AmbientLight(CoOrdinate(r, g, b));
       scene.light.ambientLight  = ambientLight;
       printf("Ambient Light %f, %f, %f :\n", r, g, b);
    }
    else if (command == "spot_light") { 
       float r, g, b, x, y, z, dx, dy, dz, angle1, angle2;
       input >> r >> g >> b >> x >> y >> z >> dx >> dy >> dz >> angle1 >> angle2;
       spotLight = SpotLight(CoOrdinate(r, g, b), CoOrdinate(x, y, z), Vec(dx, dy, dz), angle1, angle2);
       printf("Spot Light %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f :\n", r, g, b, x, y, z, dx, dy, dz, angle1, angle2);
       scene.light.spotLight     = spotLight;
    }
    else if (command == "max_depth") { 
       input >> max_depth;
       printf("max depth %d :\n", max_depth);
    }
    else {
      getline(input, line); //skip rest of line
      cout << "WARNING. Unknow command: " << command << endl;
    }
  }

  return setup;
}
