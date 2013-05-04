/*
    mjgeometry.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_GEOMETRY_H
#define MJ_GEOMETRY_H

#include "mjutil.h"

namespace mcjee {

class Geometry {
private:
    int indexCount;
    GLuint vertexBufferObject;
    GLuint indexBufferObject;
public:
    Geometry(GLfloat *vertexData,
             GLuint *indexData,
             int vertexCount,
             int indexCount,
             int vertexElements);
    ~Geometry();

    void modifyData(GLfloat *vertexData,
                    GLuint *indexData,
                    int vertexCount,
                    int indexCount,
                    int vertexElements);
    void bind(void);
    int elementCount(void);
};

}

#endif
