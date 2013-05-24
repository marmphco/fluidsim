/*
    mjhybridsolver.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_HYBRID_SOLVER_H
#define MJ_HYBRID_SOLVER_H

#include "mj.h"
#include "mjfluid.h"

namespace mcjee {

class ComputeModel : public Renderable {
public:
    Texture *texture0;
    Texture *texture1;
    ComputeModel(Geometry *geo, Shader *shader) :
        Renderable(geo, shader, GL_TRIANGLE_STRIP) {
    }
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {
        GLint tex0loc = shader->getUniformLocation("inBuffer");
        glUniform1i(tex0loc, 0);
        GLint tex1loc = shader->getUniformLocation("velocityBuffer");
        glUniform1i(tex1loc, 1);
        glActiveTexture(GL_TEXTURE0);
        texture0->bind();
        glActiveTexture(GL_TEXTURE1);
        texture1->bind();
    }
};

class HybridSolver : public FluidSolver {
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

    Scene *advectScene;
    Shader *addKernel;
    Shader *advectKernel;
    ComputeModel *model;

    void project(float *vel, float *div, float *temp);

public:
    HybridSolver(int width, int height, int depth);
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
