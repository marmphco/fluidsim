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

    // find bounds
    // hacky, assuming first 3 attributes represent position
    _min = Vector3(99999999, 99999999, 99999999);
    _max = Vector3(-99999999, -99999999, -99999999);
    for (int i = 0; i < vertexCount*vertexElements; i += vertexElements) {
        if (vertexData[i] < _min.x)
            _min.x = vertexData[i];

        if (vertexData[i+1] < _min.y)
            _min.y = vertexData[i+1];

        if (vertexData[i+2] < _min.z)
            _min.z = vertexData[i+2];

        if (vertexData[i] > _max.x)
            _max.x = vertexData[i];

        if (vertexData[i+1] > _max.y)
            _max.y = vertexData[i+1];

        if (vertexData[i+2] > _max.z)
            _max.z = vertexData[i+2];
    }
}

void Geometry::bind(void) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
}

int Geometry::elementCount(void) {
    return indexCount;
}

Vector3 Geometry::boundMin() {
    return _min;
}

Vector3 Geometry::boundMax() {
    return _max;
}

}
