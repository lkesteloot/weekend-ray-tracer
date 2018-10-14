#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <float.h>
#include <thread>
#include <unistd.h>
#include <atomic>

#ifdef DISPLAY
#include "MiniFB.h"
#endif

#include "stb_image_write.h"

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
#include "Transform.h"
#include "ConstantMedium.h"
#include "Timer.h"
#include "World.h"
#include "Scene.pb.h"

// How to deal with multiple frames and UI.
enum InteractiveMode {
    // All frames, no UI.
    IM_BATCH,

    // Single frame in UI.
    IM_SINGLE,

    // All frames in UI.
    IM_ANIMATION,
};

// https://support.google.com/youtube/answer/6375112?co=GENIE.Platform%3DDesktop&hl=en&oco=0
static const int WIDTH = 1280/2;
static const int HEIGHT = 720/2;
static const int BYTES_PER_PIXEL = 4;
static const int STRIDE = WIDTH*BYTES_PER_PIXEL;
static const int BYTE_COUNT = STRIDE*HEIGHT;
static const int PIXEL_COUNT = WIDTH*HEIGHT;

// Number of threads to use.
static int g_thread_count;

// Whether to quit the program.
static bool g_quit;

// How many worker threads are still working.
static std::atomic_int g_working;

// How many rows have been done in this image.
static std::atomic_int g_lines_done;

/*
static Hitable *cornell_box() {
    Hitable **list = new Hitable*[8];

    Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
    Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
    Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));

    int i = 0;
    list[i++] = new FlipNormals(new YzRect(0, 555, 0, 555, 555, green));    // Left
    list[i++] = new YzRect(0, 555, 0, 555, 0, red);                         // Right
    list[i++] = new XzRect(113, 443, 127, 432, 554, light);                 // Light
    list[i++] = new FlipNormals(new XzRect(0, 555, 0, 555, 555, white));    // Ceiling
    list[i++] = new XzRect(0, 555, 0, 555, 0, white);                       // Floor
    list[i++] = new FlipNormals(new XyRect(0, 555, 0, 555, 555, white));    // Back
    Hitable *b1 = new Translate(new RotateY(new Box(Vec3(0, 0, 0), Vec3(165, 165, 165), white),
                -18), Vec3(130, 0, 65));
    Hitable *b2 = new Translate(new RotateY(new Box(Vec3(0, 0, 0), Vec3(165, 330, 165), white),
                15), Vec3(265, 0, 295));
    list[i++] = new ConstantMedium(b1, 0.01, new ConstantTexture(Vec3(1, 1, 1)));
    list[i++] = new ConstantMedium(b2, 0.01, new ConstantTexture(Vec3(0, 0, 0)));
    return new HitableList(list, i);
}
*/

