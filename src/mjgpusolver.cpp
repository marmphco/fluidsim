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
    divergence = new float[width*height*depth];
    temp = new float[width*height*depth];

    memset(densityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(velocityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(divergence, 0, sizeof(float)*width*height*depth);
    memset(temp, 0, sizeof(float)*width*height*depth);

    densityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex0->initData(densityBuffer);
    densityTex0->interpolation(GL_LINEAR);
    densityTex0->wrap(GL_CLAMP_TO_BORDER);

    densityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex1->initData(densityBuffer);
    densityTex1->interpolation(GL_LINEAR);
    densityTex1->wrap(GL_CLAMP_TO_BORDER);

    velocityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex0->initData(densityBuffer);
    velocityTex0->interpolation(GL_LINEAR);
    velocityTex0->wrap(GL_CLAMP_TO_BORDER);

    velocityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex1->initData(densityBuffer);
    velocityTex1->interpolation(GL_LINEAR);
    velocityTex1->wrap(GL_CLAMP_TO_BORDER);

    tempTex0 = new Texture2D(GL_RED, GL_RED, GL_FLOAT, width, height*depth);
    tempTex0->initData(temp);
    tempTex0->interpolation(GL_NEAREST);
    tempTex0->wrap(GL_CLAMP_TO_BORDER);

    tempTex1 = new Texture2D(GL_RED, GL_RED, GL_FLOAT, width, height*depth);
    tempTex1->initData(temp);
    tempTex1->interpolation(GL_NEAREST);
    tempTex1->wrap(GL_CLAMP_TO_BORDER);

    divergenceTex = new Texture2D(GL_RED, GL_RED, GL_FLOAT, width, height*depth);
    divergenceTex->initData(divergence);
    divergenceTex->interpolation(GL_NEAREST);
    divergenceTex->wrap(GL_CLAMP_TO_BORDER);

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
    divergenceKernel = new Shader();
    project2Kernel = new Shader();
    subgradientKernel = new Shader();
    addKernel = new Shader();
    try {
        advectKernel->compile("shaders/advect.vsh", "shaders/advect.fsh");
        divergenceKernel->compile("shaders/divergence.vsh", "shaders/divergence.fsh");
        project2Kernel->compile("shaders/project2.vsh", "shaders/project2.fsh");
        subgradientKernel->compile("shaders/subgradient.vsh", "shaders/subgradient.fsh");
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

void GPUSolver::addVelocity(Vector3 pos, Vector3 amount) {
    velocityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 0)] += amount.x/_width;
    velocityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 1)] += amount.y/_height;
    velocityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 2)] += amount.z/_depth;
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

void GPUSolver::addDensity(Vector3 pos, Vector3 amount) {
    densityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 0)] += amount.x;
    densityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 1)] += amount.y;
    densityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 2)] += amount.z;
}

void GPUSolver::addStep(Texture2D *in0, Texture2D *in1, Texture2D *out) {
    model->shader = addKernel;
    model->texture0 = in0;
    model->texture1 = in1;
    outputFramebuffer->addRenderTarget(out, GL_COLOR_ATTACHMENT0);
    computeScene->render();
}

void GPUSolver::advectStep(Texture2D *in, Texture2D *out) {
    model->shader = advectKernel;
    model->texture0 = in;
    model->texture1 = velocityTex0;
    outputFramebuffer->addRenderTarget(out, GL_COLOR_ATTACHMENT0);
    computeScene->render();
}

void GPUSolver::projectStep() {
    model->shader = divergenceKernel;
    model->texture1 = velocityTex0;
    outputFramebuffer->addRenderTarget(divergenceTex, GL_COLOR_ATTACHMENT0);
    computeScene->render();

    tempTex0->initData((float *)0);
    model->shader = project2Kernel;
    for (int n = 0; n < 16; ++n) {
        model->texture0 = divergenceTex;
        model->texture1 = tempTex0;
        outputFramebuffer->addRenderTarget(tempTex1, GL_COLOR_ATTACHMENT0);
        computeScene->render();
        swapt(tempTex0, tempTex1);
    }

    model->shader = subgradientKernel;
    model->texture0 = tempTex0;
    model->texture1 = velocityTex0;
    outputFramebuffer->addRenderTarget(velocityTex1, GL_COLOR_ATTACHMENT0);
    computeScene->render();
    swapt(velocityTex0, velocityTex1);
}

void GPUSolver::solve(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    densityBufferTex->initData(densityBuffer);
    addStep(densityTex0, densityBufferTex, densityTex1);
    swapt(densityTex0, densityTex1);

    advectStep(densityTex0, densityTex1);
    swapt(densityTex0, densityTex1);

    velocityBufferTex->initData(velocityBuffer);
    addStep(velocityTex0, velocityBufferTex, velocityTex1);
    swapt(velocityTex0, velocityTex1);

    projectStep();

    advectStep(velocityTex0, velocityTex1);
    swapt(velocityTex0, velocityTex1);

    projectStep();

    //clear add buffers
    memset(densityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
    memset(velocityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
}

void GPUSolver::solveDensities(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    densityBufferTex->initData(densityBuffer);
    addStep(densityTex0, densityBufferTex, densityTex1);
    swapt(densityTex0, densityTex1);

    advectStep(densityTex0, densityTex1);
    swapt(densityTex0, densityTex1);

    memset(densityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
}

void GPUSolver::solveVelocities(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    velocityBufferTex->initData(velocityBuffer);
    addStep(velocityTex0, velocityBufferTex, velocityTex1);
    swapt(velocityTex0, velocityTex1);

    projectStep();

    advectStep(velocityTex0, velocityTex1);
    swapt(velocityTex0, velocityTex1);

    projectStep();

    memset(velocityBuffer, 0, sizeof(float)*_width*_height*_depth*3);
}

void GPUSolver::fillDensityData(float *out) {
    densityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, out);
    densityTex0->unbind();
}

void GPUSolver::fillVelocityData(float *out) {
    velocityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, out);
    velocityTex0->unbind();
}

}
