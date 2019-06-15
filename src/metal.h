#ifndef METALH
#define METALH

#include "ray.h"

class metal: public material{
    public:
        metal(const vec3& a, float f) : albedo(a){
            if(f < 1)
                fuzz = f;
            else
                fuzz = 1;
        }
        
        virtual bool scatter(const ray& inRay, const hitRecord& hitRec, vec3& attenuation, ray& scattered) const{
            vec3 reflected = reflect(unitVector(inRay.getDirection()), hitRec.normal);
            scattered = ray(hitRec.p, reflected + fuzz*randomUnitSphere());
            attenuation = albedo;
            return (dot(scattered.getDirection(), hitRec.normal) > 0);
        }
        vec3 albedo;
        float fuzz;
};

#endif
