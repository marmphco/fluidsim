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

class HybridSolver : public FluidSolver {
private:
    Texture2D *densityTex0;
    Texture2D *densityTex1;
    Texture2D *velocityTex0;
    Texture2D *velocityTex1;

    Framebuffer *density0;
    Framebuffer *density1;
    Framebuffer *velocity0;
    Framebuffer *velocity1;

    float *densityBuffer;
    float *velocityBuffer;
    float *temp0;
    float *temp1;

    Scene *advectScene;
    Shader *advectKernel;
    Renderable *model;

    void project(float *vel, float *div, float *temp);

public:
    HybridSolver(int width, int height, int depth);
    virtual void addVelocityX(int x, int y, int z, float amount);
    virtual void addVelocityY(int x, int y, int z, float amount);
    virtual void addVelocityZ(int x, int y, int z, float amount);
    virtual void addDensity(int x, int y, int z, float amount);
    virtual void solve(float dt);

    virtual void fillDensityData(float *out);
};

}

#endif
