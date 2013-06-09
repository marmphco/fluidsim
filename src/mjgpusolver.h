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

class GPUComputeModel;

class GPUSolver : public FluidSolver {
private:
    Texture2D *densityTex0;
    Texture2D *densityTex1;
    Texture2D *velocityTex0;
    Texture2D *velocityTex1;
    Texture2D *pressureTex0;
    Texture2D *pressureTex1;
    Texture2D *divergenceTex;

    Framebuffer *outputFramebuffer;

    Texture2D *densityBufferTex;
    Texture2D *velocityBufferTex;
    uint16_t *densityBuffer;
    uint16_t *velocityBuffer;

    Scene *computeScene;
    Shader *addKernel;
    Shader *advectKernel;
    Shader *divergenceKernel;
    Shader *project2Kernel;
    Shader *subgradientKernel;
    Geometry *geo;
    GPUComputeModel *model;

    void addStep(Texture2D *in0, Texture2D *in1, Texture2D *out);
    void advectStep(Texture2D *in, Texture2D *out);
    void projectStep();

public:
    GPUSolver(int width, int height, int depth);
    ~GPUSolver();
    void addVelocity(Vector3 pos, Vector3 amount);
    void addDensity(Vector3 pos, Vector4 amount);
    void solve(float dt);
    void solveDensities(float dt);
    void solveVelocities(float dt);

    void fillDensityData(float *out);
    void fillVelocityData(float *out);

    void fillDensityData(uint16_t *out);
    void fillVelocityData(uint16_t *out);

    void clearDensity();
    void clearVelocity();
};

}

#endif
