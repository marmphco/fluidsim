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

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator+(Vector3 that) const {
    return Vector3(x+that.x, y+that.y, z+that.z);
}

Vector3 Vector3::operator-(Vector3 that) const {
    return Vector3(x-that.x, y-that.y, z-that.z);
}

Vector3 Vector3::operator*(Vector3 that) const {
    return Vector3(x*that.x, y*that.y, z*that.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x*scalar, y*scalar, z*scalar);
}

Vector3 &Vector3::operator+=(Vector3 that) {
    x += that.x; y += that.y; z += that.z;
    return *this;
}

Vector3 &Vector3::operator+=(float that) {
    x += that; y += that; z += that;
    return *this;
}

Vector3 &Vector3::operator*=(Vector3 that) {
    x *= that.x; y *= that.y; z *= that.z;
    return *this;
}

Vector3 &Vector3::operator*=(float scalar) {
    x *= scalar; y *= scalar; z *= scalar;
    return *this;
}

float Vector3::length() {
    return sqrtf(x*x+y*y+z*z);
}

float Vector3::dot(Vector3 that) {
    return x*that.x+y*that.y+z*that.z;
}

Vector3 Vector3::cross(Vector3 that) {
    return Vector3(y*that.z-z*that.y, z*that.x-x*that.z, x*that.y-y*that.x);
}

Vector3 Vector3::normalized() {
    float l = length();
    return std::isfinite(1/l) ? Vector3(x/l, y/l, z/l) : *this;
}

Vector3 &Vector3::normalize() {
    float linverse = 1/length();
    if (std::isfinite(linverse)) *this *= linverse;
    return *this;
}

std::ostream &operator<<(std::ostream &out,
                         mcjee::Vector3 vec) {
    out << "[" << vec.x << ", "
        << vec.y << ", "
        << vec.z << "]";
    return out;
}

Vector4::Vector4() : r(0), g(0), b(0), a(0) {}

Vector4::Vector4(float sr, float sg, float sb, float sa):
    r(sr), g(sg), b(sb), a(sa) {}

Vector4 Vector4::operator-() const {
    return Vector4(-r, -g, -b, -a);
}

Vector4 Vector4::operator+(Vector4 that) const {
    return Vector4(r+that.r, g+that.g, b+that.b, a+that.a);
}

Vector4 Vector4::operator-(Vector4 that) const {
    return Vector4(r-that.r, g-that.g, b-that.b, a-that.a);
}

Vector4 Vector4::operator*(Vector4 that) const {
    return Vector4(r*that.r, g*that.g, b*that.b, a*that.a);
}

Vector4 Vector4::operator*(float scalar) const {
    return Vector4(r*scalar, g*scalar, b*scalar, a*scalar);
}

Vector4 &Vector4::operator+=(Vector4 that) {
    r += that.r; g += that.g; b += that.b; a += that.a;
    return *this;
}

Vector4 &Vector4::operator+=(float that) {
    r += that; g += that; b += that; a += that;
    return *this;
}

Vector4 &Vector4::operator*=(Vector4 that) {
    r *= that.r; g *= that.g; b *= that.b; a *= that.a;
    return *this;
}

Vector4 &Vector4::operator*=(float scalar) {
    r *= scalar; g *= scalar; b *= scalar; a *= scalar;
    return *this;
}

float Vector4::length() {
    return sqrtf(r*r+g*g+b*b+a*a);
}

float Vector4::dot(Vector4 that) {
    return r*that.r+g*that.g+b*that.b+a*that.a;
}

Vector4 Vector4::normalized() {
    float l = length();
    return std::isfinite(1/l) ? Vector4(r/l, g/l, b/l, a/l) : *this;
}

Vector4 &Vector4::normalize() {
    float linverse = 1/length();
    if (std::isfinite(linverse)) *this *= linverse;
    return *this;
}

std::ostream &operator<<(std::ostream &out,
                         mcjee::Vector4 vec) {
    out << "[" << vec.r << ", "
        << vec.g << ", "
        << vec.b << ", "
        << vec.a << "]";
    return out;
}

}
