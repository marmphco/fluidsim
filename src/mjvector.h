/*
	mjvector.h
	Matthew Jee
	mcjee@ucsc.edu
*/

#ifndef MJ_VECTOR_H
#define MJ_VECTOR_H

#include <iostream>

namespace mcjee {

union Vector3 {
    float data[3];
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
    struct {
        float t;
        float u;
        float v;
    };
    Vector3();
    Vector3(float sx, float sy, float sz);
    Vector3 operator+(Vector3 that);
    Vector3 operator-(Vector3 that);
    Vector3 operator*(Vector3 that);
    Vector3 operator*(float scalar);
    Vector3 &operator+=(Vector3 that);
    Vector3 &operator+=(float that);
    Vector3 &operator*=(Vector3 &that);
    Vector3 &operator*=(float scalar);
    float length();
    float dot(Vector3 &that);
    Vector3 cross(Vector3 &that);
    Vector3 normalized();
    void normalize();
    friend std::ostream &operator<<(std::ostream &out,
                                    Vector3 mat);
};

const Vector3 X_AXIS(1.0, 0.0, 0.0);
const Vector3 Y_AXIS(0.0, 1.0, 0.0);
const Vector3 Z_AXIS(0.0, 0.0, 1.0);

}

#endif
