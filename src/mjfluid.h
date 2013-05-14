/*
    mjfluid.h
    Matthew Jee
    mcjee@ucsc.edu

    A 3D fluid solver. May be later generalized to arbitrary dimension.
    needs also to be density (color) dimension generalized.
*/

#ifndef MJ_FLUID_H
#define MJ_FLUID_H

namespace mcjee {

// fluid solver interface
class FluidSolver {
protected:
    int _width;
    int _height;
    int _depth;
public:
    FluidSolver(int width, int height, int depth);
    virtual ~FluidSolver();

    virtual void addVelocityX(int x, int y, int z, float amount) = 0;
    virtual void addVelocityY(int x, int y, int z, float amount) = 0;
    virtual void addDensity(int x, int y, int z, float amount) = 0;
    virtual void solve(float dt) = 0;

    virtual void fillDensityData(float *out) = 0;
};

class CPUSolver : public FluidSolver {
private:
    float *density0;
    float *density1;
    float *vx0;
    float *vx1;
    float *vy0;
    float *vy1;
    float *vz0;
    float *vz1;

    void diffuse(float *next, float *prev, float dt);
    void advect(float *next, float *prev, float *vx, float *vy, float *vz, float dt);
    void project(float *vx, float *vy, float *vz, float *div, float *temp);
public:
    int iterations;

    CPUSolver(int width, int height, int depth);
    ~CPUSolver();

    void addVelocityX(int x, int y, int z, float amount);
    void addVelocityY(int x, int y, int z, float amount);
    void addDensity(int x, int y, int z, float amount);
    void solve(float dt);

    void fillDensityData(float *out);
};

}

#endif
