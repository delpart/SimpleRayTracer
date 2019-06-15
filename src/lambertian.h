#ifndef LAMBERTIANH
#define LAMBERTIANH

#include "material.h"

class lambertian : public material{
    public:
        lambertian(const vec3& a): albedo(a){}
        virtual bool scatter(const ray& inRay, const hitRecord& hitRec, vec3& attenuation, ray& scattered) const{
            vec3 target = hitRec.p + hitRec.normal + randomUnitSphere();
            scattered = ray(hitRec.p, target - hitRec.p);
            attenuation = albedo;
            return true;
        }
        vec3 albedo;
};

#endif
