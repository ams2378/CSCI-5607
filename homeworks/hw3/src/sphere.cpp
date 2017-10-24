
#include "sphere.h"

Intersection Sphere::hit(Ray &ray, int objectNumber) const { //(const Ray& r, float t_min, float t_max, hit_record& rec) const {
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

  Xc = center.x(); // this->get_center().x();
  Yc = center.y(); // this->get_center().y();
  Zc = center.z(); // this->get_center().z();
  Sr = radius; // this->get_radius();

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
  intersect.objectNumber = objectNumber; 
  intersect.t = t;

  if (t >= 0) {
    intersect.hit = true;
  } else { 
    intersect.hit = false;
  }

  return intersect;
}

string Sphere::objectType() const {
    string name;
    name = "sphere";
    return name;
}

void Sphere::print() const {
    printf("Origin %f, %f, %f \n", center.x(), center.y(), center.z());
    printf("Radius %f \n", radius);
    mat_ptr->print();
}

void Sphere::set_material(Material* mat) {
    mat_ptr = mat;
}

Material* Sphere::get_material() const {
    return mat_ptr;
}

vec3 Sphere::get_center() {
    return center;
}

float Sphere::get_radius() {
    return radius;
}

vec3 Sphere::getSurfaceNormal(vec3 p, vec3 dir) const {
    vec3 N  = p-center;
    N.make_unit_vector();
    return N;
}

Sphere::~Sphere() {
    //printf("cleanup- delete Material \n");
    mat_ptr->~Material();
}

void Sphere::freeMem() const {
    Sphere::~Sphere();
}