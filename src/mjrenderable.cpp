/*
    mjrenderable.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjrenderable.h"
#include "mjutil.h"

namespace mcjee {
    Renderable::Renderable(Shader *shaderi, GLenum drawTypei) :
        shader(shaderi),
        drawType(drawTypei) {
    }

    Renderable::~Renderable() {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(2, &vertexBufferObject);
    }

    void Renderable::init(GLfloat *vertexData,
                           GLuint *indexData,
                           int vertexCount,
                           int indexCounti,
                           int vertexElements) {

        indexCount = indexCounti;

        // The vertex buffer and index buffer are adjacent
        glGenBuffers(2, &vertexBufferObject);
        modifyData(vertexData, indexData, vertexCount, indexCount, vertexElements);

        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
        setupVertexAttributes();

        glBindVertexArray(0);
    }

    void Renderable::modifyData(GLfloat *vertexData,
                                GLuint *indexData,
                                int vertexCount,
                                int indexCounti,
                                int vertexElements) {

        indexCount = indexCounti;
        int vertexSize = vertexElements*vertexCount*sizeof(GLfloat);
        int indexSize = indexCount*sizeof(GLuint);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_DYNAMIC_DRAW);
    }

    void Renderable::render(void) {
        shader->use();
        setupUniforms();
        if (shader->modelMatrixEnabled()) {
            GLint loc = shader->getUniformLocation("modelMatrix");
            glUniformMatrix4fv(loc, 1, GL_FALSE, modelMatrix.data);
        }
        if (shader->diffuseEnabled() || shader->ambientEnabled()) {
            GLint loc = shader->getUniformLocation("matDiffuseColor");
            glUniform3fv(loc, 1, (const GLfloat *)&material.diffuseColor);
        }
        if (shader->specularEnabled()) {
            GLint loc = shader->getUniformLocation("matSpecularColor");
            glUniform3fv(loc, 1, (const GLfloat *)&material.specularColor);
            loc = shader->getUniformLocation("matSpecularPower");
            glUniform1f(loc, material.specularPower);
        }
        glBindVertexArray(vertexArrayObject);
        glDrawElements(drawType, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
