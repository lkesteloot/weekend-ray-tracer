#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H

#include "Texture.h"
#include "perlin.h"

// A texture with a noise pattern.
class NoiseTexture : public Texture {
public:
    float m_scale;

    NoiseTexture(float scale)
        : m_scale(scale) {

        // Nothing.
    }

    virtual Vec3 value(float, float, const Vec3 &p) const {
        //return Vec3(1, 1, 1)*0.5*(1 + perlin_noise(m_scale*p));
        //return Vec3(1, 1, 1)*perlin_turb(m_scale*p, 7);
        return Vec3(1, 1, 1)*0.5*(1 + sin(m_scale*p.x() + 5*perlin_turb(m_scale*p, 7)));
    }
};

#endif // NOISE_TEXTURE_H
