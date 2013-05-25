/*
    mjfluid.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjfluid.h"
#include <iostream>

#define idx(x, y, z) (_width*_height*(z)+_width*(y)+x)
#define swap(x, y) {float *t = (x); (x) = (y); (y) = t;}

namespace mcjee {

FluidSolver::FluidSolver(int width, int height, int depth) :
    _width(width), _height(height), _depth(depth) {

}

FluidSolver::~FluidSolver() {

}

CPUSolver::CPUSolver(int width, int height, int depth) :
    FluidSolver(width, height, depth), iterations(4) {
    density1 = new float[width*height*depth];
    density0 = new float[width*height*depth];
    vx1 = new float[width*height*depth];
    vx0 = new float[width*height*depth];
    vy1 = new float[width*height*depth];
    vy0 = new float[width*height*depth];
    vz1 = new float[width*height*depth];
    vz0 = new float[width*height*depth];

    for (int i = 0; i < _width; ++i) {
        for (int j = 0; j < _height; ++j) {
            for (int k = 0; k < _depth; ++k) {
                density1[idx(i, j, k)] = 0.0;
                density0[idx(i, j, k)] = 0.0;
                vx1[idx(i, j, k)] = 0.0;
                vx0[idx(i, j, k)] = 0.0;
                vy1[idx(i, j, k)] = 0.0;
                vy0[idx(i, j, k)] = 0.0;
                vz1[idx(i, j, k)] = 0.0;
                vz0[idx(i, j, k)] = 0.0;
            }
        }
    }
}

CPUSolver::~CPUSolver() {
    delete density1;
    delete density0;
    delete vx1;
    delete vx0;
    delete vy1;
    delete vy0;
    delete vz1;
    delete vz0;
}

void CPUSolver::diffuse(float *next, float *prev, float dt) {
    float f = dt*0.02;

    for (int n = 0; n < iterations; ++n) {
        for (int i = 1; i < _width-1; ++i) {
            for (int j = 1; j < _height-1; ++j) {
                for (int k = 1; k < _depth-1; ++k) {
                    next[idx(i, j, k)] =
                        (prev[idx(i, j, k)]+
                        f*(next[idx(i, j, k+1)]+next[idx(i, j, k-1)]
                          +next[idx(i+1, j, k)]+next[idx(i-1, j, k)]
                          +next[idx(i, j+1, k)]+next[idx(i, j-1, k)]))/(1+8*f);
                }
            }
        }
    }
}

// move stuff along a velocity field
void CPUSolver::advect(float *next, float *prev, float *vx, float *vy, float *vz, float dt) {
    for (int i = 1; i < _width-1; ++i) {
        for (int j = 1; j < _height-1; ++j) {
            for (int k = 1; k < _depth-1; ++k) {
                float srcx = i-vx[idx(i, j, k)]*dt;
                float srcy = j-vy[idx(i, j, k)]*dt;
                float srcz = k-vz[idx(i, j, k)]*dt;

                if (srcx < 0.5) srcx = 0.5;
                if (srcx > _width-0.5) srcx = _width-0.5;
                if (srcy < 0.5) srcy = 0.5;
                if (srcy > _height-0.5) srcy = _height-0.5;
                if (srcz < 0.5) srcz = 0.5;
                if (srcz > _depth-0.5) srcz = _depth-0.5;

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
                    //prev[idx(i, j, k)];
            }
        }
    }
}

// some of these scalars may be wrong
void CPUSolver::project(float *vx, float *vy, float *vz, float *div, float *temp) {
    //compute divergence
    float b = 1.0; // arbitrary scaling term
    for (int i = 1; i < _width-1; ++i) {
        for (int j = 1; j < _height-1; ++j) {
            for (int k = 1; k < _depth-1; ++k) {
                div[idx(i, j, k)] = 0.5*b*(
                    vx[idx(i+1, j, k)]-vx[idx(i-1, j, k)]+
                    vy[idx(i, j+1, k)]-vy[idx(i, j-1, k)]+
                    vz[idx(i, j, k+1)]-vz[idx(i, j, k-1)]
                );
                temp[idx(i, j, k)] = 0.0;
            }
        }
    }

    //compute base scalar field of gradient component somehow
    for (int n = 0; n < iterations; ++n) {
        for (int i = 1; i < _width-1; ++i) {
            for (int j = 1; j < _height-1; ++j) {
                for (int k = 1; k < _depth-1; ++k) {
                    temp[idx(i, j, k)] = (
                        -div[idx(i, j, k)]+
                        temp[idx(i-1, j, k)]+
                        temp[idx(i+1, j, k)]+
                        temp[idx(i, j-1, k)]+
                        temp[idx(i, j+1, k)]+
                        temp[idx(i, j, k+1)]+
                        temp[idx(i, j, k-1)]
                    )/8;
                }
            }
        }
    }

    // subtract gradient field from velocities to get
    // zero divergence field.
    for (int i = 1; i < _width-1; ++i) {
        for (int j = 1; j < _height-1; ++j) {
            for (int k = 1; k < _depth-1; ++k) {
                vx[idx(i, j, k)] -= 0.5*(temp[idx(i+1, j, k)]-temp[idx(i-1, j, k)])/b;
                vy[idx(i, j, k)] -= 0.5*(temp[idx(i, j+1, k)]-temp[idx(i, j-1, k)])/b;
                vz[idx(i, j, k)] -= 0.5*(temp[idx(i, j, k+1)]-temp[idx(i, j, k-1)])/b;
            }
        }
    }
}

void CPUSolver::addDensity(int x, int y, int z, float amount) {
    density0[idx(x, y, z)] += amount;
}

void CPUSolver::addDensity(int x, int y, int z, float r, float, float) {
    density0[idx(x, y, z)] += r;
}

void CPUSolver::addVelocityX(int x, int y, int z, float amount) {
    vx0[idx(x, y, z)] += amount;
}

void CPUSolver::addVelocityY(int x, int y, int z, float amount) {
    vy0[idx(x, y, z)] += amount;
}

void CPUSolver::addVelocityZ(int x, int y, int z, float amount) {
    vy0[idx(x, y, z)] += amount;
}

void CPUSolver::solve(float dt) {
    //diffuse(density1, density0, dt);
    //swap(density1, density0);
    advect(density1, density0, vx0, vy0, vz0, dt);
    swap(density1, density0);

    //diffuse(vx1, vx0, dt);
    //swap(vx1, vx0);
    //diffuse(vy1, vy0, dt);
    //swap(vy1, vy0);
    //diffuse(vz1, vz0, dt);
    //swap(vz1, vz0);

    project(vx0, vy0, vz0, vx1, vy1);

    advect(vx1, vx0, vx0, vy0, vz0, dt);
    advect(vy1, vy0, vx0, vy0, vz0, dt);
    advect(vz1, vz0, vx0, vy0, vz0, dt);

    project(vx1, vy1, vz1, vx0, vy0);
    swap(vx1, vx0);
    swap(vy1, vy0);
    swap(vz1, vz0);
}

void CPUSolver::fillDensityData(float *out) {
    for (int i = 0; i < _width; ++i) {
        for (int j = 0; j < _height; ++j) {
            for (int k = 0; k < _depth; ++k) {
                out[idx(i, j, k)*3] = density1[idx(i, j, k)];
                out[idx(i, j, k)*3+1] = density1[idx(i, j, k)];
                out[idx(i, j, k)*3+2] = density1[idx(i, j, k)];
            }
        }
    }
}

}
