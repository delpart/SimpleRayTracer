#ifndef SPHEREH
#define SPHERE

#include "surface.h"

class material;

class sphere: public surface{
    public:
        sphere(){}
        sphere(vec3 pos, float r, material *m) : position(pos), radius(r), mat(m)  {};
        virtual bool hit(const ray& r, float tMin, float tMax, hitRecord& hitRec) const;
        vec3 position;
        float radius;
        material *mat;
};

bool sphere::hit(const ray& r, float tMin, float tMax, hitRecord& hitRec) const{
    vec3 oc = r.getOrigin() - position;
    float a = dot(r.getDirection(), r.getDirection());
    float b = dot(oc, r.getDirection());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if(discriminant > 0){
        float temp = (-b - sqrt(discriminant))/a;
        if(temp < tMax && temp > tMin){
            hitRec.t = temp;
            hitRec.p = r.pointAtParameter(temp);
            hitRec.normal = (hitRec.p - position) / radius;
            hitRec.mat = mat;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if(temp < tMax && temp > tMin){
            hitRec.t = temp;
            hitRec.p = r.pointAtParameter(temp);
            hitRec.normal = (hitRec.p - position) / radius;
            hitRec.mat = mat;
            return true;
        }
    }
    return false;
}

#endif
