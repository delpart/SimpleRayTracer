#ifndef SURFACELISTH
#define SURFACELISTH

#include "surface.h"

class SurfaceList: public Surface{
    public:
        SurfaceList(){}
        SurfaceList(Surface **l, int n) {
            list = l;
            size = n;
        }
        virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& hitRec) const;
        Surface **list;
        int size;
};

bool SurfaceList::hit(const Ray& r, float tMin, float tMax, hitRecord& hitRec) const{
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
