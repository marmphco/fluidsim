/*
    mjloader.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_LOADER_H
#define MJ_LOADER_H

namespace mcjee {

// loads vertices
int loadVertices(float **vertices, const char *path);
// loads polygons and converts them to triangles, for GL_TRIANGLES
int loadPolygons(unsigned int **indices, const char *path);

}
#endif

