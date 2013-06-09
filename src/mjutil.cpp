/*
    mjutil.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjutil.h"
#include "mjvector.h"

#ifdef __APPLE__
#   include <mach/mach_time.h>
#else
#   include <time.h>
#endif

namespace mcjee {

// multiplatform timing
long getTimeMillis(void) {
#ifdef __APPLE__
    static mach_timebase_info_data_t timebaseInfo;
    if (timebaseInfo.denom == 0) {
        mach_timebase_info(&timebaseInfo);
    }
    unsigned long elapsed = mach_absolute_time();
    return elapsed*timebaseInfo.numer/timebaseInfo.denom/1000000;
#else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec*1000+now.tv_nsec/1000000;
#endif
}

void dumpGLError() {
    GLenum error = glGetError();
    const char *id = "OpenGL Error: ";
    switch (error) {
        case GL_NO_ERROR:
            //std::cerr << "no error"<< std::endl;
            break;
        case GL_INVALID_ENUM:
            std::cerr << id << "Invalid Enum"<< std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cerr << id << "Invalid Value"<< std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cerr << id << "Invalid Operation"<< std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << id << "Invalid Framebuffer Operation"<< std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << id << "Out of Memory"<< std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << id << "Stack Underflow"<< std::endl;
            break;
        case GL_STACK_OVERFLOW:
            std::cerr << id << "Stack Overflow"<< std::endl;
            break;
        default: break;
    }
}

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
    for (long i = 0; i < vertexCount*3; i += 3) {
        interleaved[3*i] = positionData[i];
        interleaved[3*i+1] = positionData[i+1];
        interleaved[3*i+2] = positionData[i+2];
        interleaved[3*i+3] = normalData[i];
        interleaved[3*i+4] = normalData[i+1];
        interleaved[3*i+5] = normalData[i+2];
        interleaved[3*i+6] = textureData[i];
        interleaved[3*i+7] = textureData[i+1];
        interleaved[3*i+8] = textureData[i+2];
    }
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

}
