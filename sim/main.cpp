
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
                rp3d::Vector3((m - 2)/10.0, 3 + m*2, 0),
                rp3d::Quaternion::identity());

        marble[m] = world.createRigidBody(transform);
        marble[m]->setType(rp3d::BodyType::DYNAMIC);

        marble[m]->addCollisionShape(&sphereShape, rp3d::Transform::identity(), 4);
    }

    for (int f = 0; f < 100; f++) {
        for (int m = 0; m < 5; m++) {
            rp3d::Transform transform = marble[m]->getTransform();
            rp3d::Vector3 position = transform.getPosition();
            std::cout << position.y << " ";
        }
        std::cout << "\n";

        // Match GIF delay (3/100).
        world.update(0.030);
    }

    return 0;
}

