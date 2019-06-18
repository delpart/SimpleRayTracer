#include <istream>
#include <iostream>
#include <random>
#include <boost/program_options.hpp>
#include "sphere.h"
#include "surface_list.h"
#include "float.h"
#include "camera.h"
#include "math_util.h"
#include "material.h"
#include "lodepng/lodepng.h"

namespace po = boost::program_options;

vec3 color(const Ray& r, Surface *scene, int depth);
Surface* randomScene();
std::vector<std::uint8_t> render(int width, int height, int numRaysPixel, Surface* scene, vec3 lookFrom, vec3 lookAt, float focalDistance, float aperture, float vfov);

int main(int argc, const char *argv[])
{
    int width;
    int height;
    int numRaysPixel;
    float focalDistance;
    float aperture;
    float vfov;
    int seed;
    std::string filename;

    po::options_description desc("A very simple ray tracer (╯°□°)╯︵ ┻━┻\n\nSupported parameters");
    desc.add_options()
    ("help", "print available parameters")
    ("filename", po::value<std::string>(&filename), "filename to store the rendered scene as png-file")
    ("width", po::value<int>(&width)->default_value(1280), "width for the rendered scene")
    ("height", po::value<int>(&height)->default_value(720), "height for the rendered scene")
    ("num-rays", po::value<int>(&numRaysPixel)->default_value(100), "number of rays per pixel ('Anti-Aliasing')")
    ("vfov", po::value<float>(&vfov)->default_value(20), "field of view for the camera")
    ("aperture", po::value<float>(&aperture)->default_value(0.2), "aperture of the camera")
    ("focal-distance", po::value<float>(&focalDistance)->default_value(10), "focal distance of the camera")
    ("seed", po::value<int>(&seed)->default_value(42), "random seed for the scene");
    po::positional_options_description p;
    p.add("filename", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }
    else if(!vm.count("filename"))
    {
        std::cout << "Please provide a filename to store the rendered scene." << std::endl << std::endl << desc << std::endl;
        return 1;
    }

    srand48(seed);
    Surface* scene = randomScene();
    vec3 lookFrom = vec3(13,2,3);
    vec3 lookAt = vec3(0,0,0);

    std::vector<std::uint8_t> img = render(width, height, numRaysPixel, scene, lookFrom, lookAt, focalDistance, aperture, vfov);
    unsigned error = lodepng::encode(filename, img, width, height);
    if(error)
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

std::vector<std::uint8_t> render(int width, int height, int numRaysPixel, Surface* scene, vec3 lookFrom, vec3 lookAt, float focalDistance, float aperture, float vfov)
{
    std::vector<std::uint8_t> img;
    img.resize(width*height*4);

    Camera cam(lookFrom, lookAt, vec3(0,1,0), vfov, float(width)/float(height), aperture, focalDistance);
    #pragma omp parallel for collapse(2)
    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            vec3 col(0, 0, 0);
            for(int i = 0; i < numRaysPixel; ++i)
            {
                float u = float(x + drand48())/float(width);
                float v = float(y + drand48())/float(height);
                Ray r = cam.getRay(u, v);
                col += color(r, scene, 0);
            }
            col /= float(numRaysPixel);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            img[4*width*(height - y - 1) + 4*x + 0] = int(255.99*col[0]);
            img[4*width*(height - y - 1) + 4*x + 1] = int(255.99*col[1]);
            img[4*width*(height - y - 1) + 4*x + 2] = int(255.99*col[2]);
            img[4*width*(height - y - 1) + 4*x + 3] = 255;
        }
    }
    return img;
}

vec3 color(const Ray& r, Surface *scene, int depth)
{
    hitRecord hitRec;
    if(scene->hit(r, 0.001, MAXFLOAT, hitRec))
    {
        Ray scattered;
        vec3 attenuation;
        if(depth < 150 && hitRec.mat->scatter(r, hitRec, attenuation, scattered))
        {
            return attenuation*color(scattered, scene, depth+1);
        }
        else
        {
            return vec3(0,0,0);
        }
    }
    else
    {
        vec3 dir = unitVector(r.getDirection());
        float t = 0.5*(dir.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

Surface* randomScene()
{
    int n = 500;
    Surface **list = new Surface*[n+1];
    list[0] = new Sphere(vec3(0,-1000,0), 1000, new Lambertian(vec3(0.5,0.5,0.5)));
    int i = 1;
    for(int a = -3; a < 3; ++a)
    {
        for(int b = -3; b < 3; ++b)
        {
            float randMat = drand48();
            vec3 center(a+0.9*drand48(), 0.2, b+drand48());
            if((center-vec3(4,0.2,0)).length() > 0.9)
            {
                if(randMat < 0.8)
                {
                    list[i++] = new Sphere(center, 0.2, new Lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
                }
                else if(randMat < 0.95)
                {
                    list[i++] = new Sphere(center, 0.2,
                                           new Metal(vec3(0.5*(1+drand48()), 0.5*(1+drand48()), 0.5*(1+drand48())), 0.5*drand48()));
                }
                else
                {
                    list[i++] = new Sphere(center, 0.2, new Dielectric(1.5+(drand48()*2 - 1.0)));
                }
            }

        }
    }
    list[i++] = new Sphere(vec3(0, 1, 0), 1.0, new Dielectric(1.3));
    list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, new Lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new Sphere(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new SurfaceList(list, i);
}
