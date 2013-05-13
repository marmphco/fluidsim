/*
    mjloader.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_LOADER_H
#define MJ_LOADER_H

#include "mjgeometry.h"

namespace mcjee {

// loads vertices
int loadVertices(float **vertices, const char *path);
// loads polygons and converts them to triangles, for GL_TRIANGLES
int loadTris(unsigned int **indices, const char *path);
// loads polygons and converts them to triangles, for GL_TRIANGLES
// correctly generates normals based on surfaces
// normals must have initialized ahead of time
int loadTrisAndNormals(unsigned int **indices,
                       float *normals,
                       float *vertices,
                       long vertexCount,
                       const char *path);

Geometry *loadModel(const char *coorpath, const char *polypath);

}
#endif