/*
static World *book2_scene(Camera &cam, int frame) {
    float t = frame*2*M_PI/240;
    float s = sin(t);
    float c = cos(t);

    Vec3 look_at = Vec3(278, 278, 0);
    Vec3 look_from = Vec3(478, 278, -600);
    look_from = look_at + c*Vec3(200, 0, -600) + s*Vec3(600, 0, 200) + Vec3(0, 0, 0);
    float focus_distance = 10;
    float aperature = 0.0;
    float vfov = 40;
    float time0 = 0;
    float time1 = 1;

    cam = Camera(look_from, look_at, Vec3(0, 1, 0), vfov, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    int nb = 20;

    Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    Material *ground = new Lambertian(new ConstantTexture(Vec3(0.48, 0.83, 0.53)));

    // Floor boxes.
    Hitable **box_list = new Hitable*[nb*nb];
    int b = 0;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float width = 100;
            float x0 = -1000 + i*width;
            float y0 = 0;
            float z0 = -1000 + j*width;
            float x1 = x0 + width;
            float y1 = 100*(my_rand() + 0.01);
            float z1 = z0 + width;
            box_list[b++] = new Box(Vec3(x0, y0, z0), Vec3(x1, y1, z1), ground);
        }
    }

    HitableList *list = new HitableList;
    list->add(new Bvh(box_list, b, time0, time1));

    // Area light.
    Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));
    list->add(new XzRect(123, 423, 147, 412, 554, light));

    Vec3 center(400, 400, 200);
    list->add(new MovingSphere(center, center + Vec3(0, 30, 0), 0, 1, 50,
            new Lambertian(new ConstantTexture(Vec3(0.7, 0.3, 0.1)))));
    list->add(new Sphere(Vec3(260, 150, 45), 50, new Dielectric(REF_GLASS)));
    list->add(new Sphere(Vec3(0, 150, 145), 50, new Metal(Vec3(0.8, 0.8, 0.9), 1.0)));
    Hitable *boundary = new Sphere(Vec3(360, 150, 145), 70, new Dielectric(REF_GLASS));
    list->add(boundary);
    list->add(new ConstantMedium(boundary, 0.2, new ConstantTexture(Vec3(0.2, 0.4, 0.9))));
    boundary = new Sphere(Vec3(0, 0, 0), 5000, new Dielectric(REF_GLASS));
    list->add(new ConstantMedium(boundary, 0.0001, new ConstantTexture(Vec3(1, 1, 1))));
    list->add(new Sphere(Vec3(400, 200, 400), 100,
            new Lambertian(new ImageTexture("data/earthmap.jpg"))));
    list->add(new Sphere(Vec3(220, 280, 300), 80, new Lambertian(new NoiseTexture(0.1))));

    // Random spheres packed into a box shape.
    int sphere_count = 1000;
    Hitable **sphere_list = new Hitable*[sphere_count];
    for (int j = 0; j < sphere_count; j++) {
        sphere_list[j] = new Sphere(Vec3(165*my_rand(), 165*my_rand(), 165*my_rand()), 10, white);
    }
    list->add(new Translate(new RotateY(new Bvh(sphere_list, sphere_count, time0, time1), 15),
            Vec3(-100, 270, 395)));

    return new World(list, Vec3(0, 0, 0));
    // return new World(list, new ImageTexture("data/HDR_111_Parking_Lot_2_Ref.hdr"));
}
*/

/*
static World *animation_scene(Camera &cam, int frame) {
    Vec3 look_at = Vec3(0, 1, 0);
    Vec3 look_from = Vec3(0, 3, -10);
    float focus_distance = (look_at - look_from).length();
    float aperature = 0.2;
    float vfov = 40;
    float time0 = 0;
    float time1 = 1;

    cam = Camera(look_from, look_at, Vec3(0, 1, 0), vfov, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    return g_frames[frame];
}
*/

static Vec3 convert_vec3(const Scene::Vec3 &src) {
    return Vec3(src.x(), src.y(), src.z());
}

static Quat convert_quat(const Scene::Quat &src) {
    return Quat(src.x(), src.y(), src.z(), src.w());
}

static Texture *convert_texture(const Scene::Texture &src) {
    Texture *texture = nullptr;

    switch (src.texture_type()) {
        case Scene::TEXTURE_CONSTANT:
            texture = new ConstantTexture(convert_vec3(src.color()));
            break;

        case Scene::TEXTURE_CHECKER:
            texture = new CheckerTexture(
                    convert_texture(src.odd()),
                    convert_texture(src.even()),
                    src.size());
            break;

        case Scene::TEXTURE_IMAGE:
            // To do.
            break;

        case Scene::TEXTURE_NOISE:
            // To do.
            break;

        case Scene::TEXTURE_TRANSFORM:
            texture = new TransformTexture(
                    convert_texture(src.sub_texture()),
                    convert_vec3(src.translation()),
                    convert_quat(src.orientation()));
            break;
    }

    return texture;
}

