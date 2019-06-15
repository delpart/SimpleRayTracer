#ifndef SURFACEH
#define SURFACEH

#include "ray.h"

class material;

struct hitRecord{
    float t;
    vec3 p;
    vec3 normal;
    material *mat;
};

class surface{
    public:
        virtual bool hit(const ray& r, float tMin, float tMax, hitRecord& hitRec) const = 0;
};

#endif
