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
#include "mjframebuffer.h"

namespace mcjee {

    class Scene {
    private:
        
    public:
        static const unsigned int MAX_LIGHTS = 8;
        std::vector<Light *> lights;
        std::vector<Renderable *> renderables;

        Framebuffer *framebuffer;
        Vector3 ambientColor;
        float diffuseMultiplier;
        float specularMultiplier;
        Camera camera;

        Vector4 backgroundColor;
        bool blendEnabled;
        GLenum sFactor;
        GLenum dFactor;

        Scene(Framebuffer *);
        ~Scene();
        void add(Renderable *);
        void add(Light *);
        //void remove();
        void deleteMembers();
        void render();

        void eachRenderable(void (*)(Renderable *));
        void eachLight();

        Renderable *pickObject(int x, int y);
    };
}

#endif
