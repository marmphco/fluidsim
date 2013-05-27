/*
    mjmatrix.h
    Matthew Jee
    mcjee@ucsc.edu
*/
#ifndef MJ_MATRIX_H
#define MJ_MATRIX_H

#include "mjvector.h"
#include <cmath>
#include <ostream>
#include <iostream>

namespace mcjee {

union Matrix3 {
    float data[12];
    Vector3 operator*(Vector3 that);
    Matrix3 &transpose();
};

// stored in opengl usable form
class Matrix4 {
public:
    float data[16];
    Matrix4();
    Matrix4(float a, float b, float c, float d,
            float e, float f, float g, float h,
            float i, float j, float k, float l,
            float m, float n, float p, float q);
    Matrix4 operator*(Matrix4 &that) const;
    Matrix4 &operator*=(Matrix4 &that);
    Vector3 operator*(Vector3 that) const;

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
    Matrix4 &transpose();
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
