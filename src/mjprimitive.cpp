/*
    mjprimitive.cpp
    Matthew Jee
    mcjee#ucsc.edu
*/

#include "mjprimitive.h"

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

long cubeSize() {
    return sizeof(float)*3*8;
};

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

}
