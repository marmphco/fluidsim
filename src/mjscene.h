/*
    mjscene.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#ifndef MJ_SCENE_H
#define MJ_SCENE_H

#include <vector>
#include "mjutil.h"
#include "mjlight.h"
#include "mjrenderable.h"
#include "mjcamera.h"

namespace mcjee {

    class Scene {
    private:
        std::vector<Light *> lights;
        std::vector<Renderable *> renderables;
    public:
        static const int MAX_LIGHTS = 8;
        Vector3 ambientColor;
        float diffuseMultiplier;
        float specularMultiplier;

        Camera camera;
        Scene();
        ~Scene();
        void add(Renderable *);
        void add(Light *);
        //void remove();
        void deleteMembers();
        void render();
    };

}

#endif
