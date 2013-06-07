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
        if (shader->uniformEnabled("width"))
            glUniform1f(shader->getUniformLocation("width"), width);
        if (shader->uniformEnabled("height"))
            glUniform1f(shader->getUniformLocation("height"), height);
        if (shader->uniformEnabled("depth"))
            glUniform1f(shader->getUniformLocation("depth"), depth);
        if (shader->uniformEnabled("xunit"))
            glUniform1f(shader->getUniformLocation("xunit"), 1.0/width);
        if (shader->uniformEnabled("yunit"))
            glUniform1f(shader->getUniformLocation("yunit"), 1.0/(height*depth));
        if (shader->uniformEnabled("sliceHeight"))
            glUniform1f(shader->getUniformLocation("sliceHeight"), 1.0/depth);
        if (shader->uniformEnabled("dt"))
            glUniform1f(shader->getUniformLocation("dt"), dt);

        GLint tex0loc = -1;
        if (shader->uniformEnabled("inBuffer")) 
            tex0loc = shader->getUniformLocation("inBuffer");
        else if (shader->uniformEnabled("buffer0")) 
            tex0loc = shader->getUniformLocation("buffer0");
        else if (shader->uniformEnabled("divergenceBuffer")) 
            tex0loc = shader->getUniformLocation("divergenceBuffer");
        else if (shader->uniformEnabled("scalarBuffer")) 
            tex0loc = shader->getUniformLocation("scalarBuffer");
        if (tex0loc != -1) glUniform1i(tex0loc, 0);

        GLint tex1loc = -1;
        if (shader->uniformEnabled("velocityBuffer")) 
            tex1loc = shader->getUniformLocation("velocityBuffer");
        else if (shader->uniformEnabled("buffer1")) 
            tex1loc = shader->getUniformLocation("buffer1");
        else if (shader->uniformEnabled("pressureBuffer")) 
            tex1loc = shader->getUniformLocation("pressureBuffer");
        if (tex1loc != -1) glUniform1i(tex1loc, 1);

        glActiveTexture(GL_TEXTURE0);
        texture0->bind();
        glActiveTexture(GL_TEXTURE1);
        texture1->bind();
    }
};

GPUSolver::GPUSolver(int width, int height, int depth) :
    FluidSolver(width, height, depth) {

    densityBuffer = new float[width*height*depth*4];
    velocityBuffer = new float[width*height*depth*3];
    divergence = new float[width*height*depth];
    pressure = new float[width*height*depth];

    memset(densityBuffer, 0, sizeof(float)*width*height*depth*4);
    memset(velocityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(divergence, 0, sizeof(float)*width*height*depth);
    memset(pressure, 0, sizeof(float)*width*height*depth);

    glActiveTexture(GL_TEXTURE0);
    densityTex0 = new Texture2D(GL_RGBA, GL_RGBA, GL_FLOAT, width, height*depth);
    densityTex0->initData(densityBuffer);
    densityTex0->interpolation(GL_LINEAR);
    densityTex0->wrap(GL_CLAMP_TO_BORDER);

    densityTex1 = new Texture2D(GL_RGBA, GL_RGBA, GL_FLOAT, width, height*depth);
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

    pressureTex0 = new Texture2D(GL_RED, GL_RED, GL_FLOAT, width, height*depth);
    pressureTex0->initData(pressure);
    pressureTex0->interpolation(GL_NEAREST);
    pressureTex0->wrap(GL_CLAMP_TO_BORDER);

    pressureTex1 = new Texture2D(GL_RED, GL_RED, GL_FLOAT, width, height*depth);
    pressureTex1->initData(pressure);
    pressureTex1->interpolation(GL_NEAREST);
    pressureTex1->wrap(GL_CLAMP_TO_BORDER);

    divergenceTex = new Texture2D(GL_RED, GL_RED, GL_FLOAT, width, height*depth);
    divergenceTex->initData(divergence);
    divergenceTex->interpolation(GL_NEAREST);
    divergenceTex->wrap(GL_CLAMP_TO_BORDER);

    GLenum drawTarget = GL_COLOR_ATTACHMENT0;
    outputFramebuffer = new Framebuffer(width, height*depth);
    outputFramebuffer->backgroundColor = Vector4();
    outputFramebuffer->bind();
    glDrawBuffers(1, &drawTarget);
    outputFramebuffer->unbind();

    densityBufferTex = new Texture2D(GL_RGBA, GL_RGBA, GL_FLOAT, width, height*depth);
    densityBufferTex->initData(densityBuffer);
    velocityBufferTex = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityBufferTex->initData(velocityBuffer);
    velocityBufferTex->unbind();

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
    delete divergence;
    delete pressure;
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
    delete advectKernel;
    delete divergenceKernel;
    delete project2Kernel;
    delete subgradientKernel;
    delete addKernel;
    delete model;
    delete geo;
}

void GPUSolver::addVelocity(Vector3 pos, Vector3 amount) {
    velocityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 0)] += amount.x/_width;
    velocityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 1)] += amount.y/_height;
    velocityBuffer[idv((int)pos.x, (int)pos.y, (int)pos.z, 2)] += amount.z/_depth;
}

void GPUSolver::addDensity(Vector3 pos, Vector3 amount) {
    densityBuffer[idv4((int)pos.x, (int)pos.y, (int)pos.z, 0)] += amount.x;
    densityBuffer[idv4((int)pos.x, (int)pos.y, (int)pos.z, 1)] += amount.y;
    densityBuffer[idv4((int)pos.x, (int)pos.y, (int)pos.z, 2)] += amount.z;
    densityBuffer[idv4((int)pos.x, (int)pos.y, (int)pos.z, 3)] += 10.0;
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
    pressureTex0->unbind();
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
    densityBufferTex->unbind();
    addStep(densityTex0, densityBufferTex, densityTex1);
    swapt(densityTex0, densityTex1);
    advectStep(densityTex0, densityTex1);
    swapt(densityTex0, densityTex1);

    memset(densityBuffer, 0, sizeof(float)*_width*_height*_depth*4);
}

void GPUSolver::solveVelocities(float dt) {
    glViewport(0, 0, _width, _height*_depth);
    model->dt = dt;

    velocityBufferTex->initData(velocityBuffer);
    velocityBufferTex->unbind();
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
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, out);
    densityTex0->unbind();
}

void GPUSolver::fillVelocityData(float *out) {
    velocityTex0->bind();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, out);
    velocityTex0->unbind();
}

}
