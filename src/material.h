#ifndef MATERIALH
#define MATERIALH
#include "ray.h"
#include "math_util.h"
#include "surface_list.h"

class material{
    public:
        virtual bool scatter(const ray& inRay, const hitRecord& hitRec, vec3& attenuation, ray& scattered) const = 0;
};

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

class dielectric: public material{
    public:
        dielectric(float ri): refractionIndex(ri){}
        virtual bool scatter(const ray& inRay, const hitRecord& hitRec, vec3& attenuation, ray& scattered) const{
            vec3 outNormal;
            vec3 reflected = reflect(inRay.getDirection(), hitRec.normal);
            float refractionRatio;
            attenuation = vec3(1.0, 1.0, 1.0);
            vec3 refracted;
            float reflectionProbability;
            float cosine;
            if(dot(inRay.getDirection(), hitRec.normal) > 0){
                outNormal = -hitRec.normal;
                refractionRatio = refractionIndex;
                //cosine = refractionIndex*dot(inRay.getDirection(), hitRec.normal)/inRay.getDirection().length();
                cosine = dot(inRay.getDirection(), hitRec.normal) / inRay.getDirection().length();
                cosine = sqrt(1 - refractionIndex*refractionIndex*(1-cosine*cosine));
            }else{
                outNormal = hitRec.normal;
                refractionRatio = 1.0/refractionIndex;
                cosine = -dot(inRay.getDirection(), hitRec.normal) / inRay.getDirection().length();
            }

            if(refract(inRay.getDirection(), outNormal, refractionRatio, refracted)){
                reflectionProbability = schlick(cosine, refractionIndex);
            }else{
                reflectionProbability = 1.0;
            }

            if(drand48() < reflectionProbability){
                scattered = ray(hitRec.p, reflected);
            }else{
                scattered = ray(hitRec.p, refracted);
            }
            return true;
        }
        float refractionIndex;
};

#endif
