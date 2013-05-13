/*
    mjrenderable.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjrenderable.h"
#include "mjutil.h"
#include "mjtexture.h"

namespace mcjee {

Renderable::Renderable(Geometry *geometryi, Shader *shaderi, GLenum drawTypei) :
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
        Matrix4 matrix = modelMatrix();
        GLint loc = shader->getUniformLocation("modelMatrix");
        glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.data);
    }
    if (shader->uniformEnabled("inverseModelMatrix")) {
        Matrix4 matrix = inverseModelMatrix();
        GLint loc = shader->getUniformLocation("inverseModelMatrix");
        glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.data);
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
    std::vector<Texture *>::iterator i;
    for (i = material.textures.begin(); i != material.textures.end(); ++i) {
        (*i)->bind();
    }
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    glBindVertexArray(vertexArrayObject);
    glDrawElements(drawType, geometry->elementCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderable::resetRotation() {
    rotation = Matrix4();
}

void Renderable::rotateGlobal(float angle, Vector3 axis) {
    rotation.rotate(angle, axis);
}

void Renderable::rotateLocal(float angle, Vector3 axis) {
    Vector3 realAxis = rotation.matrix3() * axis;
    rotation.rotate(angle, realAxis);
}

void Renderable::scaleUniform(float s) {
    scale *= s;
}

void Renderable::addScaleUniform(float s) {
    scale += s;
}

Matrix4 Renderable::modelMatrix() {
    Matrix4 matrix;
    matrix.translate(-center.x, -center.y, -center.z);
    matrix.scale(scale.x, scale.y, scale.z);
    matrix = rotation * matrix;
    matrix.translate(position.x, position.y, position.z);
    return matrix;
}

Matrix4 Renderable::inverseModelMatrix() {
    Matrix4 matrix;
    matrix.translate(-position.x, -position.y, -position.z);
    Matrix4 inverseRotation = rotation;
    inverseRotation.transpose();
    matrix = matrix * inverseRotation;
    matrix.scale(1/scale.x, 1/scale.y, 1/scale.z);
    matrix.translate(center.x, center.y, center.z);
    return matrix;
}
}
