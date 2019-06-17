#ifndef MATHUTILH
#define MATHUTILH
#include "vec3.h"
#include <random>

vec3 randomUnitSphere(){
    vec3 p;
    do {
        p = 2.0*vec3(drand48(), drand48(), drand48()) - vec3(1,1,1);
    }while(p.squaredLength() >= 1.0);
    return p;
}

vec3 reflect(const vec3& v, const vec3& n){
    return v - 2*dot(v,n)*n;
}

bool refract(const vec3& v, const vec3& n, float refractionRatio, vec3& refracted){
    vec3 uv = unitVector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - refractionRatio*refractionRatio*(1-dt*dt);
    if(discriminant > 0){
        refracted = refractionRatio*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }else{
        return false;
    }
}

float schlick(float cosine, float refractionIndex){
    float r0 = (1-refractionIndex)/(1+refractionIndex);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1-cosine),5);
}

#endif
