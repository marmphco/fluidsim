/*
	mjvector.cpp
	Matthew Jee
	mcjee@ucsc.edu
*/

#include "mjvector.h"
#include <cmath>

namespace mcjee {

Vector3::Vector3() : x(0), y(0), z(0) {}

Vector3::Vector3(float sx, float sy, float sz):
    x(sx), y(sy), z(sz) {}

Vector3 Vector3::operator+(Vector3 that) {
    return Vector3(x+that.x, y+that.y, z+that.z);
}

Vector3 Vector3::operator-(Vector3 that) {
    return Vector3(x-that.x, y-that.y, z-that.z);
}

Vector3 Vector3::operator*(Vector3 that) { //component-wise
    return Vector3(x*that.x, y*that.y, z*that.z);
}

Vector3 Vector3::operator*(float scalar) {
    return Vector3(x*scalar, y*scalar, z*scalar);
}

Vector3 &Vector3::operator+=(Vector3 that) {
    x += that.x;
    y += that.y;
    z += that.z;
    return *this;
}

Vector3 &Vector3::operator+=(float that) {
    x += that;
    y += that;
    z += that;
    return *this;
}

Vector3 &Vector3::operator*=(Vector3 &that) {
    x *= that.x;
    y *= that.y;
    z *= that.z;
    return *this;
}

Vector3 &Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

float Vector3::length() {
    return sqrtf(x*x+y*y+z*z);
}

float Vector3::dot(Vector3 &that) {
    return x*that.x+y*that.y+z*that.z;
}

Vector3 Vector3::cross(Vector3 &that) {
    return Vector3(y*that.z-z*that.y, z*that.x-x*that.z, x*that.y-y*that.x);
}

Vector3 Vector3::normalized() {
    float l = length();
    return Vector3(x/l, y/l, z/l);
}

void Vector3::normalize() {
    float linverse = 1/length();
    *this *= linverse;
}

}

std::ostream &operator<<(std::ostream &out,
                         mcjee::Vector3 vec) {
    out << "[" << vec.x << ", "
        << vec.y << ", "
        << vec.z << "]";
    return out;
}