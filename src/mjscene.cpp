/*
    mjscene.cpp
*/

#include "mjscene.h"

namespace mcjee {

    Scene::Scene() {

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
            lightDiffuseColors[i] = lights[i]->diffuseColor;
            lightSpecularColors[i] = lights[i]->specularColor;
        }

        // render objects
        std::vector<Renderable *>::iterator j;
        for (j = renderables.begin(); j != renderables.end(); ++j) {
            Renderable &object = **j;
            Shader &shader = *object.shader;
            shader.use();
            // matrix uniforms
            if (shader.projectionMatrixEnabled()) {
                GLint pLoc = shader.getUniformLocation("projectionMatrix");
                glUniformMatrix4fv(pLoc, 1, GL_FALSE, camera.projectionMatrix().data);
            }
            if (shader.viewMatrixEnabled()) {
                GLint vLoc = shader.getUniformLocation("viewMatrix");
                glUniformMatrix4fv(vLoc, 1, GL_FALSE, camera.viewMatrix().data);
            }
            //light uniforms
            if (shader.ambientEnabled()) {
                GLint lightAmbLoc = shader.getUniformLocation("ambientColor");
                glUniform3fv(lightAmbLoc, 1, (const GLfloat *)&ambientColor);
            }
            if (shader.lightEnabled()) {
                GLint lightPosLoc = shader.getUniformLocation("lightPositions");
                glUniform3fv(lightPosLoc, MAX_LIGHTS, (const GLfloat *)lightPositions);
            }
            if (shader.diffuseEnabled()) {
                GLint lightDifLoc = shader.getUniformLocation("lightDiffuseColors");
                glUniform3fv(lightDifLoc, MAX_LIGHTS, (const GLfloat *)lightDiffuseColors);
            }
            if (shader.specularEnabled()) {
                GLint lightSpeLoc = shader.getUniformLocation("lightSpecularColors");
                glUniform3fv(lightSpeLoc, MAX_LIGHTS, (const GLfloat *)lightSpecularColors);
            }
            object.render();
        }
    }

}
