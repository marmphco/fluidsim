/*
    light.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_LIGHT
#define MJ_LIGHT

#include <vector>
#include "mjvector.h"
#include "mjtexture.h"

namespace mcjee {

    enum LightType {
        POINT_LIGHT       = 0,
        DIRECTIONAL_LIGHT = 1
    };

    class Light {
    public:
        LightType type;
        Vector3 position;
        Vector3 diffuseColor;
        Vector3 specularColor;
    };

    class Material {
    public:
        Vector3 diffuseColor;
        Vector3 specularColor;
        float specularPower;
        std::vector<Texture *> textures;
        Material();
    };
}

#endif
