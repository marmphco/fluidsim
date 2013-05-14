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

class ComputeModel : public Renderable {
public:
    //Texture *texture1;
    //Texture *texture2;
    ComputeModel(Shader *shader) :
        Renderable(NULL, shader, GL_TRIANGLES) {
        GLfloat vertices[] = {
            -1.0, -1.0, 0.0,
            1.0, -1.0, 0.0,
            -1.0, 1.0, 0.0,
            1.0, 1.0, 0.0,
        };
        GLuint indices[6];
        squareIndices(indices);
        Geometry *geo = new Geometry(vertices, indices, 4, 6, 3);
        geometry = geo;
    }
    virtual void setupVertexAttributes() {
        GLint loc = shader->getAttribLocation("vPosition");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    virtual void setupUniforms() {
        GLint tex0loc = shader->getUniformLocation("inBuffer");
        glUniform1i(tex0loc, 0);
        GLint tex1loc = shader->getUniformLocation("velocityBuffer");
        glUniform1i(tex1loc, 1);
    }
};

HybridSolver::HybridSolver(int width, int height, int depth) :
    FluidSolver(width, height, depth) {
    densityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex0->initData((float *)0);
    densityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    densityTex1->initData((float *)0);
    velocityTex0 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex0->initData((float *)0);
    velocityTex1 = new Texture2D(GL_RGB, GL_RGB, GL_FLOAT, width, height*depth);
    velocityTex1->initData((float *)0);

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


    densityBuffer = new float[width*height*depth*3];
    velocityBuffer = new float[width*height*depth*3];
    temp0 = new float[width*height*depth];
    temp1 = new float[width*height*depth];

    memset(densityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(velocityBuffer, 0, sizeof(float)*width*height*depth*3);
    memset(temp0, 0, sizeof(float)*width*height*depth);
    memset(temp1, 0, sizeof(float)*width*height*depth);

    advectScene = new Scene(density0);
    advectKernel = new Shader();
    try {
        advectKernel->compile("shaders/advect.vsh", "shaders/advect.fsh");
    } catch (ShaderError &e) {
        std::cerr << e.what() << std::endl;
    }

    model = new ComputeModel(advectKernel);
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
    //add densitybuffer
    densityTex0->initData(densityBuffer);
    advectScene->framebuffer = density1;
    //set active texture to densityTex0
    //advectKernel->use();
    //glActiveTexture(GL_TEXTURE0);
    //densityTex0->bind();
    //glActiveTexture(GL_TEXTURE1);
    //velocityTex0->bind();
    /*if (model->material.textures.size() != 2) {
        model->material.textures.push_back(densityTex0);
        model->material.textures.push_back(velocityTex0);
    } else {
        model->material.textures[0] = densityTex0;
        model->material.textures[1] = velocityTex0;
    }
    
    advectScene->render();*/

    density1->bind();
    glViewport(0, 0, _width, _height*_depth);
    glClear(GL_COLOR_BUFFER_BIT);
    GLfloat vertices[] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        -1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
    };
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    advectKernel->use();
    GLint tex0loc = advectKernel->getUniformLocation("inBuffer");
    glUniform1i(tex0loc, 0);
    //GLint tex1loc = advectKernel->getUniformLocation("velocityBuffer");
    //glUniform1i(tex1loc, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, densityTex0->texture);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, velocityTex0->texture);
    GLint loc = advectKernel->getAttribLocation("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    swapf(density0, density1);
    swapt(densityTex0, densityTex1);

    //velocity0->bind();
    //glReadPixels(0, 0, _width, _height, GL_RGB, GL_FLOAT, velocityBuffer);
    //velocity0->unbind();
    //project(velocityBuffer, temp0, temp1);
    //velocityTex0->initData(velocityBuffer);

    //add velocitybuffers
    velocityTex0->initData(velocityBuffer);
    //advectScene->framebuffer = velocity1;
    //set active texture to velocityTex0
    //advectScene->render();
    //swapf(velocity0, velocity1);
    //swapt(velocityTex0, velocityTex1);

    //velocity1->bind();
    //glReadPixels(0, 0, _width, _height, GL_RGB, GL_FLOAT, velocityBuffer);
    //velocity1->unbind();
    //project(velocityBuffer, temp0, temp1);
    //velocityTex1->initData(velocityBuffer);
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
