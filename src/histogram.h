#ifndef HISTOGRAMH
#define HISTOGRAMH

#include "surface.h"
#include <vector>
#include <algorithm>
#include <stdio.h>

class Material;

class Histogram: public Surface{
    public:
        Histogram(){}
        Histogram(vec3 pos, Material *m, int w, int h, float s, float epsilon, std::vector<float> hi) : position(pos), mat(m), width(w), height(h), size(s), e(epsilon), hist(hi){};
        virtual bool hit(const Ray& r, float tMin, float tMax, hitRecord& hitRec) const;
        vec3 position;
        Material *mat;
        int width;
        int height;
        float size;
        float radius;
        float e;
        std::vector<float> hist;
    private:
        virtual float sdf(vec3 p, float end) const;
        virtual float boxSDF(vec3 p, float h) const;
        virtual vec3 normal(vec3 p, float end) const;
};

float Histogram::boxSDF(vec3 p, float h) const{
  vec3 d = (p - vec3(0., h/2., 0.)).abs() - vec3(size, h, size);
  float in = std::min(std::max(d.x(), std::max(h, d.z())), 0.f);
  
  float x = d.x() > 0. ? d.x() : 0.;
  float y = d.y() > 0. ? d.y() : 0.;
  float z = d.z() > 0. ? d.z() : 0.;
  float out = vec3(x, y, z).length();

  return in + out;
}

float Histogram::sdf(vec3 p, float end) const{
  float m = end;
  float d = 0.;
  for(unsigned long i = 0; i < hist.size(); ++i){
     if(hist[i] > 0.){
      d = boxSDF(p + vec3(float(i/width), 0., float(i%height)), hist[i]);
      if(d < m){
        m = d;
      }
      if(m > 0. && m < e){
        return m;
      }
     }
  }
  return m;
}

vec3 Histogram::normal(vec3 p, float end) const{
  vec3 v = vec3(
      Histogram::sdf(vec3(p.x() + e, p.y(), p.z()), end) - Histogram::sdf(vec3(p.x() - e, p.y(), p.z()), end),
      Histogram::sdf(vec3(p.x(), p.y() + e, p.z()), end) - Histogram::sdf(vec3(p.x(), p.y() - e, p.z()), end),
      Histogram::sdf(vec3(p.x(), p.y(), p.z() + e), end) - Histogram::sdf(vec3(p.x() - e, p.y(), p.z() - e), end)
      );
  v.toUnitVector();
  return v;
}

bool Histogram::hit(const Ray& r, float tMin, float tMax, hitRecord& hitRec) const{
    vec3 oc = r.getOrigin() - position;
    vec3 dir = r.getDirection();
    float t = tMin;

    while(t < tMax){
      float d = Histogram::sdf(oc + t*dir, tMax);
      if(d < e){
        hitRec.t = t + d;
        hitRec.p = r.pointAtParameter(t + d);
        hitRec.normal = Histogram::normal(hitRec.p, tMax);
        hitRec.mat = mat;
        return true;
      }
      t += d;
    }
    return false;
}
#endif

