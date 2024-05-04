#include "LightSystem.h"

LightSystem* LightSystem::instance = nullptr; // definition class variable

void LightSystem::RenderPointShadows(){
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    for (int i = 0; i < lights.size(); i++){
        glBindFramebuffer(GL_FRAMEBUFFER, lights[i]->lightShadow->getFBO());
        glClear(GL_DEPTH_BUFFER_BIT);// clears this framebuffers depth bit!

        lights[i]->lightShadow->linkShadowShaders();

        ECS::get().DrawEntityPointShadows();

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Globals::get().screenWidth, Globals::get().screenHeight);
}

LightSystem::~LightSystem() {
    for (int i = 0; i < lights.size(); i++) {
        delete lights[i];
    }
    lights.clear();
}