/*
    models.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "models.h"
#include "half.h"

#define idv(x, y, z, e) ((width*height*(z)+width*(y)+(x))*3+(e))

/////////////////////////
// FluidModel
/////////////////////////

FluidModel::FluidModel(Shader *shader, GLenum drawType, int width, int height, int depth) :
    Renderable(NULL, shader, drawType), samples(64) {
    geometry = loadCube(1.0, 1.0, 1.0);
    densityTexture = new Texture3D(GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT, width, height, depth);
    densityTexture->interpolation(GL_LINEAR);
    densityTexture->initData((uint16_t *)NULL);
}

FluidModel::~FluidModel() {
    delete geometry;
    delete densityTexture;
}

void FluidModel::setupVertexAttributes() {
    GLint loc = shader->getAttribLocation("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // HACKY use same vertices for texture coordinates
    loc = shader->getAttribLocation("vTexCoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
void FluidModel::setupUniforms() {
    GLint sampleloc = shader->getUniformLocation("samples");
    glUniform1i(sampleloc, samples);

    GLint loc = shader->getUniformLocation("texture0");
    glUniform1i(loc, 0);
    glActiveTexture(GL_TEXTURE0);
    densityTexture->bind();
}

/////////////////////////
// BoundingBox
/////////////////////////

BoundingBox::BoundingBox(Shader *shader, FluidModel *target) :
    Renderable(NULL, shader, GL_LINES), _target(target) {
    geometry = loadWireCube(1.0, 1.0, 1.0);
}

BoundingBox::~BoundingBox() {
    delete geometry;
}

void BoundingBox::setupVertexAttributes() {
    GLint loc = shader->getAttribLocation("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void BoundingBox::setupUniforms() {
    rotation = _target->rotation;
    center = _target->center;
    scale = _target->scale;
}

/////////////////////////
// ParticleSystem
/////////////////////////

ParticleSystem::ParticleSystem(Shader *shader, int width, int height, int depth) :
    Renderable(NULL, shader, GL_LINES), width(width), height(height), depth(depth) {
    geometry = loadWireCube(1.0, 1.0, 1.0);
    velocityBuffer = new float[width*height*depth*3];
}

ParticleSystem::~ParticleSystem() {
    delete geometry;
    delete velocityBuffer;
}

void ParticleSystem::setupVertexAttributes() {
    GLint loc = shader->getAttribLocation("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void ParticleSystem::setupUniforms() {
    //update geometry
    geometry->modifyData((GLfloat *)&(particles[0]),
                         (GLuint *)&(indices[0]),
                         particles.size(),
                         indices.size(),
                         3);
}

void ParticleSystem::add(Vector3 position, float life) {
    particles.push_back(position);
    particles.push_back(position);
    indices.push_back(indices.size());
    indices.push_back(indices.size());
    lifes.push_back(life);
}

void ParticleSystem::update(float dt) {
    if (!visible) return;
    for (unsigned int i = 0; i < particles.size(); i += 2) {
        float scale = 1.0;
        // constrain to bounds
        Vector3 p = particles[i];
        if (p.x > 1.0 || p.y > 1.0 ||p.z > 1.0 ||
            p.x < 0.0 || p.y < 0.0 || p.z < 0.0 ||
            lifes[i/2] < 0.0) {

            p.x = 1.0*rand()/RAND_MAX;
            p.y = 1.0*rand()/RAND_MAX;
            p.z = 1.0*rand()/RAND_MAX;
            lifes[i/2] = 2.0*rand()/RAND_MAX;
        }
        if (p.x > 1.0) p.x = 1.0;
        if (p.y > 1.0) p.y = 1.0;
        if (p.z > 1.0) p.z = 1.0;
        if (p.x < 0.0) p.x = 0.0;
        if (p.y < 0.0) p.y = 0.0;
        if (p.z < 0.0) p.z = 0.0;

        lifes[i/2] -= 0.01;
        //update previous position
        particles[i+1] = p;

        int tx = p.x*width;
        int ty = p.y*height;
        int tz = p.z*depth;
        float vx = velocityBuffer[idv((int)tx, (int)ty, (int)tz, 0)];
        float vy = velocityBuffer[idv((int)tx, (int)ty, (int)tz, 1)];
        float vz = velocityBuffer[idv((int)tx, (int)ty, (int)tz, 2)];

        Vector3 vel = Vector3(vx, vy, vz);
        if (vel.length() > 1) vel.normalize();

        particles[i] = p + vel * dt * scale; // normalize all for cool stuff

    }
    glLineWidth(1.0);
}
