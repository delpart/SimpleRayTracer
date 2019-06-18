#ifndef SURFACEH
#define SURFACEH

#include "ray.h"

class Material;

struct hitRecord{
    float t;
    vec3 p;
    vec3 normal;
    Material *mat;
};

class Surface{
    public:
        virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& hitRec) const = 0;
};

#endif
