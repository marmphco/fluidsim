/*
    mjgeometry.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjgeometry.h"

namespace mcjee {

Geometry::~Geometry() {
    glDeleteBuffers(2, &vertexBufferObject);
}

Geometry::Geometry(GLfloat *vertexData,
                   GLuint *indexData,
                   int vertexCount,
                   int indexCounti,
                   int vertexElements) {
    indexCount = indexCounti;
    // The vertex buffer and index buffer are adjacent
    glGenBuffers(2, &vertexBufferObject);
    modifyData(vertexData, indexData, vertexCount, indexCount, vertexElements);
}

void Geometry::modifyData(GLfloat *vertexData,
                          GLuint *indexData,
                          int vertexCount,
                          int indexCounti,
                          int vertexElements) {

    indexCount = indexCounti;
    int vertexSize = vertexElements*vertexCount*sizeof(GLfloat);
    int indexSize = indexCount*sizeof(GLuint);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_DYNAMIC_DRAW);
}

void Geometry::bind(void) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
}

int Geometry::elementCount(void) {
    return indexCount;
}

}
