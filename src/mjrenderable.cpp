/*
    mjrenderable.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjrenderable.h"
#include "mjutil.h"

namespace mcjee {

    static const Vector3 X_AXIS(1.0, 0.0, 0.0);
    static const Vector3 Y_AXIS(0.0, 1.0, 0.0);
    static const Vector3 Z_AXIS(0.0, 0.0, 1.0);

    Renderable::Renderable(Geometry *geometryi, Shader *shaderi, GLenum drawTypei) :
        localAxisX(X_AXIS),
        localAxisY(Y_AXIS),
        localAxisZ(Z_AXIS),
        geometry(geometryi),
        shader(shaderi),
        drawType(drawTypei),
        polygonMode(GL_FILL),
        center(Vector3(0.0, 0.0, 0.0)),
        scale(Vector3(1.0, 1.0, 1.0)),
        position(Vector3(0.0, 0.0, 0.0)) {
    }

    Renderable::~Renderable() {
        glDeleteVertexArrays(1, &vertexArrayObject);
    }

    void Renderable::init() {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);
        geometry->bind();
        setupVertexAttributes();
        glBindVertexArray(0);
    }

    void Renderable::render(void) {
        shader->use();
        setupUniforms();
        if (shader->uniformEnabled("modelMatrix")) {
            Matrix4 modelMatrix;
            modelMatrix.translate(-center.x, -center.y, -center.z);
            modelMatrix.scale(scale.x, scale.y, scale.z);
            modelMatrix = rotation * modelMatrix;
            modelMatrix.translate(position.x, position.y, position.z);
            GLint loc = shader->getUniformLocation("modelMatrix");
            glUniformMatrix4fv(loc, 1, GL_FALSE, modelMatrix.data);
        }
        if (shader->uniformEnabled("inverseModelMatrix")) {
            Matrix4 inverseModelMatrix;
            inverseModelMatrix.translate(-position.x, -position.y, -position.z);
            inverseModelMatrix = inverseModelMatrix * inverseRotation;
            inverseModelMatrix.scale(1/scale.x, 1/scale.y, 1/scale.z);
            inverseModelMatrix.translate(center.x, center.y, center.z);
            GLint loc = shader->getUniformLocation("inverseModelMatrix");
            glUniformMatrix4fv(loc, 1, GL_FALSE, inverseModelMatrix.data);
        }
        if (shader->uniformEnabled("matDiffuseColor") ||
            shader->uniformEnabled("ambientColor")) {
            GLint loc = shader->getUniformLocation("matDiffuseColor");
            glUniform3fv(loc, 1, (const GLfloat *)&material.diffuseColor);
        }
        if (shader->uniformEnabled("matSpecularColor")) {
            GLint loc = shader->getUniformLocation("matSpecularColor");
            glUniform3fv(loc, 1, (const GLfloat *)&material.specularColor);
            loc = shader->getUniformLocation("matSpecularPower");
            glUniform1f(loc, material.specularPower);
        }
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        glBindVertexArray(vertexArrayObject);
        glDrawElements(drawType, geometry->elementCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Renderable::resetRotation() {
        //not implemented
    }

    void Renderable::rotate(float angle, Vector3 axis) {
        rotation.rotate(angle, axis);
        inverseRotation.rotate(-angle, axis);
        localAxisX = rotation.matrix3() * X_AXIS;
        localAxisY = rotation.matrix3() * Y_AXIS;
        localAxisZ = rotation.matrix3() * Z_AXIS;
    }

    // not very efficient
    void Renderable::rotateLocalX(float angle) {
        rotation.rotate(angle, localAxisX);
        inverseRotation.rotate(-angle, localAxisX);
        localAxisY = rotation.matrix3() * Y_AXIS;
        localAxisZ = rotation.matrix3() * Z_AXIS;
    }

    void Renderable::rotateLocalY(float angle) {
        rotation.rotate(angle, localAxisY);
        inverseRotation.rotate(-angle, localAxisY);
        localAxisX = rotation.matrix3() * X_AXIS;
        localAxisZ = rotation.matrix3() * Z_AXIS;
    }

    void Renderable::rotateLocalZ(float angle) {
        rotation.rotate(angle, localAxisZ);
        inverseRotation.rotate(-angle, localAxisZ);
        localAxisX = rotation.matrix3() * X_AXIS;
        localAxisY = rotation.matrix3() * Y_AXIS;
    }
}
