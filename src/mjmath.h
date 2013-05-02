/*
    mjmath.h
    Matthew Jee
    mcjee@ucsc.edu

    Only C++ for now.
*/
#ifndef MJ_MATH_H
#define MJ_MATH_H

#include <cmath>
#include <ostream>
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
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float sx, float sy, float sz):
        x(sx), y(sy), z(sz) {}
    Vector3 operator+(Vector3 &that) {
        return Vector3(x+that.x, y+that.y, z+that.z);
    }
    Vector3 operator-(Vector3 &that) {
        return Vector3(x-that.x, y-that.y, z-that.z);
    }
    Vector3 operator*(Vector3 &that) { //component-wise
        return Vector3(x*that.x, y*that.y, z*that.z);
    }
    Vector3 operator*(float scalar) {
        return Vector3(x*scalar, y*scalar, z*scalar);
    }
    Vector3 &operator+=(Vector3 that) {
        x += that.x;
        y += that.y;
        z += that.z;
        return *this;
    }
    Vector3 &operator*=(Vector3 &that) {
        x *= that.x;
        y *= that.y;
        z *= that.z;
        return *this;
    }
    Vector3 &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    float length() {
        return sqrtf(x*x+y*y);
    }
    float dot(Vector3 &that) {
        return x*that.x+y*that.y+z*that.z;
    }
    Vector3 cross(Vector3 &that) {
        return Vector3(y*that.z-z*that.y, z*that.x-x*that.z, x*that.y-y*that.z);
    }
    Vector3 normalized() {
        float l = length();
        return Vector3(x/l, y/l, z/l);
    }
    void normalize() {
        float linverse = 1/length();
        *this *= linverse;
    }
    friend std::ostream &operator<<(std::ostream &out,
                         mcjee::Vector3 &mat);
};

union Matrix3 {
    float data[12];
    Vector3 operator*(Vector3 that);
    Matrix3 &transpose();
};

// stored in opengl usable form
union Matrix4 {
    float data[16];
    struct { //sort of for convenience
        float sx; float  a; float  b; float c;
        float  d; float sy; float  e; float f;
        float  g; float  h; float sz; float i;
        float tx; float ty; float tz; float w;
    };
    struct {
        float c0[4];
        float c1[4];
        float c2[4];
        float c3[4];
    };
    Matrix4();
    Matrix4(float a, float b, float c, float d,
            float e, float f, float g, float h,
            float i, float j, float k, float l,
            float m, float n, float p, float q);
    Matrix4 operator*(Matrix4 &that);
    //Matrix4 operator[]();
    void identity();
    void frustum(float left,
                 float right,
                 float bottom,
                 float top,
                 float near,
                 float far);
    void ortho(float left,
               float right,
               float bottom,
               float top,
               float near,
               float far);
    Matrix4 &translate(float x, float y, float z);
    Matrix4 &scale(float x, float y, float z);
    Matrix4 &rotate(float angle, Vector3 axis);
    Matrix4 &rotateX(float angle);
    Matrix4 &rotateY(float angle);
    Matrix4 &rotateZ(float angle);
    Matrix3 matrix3();
    friend std::ostream &operator<<(std::ostream &out,
                         mcjee::Matrix4 &mat);
};

}

#endif