static Material *convert_material(const Scene::Material &src) {
    Material *material = nullptr;

    switch (src.material_type()) {
        case Scene::MATERIAL_LAMBERTIAN:
            material = new Lambertian(convert_texture(src.texture()));
            break;

        case Scene::MATERIAL_DIELECTRIC:
            material = new Dielectric(src.refraction_index());
            break;

        case Scene::MATERIAL_METAL:
            // To do.
            break;

        case Scene::MATERIAL_DIFFUSE_LIGHT:
            material = new DiffuseLight(convert_texture(src.texture()));
            break;

        case Scene::MATERIAL_ISOTROPIC:
            // To do.
            break;
    }

    return material;
}

// Load a world from a protobuf scene.
static World *animation_scene(const Scene::Scene &scene, Camera &cam, int frame) {
    const Scene::World &world = scene.world(frame);

    // Set up camera, if specified.
    Vec3 look_at;
    Vec3 look_from;
    float focus_distance;
    float aperature;
    float vertical_fov;
    if (world.has_camera()) {
        const Scene::Camera &camera = world.camera();

        look_at = convert_vec3(camera.look_at());
        look_from = convert_vec3(camera.look_from());
        focus_distance = (look_at - look_from).length();
        aperature = camera.aperature();
        vertical_fov = camera.vertical_fov();
    } else {
        look_at = Vec3(0, 1, 0);
        look_from = Vec3(0, 3, -10);
        focus_distance = (look_at - look_from).length();
        aperature = 0.2;
        vertical_fov = 40;
    }
    float time0 = 0;
    float time1 = 1;

    cam = Camera(look_from, look_at, Vec3(0, 1, 0), vertical_fov, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    // Put together Thing list.
    HitableList *list = new HitableList;

    for (int i = 0; i < world.thing_size(); i++) {
        const Scene::Thing &thing = world.thing(i);

        Vec3 center = convert_vec3(thing.center());
        Vec3 half_size = convert_vec3(thing.half_size());
        Material *material = convert_material(thing.material());

        Hitable *hitable = nullptr;
        switch (thing.shape()) {
            case Scene::SHAPE_SPHERE:
                hitable = new Sphere(center, half_size.x(), material);
                break;

            case Scene::SHAPE_BOX:
                hitable = new Box(center - half_size, center + half_size, material);
                break;

            case Scene::SHAPE_XZ_RECT:
                hitable = new XzRect(
                        center.x() - half_size.x(),
                        center.x() + half_size.x(),
                        center.z() - half_size.z(),
                        center.z() + half_size.z(),
                        center.y(),
                        material);
                break;

            case Scene::SHAPE_XY_RECT:
            case Scene::SHAPE_YZ_RECT:
                // To do.
                break;
        }

        if (hitable != nullptr) {
            list->add(hitable);
        }
    }

    Vec3 background_color = world.has_background_color()
        ? convert_vec3(world.background_color())
        : Vec3(0, 0, 0);

    return new World(list, background_color);
}

/*
static void add_marble(HitableList *list, const Vec3 &center, float radius, const Vec3 &color) {
    Material *marble = new Dielectric(REF_GLASS);
    Hitable *sphere = new Sphere(center, radius, marble);

    list->add(sphere);
    list->add(new ConstantMedium(sphere, 0.2, new ConstantTexture(color)));
}

static World *marble_scene(Camera &cam, int frame) {
    int room_width = 3000;
    int room_height = 2500;
    int table_width = 1000;
    int table_height = 1000;
    int marble_radius = 5;
    int window_size = 800;
    int window_height = 1000;
    int window_spacing = 200;
    int window_position = room_width/2 - 10;

    Material *wall = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    Material *table = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    table = new Metal(Vec3(0.73, 0.73, 0.73), 0.01);
    Material *light = new DiffuseLight(new ConstantTexture(Vec3(2, 2, 2)));

    float t = frame*2*M_PI/60;
    float s = sin(t);
    float c = cos(t);
    Vec3 look_at = Vec3(0, table_height + marble_radius, 0);
    Vec3 look_from = look_at + c*Vec3(0, 0, 50) + s*Vec3(50, 0, 0) + Vec3(0, 10, 0);
    // look_from -= (look_at - look_from).unit()*frame*10;
    float focus_distance = (look_at - look_from).length();
    float aperature = 0.0;
    float vfov = 20;
    float time0 = 0;
    float time1 = 1;

    cam = Camera(look_from, look_at, Vec3(0, 1, 0), vfov, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    HitableList *list = new HitableList;
    // Walls.
    if (false) {
        list->add(new XyRect(-room_width/2, room_width/2, 0, room_height, -room_width/2, wall));
        list->add(new FlipNormals(new XyRect(-room_width/2, room_width/2, 0, room_height, room_width/2, wall)));
        list->add(new YzRect(0, room_height, -room_width/2, room_width/2, -room_width/2, wall));
        list->add(new FlipNormals(new YzRect(0, room_height, -room_width/2, room_width/2, room_width/2, wall)));
        list->add(new XzRect(-room_width/2, room_width/2, -room_width/2, room_width/2, 0, wall));
        list->add(new FlipNormals(new XzRect(-room_width/2, room_width/2, -room_width/2, room_width/2, room_height, wall)));
    }

    // Table.
    list->add(new XzRect(-table_width/2, table_width/2, -table_width/2, table_width/2, table_height, table));

    // Marble.
    if (false) {
        Material *matte = new Metal(Vec3(1, 1, 1), 0);
        matte = new Lambertian(new ConstantTexture(Vec3(1, 1, 1)));
        list->add(new Sphere(look_at, marble_radius, matte));
    } else if (true) {
        add_marble(list, look_at, marble_radius, Vec3(0.6, 0.4, 0.9));
        add_marble(list, look_at + Vec3(marble_radius*2, 0, marble_radius*2), marble_radius, Vec3(0.9, 0.6, 0.4));
        add_marble(list, look_at + Vec3(marble_radius*2, 0, -marble_radius*2), marble_radius, Vec3(0.6, 0.9, 0.4));
    } else {
        Material *matte = new Metal(Vec3(0.6, 0.4, 0.9), 0.1);
        list->add(new Sphere(look_at, marble_radius, matte));
        matte = new Metal(Vec3(0.9, 0.6, 0.4), 0.1);
        list->add(new Sphere(look_at + Vec3(marble_radius*2, 0, marble_radius*2), marble_radius, matte));
        matte = new Metal(Vec3(0.6, 0.9, 0.4), 0.1);
        list->add(new Sphere(look_at + Vec3(marble_radius*2, 0, -marble_radius*2), marble_radius, matte));
    }

    // Windows.
    if (false) {
        list->add(new YzRect(window_height, window_height + window_size, -window_size - window_spacing/2, -window_spacing/2, window_position, light));
        list->add(new YzRect(window_height, window_height + window_size, window_spacing/2, window_spacing/2 + window_size, window_position, light));
        list->add(new XyRect(-window_size - window_spacing/2, -window_spacing/2, window_height, window_height + window_size, -window_position, light));
        list->add(new XyRect(window_spacing/2, window_spacing/2 + window_size, window_height, window_height + window_size, -window_position, light));
    }

    // return new World(list, Vec3(.2, .5, .8));
    return new World(list, new ImageTexture("data/HDR_111_Parking_Lot_2_Ref.hdr"));
}
*/

static Vec3 trace_ray(const Ray &r, World *world, int depth) {
    HitRecord rec;
    Vec3 color;

    if (world->m_hitable->hit(r, 0.001, FLT_MAX, rec)) {
        Ray scattered;
        Vec3 attenuation;

        // Start with whatever color the material is emitting.
        color = rec.material->emitted(rec.u, rec.v, rec.p);

        // And add any scattering.
        if (depth < 50 && rec.material->scatter(r, rec, attenuation, scattered)) {
            color += attenuation*trace_ray(scattered, world, depth + 1);
        }
    } else {
        if (world->m_background_texture != 0) {
            // Environment map.
            float u, v;
            vector_to_polar(r.direction().unit(), u, v);
            color = world->m_background_texture->value(u, v, VEC3_ORIGIN);
        } else {
            // Plain background.
            color = world->m_background_color;
        }
    }

    return color;
}

// Trace line "j".
static void trace_line(unsigned char *row, int j, int sample_count, const Camera &cam, World *world) {
    for (int i = 0; i < WIDTH && !g_quit; i++) {
        // Mark our pixel so it's visible in the interactive version.
        row[0] = 0;
        row[1] = 0;
        row[2] = 0;

        // Oversample.
        Vec3 c(0, 0, 0);
        for (int s = 0; s < sample_count; s++) {
            float u = (i + my_rand())/WIDTH;
            float v = (j + my_rand())/HEIGHT;

            Ray r = cam.get_ray(u, v);
            c += trace_ray(r, world, 0);
        }
        c /= sample_count;

        // Gamma correct.
        c = Vec3(sqrt(c.r()), sqrt(c.g()), sqrt(c.b()));

        // Clamp.
        c = c.min(VEC3_ONES);

        int ir = int(255.99*c.r());
        int ig = int(255.99*c.g());
        int ib = int(255.99*c.b());

        // minifb wants BGR:
        row[0] = ib;
        row[1] = ig;
        row[2] = ir;
        row += BYTES_PER_PIXEL;
    }
}

// Trace line "start" and every "skip" lines after that.
static void trace_lines(unsigned char *image, int start, int skip, int sample_count,
        const Camera &cam, World *world, int seed) {

    // Initialize the seed for our thread.
    init_rand(seed);

    for (int j = start; j < HEIGHT && !g_quit; j += skip) {
        unsigned char *row = image + j*STRIDE;

        trace_line(row, HEIGHT - 1 - j, sample_count, cam, world);

        g_lines_done++;
    }

    // We're no longer working.
    g_working--;
}

void render_frame(const Scene::Scene &scene, int frame, const char *output_pathname,
        int sample_count, InteractiveMode interactive_mode) {

    Camera cam;
    World *world = animation_scene(scene, cam, frame);

    unsigned char *image = new unsigned char[BYTE_COUNT];

    g_quit = false;

    while (!g_quit) {
        g_working = g_thread_count;
        g_lines_done = 0;

        // Time this pass.
        Timer pass_timer;

        // Generate the image on multiple threads.
        std::vector<std::thread *> thread;
        for (int t = 0; t < g_thread_count; t++) {
            thread.push_back(new std::thread(trace_lines, image, t, g_thread_count, sample_count,
                        cam, world, random()));
        }

#ifdef DISPLAY
        if (interactive_mode == IM_SINGLE) {
            while (g_working > 0) {
                int state = mfb_update(image);
                if (state < 0) {
                    // Tell workers to quit.
                    g_quit = true;
                } else {
                    usleep(30*1000);
                }
            }
        }
#endif

        if (interactive_mode == IM_BATCH) {
            while (g_working > 0) {
                int percent = g_lines_done*100/HEIGHT;
                std::cout << "\r" << percent << "%";
                std::cout.flush();
                usleep(100*1000);
            }
            std::cout << "\r              \r";
        }

        // Wait for worker threads to quit.
        for (int t = 0; t < g_thread_count; t++) {
            thread[t]->join();
            delete thread[t];
            thread[t] = 0;
        }

#ifdef DISPLAY
        if (interactive_mode == IM_ANIMATION) {
            int state = mfb_update(image);
            if (state < 0) {
                g_quit = true;
            }
        }
#endif

        if (!g_quit) {
            double elapsed = pass_timer.elapsed();

            std::cerr << "Frame " << frame << ": Pass with " << sample_count << " samples took " <<
                std::fixed << std::setprecision(elapsed < 0.2 ? 3 : 1) << elapsed << " seconds.\n";
        }

        // If we're animating, don't save, just return and do the next frame.
        if (interactive_mode == IM_ANIMATION) {
            break;
        }

        // Save the image if we weren't interrupted by the user.
        if (!g_quit) {
            // Convert from RGBA to RGB.
            const int RGB_BYTE_COUNT = PIXEL_COUNT*3;
            unsigned char *rgb_image = new unsigned char[RGB_BYTE_COUNT];
            unsigned char *rgba = image;
            unsigned char *rgb = rgb_image;
            for (int i = 0; i < PIXEL_COUNT; i++) {
                rgb[0] = rgba[2];
                rgb[1] = rgba[1];
                rgb[2] = rgba[0];

                rgba += 4;
                rgb += 3;
            }

            // Write image.
            std::ostringstream final_pathname;
            final_pathname << output_pathname << "-" << std::setfill('0') <<
                std::setw(3) << frame << ".png";

            int success = stbi_write_png(final_pathname.str().c_str(),
                    WIDTH, HEIGHT, 3, rgb_image, WIDTH*3);
            if (!success) {
                std::cerr << "Cannot write output image.\n";
            }

            if (interactive_mode == IM_SINGLE) {
                // Do another pass with more samples.
                sample_count *= 10;
            } else {
                // We're done.
                break;
            }
        }
    }
}

// Format is "first[,last[,step]]". Returns whether successful.
static bool parse_frames(char *s, int &first_frame, int &last_frame, int &frame_step) {
    char *orig = s;

    first_frame = strtol(s, &s, 10);
    if (*s == ',') {
        s++;
        last_frame = strtol(s, &s, 10);
        if (*s == ',') {
            s++;
            frame_step = strtol(s, &s, 10);
            if (*s != '\0') {
                std::cerr << "Invalid frame specification: " << orig << "\n";
                return false;
            }
        } else {
            frame_step = 1;
        }
    } else {
        last_frame = first_frame;
        frame_step = 1;
    }

    if (last_frame < first_frame) {
        std::cerr << "Frames cannot be in reverse order: " << orig << "\n";
        return false;
    }

    return true;
}

void usage() {
    std::cerr << "Usage: ray in.scene [frame [outprefix samples]]\n";
}

int main(int argc, char *argv[]) {
    // Make sure we linked with the right protobuf library.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Scene::Scene scene;
    int first_frame = 0;
    int last_frame = 0;
    int frame_step = 0;
    const char *output_pathname = "out";
    InteractiveMode interactive_mode;
    int sample_count = 1;

    // Skip program name
    argc--;
    argv++;

    // Required scene name.
    if (argc == 0) {
        usage();
        return -1;
    }
    char *scene_pathname = argv[0];
    argc--;
    argv++;

    // Read the scene.
    std::fstream input(scene_pathname, std::ios::in | std::ios::binary);
    if (!input) {
        std::cerr << scene_pathname << ": File not found.\n";
        usage();
        return -1;
    }

    if (!scene.ParseFromIstream(&input)) {
        std::cerr << "Failed to parse scene.\n";
        return -1;
    }

    if (argc > 0) {
        bool success = parse_frames(argv[0], first_frame, last_frame, frame_step);
        if (!success) {
            return -1;
        }
        argc--;
        argv++;
    }

    if (argc == 2) {
        // Batch.
        output_pathname = argv[0];
        sample_count = atoi(argv[1]);
        interactive_mode = IM_BATCH;
    } else {
#ifdef DISPLAY
        if (first_frame == last_frame) {
            interactive_mode = IM_SINGLE;
        } else {
            interactive_mode = IM_ANIMATION;
        }
#else
        interactive_mode = IM_BATCH;
        sample_count = 10;
#endif
    }

    g_thread_count = std::thread::hardware_concurrency();
    std::cout << "Using " << g_thread_count << " threads.\n";

#ifdef DISPLAY
    if (interactive_mode != IM_BATCH) {
        if (!mfb_open("ray", WIDTH, HEIGHT)) {
            std::cerr << "Failed to open the display.\n";
            return 0;
        }
    }
#endif

    for (int frame = first_frame; frame <= last_frame && !g_quit; frame += frame_step) {
        render_frame(scene, frame, output_pathname, sample_count, interactive_mode);
    }

    return 0;
}
