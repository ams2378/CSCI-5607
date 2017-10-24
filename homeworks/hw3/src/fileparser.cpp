#ifndef FILEPARSER
#define FILEPARSER

//File parsing example using filestream >> operator

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>
#include <vector>

#include "object.h"
#include "datastructure.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"

using namespace std;

void readInputFile (string fileName, Scene& scene, Setup& setup) {

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

  //Setup setup;
  int width, height;
  PointLight pointLight;
  DirectionalLight dirLight;
  AmbientLight ambientLight;
  SpotLight spotLight;
  string outFile;
  
  width = 480;
  height = 640;

  int num_sphere = 0;
  int max_depth = 5;
  int material_pointer = 0;

  // Defaults
  outFile = "raytraced.bmp";

  vec3 ambient, diffuse, specular, transmissive;
  float ior, ns;

  // default material
  ambient =  vec3(0, 0, 0);
  diffuse =  vec3(1, 1, 1);
  specular =  vec3(0, 0, 0);
  transmissive =  vec3(5, 0, 0);
  ior = 0;
  ns = 1;  

  vector<Material*> material_vec;

  Material* material;
  material = new Material(ambient, diffuse, specular, transmissive, ns, ior);
  material_vec.push_back(material);

  setup.max_depth = max_depth;

  // default width and height
  setup.res.width = width;
  setup.res.height = height;

  // 0 0 0 0 0 1 0 1 0 45
  setup.camera.origin = vec3(0, 0, 0);
  setup.camera.direction = vec3(0, 0, 1);
  setup.camera.up = vec3(0, 1, 0);
  setup.camera.ha = 45 * (PI/180);

  setup.background = vec3(0, 0, 0);
  setup.outfile = outFile;
  
  int max_v, max_n;
  std::vector<vec3> vertices;
  std::vector<vec3> normals;

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
       setup.camera.origin = vec3(px, py, pz);
       setup.camera.direction = vec3(dx, dy, dz);
       setup.camera.up = vec3(ux, uy, uz);
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
    else if (command == "max_vertices") {
       input >> max_v;
    }
    else if (command == "max_normals") {
       input >> max_n;
    }
    else if (command == "vertex") { 
       float x,y,z;
       input >> x >> y >> z;
       vertices.push_back(vec3(x,y,z));
    }
    else if (command == "normal") { 
       float x,y,z;
       input >> x >> y >> z;
       normals.push_back(vec3(x,y,z));
    }
    else if (command == "sphere") { 
       float x,y,z,r;
       input >> x >> y >> z >> r;

       // create sphere
       Object *new_sphere;
       new_sphere = new Sphere(vec3(x,y,z), r);
       new_sphere->set_material(material_vec[material_pointer]);

       num_sphere++;
       scene.push_back(new_sphere);

       printf("Sphere as position (%f,%f,%f) with radius %f\n",x,y,z,r);
    }
    else if (command == "triangle") { 
       int v0,v1,v2;
       input >> v0 >> v1 >> v2;

       assert(v0 <= max_v);
       assert(v1 <= max_v);
       assert(v2 <= max_v);

       Object *new_tri;
       new_tri = new Triangle(vertices[v0], vertices[v1], vertices[v2],  // vertex xyz
                              vertices[v0], vertices[v1], vertices[v2],  // normal 
                              false);
       new_tri->set_material(material_vec[material_pointer]);
       scene.push_back(new_tri);
       printf("Triangle with vertices- \n");
       printf("vertex0- ");  vertices[v0].print();
       printf("vertex1- ");  vertices[v1].print();
       printf("vertex2- ");  vertices[v2].print();
    }
    else if (command == "normal_triangle") { 
       int v0,v1,v2;
       input >> v0 >> v1 >> v2;

       assert(v0 <= max_v);
       assert(v1 <= max_v);
       assert(v2 <= max_v);

       Object *new_tri;
       new_tri = new Triangle(vertices[v0], vertices[v1], vertices[v2],  // vertex xyz
                              normals[v0], normals[v1], normals[v2],  // normal 
                              true);
       new_tri->set_material(material_vec[material_pointer]);
       scene.push_back(new_tri);
    }
    else if (command == "background"){ //If the command is a background command
       float r,g,b;
       input >> r >> g >> b;
       setup.background = vec3(r,g,b);
       printf("Background color of (%f,%f,%f)\n",r,g,b);
    }
    else if (command == "material"){ //If the command is a background command
       float ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior;
       input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> ns >> tr >> tg >> tb >> ior;
       ambient = vec3(ar, ag, ab);
       diffuse = vec3(dr, dg, db);
       specular = vec3(sr, sg, sb);
       transmissive = vec3(tr, tg, tb);
       ior = ior;
       ns = ns;

       material_pointer++;

       Material* materialP;
       materialP = new Material(ambient, diffuse, specular, transmissive, ns, ior); 
       material_vec.push_back(materialP);   

       printf("material color of (%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)\n",ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior);
    }
    else if (command == "output_image"){ //If the command is an output_image command
       input >> outFile;
       setup.outfile = outFile;
       printf("Render to file named: %s\n", outFile.c_str());
    }    
    else if (command == "directional_light") { 
       float r, g, b, x, y, z;
       input >> r >> g >> b >> x >> y >> z;
       printf("Directional Light %f, %f, %f, %f, %f, %f:\n", r, g, b, x, y, z);

       Light *new_dir_light;
       new_dir_light = new DirectionalLight(vec3(r, g, b), vec3(x, y, z));
       setup.light.push_back(new_dir_light);
    }
    else if (command == "point_light") { 
       float r, g, b, x, y, z;
       input >> r >> g >> b >> x >> y >> z;
       printf("Point Light %f, %f, %f, %f, %f, %f:\n", r, g, b, x, y, z);

       Light *new_point_light;
       new_point_light = new PointLight(vec3(r, g, b), vec3(x, y, z));
       setup.light.push_back(new_point_light);
    }
    else if (command == "ambient_light") { 
       float r, g, b;
       input >> r >> g >> b;
       printf("Ambient Light %f, %f, %f :\n", r, g, b);

       Light *new_amb_light;
       new_amb_light = new AmbientLight(vec3(r, g, b));
       setup.light.push_back(new_amb_light);
    }
    else if (command == "spot_light") { 
       float r, g, b, x, y, z, dx, dy, dz, angle1, angle2;
       input >> r >> g >> b >> x >> y >> z >> dx >> dy >> dz >> angle1 >> angle2;
       printf("Spot Light %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f :\n", r, g, b, x, y, z, dx, dy, dz, angle1, angle2);

       Light *new_spot_light;
       new_spot_light = new SpotLight(vec3(r, g, b), vec3(x, y, z), vec3(dx, dy, dz), angle1, angle2);
       setup.light.push_back(new_spot_light);
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

  // defualt ambient light
  string ambientLightType = "ambient";
  bool ambientLightPres = false;

  for (int i=0; i<setup.light.size(); i++) {
     if (setup.light[i]->lightType() == ambientLightType) {
        ambientLightPres = true;
        break;
     }
  }

  if (ambientLightPres == false) {
    Light *new_amb_light;
    new_amb_light = new AmbientLight(vec3(0, 0, 0));
    setup.light.push_back(new_amb_light);
  }
  //return setup;
}


#endif
