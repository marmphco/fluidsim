/*
    mjutil.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_UTIL_H
#define MJ_UTIL_H

#include <iostream>

#define _USE_MATH_DEFINES
#ifdef __APPLE__
#   include <GLUT/glut.h>
#   define glGenVertexArrays glGenVertexArraysAPPLE
#   define glBindVertexArray glBindVertexArrayAPPLE
#   define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#else
#   include "libglew/glew.h"
#   include <GL/glut.h>
#endif

namespace mcjee {

long getTimeMillis(void);

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

}

#endif
