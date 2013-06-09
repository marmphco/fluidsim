/*
    mjcamera.h
*/

#ifndef MJ_CAMERA_H
#define MJ_CAMERA_H

#include "mjmatrix.h"
#include "mjvector.h"

namespace mcjee {

class Camera {
private:
    Matrix4 _projectionMatrix;
    Matrix4 _viewMatrix;
    Matrix4 _inverseViewMatrix;
    float _left;
    float _right;
    float _bottom;
    float _top;
    float _near;
    float _far;
public:
    Vector3 position;
    Matrix4 rotation;
    float zoom;

    Camera();
    void perspective(float left,
                     float right,
                     float bottom,
                     float top,
                     float near,
                     float far);
    void orthographic(float left,
                     float right,
                     float bottom,
                     float top,
                     float near,
                     float far);

    void resetRotation();
    void rotateGlobal(float angle, Vector3 axis);
    void rotateLocal(float angle, Vector3 axis);
    void translateGlobal(float amount, Vector3 axis);
    void translateLocal(float amount, Vector3 axis);

    void lookAt(Vector3 target, Vector3 up);
    Matrix4 &projectionMatrix();
    Matrix4 &viewMatrix();
    Matrix4 &inverseViewMatrix();
};

}

#endif
