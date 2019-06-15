#include <iostream>
#include "sphere.h"
#include "surface_list.h"
#include "float.h"
#include "math_util.h"
#include "camera.h"
#include <random>
#include "material.h"

vec3 color(const ray& r, surface *world, int depth);
surface* randomScene();

int main(){
    int nx = 200;
    int ny = 100;
    int ns = 10;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);
    
    //surface *list[5];
    //list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    //list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    //list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 0.0));
    //list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    //list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    
    surface *world = randomScene();
    vec3 lookFrom = vec3(13,2,3);
    vec3 lookAt = vec3(0,0,0);
    float focusDistance = (lookFrom - lookAt).length();
    float aperture = 0.1;

    camera cam(lookFrom, lookAt, vec3(0,1,0), 20, float(nx)/float(ny), aperture, focusDistance);
    for(int y = ny - 1; y >= 0; --y){
        for(int x = 0; x < nx; ++x){
            vec3 col(0, 0, 0);
            for(int i = 0; i < ns; ++i){
                float u = float(x + drand48())/float(nx);
                float v = float(y + drand48())/float(ny);
                ray r = cam.getRay(u, v);
                vec3 p = r.pointAtParameter(2.0);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int red = int(255.99*col[0]);
            int green = int(255.99*col[1]);
            int blue = int(255.99*col[2]);
            std::cout << red << " " << green << " " << blue << "\n";
        } 
    }
}

vec3 color(const ray& r, surface *world, int depth){
    hitRecord hitRec;
    if(world->hit(r, 0.001, MAXFLOAT, hitRec)){
        ray scattered;
        vec3 attenuation;
        if(depth < 50 && hitRec.mat->scatter(r, hitRec, attenuation, scattered)){
            return attenuation*color(scattered, world, ++depth);
        }else{
            return vec3(0,0,0);
        }
    }else{
        vec3 dir = unitVector(r.getDirection());
        float t = 0.5*(dir.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

surface* randomScene(){
    int n = 500;
    surface **list = new surface*[n+1];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5,0.5,0.5)));
    int i = 1;
    for(int a = -11; a < 11; ++a){
        for(int b = -11; b < 11; ++b){
            float randMat = drand48();
            vec3 center(a+0.9*drand48(), 0.2, b+drand48());
            if((center-vec3(4,0.2,0)).length() > 0.9){
                if(randMat < 0.8){
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
                }else if(randMat < 0.95){
                    list[i++] = new sphere(center, 0.2, 
                            new metal(vec3(0.5*(1+drand48()), 0.5*(1+drand48()), 0.5*(1+drand48())), 0.5*drand48()));
                }else{
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }

        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new surfaceList(list, i);
}
