/*
    mjgeometry.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_GEOMETRY_H
#define MJ_GEOMETRY_H

#include "mjutil.h"
#include "mjvector.h"

namespace mcjee {

class Geometry {
private:
    int indexCount;
    GLuint vertexBufferObject;
    GLuint indexBufferObject;

    Vector3 _min;
    Vector3 _max;

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

    Vector3 boundMin();
    Vector3 boundMax();
};

}

#endif
