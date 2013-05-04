/*
    mjscene.cpp
*/

#include "mjscene.h"

namespace mcjee {

    Scene::Scene() : diffuseMultiplier(1.0), specularMultiplier(1.0 ){

    }

    Scene::~Scene() {
        // delete all objects
    }

    void Scene::add(Renderable *object) {
        renderables.push_back(object);
    }

    void Scene::add(Light *light) {
        lights.push_back(light);
    }

    void Scene::deleteMembers() {

    }

    void Scene::render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        std::vector<Renderable *>::iterator j;
        for (j = renderables.begin(); j != renderables.end(); ++j) {
            Renderable &object = **j;
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
            object.render();
        }
    }

}
