/*
    light.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjlight.h"

namespace mcjee {

Material::Material()  :
            diffuseColor(Vector3(1.0, 1.0, 1.0)),
            specularColor(Vector3(1.0, 1.0, 1.0)),
            specularPower(1.0) {}

}
