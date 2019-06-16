#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

vec3 randomUnitDisk(){
    vec3 p;
    do{
        p = 2.0*vec3(drand48(), drand48(),0)-vec3(1,1,0);
    }while(dot(p,p) >= 1.0);
    return p;
}

class camera{
    public:
        camera(vec3 lookFrom, vec3 lookAt, vec3 vup, float vfov, float aspect, float aperture, float focusDistance){
            lensRadius = aperture/2;
            float theta = vfov*M_PI/180;
            float halfHeight = tan(theta/2);
            float halfWidth = aspect*halfHeight;
            origin = lookFrom;
            w = unitVector(lookFrom - lookAt);
            u = unitVector(cross(vup, w));
            v = cross(w, u);
            lowerLeftCorner = origin - halfWidth*focusDistance*u - halfHeight*focusDistance*v - focusDistance*w;
            horizontal = 2*halfWidth*focusDistance*u;
            vertical = 2*halfHeight*focusDistance*v;
        }
        ray getRay(float s, float t){
            vec3 rd = lensRadius*randomUnitDisk();
            vec3 offset = u*rd.x() + v*rd.y();
            return ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset);
        }

        vec3 origin;
        vec3 lowerLeftCorner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float lensRadius;
};

#endif
