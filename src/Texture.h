#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vec3.h"

// Interface for a procedural texture.
class Texture {
public:
    // Use u,v for 2D textures, p for 3D textures.
    virtual Vec3 value(float u, float v, const Vec3 &p) const = 0;
};

#endif // TEXTURE_H
