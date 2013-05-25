/*
    mjgpusolver.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjgpusolver.h"
#include <iostream>
#include <cstring>
//(x, y, z) -> (x, y+zh) -> (x+w*(y+zh)) -> (x+wy+whz)
#define idv(x, y, z, e) ((_width*_height*(z)+_width*(y)+(x))*3+(e))
#define idx(x, y, z) (_width*_height*(z)+_width*(y)+(x))
#define swapt(x, y) {Texture2D *t = (x); (x) = (y); (y) = t;}

namespace mcjee {

GPUSolver::GPUSolver(int width, int height, int depth) :
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
    densityTex0->interpolation(GL_LINEAR);
    densityTex0->wrap(GL_CLAMP_TO_EDGE);

    densityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex1->initData(densityBuffer);
    densityTex1->interpolation(GL_LINEAR);
    densityTex1->wrap(GL_CLAMP_TO_EDGE);

    velocityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex0->initData(densityBuffer);
    velocityTex0->interpolation(GL_LINEAR);
    velocityTex0->wrap(GL_CLAMP_TO_EDGE);

    velocityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex1->initData(densityBuffer);
    velocityTex1->interpolation(GL_LINEAR);
    velocityTex1->wrap(GL_CLAMP_TO_EDGE);

    GLenum drawTarget = GL_COLOR_ATTACHMENT0;
    outputFramebuffer = new Framebuffer(width, height*depth);
    outputFramebuffer->bind();
    glDrawBuffers(1, &drawTarget);
    outputFramebuffer->unbind();

    densityBufferTex = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityBufferTex->initData(densityBuffer);
    velocityBufferTex = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityBufferTex->initData(velocityBuffer);

    computeScene = new Scene(outputFramebuffer);
    advectKernel = new Shader();
    projectKernel = new Shader();
    addKernel = new Shader();
    try {
        advectKernel->compile("shaders/advect.vsh", "shaders/advect.fsh");
        projectKernel->compile("shaders/project.vsh", "shaders/project.fsh");
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

    model = new GPUComputeModel(geo, advectKernel);
    model->init();
    model->width = width;
    model->height = height;
    model->depth = depth;
    computeScene->add(model);
}

void GPUSolver::addVelocityX(int x, int y, int z, float amount) {
    velocityBuffer[idv(x, y, z, 0)] += amount/_width;
}

void GPUSolver::addVelocityY(int x, int y, int z, float amount) {
    velocityBuffer[idv(x, y, z, 1)] += amount/_height;
}

void GPUSolver::addVelocityZ(int x, int y, int z, float amount) {
    velocityBuffer[idv(x, y, z, 2)] += amount/_depth;
}

void GPUSolver::addDensity(int x, int y, int z, float amount) {
    densityBuffer[idv(x, y, z, 0)] += amount;
    densityBuffer[idv(x, y, z, 1)] += amount;
    densityBuffer[idv(x, y, z, 2)] += amount;
}

void GPUSolver::addDensity(int x, int y, int z, float r, float g, float b) {
    densityBuffer[idv(x, y, z, 0)] += r;
    densityBuffer[idv(x, y, z, 1)] += g;
    densityBuffer[idv(x, y, z, 2)] += b;
}

void GPUSolver::solve(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    //add densitybuffer
    densityBufferTex->initData(densityBuffer);

    model->shader = addKernel;
    model->texture0 = densityTex0;
    model->texture1 = densityBufferTex;
    outputFramebuffer->addRenderTarget(densityTex1, GL_COLOR_ATTACHMENT0);
    computeScene->render();
    swapt(densityTex0, densityTex1);

    //advect densities
    model->shader = advectKernel;
    model->texture0 = densityTex0;
    model->texture1 = velocityTex0;
    outputFramebuffer->addRenderTarget(densityTex1, GL_COLOR_ATTACHMENT0);
    computeScene->render();
    swapt(densityTex0, densityTex1);

    //add velocitybuffer
    velocityBufferTex->initData(velocityBuffer);

    model->shader = addKernel;
    model->texture0 = velocityTex0;
    model->texture1 = velocityBufferTex;
    outputFramebuffer->addRenderTarget(velocityTex1, GL_COLOR_ATTACHMENT0);
    computeScene->render();
    swapt(velocityTex0, velocityTex1);

    //project velocities
    outputFramebuffer->bind();
    glReadPixels(0, 0, _width, _height*_depth, GL_RGB, GL_FLOAT, velocityBuffer);
    outputFramebuffer->unbind();
    project(velocityBuffer, temp0, temp1);
    velocityTex0->initData(velocityBuffer);

    //advect velocities
    model->shader = advectKernel;
    model->texture0 = velocityTex0;
    model->texture1 = velocityTex0;
    outputFramebuffer->addRenderTarget(velocityTex1, GL_COLOR_ATTACHMENT0);
    computeScene->render();
    swapt(velocityTex0, velocityTex1);

    //project velocities
    outputFramebuffer->bind();
    glReadPixels(0, 0, _width, _height*_depth, GL_RGB, GL_FLOAT, velocityBuffer);
    outputFramebuffer->unbind();
    project(velocityBuffer, temp0, temp1);
    velocityTex0->initData(velocityBuffer);

    //clear add buffers
    memset(densityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
    memset(velocityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
}

void GPUSolver::fillDensityData(float *out) {
    outputFramebuffer->addRenderTarget(densityTex0, GL_COLOR_ATTACHMENT0);
    outputFramebuffer->bind();
    glReadPixels(0, 0, _width, _height*_depth, GL_RGB, GL_FLOAT, out);
    outputFramebuffer->unbind();
}

void GPUSolver::project(float *vel, float *div, float *temp) {
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
