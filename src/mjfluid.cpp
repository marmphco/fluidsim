/*
    mjfluid.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjfluid.h"
    #include <iostream>

#define idx(x, y, z) _width*_height*(z)+_width*(y)+x
#define swap(x, y) {float *t = (x); (x) = (y); (y) = t;}

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
                density2[idx(i, j, k)] = 0.0;
                vx1[idx(i, j, k)] = 0.0;
                vx2[idx(i, j, k)] = 0.0;
                vy1[idx(i, j, k)] = 0.0;
                vy2[idx(i, j, k)] = 0.0;
                vz1[idx(i, j, k)] = 0.0;
                vz2[idx(i, j, k)] = 0.0;
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

// move stuff along a velocity field
void FluidSolver::advect(float *prev, float *next, float *vx, float *vy, float *vz, float dt) {
    for (int i = 1; i < _width-1; ++i) {
        for (int j = 1; j < _height-1; ++j) {
            for (int k = 1; k < _depth-1; ++k) {
                float srcx = i-vx[idx(i, j, k)]*dt;
                float srcy = j-vy[idx(i, j, k)]*dt;
                float srcz = k-vz[idx(i, j, k)]*dt;

                /*if (srcx < 0.5) srcx = 0.5;
                if (srcx > _width-0.5) srcx = _width-0.5;
                if (srcy < 0.5) srcy = 0.5;
                if (srcy > _height-0.5) srcy = _height-0.5;
                if (srcz < 0.5) srcz = 0.5;
                if (srcz > _depth-0.5) srcz = _depth-0.5;*/

                int x0 = (int)srcx; int x1 = x0+1;
                int y0 = (int)srcy; int y1 = y0+1;
                int z0 = (int)srcz; int z1 = z0+1;
                float xl = x1-srcx; float xr = 1-xl;
                float yl = y1-srcy; float yr = 1-yl;
                float zl = z1-srcz; float zr = 1-zl;

                if (x0 < 0 || y0 < 0 || z0 < 0 || x1 >= _width || y1 >= _height || z1 >= _depth) continue;

                next[idx(i, j, k)] = 
                zl*(yl*(xl*prev[idx(x0, y0, z0)]+xr*prev[idx(x1, y0, z0)])+
                    yr*(xl*prev[idx(x0, y1, z0)]+xr*prev[idx(x1, y1, z0)]))+
                zr*(yl*(xl*prev[idx(x0, y0, z1)]+xr*prev[idx(x1, y0, z1)])+
                    yr*(xl*prev[idx(x0, y1, z1)]+xr*prev[idx(x1, y1, z1)]));
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
    advect(density1, density2, vx2, vy2, vz2, dt);
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
