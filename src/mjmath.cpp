/*
    mjmath.cpp
*/

#include "mjmath.h"

namespace mcjee {

Matrix4::Matrix4() {
    identity();
}
Matrix4::Matrix4(float a, float b, float c, float d,
                 float e, float f, float g, float h,
                 float i, float j, float k, float l,
                 float m, float n, float p, float q) {
    data[0] = a;   data[1] = e;  data[2] = i;  data[3] = m;
    data[4] = b;   data[5] = f;  data[6] = j;  data[7] = n;
    data[8] = c;   data[9] = g; data[10] = k; data[11] = p;
    data[12] = d; data[13] = h; data[14] = l; data[15] = q;
}

Matrix4 Matrix4::operator*(Matrix4 &m) {
    Matrix4 temp;
    temp.c0[0] = c0[0]*m.c0[0]+c1[0]*m.c0[1]+c2[0]*m.c0[2]+c3[0]*m.c0[3];
    temp.c0[1] = c0[1]*m.c0[0]+c1[1]*m.c0[1]+c2[1]*m.c0[2]+c3[1]*m.c0[3];
    temp.c0[2] = c0[2]*m.c0[0]+c1[2]*m.c0[1]+c2[2]*m.c0[2]+c3[2]*m.c0[3];
    temp.c0[3] = c0[3]*m.c0[0]+c1[3]*m.c0[1]+c2[3]*m.c0[2]+c3[3]*m.c0[3];

    temp.c1[0] = c0[0]*m.c1[0]+c1[0]*m.c1[1]+c2[0]*m.c1[2]+c3[0]*m.c1[3];
    temp.c1[1] = c0[1]*m.c1[0]+c1[1]*m.c1[1]+c2[1]*m.c1[2]+c3[1]*m.c1[3];
    temp.c1[2] = c0[2]*m.c1[0]+c1[2]*m.c1[1]+c2[2]*m.c1[2]+c3[2]*m.c1[3];
    temp.c1[3] = c0[3]*m.c1[0]+c1[3]*m.c1[1]+c2[3]*m.c1[2]+c3[3]*m.c1[3];

    temp.c2[0] = c0[0]*m.c2[0]+c1[0]*m.c2[1]+c2[0]*m.c2[2]+c3[0]*m.c2[3];
    temp.c2[1] = c0[1]*m.c2[0]+c1[1]*m.c2[1]+c2[1]*m.c2[2]+c3[1]*m.c2[3];
    temp.c2[2] = c0[2]*m.c2[0]+c1[2]*m.c2[1]+c2[2]*m.c2[2]+c3[2]*m.c2[3];
    temp.c2[3] = c0[3]*m.c2[0]+c1[3]*m.c2[1]+c2[3]*m.c2[2]+c3[3]*m.c2[3];

    temp.c3[0] = c0[0]*m.c3[0]+c1[0]*m.c3[1]+c2[0]*m.c3[2]+c3[0]*m.c3[3];
    temp.c3[1] = c0[1]*m.c3[0]+c1[1]*m.c3[1]+c2[1]*m.c3[2]+c3[1]*m.c3[3];
    temp.c3[2] = c0[2]*m.c3[0]+c1[2]*m.c3[1]+c2[2]*m.c3[2]+c3[2]*m.c3[3];
    temp.c3[3] = c0[3]*m.c3[0]+c1[3]*m.c3[1]+c2[3]*m.c3[2]+c3[3]*m.c3[3];
    return temp;
}

void Matrix4::identity() {
    data[0] = 1;   data[1] = 0;  data[2] = 0;  data[3] = 0;
    data[4] = 0;   data[5] = 1;  data[6] = 0;  data[7] = 0;
    data[8] = 0;   data[9] = 0; data[10] = 1; data[11] = 0;
    data[12] = 0; data[13] = 0; data[14] = 0; data[15] = 1;
}
void Matrix4::ortho(float left,
                    float right,
                    float bottom,
                    float top,
                    float near,
                    float far) {
    float xlength = right-left;
    float ylength = top-bottom;
    float zlength = far-near;
    sx = 2/xlength;
    sy = 2/ylength;
    sz = 2/zlength;
    tx = (right+left)/xlength;
    ty = (top+bottom)/ylength;
    tz = (far+near)/zlength;
}
void Matrix4::frustum(float left,
                      float right,
                      float bottom,
                      float top,
                      float near,
                      float far) {
    float xlength = right-left;
    float ylength = top-bottom;
    float zlength = far-near;
    float near2 = 2*near;
    //this shorthand is not good semantically
    sx = near2/xlength;
    sy = near2/ylength;
    sz = -(far+near)/zlength;
    g = (right+left)/xlength;
    h = (top+bottom)/ylength;
    tz = -near2*far/zlength;
    i = -1;
}

Matrix4 &Matrix4::translate(float x, float y, float z) {
    data[0] += data[3]*x;
    data[1] += data[3]*y;
    data[2] += data[3]*z;

    data[4] += data[7]*x;
    data[5] += data[7]*y;
    data[6] += data[7]*z;

    data[8] += data[11]*x;
    data[9] += data[11]*y;
    data[10] += data[11]*z;

    data[12] += data[15]*x;
    data[13] += data[15]*y;
    data[14] += data[15]*z;

    return *this;
}

Matrix4 &Matrix4::scale(float x, float y, float z) {
    data[0] *= x;
    data[1] *= y;
    data[2] *= z;

    data[4] *= x;
    data[5] *= y;
    data[6] *= z;

    data[8] *= x;
    data[9] *= y;
    data[10] *= z;

    data[12] *= x;
    data[13] *= y;
    data[14] *= z;
    return *this;
}

// inefficient
Matrix4 &Matrix4::rotate(float angle, Vector3 axis) {
    float rad = angle*3.141592654/180;
    float f = sinf(rad/2.0);
    float w = cosf(rad/2.0);
    float x = f*axis.x;
    float y = f*axis.y;
    float z = f*axis.z;

    float xx2 = 2*x*x;
    float yy2 = 2*y*y;
    float zz2 = 2*z*z;
    float xy2 = 2*x*y;
    float xz2 = 2*x*z;
    float yz2 = 2*y*z;
    float wx2 = 2*w*x;
    float wy2 = 2*w*y;
    float wz2 = 2*w*z;
    
    Matrix4 temp;
    temp.data[0] = 1-yy2-zz2; temp.data[1] = xy2+wz2;   temp.data[2] = xz2-wy2;    temp.data[3] = 0;
    temp.data[4] = xy2-wz2;   temp.data[5] = 1-xx2-zz2; temp.data[6] = yz2+wx2;    temp.data[7] = 0;
    temp.data[8] = xz2+wy2;   temp.data[9] = yz2-wx2;   temp.data[10] = 1-xx2-yy2; temp.data[11] = 0;
    temp.data[12] = 0;        temp.data[13] = 0;        temp.data[14] = 0;         temp.data[15] = 1;

    *this = temp**this;
    return *this;
}

Matrix4 &Matrix4::rotateX(float angle) {
    float c = cosf(angle*3.141592654/180);
    float s = sinf(angle*3.141592654/180);

    float d1 = data[1];
    float d5 = data[5];
    float d9 = data[9];
    float d13 = data[13];
    float d2 = data[2];
    float d6 = data[6];
    float d10 = data[10];
    float d14 = data[14];

    data[1] = c*d1-s*d2;
    data[5] = c*d5-s*d6;
    data[9] = c*d9-s*d10;
    data[13] = c*d13-s*d14;

    data[2] = s*d1+c*d2;
    data[6] = s*d5+c*d6;
    data[10] = s*d9+c*d10;
    data[14] = s*d13+c*d14;
    return *this;
}

Matrix4 &Matrix4::rotateY(float angle) {
    float c = cosf(angle*3.141592654/180);
    float s = sinf(angle*3.141592654/180);

    float d0 = data[0];
    float d4 = data[4];
    float d8 = data[8];
    float d12 = data[12];
    float d2 = data[2];
    float d6 = data[6];
    float d10 = data[10];
    float d14 = data[14];

    data[0] = c*d0+s*d2;
    data[4] = c*d4+s*d6;
    data[8] = c*d8+s*d10;
    data[12] = c*d12+s*d14;

    data[2] = -s*d0+c*d2;
    data[6] = -s*d4+c*d6;
    data[10] = -s*d8+c*d10;
    data[14] = -s*d12+c*d14;
    return *this;
}

Matrix4 &Matrix4::rotateZ(float angle) {
    float c = cosf(angle*3.141592654/180);
    float s = sinf(angle*3.141592654/180);

    float d0 = data[0];
    float d4 = data[4];
    float d8 = data[8];
    float d12 = data[12];
    float d1 = data[1];
    float d5 = data[5];
    float d9 = data[9];
    float d13 = data[13];

    data[0] = c*d0-s*d1;
    data[4] = c*d4-s*d5;
    data[8] = c*d8-s*d9;
    data[12] = c*d12-s*d13;

    data[1] = s*d0+c*d1;
    data[5] = s*d4+c*d5;
    data[9] = s*d8+c*d9;
    data[13] = s*d12+c*d13;
    return *this;
}

Matrix3 Matrix4::matrix3() {
    Matrix3 temp;
    temp.data[0] = data[0]; temp.data[1] = data[1]; temp.data[2] = data[2];
    temp.data[3] = data[4]; temp.data[4] = data[5]; temp.data[5] = data[6];
    temp.data[6] = data[8]; temp.data[7] = data[9]; temp.data[8] = data[10];
    return temp;
}

Vector3 Matrix3::operator*(Vector3 that) {
    Vector3 temp;
    temp.x = that.x*data[0]+that.y*data[3]+that.z*data[6];
    temp.y = that.x*data[1]+that.y*data[4]+that.z*data[7];
    temp.z = that.x*data[2]+that.y*data[5]+that.z*data[8];
    return temp;
}

Matrix3 &Matrix3::transpose() {
    data[1] = data[3];
    data[2] = data[6];
    data[3] = data[1];
    data[5] = data[7];
    data[6] = data[2];
    data[7] = data[7];
    return *this;
}

std::ostream &operator<<(std::ostream &out,
                         mcjee::Matrix4 &mat) {
    out << mat.data[0] << ", "
        << mat.data[4] << ", "
        << mat.data[8] << ", "
        << mat.data[12] << std::endl;
    out << mat.data[1] << ", "
        << mat.data[5] << ", "
        << mat.data[9] << ", "
        << mat.data[13] << std::endl;
    out << mat.data[2] << ", "
        << mat.data[6] << ", "
        << mat.data[10] << ", "
        << mat.data[14] << std::endl;
    out << mat.data[3] << ", "
        << mat.data[7] << ", "
        << mat.data[11] << ", "
        << mat.data[15] << std::endl;
    return out;
}


std::ostream &operator<<(std::ostream &out,
                         mcjee::Vector3 &vec) {
    out << "[" << vec.x << ", "
        << vec.y << ", "
        << vec.z << "]";
    return out;
}

};