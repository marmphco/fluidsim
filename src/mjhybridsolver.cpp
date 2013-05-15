/*
    mjhybridsolver.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjhybridsolver.h"
#include <iostream>
#include <cstring>
//(x, y, z) -> (x, y+zh) -> (x+w*(y+zh)) -> (x+wy+whz)
#define idv(x, y, z, e) ((_width*_height*(z)+_width*(y)+(x))*3+(e))
#define idx(x, y, z) (_width*_height*(z)+_width*(y)+(x))
#define swapt(x, y) {Texture2D *t = (x); (x) = (y); (y) = t;}
#define swapf(x, y) {Framebuffer *t = (x); (x) = (y); (y) = t;}

namespace mcjee {

HybridSolver::HybridSolver(int width, int height, int depth) :
    FluidSolver(width, height, depth) {

    densityBuffer = new float[width*height*depth*3];
    velocityBuffer = new float[width*height*depth*3];
    temp0 = new float[width*height*depth];
    temp1 = new float[width*height*depth];

    memset(densityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(velocityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(temp0, 0, sizeof(float)*width*height*depth);
    memset(temp1, 0, sizeof(float)*width*height*depth);

    densityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex0->initData(densityBuffer);
    densityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex1->initData(densityBuffer);
    velocityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex0->initData(densityBuffer);
    velocityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex1->initData(densityBuffer);

    GLenum drawTarget = GL_COLOR_ATTACHMENT0;

    density0 = new Framebuffer(width, height*depth);
    density0->addRenderTarget(densityTex0, GL_COLOR_ATTACHMENT0);
    glDrawBuffers(1, &drawTarget);
    density1 = new Framebuffer(width, height*depth);
    density1->addRenderTarget(densityTex1, GL_COLOR_ATTACHMENT0);
    glDrawBuffers(1, &drawTarget);

    velocity0 = new Framebuffer(width, height*depth);
    velocity0->addRenderTarget(velocityTex0, GL_COLOR_ATTACHMENT0);
    glDrawBuffers(1, &drawTarget);

    velocity1 = new Framebuffer(width, height*depth);
    velocity1->addRenderTarget(velocityTex1, GL_COLOR_ATTACHMENT0);
    glDrawBuffers(1, &drawTarget);

    densityBufferTex = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityBufferTex->initData(densityBuffer);
    velocityBufferTex = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityBufferTex->initData(velocityBuffer);

    advectScene = new Scene(density0);
    advectKernel = new Shader();
    addKernel = new Shader();
    try {
        advectKernel->compile("shaders/advect.vsh", "shaders/advect.fsh");
        addKernel->compile("shaders/add.vsh", "shaders/add.fsh");
    } catch (ShaderError &e) {
        std::cerr << e.what() << std::endl;
    }

    GLfloat vertices[] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        -1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
    };
    GLuint indices[] = {
        0, 1, 2, 3
    };
    Geometry *geo = new Geometry(vertices, indices, 4, 4, 3);

    model = new ComputeModel(geo, advectKernel);
    model->init();
    advectScene->add(model);
}

void HybridSolver::addVelocityX(int x, int y, int z, float amount) {
    velocityBuffer[idv(x, y, z, 0)] += amount;
}

void HybridSolver::addVelocityY(int x, int y, int z, float amount) {
    velocityBuffer[idv(x, y, z, 1)] += amount;
}

void HybridSolver::addVelocityZ(int x, int y, int z, float amount) {
    velocityBuffer[idv(x, y, z, 2)] += amount;
}

void HybridSolver::addDensity(int x, int y, int z, float amount) {
    densityBuffer[idv(x, y, z, 0)] += amount;
    densityBuffer[idv(x, y, z, 1)] += amount;
    densityBuffer[idv(x, y, z, 2)] += amount;
}

void HybridSolver::solve(float dt) {
    glViewport(0, 0, _width, _height*_depth);

    //add densitybuffer
    densityBufferTex->initData(densityBuffer);

    model->shader = addKernel;
    advectScene->framebuffer = density1;
    model->texture0 = densityTex0;
    model->texture1 = densityBufferTex;
    advectScene->render();
    swapf(density0, density1);
    swapt(densityTex0, densityTex1);

    //advect densities
    model->shader = advectKernel;
    advectScene->framebuffer = density1;
    model->texture0 = densityTex0;
    model->texture1 = velocityTex0;
    advectScene->render();
    swapf(density0, density1);
    swapt(densityTex0, densityTex1);

    //add velocitybuffer
    velocityBufferTex->initData(velocityBuffer);

    model->shader = addKernel;
    advectScene->framebuffer = velocity1;
    model->texture0 = velocityTex0;
    model->texture1 = velocityBufferTex;
    advectScene->render();
    swapf(velocity0, velocity1);
    swapt(velocityTex0, velocityTex1);

    //project velocities
    velocity0->bind();
    glReadPixels(0, 0, _width, _height, GL_RGB, GL_FLOAT, velocityBuffer);
    velocity0->unbind();
    project(velocityBuffer, temp0, temp1);
    velocityTex0->initData(velocityBuffer);

    //advect velocities
    model->shader = advectKernel;
    advectScene->framebuffer = velocity1;
    model->texture0 = velocityTex0;
    model->texture1 = velocityTex0;
    advectScene->render();
    swapf(velocity0, velocity1);
    swapt(velocityTex0, velocityTex1);

    //project velocities
    velocity0->bind();
    glReadPixels(0, 0, _width, _height, GL_RGB, GL_FLOAT, velocityBuffer);
    velocity0->unbind();
    project(velocityBuffer, temp0, temp1);
    velocityTex0->initData(velocityBuffer);

    //clear add buffers
    memset(densityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
    memset(velocityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
}

void HybridSolver::fillDensityData(float *out) {
    density0->bind();
    glReadPixels(0, 0, _width, _height*_depth, GL_RGB, GL_FLOAT, out);
    /*for (int i = 0; i < _width; ++i) {
        for (int j = 0; j < _height; ++j) {
            for (int k = 0; k < _depth; ++k) {
                out[idv(i, j, k, 0)] = densityBuffer[idv(i, j, k, 0)];
                out[idv(i, j, k, 1)] = densityBuffer[idv(i, j, k, 1)];
                out[idv(i, j, k, 2)] = densityBuffer[idv(i, j, k, 2)];
            }
        }
    }*/
    density0->unbind();
}

