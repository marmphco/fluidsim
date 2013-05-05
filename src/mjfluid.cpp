/*
    mjfluid.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjfluid.h"

#define idx(x, y, z) _width*_height*(z)+_width*(y)+x
#define swap(x, y) float *t = (x); (x) = (y); (y) = t;

namespace mcjee {

FluidSolver::FluidSolver(int width, int height, int depth) :
    _width(width), _height(height), _depth(depth) {
    density1 = new float[width*height*depth];
    density2 = new float[width*height*depth];
    vx1 = new float[width*height*depth];
    vx2 = new float[width*height*depth];
    vy1 = new float[width*height*depth];
    vy2 = new float[width*height*depth];
    vz1 = new float[width*height*depth];
    vz2 = new float[width*height*depth];

    for (int i = 0; i < _width; ++i) {
        for (int j = 0; j < _height; ++j) {
            for (int k = 0; k < _depth; ++k) {
                density1[idx(i, j, k)] = 0.0;
            }
        }
    }
    for (int i = 1; i < _width-1; ++i) {
        for (int j = 1; j < _height-1; ++j) {
            for (int k = 1; k < _depth-1; ++k) {
                density1[idx(i, j, k)] = 0.0;
            }
        }
    }
}

FluidSolver::~FluidSolver() {
    delete density1;
    delete density2;
    delete vx1;
    delete vx2;
    delete vy1;
    delete vy2;
    delete vz1;
    delete vz2;
}

void FluidSolver::diffuse(float *prev, float *next, float dt) {
    float f = dt*0.1;

    for (int n = 0; n < 20; ++n) {
        for (int i = 1; i < _width-1; ++i) {
            for (int j = 1; j < _height-1; ++j) {
                for (int k = 1; k < _depth-1; ++k) {
                    next[idx(i, j, k)] =
                        prev[idx(i, j, k)]+
                        f*(next[idx(i, j, k+1)]+next[idx(i, j, k-1)]
                          +next[idx(i+1, j, k)]+next[idx(i-1, j, k)]
                          +next[idx(i, j+1, k)]+next[idx(i, j-1, k)])/(1+8*f);
                }
            }
        }
    }
}

void FluidSolver::addDensity(int x, int y, int z, float amount) {
    density1[idx(x, y, z)] += amount;
    density1[idx(x, y+1, z)] += amount;
    density1[idx(x, y-1, z)] += amount;
    density1[idx(x, y, z+1)] += amount;
    density1[idx(x, y, z-1)] += amount;
    density1[idx(x+1, y, z)] += amount;
    density1[idx(x-1, y, z)] += amount;
    density2[idx(x, y, z)] += amount;
    density2[idx(x, y+1, z)] += amount;
    density2[idx(x, y-1, z)] += amount;
    density2[idx(x, y, z+1)] += amount;
    density2[idx(x, y, z-1)] += amount;
    density2[idx(x+1, y, z)] += amount;
    density2[idx(x-1, y, z)] += amount;
}

void FluidSolver::solve(float dt) {
    diffuse(density1, density2, dt);
    swap(density1, density2);
}

// until i figure out wether we can use float textures
void FluidSolver::fillDensityData(unsigned int *out) {
    for (int i = 0; i < _width; ++i) {
        for (int j = 0; j < _height; ++j) {
            for (int k = 0; k < _depth; ++k) {
                float c =  density1[idx(i, j, k)] > 1.0 ? 1.0 : density1[idx(i, j, k)];
                int num = (int)(c*0xff);
                out[idx(i, j, k)] = (num<<8) | (num<<16) | (num<<24) | 0xff;
            }
        }
    }
}

}
