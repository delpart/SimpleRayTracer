#ifndef SURFACELISTH
#define SURFACELISTH

#include "surface.h"

class surfaceList: public surface{
    public: 
        surfaceList(){}
        surfaceList(surface **l, int n) {
            list = l;
            size = n;
        }
        virtual bool hit(const ray& r, float tMin, float tMax, hitRecord& hitRec) const;
        surface **list;
        int size;
};

bool surfaceList::hit(const ray& r, float tMin, float tMax, hitRecord& hitRec) const{
    hitRecord tempRec;
    bool hitAnything = false;
    float closestHit = tMax;
    for(int i = 0; i < size; ++i){
        if(list[i]->hit(r, tMin, closestHit, tempRec)){
            hitAnything = true;
            closestHit = tempRec.t;
            hitRec = tempRec;
        }

    }
    return hitAnything;
}

#endif
