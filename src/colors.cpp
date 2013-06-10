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
    pastel,
    darkSmoke,
    lightSmoke,
    fire,
    rainbows,
    plasma
};

Vector4 (*(getColorFunc)(int index))(float) {
    return functions[index];
}

Vector4 pastel(float t) {
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
    float vx = sinf(t)*15.0+85.0;
    float vy = 40.0;
    float r = vx < 0 ? 0 : vx;
    float g = vy < 0 ? 0 : vy;
    return Vector4(r, g, 10.0, 1);
}

Vector4 rainbows(float t) {
    float x = (t-10.0*(int)(t/10.0))/10.0;
    float vr = 0.0;
    float vg = 0.0;
    float vb = 0.0;
    if (x > 0.0 && x <= 1.0/3.0) {
        vr = 1.0-x*3.0;
        vg = x*3.0;
    } else if (x > 1.0/3.0 && x <= 2.0/3.0) {
        vg = 1.0-(x-1.0/3.0)*3.0;
        vb = (x-1.0/3.0)*3.0;
    } else if (x > 2.0/3.0 && x <= 1.0) {
        vb = 1.0-(x-2.0/3.0)*3.0;
        vr = (x-2.0/3.0)*3.0;
    }

    float r = vr < 0 ? 0 : vr;
    float g = vg < 0 ? 0 : vg;
    float b = vb < 0 ? 0 : vb;
    return Vector4(r, g, b, 0)*100.0;
}

Vector4 plasma(float t) {
    float vx = sinf(t)*20.0+40.0;
    float vy = cosf(t)*20.0+80.0;
    float r = vx < 0 ? 0 : vx;
    float b = vy < 0 ? 0 : vy;
    return Vector4(r, 10.0, b, 1);
}

}
