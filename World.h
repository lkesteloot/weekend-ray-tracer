#ifndef WORLD_H
#define WORLD_H

#include "Hitable.h"
#include "Texture.h"

class World {
public:
    Hitable *m_hitable;
    // Set one or the other:
    Vec3 m_background_color;
    Texture *m_background_texture;

    World(Hitable *hitable, const Vec3 &background_color)
        : m_hitable(hitable), m_background_color(background_color), m_background_texture(0) {

        // Nothing.
    }
    World(Hitable *hitable, Texture *background_texture)
        : m_hitable(hitable), m_background_texture(background_texture) {

        // Nothing.
    }
};

#endif // WORLD_H
