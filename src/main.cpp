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
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <thread>
#include <algorithm>

namespace po = boost::program_options;

vec3 color(const Ray& r, Surface *scene, int depth);
Surface* randomScene(int varA, int varB);
void render(std::vector<std::uint8_t> *img, int width, int height, int numRaysPixel, Surface* scene, vec3 lookFrom, vec3 lookAt, float focalDistance, float aperture, float vfov, bool shuffle);
int preview(std::vector<std::uint8_t> *img, int width, int height, int pwidth, int pheight);

int main(int argc, const char *argv[])
{
    int width;
    int height;
    int numRaysPixel;
    float focalDistance;
    float aperture;
    float vfov;
    int seed;
    int varA;
    int varB;
    int pwidth;
    int pheight;
    bool shuffle;
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
    ("seed", po::value<int>(&seed)->default_value(42), "random seed for the scene")
    ("var-a", po::value<int>(&varA)->default_value(11), "controls the number of random spheres")
    ("var-b", po::value<int>(&varB)->default_value(11), "controls the number of random spheres")
    ("pwidth", po::value<int>(&pwidth)->default_value(1280), "width for the preview frame")
    ("pheight", po::value<int>(&pheight)->default_value(720), "height for the preview frame")
    ("shuffle", po::bool_switch(&shuffle)->default_value(false), "randomizes the order of computation of the pixels");

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
    Surface* scene = randomScene(varA, varB);
    vec3 lookFrom = vec3(13,2,3);
    vec3 lookAt = vec3(0,0,0);

    std::vector<std::uint8_t> img;
    img.resize(width*height*4);

    std::thread t1(preview, &img, width, height, pwidth, pheight);

    render(&img, width, height, numRaysPixel, scene, lookFrom, lookAt, focalDistance, aperture, vfov, shuffle);

    unsigned error = lodepng::encode(filename, img, width, height);
    if(error)
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);

    t1.join();

    std::cout << "Done. Rendere scene saved as " << filename << std::endl;
}

void render(std::vector<std::uint8_t> *img, int width, int height, int numRaysPixel, Surface* scene, vec3 lookFrom, vec3 lookAt, float focalDistance, float aperture, float vfov, bool shuffle)
{
    Camera cam(lookFrom, lookAt, vec3(0,1,0), vfov, float(width)/float(height), aperture, focalDistance);
    std::vector<int> indices;
    if(shuffle){
        indices.resize(width*height);
        for(int i = 0; i < width*height; ++i)
            indices[i] = i;

        std::random_shuffle(indices.begin(), indices.end());

    }

    #pragma omp parallel for
    for(int i = 0; i < height*width; ++i){
        int j = i;

        if(shuffle){
            j = indices[i];
        }

        int x = j % width;
        int y = j / width;

        vec3 col(0, 0, 0);
        for (int i = 0; i < numRaysPixel; ++i) {
            float u = float(x + drand48()) / float(width);
            float v = float(y + drand48()) / float(height);
            Ray r = cam.getRay(u, v);
            col += color(r, scene, 0);
        }
        col /= float(numRaysPixel);
        col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
        (*img)[4 * width * (height - y - 1) + 4 * x + 0] = int(255.99 * col[0]);
        (*img)[4 * width * (height - y - 1) + 4 * x + 1] = int(255.99 * col[1]);
        (*img)[4 * width * (height - y - 1) + 4 * x + 2] = int(255.99 * col[2]);
        (*img)[4 * width * (height - y - 1) + 4 * x + 3] = 255;
    }
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

Surface* randomScene(int varA, int varB)
{
    int n = 500;
    Surface **list = new Surface*[n+1];
    list[0] = new Sphere(vec3(0,-1000,0), 1000, new Lambertian(vec3(0.5,0.5,0.5)));
    int i = 1;
    for(int a = -varA; a < varA; ++a)
    {
        for(int b = -varB; b < varB; ++b)
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

int preview(std::vector<std::uint8_t> *img, int width, int height, int pwidth, int pheight){

    if(pwidth > width)
        pwidth = width;
    if(pheight > height)
        pheight = height;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
        std::cout << "Error: Unable to init SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Surface* scr = SDL_SetVideoMode(pwidth, pheight, 32, SDL_OPENGL);

    if(scr == 0) {
        std::cout << "Error: Unable to set video. SDL error message: " << SDL_GetError() << std::endl;
        return 1;
    }

    glViewport(0, 0, pwidth, pheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, pwidth, pheight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    if(glGetError() != GL_NO_ERROR) {
        std::cout << "Error initing GL" << std::endl;
        return 1;
    }

    size_t u2 = 1; while(u2 < width) u2 *= 2;
    size_t v2 = 1; while(v2 < height) v2 *= 2;
    double u3 = (double)width / u2;
    double v3 = (double)height / v2;

    std::vector<unsigned char> image2(u2 * v2 * 4);

    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);

    bool done = false;
    SDL_Event event = {0};
    glColor4ub(255, 255, 255, 255);

    while(!done) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) done = 1;
        }
        for(size_t y = 0; y < height; y++)
            for(size_t x = 0; x < width; x++)
                for(size_t c = 0; c < 4; c++) {
                    image2[4 * u2 * y + 4 * x + c] = (*img)[4 * width * y + 4 * x + c];
                }
        glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
        glBegin(GL_QUADS);
        glTexCoord2d( 0,  0); glVertex2f(    0,      0);
        glTexCoord2d(u3,  0); glVertex2f(width,      0);
        glTexCoord2d(u3, v3); glVertex2f(width, height);
        glTexCoord2d( 0, v3); glVertex2f(    0, height);
        glEnd();

        SDL_GL_SwapBuffers();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_Delay(16);
    }

    return 0;
}
