/*
    mjprimitive.h
    Matthew Jee
    mcjee#ucsc.edu

    Eveything assumes GL_TRIANGLES
*/

#ifndef MJ_PRIMITIVE_H
#define MJ_PRIMITIVE_H

namespace mcjee {

// can also be used for texture coordinates
void interleaveVertexData(float *interleaved,
                          float *positionData,
                          float *normalData,
                          long vertexCount);

// can also be used for texture coordinates
// for texture and normals
void interleaveVertexData(float *interleaved,
                          float *positionData,
                          float *normalData,
                          float *textureData,
                          long vertexCount);

// Assuming vertices is meant for GL_TRIANGLES
void generateNormals(float *normals,
                     float *vertices,
                     unsigned int *indices,
                     long vertexCount,
                     long indexCount);

int cubeVerticesSize();
int cubeIndicesSize();
int cubeWireframeIndicesSize();
void cubeVertices(float *out,
                  float width,
                  float height,
                  float depth);
void cubeIndices(unsigned int *out);
void cubeWireframeIndices(unsigned int *out);

int squareVerticesSize();
int squareIndicesSize();
void squareVertices(float *out, float width, float height);
void squareIndices(unsigned int *out);


}
#endif