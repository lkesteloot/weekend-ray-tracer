
#include <fstream>
#include <iostream>

#include "Scene.pb.h"

#include "reactphysics3d.h"

using namespace reactphysics3d;

static rp3d::RigidBody *makeTable(rp3d::DynamicsWorld &world, rp3d::BoxShape &tableShape) {
    rp3d::Transform transform(
            rp3d::Vector3(0, -1, 0),
            rp3d::Quaternion::identity());
    rp3d::RigidBody *table = world.createRigidBody(transform);
    table->setType(rp3d::BodyType::STATIC);

    // XXX is mass important?
    table->addCollisionShape(&tableShape, rp3d::Transform::identity(), 4);

    return table;
}

static void set_vec3(Scene::Vec3 *dest, float x, float y, float z) {
    dest->set_x(x);
    dest->set_y(y);
    dest->set_z(z);
}

static void copy_vec3(Scene::Vec3 *dest, const rp3d::Vector3 &src) {
    set_vec3(dest, src.x, src.y, src.z);
}

static void set_quat(Scene::Quat *dest, float x, float y, float z, float w) {
    dest->set_x(x);
    dest->set_y(y);
    dest->set_z(z);
    dest->set_w(w);
}

static void copy_quat(Scene::Quat *dest, const rp3d::Quaternion &src) {
    set_quat(dest, src.x, src.y, src.z, src.w);
}

void usage() {
    std::cerr << "Usage: sim out.scene\n";
}


int main(int argc, char *argv[]) {
    Scene::Scene scene;

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

    // Create the dynamics world
    rp3d::Vector3 gravity(0.0, -9.81, 0.0);
    rp3d::DynamicsWorld dynamicsWorld(gravity);

    // Create the table.
    rp3d::BoxShape tableShape(rp3d::Vector3(6, 1, 6));
    makeTable(dynamicsWorld, tableShape);

    // Create a sphere.
    rp3d::SphereShape sphereShape(1);
    rp3d::RigidBody *marble[5];
    for (int m = 0; m < 5; m++) {
        rp3d::Transform transform(
                rp3d::Vector3((m - 2)/10.0, 3 + m*4, 0),
                rp3d::Quaternion::identity());

        marble[m] = dynamicsWorld.createRigidBody(transform);
        marble[m]->setType(rp3d::BodyType::DYNAMIC);

        marble[m]->addCollisionShape(&sphereShape, rp3d::Transform::identity(), 4);
    }

    /*
    // Generate header.
    std::cout << "static Texture *g_color[] = {\n";
    for (int m = 0; m < 5; m++) {
        // std::cout << "    new ConstantTexture(hsv2rgb(Vec3(" << ((float) m / 5) << ", 1, 1))),\n";
        std::cout << "    new CheckerTexture(\n";
        std::cout << "        new ConstantTexture(hsv2rgb(Vec3(" << ((float) m / 5) << ", 1, 1))),\n";
        std::cout << "        new ConstantTexture(hsv2rgb(Vec3(" << ((float) m / 5) << ", .9, 1))),\n";
        std::cout << "        0.2),\n";
    }
    std::cout << "};\n";
    std::cout << "static Material *g_marblesIn[] = {\n";
    for (int m = 0; m < 5; m++) {
        std::cout << "    new Lambertian(g_color[" << m << "]),\n";
    }
    std::cout << "};\n";
    std::cout << "static Material *g_marblesOut[] = {\n";
    for (int m = 0; m < 5; m++) {
        std::cout << "    new Dielectric(REF_GLASS),\n";
    }
    std::cout << "};\n";
    std::cout << "\n";
    std::cout << "std::vector<World *> g_frames = {\n";
    */

    for (int f = 0; f < 200; f++) {
        Scene::World *world = scene.add_world();

        set_vec3(world->mutable_background_color(), 0.5, 0.5, 0.5);

        for (int m = 0; m < 5; m++) {
            rp3d::Transform transform = marble[m]->getTransform();
            rp3d::Vector3 position = transform.getPosition();
            rp3d::Quaternion orientation = transform.getOrientation();

            Scene::Thing *thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_SPHERE);
            copy_vec3(thing->mutable_center(), position);
            set_vec3(thing->mutable_half_size(), 1, 1, 1);
            copy_quat(thing->mutable_orientation(), orientation);
            set_vec3(thing->mutable_color(), 1, 0, 0);
            thing->set_is_light(false);
        }

        // Table.
        {
            Scene::Thing *thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_BOX);
            set_vec3(thing->mutable_center(), 0, -1, 0);
            set_vec3(thing->mutable_half_size(), 6, 1, 6);
            set_quat(thing->mutable_orientation(), 0, 0, 0, 1);
            set_vec3(thing->mutable_color(), 0.3, 0.3, 0.3);
            thing->set_is_light(false);
        }

        // Light.
        {
            Scene::Thing *thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_XZ_RECT);
            set_vec3(thing->mutable_center(), 0, 10, 0);
            set_vec3(thing->mutable_half_size(), 5, 0, 5);
            set_quat(thing->mutable_orientation(), 0, 0, 0, 1);
            set_vec3(thing->mutable_color(), 7, 7, 7);
            thing->set_is_light(true);
        }

        // Match GIF delay (3/100).
        dynamicsWorld.update(0.030);
    }

    std::fstream output(scene_pathname, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!scene.SerializeToOstream(&output)) {
        std::cerr << "Failed to write scene to " << scene_pathname << "\n";
        return -1;
    }

    return 0;
}

