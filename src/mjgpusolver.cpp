/*
    mjgpusolver.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjgpusolver.h"
#include "half.h"
#include <iostream>
#include <cstring>
//(x, y, z) -> (x, y+zh) -> (x+w*(y+zh)) -> (x+wy+whz)
#define idv(x, y, z, e) ((_width*_height*(z)+_width*(y)+(x))*3+(e))
#define idv4(x, y, z, e) ((_width*_height*(z)+_width*(y)+(x))*4+(e))
#define idx(x, y, z) (_width*_height*(z)+_width*(y)+(x))
#define swapt(x, y) {Texture2D *t = (x); (x) = (y); (y) = t;}

namespace mcjee {

class GPUComputeModel : public Renderable {
public:
    float width;
    float height;
    float depth;
    float dt;

    Texture *texture0;
    Texture *texture1;
    GPUComputeModel(Geometry *geo, Shader *shader) :
        Renderable(geo, shader, GL_TRIANGLE_STRIP) {
    }
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {
        shader->setUniform1f("width", width);
        shader->setUniform1f("height", height);
        shader->setUniform1f("depth", depth);
        shader->setUniform1f("xunit", 1.0/width);
        shader->setUniform1f("yunit", 1.0/(height*depth));
        shader->setUniform1f("zunit", 1.0/(height*depth));
        shader->setUniform1f("sliceHeight", 1.0/depth);
        shader->setUniform1f("dt", dt);

        shader->setUniform1i("inBuffer", 0);
        shader->setUniform1i("buffer0", 0);
        shader->setUniform1i("divergenceBuffer", 0);
        shader->setUniform1i("scalarBuffer", 0);
        shader->setUniform1i("velocityBuffer", 1);
        shader->setUniform1i("buffer1", 1);
        shader->setUniform1i("pressureBuffer", 1);

        texture0->bindToUnit(GL_TEXTURE0);
        texture1->bindToUnit(GL_TEXTURE1);
    }
};

GPUSolver::GPUSolver(int width, int height, int depth) :
    FluidSolver(width, height, depth) {

    densityBuffer = new uint16_t[width*height*depth*4];
    velocityBuffer = new uint16_t[width*height*depth*3];

    memset(densityBuffer, 0, sizeof(uint16_t)*width*height*depth*4);
    memset(velocityBuffer, 0, sizeof(uint16_t)*width*height*depth*3);

    densityTex0 = new Texture2D(GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT, width, height*depth);
    densityTex0->initData(densityBuffer);
    densityTex0->interpolation(GL_LINEAR);
    densityTex0->wrap(GL_CLAMP_TO_BORDER);

    densityTex1 = new Texture2D(GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT, width, height*depth);
    densityTex1->initData(densityBuffer);
    densityTex1->interpolation(GL_LINEAR);
    densityTex1->wrap(GL_CLAMP_TO_BORDER);

    velocityTex0 = new Texture2D(GL_RGB16F_ARB, GL_RGB, GL_HALF_FLOAT, width, height*depth);
    velocityTex0->initData(densityBuffer);
    velocityTex0->interpolation(GL_LINEAR);
    velocityTex0->wrap(GL_CLAMP_TO_BORDER);

    velocityTex1 = new Texture2D(GL_RGB16F_ARB, GL_RGB, GL_HALF_FLOAT, width, height*depth);
    velocityTex1->initData(densityBuffer);
    velocityTex1->interpolation(GL_LINEAR);
    velocityTex1->wrap(GL_CLAMP_TO_BORDER);

    pressureTex0 = new Texture2D(GL_R16F, GL_RED, GL_HALF_FLOAT, width, height*depth);
    pressureTex0->initData(densityBuffer);
    pressureTex0->interpolation(GL_NEAREST);
    pressureTex0->wrap(GL_CLAMP_TO_BORDER);

    pressureTex1 = new Texture2D(GL_R16F, GL_RED, GL_HALF_FLOAT, width, height*depth);
    pressureTex1->initData(densityBuffer);
    pressureTex1->interpolation(GL_NEAREST);
    pressureTex1->wrap(GL_CLAMP_TO_BORDER);

    divergenceTex = new Texture2D(GL_R16F, GL_RED, GL_HALF_FLOAT, width, height*depth);
    divergenceTex->initData(densityBuffer);
    divergenceTex->interpolation(GL_NEAREST);
    divergenceTex->wrap(GL_CLAMP_TO_BORDER);

    GLenum drawTarget = GL_COLOR_ATTACHMENT0;
    outputFramebuffer = new Framebuffer(width, height*depth);
    outputFramebuffer->backgroundColor = Vector4();
    outputFramebuffer->setDrawBuffers(1, &drawTarget);

    densityBufferTex = new Texture2D(GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT, width, height*depth);
    densityBufferTex->initData(densityBuffer);
    velocityBufferTex = new Texture2D(GL_RGB16F_ARB, GL_RGB, GL_HALF_FLOAT, width, height*depth);
    velocityBufferTex->initData(velocityBuffer);

    computeScene = new Scene(outputFramebuffer);
    computeScene->blendEnabled = false;
    advectKernel = new Shader();
    divergenceKernel = new Shader();
    project2Kernel = new Shader();
    subgradientKernel = new Shader();
    addKernel = new Shader();
    try {
        advectKernel->compile("shaders/kernel.vsh", "shaders/advect.fsh");
        divergenceKernel->compile("shaders/kernel.vsh", "shaders/divergence.fsh");
        project2Kernel->compile("shaders/kernel.vsh", "shaders/project2.fsh");
        subgradientKernel->compile("shaders/kernel.vsh", "shaders/subgradient.fsh");
        addKernel->compile("shaders/kernel.vsh", "shaders/add.fsh");
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
    geo = new Geometry(vertices, indices, 4, 4, 3);

    model = new GPUComputeModel(geo, advectKernel);
    model->init();
    model->width = width;
    model->height = height;
    model->depth = depth;
    computeScene->add(model);
}

GPUSolver::~GPUSolver() {
    delete densityBuffer;
    delete velocityBuffer;
    delete densityTex0;
    delete densityTex1;
    delete velocityTex0;
    delete velocityTex1;
    delete pressureTex0;
    delete pressureTex1;
    delete divergenceTex;
    delete outputFramebuffer;
    delete densityBufferTex;
    delete velocityBufferTex;
    delete computeScene;
    delete addKernel;
    delete advectKernel;
    delete divergenceKernel;
    delete project2Kernel;
    delete subgradientKernel;
    delete model;
    delete geo;
}

void GPUSolver::addVelocity(Vector3 pos, Vector3 amount) {
    float valx = amount.x;
    float valy = amount.y;
    float valz = amount.z;
    int ix = idv((int)pos.x, (int)pos.y, (int)pos.z, 0);
    if (ix+2 < _width*_height*_depth*3) {
        velocityBuffer[ix] = half_from_float(*(uint32_t *)(&valx));
        velocityBuffer[ix+1] = half_from_float(*(uint32_t *)(&valy));
        velocityBuffer[ix+2] = half_from_float(*(uint32_t *)(&valz));
    }
}

void GPUSolver::addDensity(Vector3 pos, Vector4 amount) {
    float valr = amount.r;
    float valg = amount.g;
    float valb = amount.b;
    float vala = amount.a;
    int ir = idv4((int)pos.x, (int)pos.y, (int)pos.z, 0);
    if (ir+2 < _width*_height*_depth*4) {
        densityBuffer[ir] = half_from_float(*(uint32_t *)(&valr));
        densityBuffer[ir+1] = half_from_float(*(uint32_t *)(&valg));
        densityBuffer[ir+2] = half_from_float(*(uint32_t *)(&valb));
        densityBuffer[ir+3] = half_from_float(*(uint32_t *)(&vala));
    }
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

    pressureTex0->initData((float *)0);
    model->shader = project2Kernel;
    for (int n = 0; n < iterations; ++n) {
        model->texture0 = divergenceTex;
        model->texture1 = pressureTex0;
        outputFramebuffer->addRenderTarget(pressureTex1, GL_COLOR_ATTACHMENT0);
        computeScene->render();
        swapt(pressureTex0, pressureTex1);
    }

    model->shader = subgradientKernel;
    model->texture0 = pressureTex0;
    model->texture1 = velocityTex0;
    outputFramebuffer->addRenderTarget(velocityTex1, GL_COLOR_ATTACHMENT0);
    computeScene->render();
    swapt(velocityTex0, velocityTex1);
}

void GPUSolver::solve(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    solveDensities(dt);
    solveVelocities(dt);
}

void GPUSolver::solveDensities(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    densityBufferTex->initData(densityBuffer);
    addStep(densityTex0, densityBufferTex, densityTex1);
    swapt(densityTex0, densityTex1);
    advectStep(densityTex0, densityTex1);
    swapt(densityTex0, densityTex1);

    memset(densityBuffer, 0, sizeof(uint16_t)*_width*_height*_depth*4);
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

    memset(velocityBuffer, 0, sizeof(uint16_t)*_width*_height*_depth*3);
}

void GPUSolver::fillDensityData(float *out) {
    densityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, out);
    densityTex0->unbind();
}

void GPUSolver::fillVelocityData(float *out) {
    velocityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, out);
    velocityTex0->unbind();
}

void GPUSolver::fillDensityData(uint16_t *out) {
    densityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_HALF_FLOAT, out);
    densityTex0->unbind();
}

void GPUSolver::fillVelocityData(uint16_t *out) {
    velocityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_HALF_FLOAT, out);
    velocityTex0->unbind();
}

void GPUSolver::clearDensity() {
    outputFramebuffer->addRenderTarget(densityTex0, GL_COLOR_ATTACHMENT0);
    outputFramebuffer->clear(GL_COLOR_BUFFER_BIT);
    outputFramebuffer->addRenderTarget(densityTex1, GL_COLOR_ATTACHMENT0);
    outputFramebuffer->clear(GL_COLOR_BUFFER_BIT);
}

void GPUSolver::clearVelocity() {
    outputFramebuffer->addRenderTarget(velocityTex0, GL_COLOR_ATTACHMENT0);
    outputFramebuffer->clear(GL_COLOR_BUFFER_BIT);
    outputFramebuffer->addRenderTarget(velocityTex1, GL_COLOR_ATTACHMENT0);
    outputFramebuffer->clear(GL_COLOR_BUFFER_BIT);
}

}
