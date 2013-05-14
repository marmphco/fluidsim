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
    Framebuffer *density0;
    Framebuffer *density1;
    Framebuffer *velocity1;
    Framebuffer *velocity2;

public:
    HybridSolver(int width, int height, int depth);
    virtual void addVelocityX(int x, int y, int z, float amount);
    virtual void addVelocityY(int x, int y, int z, float amount);
    virtual void addDensity(int x, int y, int z, float amount);
    virtual void solve(float dt);

    virtual void fillDensityData(float *out);
};

}

#endif
