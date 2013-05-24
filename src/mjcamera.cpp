/*
    mjcamera.cpp
*/

#include "mjcamera.h"

namespace mcjee {

Camera::Camera() : zoom(1.0) {}

void Camera::perspective(float left,
                         float right,
                         float bottom,
                         float top,
                         float near,
                         float far) {
    _left = left;
    _right = right;
    _bottom = bottom;
    _top = top;
    _near = near;
    _far = far;
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
    
void Camera::resetRotation() {
    rotation = Matrix4();
}

void Camera::rotateGlobal(float angle, Vector3 axis) {
    rotation.rotate(angle, axis);
}

void Camera::rotateLocal(float angle, Vector3 axis) {
    Vector3 realAxis = rotation.matrix3() * axis;
    rotation.rotate(angle, realAxis);
}

void Camera::translateGlobal(float amount, Vector3 axis) {
    position += axis*amount;
}

void Camera::translateLocal(float amount, Vector3 axis) {
    Vector3 realAxis = rotation.matrix3() * axis;
    position += realAxis*amount;
}

void Camera::lookAt(Vector3 target, Vector3 up) {
    //align view vector
    Vector3 dir = target-position;
    Vector3 viewVector = rotation.matrix3() * -Z_AXIS;

    float costheta = dir.normalize().dot(viewVector);
    float angle = acos(costheta > 1.0 ? 1.0 : costheta);

    Vector3 axis = viewVector.cross(dir.normalize());
    rotateGlobal(angle*180/M_PI, axis.normalize());

    //align up vector
    axis = rotation.matrix3() * Z_AXIS;
    Vector3 projection = axis*axis.dot(up);
    Vector3 realUp = (up-projection).normalize();
    Vector3 curUp = rotation.matrix3() * Y_AXIS;
    costheta = curUp.dot(realUp);
    angle = acos(costheta > 1.0 ? 1.0 : costheta);

    rotateGlobal(angle*180/M_PI, curUp.cross(realUp).normalize());
}

Matrix4 &Camera::projectionMatrix() {
    _projectionMatrix.frustum(_left,
                              _right,
                              _bottom,
                              _top,
                              _near*zoom,
                              _far);
    return _projectionMatrix;
}

Matrix4 &Camera::viewMatrix() {
    _viewMatrix.identity();
    _viewMatrix.translate(-position.x, -position.y, -position.z);
    Matrix4 temp = rotation;
    temp.transpose();
    _viewMatrix = temp * _viewMatrix;
    return _viewMatrix;
}

Matrix4 &Camera::inverseViewMatrix() {
    _viewMatrix.identity();
    _viewMatrix = rotation;
    _viewMatrix.translate(position.x, position.y, position.z);
    return _viewMatrix;
}

}
