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
        backgroundColor(Vector4()),
        clearEnabled(true), //must be manuaklly disabled
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
            if (shader.uniformEnabled("projectionMatrix")) {
                GLint pLoc = shader.getUniformLocation("projectionMatrix");
                glUniformMatrix4fv(pLoc, 1, GL_FALSE, camera.projectionMatrix().data);
            }
            if (shader.uniformEnabled("viewMatrix")) {
                GLint vLoc = shader.getUniformLocation("viewMatrix");
                glUniformMatrix4fv(vLoc, 1, GL_FALSE, camera.viewMatrix().data);
            }
            if (shader.uniformEnabled("inverseViewMatrix")) {
                GLint vLoc = shader.getUniformLocation("inverseViewMatrix");
                glUniformMatrix4fv(vLoc, 1, GL_FALSE, camera.inverseViewMatrix().data);
            }
            //light uniforms
            if (shader.uniformEnabled("ambientColor")) {
                GLint lightAmbLoc = shader.getUniformLocation("ambientColor");
                glUniform3fv(lightAmbLoc, 1, (const GLfloat *)&ambientColor);
            }
            if (shader.uniformEnabled("lightPositions")) {
                GLint lightPosLoc = shader.getUniformLocation("lightPositions");
                glUniform3fv(lightPosLoc, MAX_LIGHTS, (const GLfloat *)lightPositions);
            }
            if (shader.uniformEnabled("lightDiffuseColors")) {
                GLint lightDifLoc = shader.getUniformLocation("lightDiffuseColors");
                glUniform3fv(lightDifLoc, MAX_LIGHTS, (const GLfloat *)lightDiffuseColors);
            }
            if (shader.uniformEnabled("lightSpecularColors")) {
                GLint lightSpeLoc = shader.getUniformLocation("lightSpecularColors");
                glUniform3fv(lightSpeLoc, MAX_LIGHTS, (const GLfloat *)lightSpecularColors);
            }
            //object ID
            if (shader.uniformEnabled("objectID")) {
                GLint loc = shader.getUniformLocation("objectID");
                glUniform1i(loc, i+1);
            }
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
