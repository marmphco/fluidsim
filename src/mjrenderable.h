/*
    mjrenderable.h
    Matthew Jee
    mcjee@ucsc.edu

    Abstract class meant to be subclassed before usage.
    Definitely not the most efficient nor the most flexible,
    but it is very convenient for this project.
*/

#ifndef MJ_RENDERABLE_H
#define MJ_RENDERABLE_H

#include "mjutil.h"
#include "mjshader.h"
#include "mjlight.h"
#include "mjgeometry.h"

namespace mcjee {
    class Renderable {
    friend class Scene;
    private:
        GLuint vertexArrayObject;
        // local rotation axes
        Vector3 localAxisX;
        Vector3 localAxisY;
        Vector3 localAxisZ;
        Matrix4 rotation;
        Matrix4 inverseRotation;

    public:
        Geometry *geometry;
        Shader *shader;
        Material material;
        GLenum drawType;
        GLenum polygonMode;

        Vector3 center;
        Vector3 scale;
        Vector3 position;

        Renderable(Geometry *, Shader *, GLenum drawType);
        virtual ~Renderable();
        void init();
        void render(void);

        // rotation can not be directly manipulated
        // in the way scale and translate can
        void resetRotation();
        void rotate(float angle, Vector3 axis);
        void rotateLocalX(float angle);
        void rotateLocalY(float angle);
        void rotateLocalZ(float angle);

        // To be overridden by subclasses
        virtual void setupVertexAttributes() = 0;
        virtual void setupUniforms() = 0;
    };
}

#endif
