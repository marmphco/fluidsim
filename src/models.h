/*
    models.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MODELS_H
#define MODELS_H

#include "mj.h"

using namespace mcjee;

class FluidModel : public Renderable {
public:
    Texture3D *densityBuffer;
    int samples;

    FluidModel(Geometry *geo, Shader *shader, GLenum drawType) :
        Renderable(geo, shader, drawType), samples(64) {}
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // HACKY use same vertices for texture coordinates
        loc = shader->getAttribLocation("vTexCoord");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {
        GLint sampleloc = shader->getUniformLocation("samples");
        glUniform1i(sampleloc, samples);

        GLint loc = shader->getUniformLocation("texture0");
        glUniform1i(loc, 0);
        glActiveTexture(GL_TEXTURE0);
        densityBuffer->bind();
    }
};

class BoundingBox : public Renderable {
private:
    FluidModel *_target;
public:
    BoundingBox(Geometry *geo, Shader *shader, FluidModel *target) :
        Renderable(geo, shader, GL_LINES), _target(target) {}
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {
        rotation = _target->rotation;
    }
};

#endif