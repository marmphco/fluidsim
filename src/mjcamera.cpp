/*
    mjcamera.cpp
*/

#include "mjcamera.h"

namespace mcjee {

    Camera::Camera() {}

    void Camera::perspective(float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far) {
        _projectionMatrix.frustum(left,
                                  right,
                                  bottom,
                                  top,
                                  near,
                                  far);
    }
    void Camera::orthographic(float left,
                              float right,
                              float bottom,
                              float top,
                              float near,
                              float far) {
        _projectionMatrix.ortho(left,
                                right,
                                bottom,
                                top,
                                near,
                                far);
    }
    void Camera::translate(float x, float y, float z) {
        translation += Vector3(x, y, z);
    }
    void Camera::rotate(float x, float y, float z) {
        rotation += Vector3(x, y, z);
    }
    void Camera::lookAt() {

    }

    Matrix4 &Camera::projectionMatrix() {
        return _projectionMatrix;
    }

    Matrix4 &Camera::viewMatrix() {
        _viewMatrix.identity();
        _viewMatrix.translate(-translation.x, -translation.y, -translation.z);
        _viewMatrix.rotateY(-rotation.y);
        _viewMatrix.rotateX(-rotation.x);
        _viewMatrix.rotateZ(-rotation.z);
        return _viewMatrix;
    }

    Matrix4 &Camera::inverseViewMatrix() {
        _viewMatrix.identity();
        _viewMatrix.rotateZ(rotation.z);
        _viewMatrix.rotateX(rotation.x);
        _viewMatrix.rotateY(rotation.y);
        _viewMatrix.translate(translation.x, translation.y, translation.z);
        return _viewMatrix;
    }

}
