/*
    colors.h
    Matthew Jee
    mcjee@ucsc.edu

    Fun color cycling functions
*/

#ifndef COLORS_H
#define COLORS_H

#include "mjvector.h"

namespace mcjee {

Vector4 (*(getColorFunc)(int index))(float);
Vector4 bluish(float t);
Vector4 darkSmoke(float t);
Vector4 lightSmoke(float t);
Vector4 fire(float t);
Vector4 rainbows(float t);
Vector4 plasma(float t);

}

#endif
