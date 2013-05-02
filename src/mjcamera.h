/*
    mjcamera.h
*/

#ifndef MJ_CAMERA_H
#define MJ_CAMERA_H

#include "mjmath.h"

namespace mcjee {

    class Camera {
    private:
        Matrix4 _projectionMatrix;
        Matrix4 _viewMatrix;
    public:
        Vector3 translation;
        Vector3 rotation;
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
        void translate(float x, float y, float z);
        void rotate(float x, float y, float z);
        void lookAt();
        Matrix4 &projectionMatrix();
        Matrix4 &viewMatrix();
        Matrix4 &inverseViewMatrix();
    };

}

#endif
