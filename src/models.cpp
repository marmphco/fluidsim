/*
    models.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "models.h"
#include "half.h"

#define idv(x, y, z, e) ((width*height*(z)+width*(y)+(x))*3+(e))

ParticleSystem::ParticleSystem(Shader *shader, int width, int height, int depth) :
    Renderable(NULL, shader, GL_LINES), width(width), height(height), depth(depth) {
    geometry = loadWireCube(1.0, 1.0, 1.0);
    velocityBuffer = new float[width*height*depth*3];
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
    for (unsigned int i = 0; i < particles.size(); i += 2) {
        float scale = 1.0;
        // constrain to bounds? // may be more complicated with fluid domain rotating
        //keep it in a unit sphere for now
        Vector3 p = particles[i];
        std::cout << lifes.size() << std::endl;
        if (p.x > 1.0 || p.y > 1.0 || p.z > 1.0 || p.x < 0.0 || p.y < 0.0 || p.z < 0.0 || lifes[i/2] < 0.0) {
            p.x = 1.0*rand()/RAND_MAX;
            p.y = 1.0*rand()/RAND_MAX;
            p.z = 1.0*rand()/RAND_MAX;
            lifes[i/2] = 1.0;
        }
        //std::cout << p.x << std::endl;
        if (p.x > 1.0) {
            p.x = 1.0;
        }
        if (p.y > 1.0) {
            p.y = 1.0;
        }
        if (p.z > 1.0) {
            p.z = 1.0;
        }
        if (p.x < 0.0) {
            p.x = 0.0;
        }
        if (p.y < 0.0) {
            p.y = 0.0;
        }
        if (p.z < 0.0) {
            p.z = 0.0;
        }

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

        particles[i] = p + vel * dt * scale; // normalize all for coool stuff

    }
    glLineWidth(1.0);
}
