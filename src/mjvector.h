/*
	mjvector.h
	Matthew Jee
	mcjee@ucsc.edu
*/

#ifndef MJ_VECTOR_H
#define MJ_VECTOR_H

#include <iostream>

namespace mcjee {

class Vector3 {
public:
    float x;
    float y;
    float z;
    Vector3();
    Vector3(float sx, float sy, float sz);
    Vector3 operator-() const;
    Vector3 operator+(Vector3 that) const;
    Vector3 operator-(Vector3 that) const;
    Vector3 operator*(Vector3 that) const;
    Vector3 operator*(float scalar) const;
    Vector3 &operator+=(Vector3 that);
    Vector3 &operator+=(float that);
    Vector3 &operator*=(Vector3 that);
    Vector3 &operator*=(float scalar);
    float length();
    float dot(Vector3 that);
    Vector3 cross(Vector3 that);
    Vector3 normalized();
    Vector3 &normalize();
    friend std::ostream &operator<<(std::ostream &out,
                                    Vector3 mat);
};

class Vector4 {
public:
    float r;
    float g;
    float b;
    float a;
    Vector4();
    Vector4(float sr, float sg, float sb, float sa);
    Vector4 operator-() const;
    Vector4 operator+(Vector4 that) const;
    Vector4 operator-(Vector4 that) const;
    Vector4 operator*(Vector4 that) const;
    Vector4 operator*(float scalar) const;
    Vector4 &operator+=(Vector4 that);
    Vector4 &operator+=(float that);
    Vector4 &operator*=(Vector4 that);
    Vector4 &operator*=(float scalar);
    float length();
    float dot(Vector4 that);
    Vector4 normalized();
    Vector4 &normalize();
    friend std::ostream &operator<<(std::ostream &out,
                                    Vector4 mat);
};

const Vector3 X_AXIS(1.0, 0.0, 0.0);
const Vector3 Y_AXIS(0.0, 1.0, 0.0);
const Vector3 Z_AXIS(0.0, 0.0, 1.0);

}

#endif
