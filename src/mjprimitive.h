/*
    mjprimitive.h
    Matthew Jee
    mcjee#ucsc.edu
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

long cubeSize();

void cubeVertices(float *out,
          float width, float height, float depth);

void cubeIndices();

}
#endif