void HybridSolver::project(float *vel, float *div, float *temp) {
    //compute divergence
    float b = 1.0; // arbitrary scaling term
    for (int i = 1; i < _width-1; ++i) {
        for (int j = 1; j < _height-1; ++j) {
            for (int k = 1; k < _depth-1; ++k) {
                div[idx(i, j, k)] = 0.5*b*(
                    vel[idv(i+1, j, k, 0)]-vel[idv(i-1, j, k, 0)]+
                    vel[idv(i, j+1, k, 1)]-vel[idv(i, j-1, k, 1)]+
                    vel[idv(i, j, k+1, 2)]-vel[idv(i, j, k-1, 2)]
                );
                temp[idx(i, j, k)] = 0.0;
            }
        }
    }

    //compute base scalar field of gradient component somehow
    for (int n = 0; n < 4; ++n) {
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
                vel[idv(i, j, k, 0)] -= 0.5*(temp[idx(i+1, j, k)]-temp[idx(i-1, j, k)])/b;
                vel[idv(i, j, k, 1)] -= 0.5*(temp[idx(i, j+1, k)]-temp[idx(i, j-1, k)])/b;
                vel[idv(i, j, k, 2)] -= 0.5*(temp[idx(i, j, k+1)]-temp[idx(i, j, k-1)])/b;
            }
        }
    }
}

}
