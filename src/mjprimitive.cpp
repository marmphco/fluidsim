/*
    mjprimitive.cpp
    Matthew Jee
    mcjee#ucsc.edu
*/

#include "mjprimitive.h"
#include "mjutil.h"

namespace mcjee {

// can also be used for texture coordinates
void interleaveVertexData(float *interleaved,
                          float *positionData,
                          float *normalData,
                          long vertexCount) {
    for (long i = 0; i < vertexCount*3; i += 3) {
        interleaved[2*i] = positionData[i];
        interleaved[2*i+1] = positionData[i+1];
        interleaved[2*i+2] = positionData[i+2];
        interleaved[2*i+3] = normalData[i];
        interleaved[2*i+4] = normalData[i+1];
        interleaved[2*i+5] = normalData[i+2];
    }
}

// can also be used for texture coordinates
// for texture and normals
void interleaveVertexData(float *interleaved,
                          float *positionData,
                          float *normalData,
                          float *textureData,
                          long vertexCount) {
    /*for (long i = 0; i < vertexCount*3; i += 3) {
        interleaved[2*i] = positionData[i];
        interleaved[2*i+1] = positionData[i+1];
        interleaved[2*i+2] = positionData[i+2];
        interleaved[2*i+3] = normalData[i];
        interleaved[2*i+4] = normalData[i+1];
        interleaved[2*i+5] = normalData[i+2];
    }*/
}

void generateNormals(float *normals,
                     float *vertices,
                     unsigned int *indices,
                     long vertexCount,
                     long indexCount) {
    Vector3 *vnormals = (Vector3*)normals;
    Vector3 *vvertices = (Vector3*)vertices;
    for (int i = 0; i < vertexCount; ++i) {
        vnormals[i] = Vector3(0, 0, 0);
    }
    for (int i = 0; i < indexCount; i += 3) {
        int va = indices[i];
        int vb = indices[i+1];
        int vc = indices[i+2];
        Vector3 a = vvertices[va]-vvertices[vb];
        Vector3 b = vvertices[vc]-vvertices[vb];
        Vector3 snormal = b.cross(a);
        vnormals[va] += snormal;
        vnormals[vb] += snormal;
        vnormals[vc] += snormal;
    }
    for (int i = 0; i < vertexCount; ++i) {
        vnormals[i].normalize();
    }

}

int cubeVerticesSize() {
    return 24;
};

int cubeIndicesSize() {
    return 36;
}

void cubeVertices(float *out,
                  float width, float height, float depth) {
    out[0] = 0.0; out[1] = 0.0; out[2] = 0.0;
    out[3] = width; out[4] = 0.0; out[5] = 0.0;
    out[6] = width; out[7] = height; out[8] = 0.0;
    out[9] = 0.0; out[10] = height; out[11] = 0.0;

    out[12] = 0.0; out[13] = 0.0; out[14] = depth;
    out[15] = width; out[16] = 0.0; out[17] = depth;
    out[18] = width; out[19] = height; out[20] = depth;
    out[21] = 0.0; out[22] = height; out[23] = depth;
}

void cubeIndices(unsigned int *out) {
    out[0] = 0; out[1] = 3; out[2] = 1;
    out[3] = 3; out[4] = 2; out[5] = 1;
    out[6] = 1; out[7] = 2; out[8] = 5;
    out[9] = 2; out[10] = 6; out[11] = 5;
    out[12] = 0; out[13] = 7; out[14] = 3;
    out[15] = 0; out[16] = 4; out[17] = 7;
    out[18] = 3; out[19] = 7; out[20] = 2;
    out[21] = 7; out[22] = 6; out[23] = 2;
    out[24] = 4; out[25] = 5; out[26] = 7;
    out[27] = 7; out[28] = 5; out[29] = 6;
    out[30] = 0; out[31] = 1; out[32] = 4;
    out[33] = 1; out[34] = 5; out[35] = 4;
}

int squareVerticesSize() {
    return 12;
}

int squareIndicesSize() {
    return 6;
}

void squareVertices(float *out, float width, float height) {
    out[0] = 0.0; out[1] = 0.0; out[2] = 0.0;
    out[3] = width; out[4] = 0.0; out[5] = 0.0;
    out[6] = width; out[7] = height; out[8] = 0.0;
    out[9] = 0.0; out[10] = height; out[11] = 0.0;
}

void squareIndices(unsigned int *out) {
    out[0] = 0;
    out[1] = 1;
    out[2] = 2;
    out[3] = 0;
    out[4] = 2;
    out[5] = 3;
}

}
