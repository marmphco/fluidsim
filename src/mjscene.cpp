/*
    mjscene.cpp
*/

#include "mjscene.h"

namespace mcjee {

    Scene::Scene(Framebuffer *framebuffer) :
        framebuffer(framebuffer),
        ambientColor(Vector3()),
        diffuseMultiplier(1.0),
        specularMultiplier(1.0),
        clearEnabled(true), //must be manually disabled
        blendEnabled(false),
        sFactorRGB(GL_SRC_ALPHA),
        dFactorRGB(GL_ONE_MINUS_SRC_ALPHA),
        sFactorA(GL_SRC_ALPHA),
        dFactorA(GL_ONE_MINUS_SRC_ALPHA),
        blendEquationRGB(GL_FUNC_ADD),
        blendEquationA(GL_FUNC_ADD) {
    }

    Scene::~Scene() {
        // delete all objects?
    }

    void Scene::add(Renderable *object) {
        renderables.push_back(object);
    }

    void Scene::add(Light *light) {
        lights.push_back(light);
    }

    void Scene::deleteMembers() {
        // delete all data held in renderables and lights
    }

    void Scene::render() {
        if (clearEnabled) {
            framebuffer->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        framebuffer->bind();
        if (blendEnabled) {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(blendEquationRGB, blendEquationA);
            glBlendFuncSeparate(sFactorRGB, dFactorRGB, sFactorA, dFactorA);
        } else {
            glDisable(GL_BLEND);
        }

        // create light arrays
        Vector3 lightPositions[MAX_LIGHTS];
        Vector3 lightDiffuseColors[MAX_LIGHTS];
        Vector3 lightSpecularColors[MAX_LIGHTS];
        for (unsigned int i = 0; i < MAX_LIGHTS && i < lights.size(); ++i) {
            lightPositions[i] = lights[i]->position;
            lightDiffuseColors[i] = lights[i]->diffuseColor*diffuseMultiplier;
            lightSpecularColors[i] = lights[i]->specularColor*specularMultiplier;
        }

        // render objects
        for (unsigned int i = 0; i != renderables.size(); ++i) {
            Renderable &object = *renderables[i];
            Shader &shader = *object.shader;
            shader.use();
            // matrix uniforms
            shader.setUniformMatrix4fv("projectionMatrix", 1, GL_FALSE, camera.projectionMatrix().data);
            shader.setUniformMatrix4fv("viewMatrix", 1, GL_FALSE, camera.viewMatrix().data);
            shader.setUniformMatrix4fv("inverseViewMatrix", 1, GL_FALSE, camera.inverseViewMatrix().data);
            //light uniforms
            shader.setUniform3fv("ambientColor", 1, (const GLfloat *)&ambientColor);
            shader.setUniform3fv("lightPositions", MAX_LIGHTS, (const GLfloat *)&lightPositions);
            shader.setUniform3fv("lightDiffuseColors", MAX_LIGHTS, (const GLfloat *)lightDiffuseColors);
            shader.setUniform3fv("lightSpecularColors", MAX_LIGHTS, (const GLfloat *)lightSpecularColors);
            shader.setUniform1i("objectID", i+1);
            object.render();
        }
        framebuffer->unbind();
    }

    Renderable *Scene::pickObject(int x, int y) {
        framebuffer->bind();
        unsigned int id;
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &id);
        glReadBuffer(GL_NONE);
        framebuffer->unbind();
        id = (id & 0xff)-1;
        if (id < renderables.size()) {
            return renderables[id];
        } else {
            return NULL;
        }
        framebuffer->unbind();
    }
}
