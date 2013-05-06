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

class FluidSolver {
private:
    float *density1;
    float *density2;
    float *vx1;
    float *vx2;
    float *vy1;
    float *vy2;
    float *vz1;
    float *vz2;

    float *density;
    float *velocity;

    int _width;
    int _height;
    int _depth;

    void diffuse(float *prev, float *next, float dt);
    void advect(float *prev, float *next, float *vx, float *vy, float *vz, float dt);
    void project(float dt);
public:
    FluidSolver(int width, int height, int depth);
    ~FluidSolver();

    void addDensity(int x, int y, int z, float amount);
    void solve(float dt);

    void fillDensityData(unsigned int *out);
};

}

#endif
