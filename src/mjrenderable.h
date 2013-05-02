/*
    mjrenderable.h
    Matthew Jee
    mcjee@ucsc.edu

    Abstract class meant to be subclassed before usage.
    Makes a lot of assumptions on what kind of data is rendered.
    - Assumes GL_UNSIGNED_INT indices
    - Assumes GL_FLOAT vertices
    Should be template-ized!
    Definitely not the most efficient nor the most flexible,
    but it is very convenient for this project.
*/

#ifndef MJ_RENDERABLE_H
#define MJ_RENDERABLE_H

#include "mjutil.h"
#include "mjshader.h"
#include "mjlight.h"

namespace mcjee {
    class Renderable {
    friend class Scene;
    private:
        size_t indexCount;
        GLuint vertexBufferObject;
        GLuint indexBufferObject;
        GLuint vertexArrayObject;
    public:
        Shader *shader;
        Material material;
        Matrix4 modelMatrix;
        GLenum drawType;

        Renderable(Shader *shader, GLenum drawType);
        virtual ~Renderable();
        void init(GLfloat *vertexData,
                  GLuint *indexData,
                  int vertexCount,
                  int indexCount,
                  int vertexElements);
        void modifyData(GLfloat *vertexData,
                        GLuint *indexData,
                        int vertexCount,
                        int indexCount,
                        int vertexElements);
        void render(void);

        // To be overridden by subclasses
        virtual void setupVertexAttributes() = 0;
        virtual void setupUniforms() = 0;
    };
}

#endif
