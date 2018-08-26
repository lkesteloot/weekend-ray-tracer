#include <iostream>
#include <float.h>
#include <thread>

#include "Sphere.h"
#include "MovingSphere.h"
#include "HitableList.h"
#include "Camera.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"
#include "Bvh.h"
#include "ConstantTexture.h"
#include "CheckerTexture.h"
#include "NoiseTexture.h"
#include "ImageTexture.h"
#include "DiffuseLight.h"
#include "XyRect.h"

static const int WIDTH = 200*4;
static const int HEIGHT = 100*4;
static const int STRIDE = WIDTH*3;
static const int BYTE_COUNT = STRIDE*HEIGHT;
static const int SAMPLE_COUNT = 1000;
static const int THREAD_COUNT = 8;

static Hitable *simple_light() {
    Texture *perlin = new NoiseTexture(4);

    Hitable **list = new Hitable*[4];
    list[0] = new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(perlin));
    list[1] = new Sphere(Vec3(0, 2, 0), 2, new Lambertian(perlin));
    list[2] = new Sphere(Vec3(0, 7, 0), 2, new DiffuseLight(new ConstantTexture(Vec3(4, 4, 4))));
    list[3] = new XyRect(3, 5, 1, 3, -2, new DiffuseLight(new ConstantTexture(Vec3(4, 4, 4))));
    return new HitableList(list, 4);
}

static Vec3 trace_ray(const Ray &r, Hitable *world, int depth) {
    HitRecord rec;
    Vec3 color;

    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        Ray scattered;
        Vec3 attenuation;

        // Start with whatever color the material is emitting.
        color = rec.material->emitted(rec.u, rec.v, rec.p);

        // And add any scattering.
        if (depth < 50 && rec.material->scatter(r, rec, attenuation, scattered)) {
            color += attenuation*trace_ray(scattered, world, depth + 1);
        }
    } else {
        // Black background.
        color = Vec3(0, 0, 0);
    }

    return color;
}

// Trace line "j".
static void trace_line(unsigned char *row, int j, const Camera &cam, Hitable *world) {
    for (int i = 0; i < WIDTH; i++) {
        // Oversample.
        Vec3 c(0, 0, 0);
        for (int s = 0; s < SAMPLE_COUNT; s++) {
            float u = (i + drand48())/WIDTH;
            float v = (j + drand48())/HEIGHT;

            Ray r = cam.get_ray(u, v);
            c += trace_ray(r, world, 0);
        }
        c /= SAMPLE_COUNT;

        // Gamma correct.
        c = Vec3(sqrt(c.r()), sqrt(c.g()), sqrt(c.b()));

        // Clamp.
        c = c.min(VEC3_ONES);

        int ir = int(255.99*c.r());
        int ig = int(255.99*c.g());
        int ib = int(255.99*c.b());
        row[0] = ir;
        row[1] = ig;
        row[2] = ib;
        row += 3;
    }
}

// Trace line "start" and every "skip" lines after that.
static void trace_lines(unsigned char *image, int start, int skip,
        const Camera &cam, Hitable *world) {

    for (int j = start; j < HEIGHT; j += skip) {
        unsigned char *row = image + j*STRIDE;

        trace_line(row, HEIGHT - 1 - j, cam, world);
    }
}

int main() {
    Vec3 look_from = Vec3(23, 6, 3);
    Vec3 look_at = Vec3(0, 2, 0);
    float focus_distance = 10;
    float aperature = 0.0;
    float time0 = 0;
    float time1 = 1;

    Camera cam(look_from, look_at, Vec3(0, 1, 0), 20, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    Hitable *world = simple_light();

    unsigned char *image = new unsigned char[BYTE_COUNT];

    // Generate the image on multiple threads.
    std::thread *thread[THREAD_COUNT];
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t] = new std::thread(trace_lines, image, t, THREAD_COUNT, cam, world);
    }
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t]->join();
        delete thread[t];
        thread[t] = 0;
    }

    // Write image.
    std::cout << "P6 " << WIDTH << " " << HEIGHT << " 255\n";
    std::cout.write((char *) image, BYTE_COUNT);

    return 0;
}
