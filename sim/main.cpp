
#include <iostream>

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

int main() {
    // Create the dynamics world
    rp3d::Vector3 gravity(0.0, -9.81, 0.0);
    rp3d::DynamicsWorld world(gravity);

    // Create the table.
    rp3d::BoxShape tableShape(rp3d::Vector3(10, 1, 10));
    rp3d::RigidBody *table = makeTable(world, tableShape);

    // Create a sphere.
    rp3d::SphereShape sphereShape(1);
    rp3d::RigidBody *marble[5];
    for (int m = 0; m < 5; m++) {
        rp3d::Transform transform(
                rp3d::Vector3((m - 2)/10.0, 3 + m*4, 0),
                rp3d::Quaternion::identity());

        marble[m] = world.createRigidBody(transform);
        marble[m]->setType(rp3d::BodyType::DYNAMIC);

        marble[m]->addCollisionShape(&sphereShape, rp3d::Transform::identity(), 4);
    }

    // Generate header.
    std::cout << "// This file is auto-generated. See the \"sim\" directory. Do not modify.\n";
    std::cout << "\n";
    std::cout << "#include \"animation.h\"\n";
    std::cout << "#include \"HitableList.h\"\n";
    std::cout << "#include \"Lambertian.h\"\n";
    std::cout << "#include \"ConstantTexture.h\"\n";
    std::cout << "#include \"Dielectric.h\"\n";
    std::cout << "#include \"Box.h\"\n";
    std::cout << "#include \"Rect.h\"\n";
    std::cout << "#include \"DiffuseLight.h\"\n";
    std::cout << "#include \"ConstantMedium.h\"\n";
    std::cout << "\n";
    std::cout << "static Texture *g_color[] = {\n";
    for (int m = 0; m < 5; m++) {
        std::cout << "    new ConstantTexture(hsv2rgb(Vec3(" << ((float) m / 5) << ", 1, 1))),\n";
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

    for (int f = 0; f < 200; f++) {
        std::cout << "    new World(new HitableList({\n";
        for (int m = 0; m < 5; m++) {
            rp3d::Transform transform = marble[m]->getTransform();
            rp3d::Vector3 position = transform.getPosition();
            std::cout << "            new Sphere(Vec3(" << position.x << ", " <<
                position.y << ", " << position.z << "), 1, g_marblesOut[" << m << "]),\n";
            std::cout << "            new ConstantMedium(new Sphere(Vec3(" << position.x << ", " <<
                position.y << ", " << position.z << "), 1, g_marblesOut[" << m << "]), 0.8, g_color[" << m << "]),\n";
        }
        std::cout << "            new Box(Vec3(-10, -2, -10), Vec3(10, 0, 10), new Lambertian(new ConstantTexture(Vec3(0.3, 0.3, 0.3)))),\n";
        std::cout << "            new XzRect(-5, 5, -5, 5, 10, new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)))),\n";
        std::cout << "        }), Vec3(.5, .5, .5)),\n";

        // Match GIF delay (3/100).
        world.update(0.030);
    }

    std::cout << "};\n";

    return 0;
}

