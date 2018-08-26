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
#include "Rect.h"
#include "FlipNormals.h"
#include "Box.h"

static const int WIDTH = 400;
static const int HEIGHT = 400;
static const int STRIDE = WIDTH*3;
static const int BYTE_COUNT = STRIDE*HEIGHT;
static const int SAMPLE_COUNT = 100;
static const int THREAD_COUNT = 8;

static Hitable *cornell_box() {
    Hitable **list = new Hitable*[8];

    Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
    Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
    Material *light = new DiffuseLight(new ConstantTexture(Vec3(15, 15, 15)));

    int i = 0;
    list[i++] = new FlipNormals(new YzRect(0, 555, 0, 555, 555, green));    // Left
    list[i++] = new YzRect(0, 555, 0, 555, 0, red);                         // Right
    list[i++] = new XzRect(213, 343, 227, 332, 554, light);                 // Light
    list[i++] = new FlipNormals(new XzRect(0, 555, 0, 555, 555, white));    // Ceiling
    list[i++] = new XzRect(0, 555, 0, 555, 0, white);                       // Floor
    list[i++] = new FlipNormals(new XyRect(0, 555, 0, 555, 555, white));    // Back
    list[i++] = new Box(Vec3(130, 0, 65), Vec3(295, 165, 230), white);
    list[i++] = new Box(Vec3(265, 0, 295), Vec3(430, 330, 460), white);
    return new HitableList(list, i);
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
            float u = (i + my_rand())/WIDTH;
            float v = (j + my_rand())/HEIGHT;

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
        const Camera &cam, Hitable *world, int seed) {

    // Initialize the seed for our thread.
    init_rand(seed);

    for (int j = start; j < HEIGHT; j += skip) {
        unsigned char *row = image + j*STRIDE;

        trace_line(row, HEIGHT - 1 - j, cam, world);
    }
}

int main() {
    Vec3 look_from = Vec3(278, 278, -800);
    Vec3 look_at = Vec3(278, 278, 0);
    float focus_distance = 10;
    float aperature = 0.0;
    float vfov = 40;
    float time0 = 0;
    float time1 = 1;

    Camera cam(look_from, look_at, Vec3(0, 1, 0), vfov, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    Hitable *world = cornell_box();

    unsigned char *image = new unsigned char[BYTE_COUNT];

    // Generate the image on multiple threads.
    std::thread *thread[THREAD_COUNT];
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t] = new std::thread(trace_lines, image, t, THREAD_COUNT, cam, world, random());
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
