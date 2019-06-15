#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray{
    public:
        ray(){}
        ray(const vec3& orig, const vec3& dir){
            origin = orig;
            direction = dir;
        }
        
        vec3 getOrigin() const {
            return origin;
        }
        
        vec3 getDirection() const{
            return direction;
        }

        vec3 pointAtParameter(float t) const{
            return origin + t*direction;
        }

        vec3 origin;
        vec3 direction;

};

#endif
