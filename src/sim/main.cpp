
#include <fstream>
#include <iostream>

#include "Scene.pb.h"

#include "reactphysics3d.h"

using namespace reactphysics3d;

const float REF_AIR = 1;
const float REF_GLASS = 1.5;  // 1.3 to 1.7
const float REF_DIAMOND = 2.4;
const int MARBLE_COUNT = 20;
const float MARBLE_RADIUS = 1;

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

static Scene::Vec3 *make_vec3(float x, float y, float z) {
    Scene::Vec3 *v = new Scene::Vec3;
    v->set_x(x);
    v->set_y(y);
    v->set_z(z);
    return v;
}

static Scene::Vec3 *from_vector3(const rp3d::Vector3 &src) {
    return make_vec3(src.x, src.y, src.z);
}

static Scene::Quat *make_quat(float x, float y, float z, float w) {
    Scene::Quat *q = new Scene::Quat;
    q->set_x(x);
    q->set_y(y);
    q->set_z(z);
    q->set_w(w);
    return q;
}

static Scene::Quat *from_quaternion(const rp3d::Quaternion &src) {
    return make_quat(src.x, src.y, src.z, src.w);
}

// https://stackoverflow.com/a/7901693/211234
Scene::Vec3 *hsv2rgb(float h, float s, float v) {
    // Unsaturated is just gray.
    if (s <= 0.0) {
        return make_vec3(v, v, v);
    }

    // Wrap hue.
    h = fmodf(h, 1);

    // Find section.
    h *= 6;
    int section = (int) h;

    // Find hue within section.
    h -= section;

    float p = v*(1 - s);
    float q = v*(1 - s*h);
    float t = v*(1 - s*(1.0 - h));

    switch(section) {
        case 0:
            return make_vec3(v, t, p);
            break;

        case 1:
            return make_vec3(q, v, p);
            break;

        case 2:
            return make_vec3(p, v, t);
            break;

        case 3:
            return make_vec3(p, q, v);
            break;

        case 4:
            return make_vec3(t, p, v);
            break;

        case 5:
        default:
            return make_vec3(v, p, q);
            break;
    }
}

static Scene::Texture *make_constant_texture(Scene::Vec3 *color) {
    Scene::Texture *texture = new Scene::Texture;
    texture->set_texture_type(Scene::TEXTURE_CONSTANT);
    texture->set_allocated_color(color);
    return texture;
}

static Scene::Texture *make_transform_texture(Scene::Texture *sub_texture,
        Scene::Vec3 *translation, Scene::Quat *orientation) {

    Scene::Texture *texture = new Scene::Texture;
    texture->set_texture_type(Scene::TEXTURE_TRANSFORM);
    texture->set_allocated_sub_texture(sub_texture);
    texture->set_allocated_translation(translation);
    texture->set_allocated_orientation(orientation);
    return texture;
}

static Scene::Texture *make_marble_texture(float m) {
    Scene::Texture *texture = new Scene::Texture;
    texture->set_texture_type(Scene::TEXTURE_CHECKER);
    texture->set_allocated_odd(make_constant_texture(hsv2rgb(m, 1, 1)));
    texture->set_allocated_even(make_constant_texture(hsv2rgb(m, 0.9, 1)));
    texture->set_size(0.2);
    return texture;
}

static Scene::Material *make_lambertian_material(Scene::Texture *texture) {
    Scene::Material *material = new Scene::Material;
    material->set_material_type(Scene::MATERIAL_LAMBERTIAN);
    material->set_allocated_texture(texture);
    return material;
}

static Scene::Material *make_dielectric_material(float refraction_index) {
    Scene::Material *material = new Scene::Material;
    material->set_material_type(Scene::MATERIAL_DIELECTRIC);
    material->set_refraction_index(refraction_index);
    return material;
}

static Scene::Material *make_diffuse_light_material(Scene::Texture *texture) {
    Scene::Material *material = new Scene::Material;
    material->set_material_type(Scene::MATERIAL_DIFFUSE_LIGHT);
    material->set_allocated_texture(texture);
    return material;
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
    rp3d::SphereShape sphereShape(MARBLE_RADIUS);
    rp3d::RigidBody *marble[MARBLE_COUNT];
    for (int m = 0; m < MARBLE_COUNT; m++) {
        rp3d::Transform transform(
                rp3d::Vector3((m - 2)/10.0, 3 + m*4, 0),
                rp3d::Quaternion::identity());

        marble[m] = dynamicsWorld.createRigidBody(transform);
        marble[m]->setType(rp3d::BodyType::DYNAMIC);

        marble[m]->addCollisionShape(&sphereShape, rp3d::Transform::identity(), 4);
    }

    for (int f = 0; f < 200; f++) {
        Scene::World *world = scene.add_world();

        // Set up camera.
        Scene::Camera *camera = world->mutable_camera();
        camera->set_allocated_look_at(make_vec3(0, 1, 0));
        camera->set_allocated_look_from(make_vec3(0, 3, -10 - f/10.0));
        camera->set_aperature(0.2);
        camera->set_vertical_fov(20);

        // Neutral background.
        world->set_allocated_background_color(make_vec3(0.5, 0.5, 0.5));

        for (int m = 0; m < MARBLE_COUNT; m++) {
            rp3d::Transform transform = marble[m]->getTransform();
            rp3d::Vector3 position = transform.getPosition();
            rp3d::Quaternion orientation = transform.getOrientation();

            // External (dielectric) sphere.
            Scene::Thing *thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_SPHERE);
            thing->set_allocated_center(from_vector3(position));
            thing->set_allocated_half_size(make_vec3(MARBLE_RADIUS, MARBLE_RADIUS, MARBLE_RADIUS));
            thing->set_allocated_material(make_dielectric_material(REF_GLASS));

            // Internal (diffuse) sphere.
            thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_SPHERE);
            thing->set_allocated_center(from_vector3(position));
            thing->set_allocated_half_size(make_vec3(MARBLE_RADIUS*0.95, MARBLE_RADIUS*0.95, MARBLE_RADIUS*0.95));
            thing->set_allocated_orientation(from_quaternion(orientation));
            thing->set_allocated_material(make_lambertian_material(
                    make_transform_texture(
                        make_marble_texture(float(m)/MARBLE_COUNT),
                        from_vector3(position),
                        from_quaternion(orientation))));
        }

        // Table.
        {
            Scene::Thing *thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_BOX);
            thing->set_allocated_center(make_vec3(0, -1, 0));
            thing->set_allocated_half_size(make_vec3(6, 1, 6));
            thing->set_allocated_orientation(make_quat(0, 0, 0, 1));
            thing->set_allocated_material(make_lambertian_material(
                        make_constant_texture(make_vec3(0.3, 0.3, 0.3))));
        }

        // Light.
        {
            Scene::Thing *thing = world->add_thing();
            thing->set_shape(Scene::SHAPE_XZ_RECT);
            thing->set_allocated_center(make_vec3(0, 10, 0));
            thing->set_allocated_half_size(make_vec3(5, 0, 5));
            thing->set_allocated_orientation(make_quat(0, 0, 0, 1));
            thing->set_allocated_material(make_diffuse_light_material(
                        make_constant_texture(make_vec3(7, 7, 7))));
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

