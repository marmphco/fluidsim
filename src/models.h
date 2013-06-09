/*
    models.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MODELS_H
#define MODELS_H

#include "mj.h"
#include <vector>

using namespace mcjee;

class FluidModel : public Renderable {
public:
    Texture3D *densityTexture;
    int samples;

    FluidModel(Shader *shader, GLenum drawType, int width, int height, int depth);
    ~FluidModel();
    void setupVertexAttributes();
    void setupUniforms();
};

class BoundingBox : public Renderable {
private:
    FluidModel *_target;
public:
    BoundingBox(Shader *shader, FluidModel *target);
    ~BoundingBox();
    void setupVertexAttributes();
    void setupUniforms();
};

class ParticleSystem : public Renderable {
private:
    vector<Vector3> particles;
    vector<GLuint> indices;
    vector<float> lifes;
    int width;
    int height;
    int depth;
public:
    float *velocityBuffer;

    ParticleSystem(Shader *shader, int width, int height, int depth);
    ~ParticleSystem();
    void update(float dt);
    void add(Vector3 position, float life);
    void setupVertexAttributes(void);
    void setupUniforms(void);
};

#endif
