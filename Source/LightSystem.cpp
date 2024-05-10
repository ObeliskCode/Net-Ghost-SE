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

void LightSystem::linkShader(Shader sh){
    sh.Activate();
    int lightNo = lights.size();
    glUniform1i(glGetUniformLocation(sh.ID, "NR_POINT_LIGHTS"), lightNo);
    for (int i = 0; i < lights.size(); i++) {
        std::string arrStr = "pointLights[" + std::to_string(i) + "]";
        glUniform3f(glGetUniformLocation(sh.ID, (arrStr+".position").c_str()), lights[i]->lightPos.x, lights[i]->lightPos.y, lights[i]->lightPos.z);
        glUniform3f(glGetUniformLocation(sh.ID, (arrStr+".ambient").c_str()), lights[i]->ambient.x, lights[i]->ambient.y, lights[i]->ambient.z);
        glUniform3f(glGetUniformLocation(sh.ID, (arrStr+".diffuse").c_str()), lights[i]->diffuse.x, lights[i]->diffuse.y, lights[i]->diffuse.z);
        glUniform3f(glGetUniformLocation(sh.ID, (arrStr+".specular").c_str()), lights[i]->specular.x, lights[i]->specular.y, lights[i]->specular.z);
        glUniform1f(glGetUniformLocation(sh.ID, (arrStr+".constant").c_str()), lights[i]->constant);
        glUniform1f(glGetUniformLocation(sh.ID, (arrStr+".linear").c_str()), lights[i]->linear);
        glUniform1f(glGetUniformLocation(sh.ID, (arrStr+".quadratic").c_str()), lights[i]->quadratic);

        glActiveTexture(GL_TEXTURE0 + 7 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i]->lightShadow->getMap());
        glUniform1i(glGetUniformLocation(sh.ID, ("shadowCubeMap[" + std::to_string(i) + "]").c_str()), 7 + i);
    }
}

