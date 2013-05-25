/*
    mjgpusolver.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_GPU_SOLVER_H
#define MJ_GPU_SOLVER_H

#include "mj.h"
#include "mjfluid.h"

namespace mcjee {

class GPUComputeModel : public Renderable {
public:
    float width;
    float height;
    float depth;
    float dt;

    Texture *texture0;
    Texture *texture1;
    GPUComputeModel(Geometry *geo, Shader *shader) :
        Renderable(geo, shader, GL_TRIANGLE_STRIP) {
    }
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {
        width = 32.0;
        height = 32.0;
        depth = 32.0;
        if (shader->uniformEnabled("width")) {
            GLint loc = shader->getUniformLocation("width");
            glUniform1f(loc, width);
        }
        if (shader->uniformEnabled("height")) {
            GLint loc = shader->getUniformLocation("height");
            glUniform1f(loc, height);
        }
        if (shader->uniformEnabled("depth")) {
            GLint loc = shader->getUniformLocation("depth");
            glUniform1f(loc, depth);
        }
        if (shader->uniformEnabled("xunit")) {
            GLint loc = shader->getUniformLocation("xunit");
            glUniform1f(loc, 1.0/width);
        }
        if (shader->uniformEnabled("yunit")) {
            GLint loc = shader->getUniformLocation("yunit");
            glUniform1f(loc, 1.0/(height*depth));
        }
        if (shader->uniformEnabled("sliceHeight")) {
            GLint loc = shader->getUniformLocation("sliceHeight");
            glUniform1f(loc, 1.0/depth);
        }
        if (shader->uniformEnabled("dt")) {
            GLint loc = shader->getUniformLocation("dt");
            glUniform1f(loc, dt);
        }

        if (shader->uniformEnabled("inBuffer")) {
            GLint tex0loc = shader->getUniformLocation("inBuffer");
            glUniform1i(tex0loc, 0);
        } else if (shader->uniformEnabled("buffer0")) {
            GLint tex0loc = shader->getUniformLocation("buffer0");
            glUniform1i(tex0loc, 0);
        }

        if (shader->uniformEnabled("velocityBuffer")) {
            GLint tex1loc = shader->getUniformLocation("velocityBuffer");
            glUniform1i(tex1loc, 1);
        } else if (shader->uniformEnabled("buffer1")) {
            GLint tex1loc = shader->getUniformLocation("buffer1");
            glUniform1i(tex1loc, 1);
        }

        glActiveTexture(GL_TEXTURE0);
        texture0->bind();
        glActiveTexture(GL_TEXTURE1);
        texture1->bind();
    }
};

class GPUSolver : public FluidSolver {
private:
    Texture2D *densityTex0;
    Texture2D *densityTex1;
    Texture2D *velocityTex0;
    Texture2D *velocityTex1;

    Framebuffer *outputFramebuffer;

    Texture2D *densityBufferTex;
    Texture2D *velocityBufferTex;
    float *densityBuffer;
    float *velocityBuffer;
    float *temp0;
    float *temp1;

    Scene *computeScene;
    Shader *addKernel;
    Shader *advectKernel;
    Shader *projectKernel;
    GPUComputeModel *model;

    void project(float *vel, float *div, float *temp);

public:
    GPUSolver(int width, int height, int depth);
    virtual void addVelocityX(int x, int y, int z, float amount);
    virtual void addVelocityY(int x, int y, int z, float amount);
    virtual void addVelocityZ(int x, int y, int z, float amount);
    virtual void addDensity(int x, int y, int z, float amount);
    virtual void addDensity(int x, int y, int z, float r, float g, float b);
    virtual void solve(float dt);

    virtual void fillDensityData(float *out);
};

}

#endif
