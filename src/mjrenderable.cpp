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
    position(Vector3(0.0, 0.0, 0.0)),
    visible(true) {
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
    geometry->unbind();
}

void Renderable::render(void) {
    if (!visible) return;
    shader->use();
    setupUniforms();
    shader->setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, modelMatrix().data);
    shader->setUniformMatrix4fv("inverseModelMatrix", 1, GL_FALSE, inverseModelMatrix().data);
    shader->setUniform3fv("matDiffuseColor", 1, (const GLfloat *)&material.diffuseColor);
    shader->setUniform3fv("matSpecularColor", 1, (const GLfloat *)&material.specularPower);
    shader->setUniform1f("matSpecularPower", material.specularPower);
    std::vector<Texture *>::iterator i;
    for (i = material.textures.begin(); i != material.textures.end(); ++i) {
        (*i)->bind();
    }
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    glBindVertexArray(vertexArrayObject);
    geometry->bind();
    glDrawElements(drawType, geometry->elementCount(), GL_UNSIGNED_INT, 0);
    geometry->unbind();
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

void Renderable::translateGlobal(float amount, Vector3 axis) {
    position += axis*amount;
}

void Renderable::translateLocal(float amount, Vector3 axis) {
    Vector3 realAxis = rotation.matrix3() * axis;
    position += realAxis*amount;
    
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
