/*
    colors.cpp
    Matthew Jee
    mcjee@ucsc.edu

    Fun color cycling functions
*/

#include "colors.h"
#include <cmath>

namespace mcjee {

static Vector4 (*functions[])(float) = {
    bluish,
    darkSmoke,
    lightSmoke,
    fire,
    rainbows,
    plasma
};

Vector4 (*(getColorFunc)(int index))(float) {
    return functions[index];
}

Vector4 bluish(float t) {
    float vx = sinf(t)*100.0;
    float vy = cosf(t)*100.0;
    float r = vx < 0 ? 0 : vx;
    float g = vy < 0 ? 0 : vy;
    float b = 50;
    return Vector4(r, g, b, 10);
}

Vector4 darkSmoke(float) {
    return Vector4(10, 10, 10, 10);
}

Vector4 lightSmoke(float) {
    return Vector4(10, 10, 10, 10);
}

Vector4 fire(float t) {
    float vx = sinf(t)*100.0;
    float vy = cosf(t)*100.0;
    float r = vx < 0 ? 0 : vx;
    float g = vy < 0 ? 0 : vy;
    float b = 50;
    return Vector4(r, g, b, 10);
}

Vector4 rainbows(float t) {
    float vx = sinf(t)*100.0;
    float vy = cosf(t)*100.0;
    float r = vx < 0 ? 0 : vx;
    float g = vy < 0 ? 0 : vy;
    float b = 50;
    return Vector4(r, g, b, 10);
}

Vector4 plasma(float t) {
    float vx = sinf(t)*100.0;
    float vy = cosf(t)*100.0;
    float r = vx < 0 ? 0 : vx;
    float g = vy < 0 ? 0 : vy;
    float b = 50;
    return Vector4(r, g, b, 10);
}

}
