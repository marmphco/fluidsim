/*
    light.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_LIGHT
#define MJ_LIGHT

#include <vector>
#include "mjmath.h"
#include "mjtexture.h"

namespace mcjee {

    enum LightType {
        POINT_LIGHT       = 0,
        DIRECTIONAL_LIGHT = 1,
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
        Material() :
            diffuseColor(Vector3(1.0, 1.0, 1.0)),
            specularColor(Vector3(1.0, 1.0, 1.0)),
            specularPower(1.0) {};
    };
}

#endif
