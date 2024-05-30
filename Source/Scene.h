#ifndef SCENE_H
#define SCENE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <math.h>
#include <cmath>

#include "stb_image.h"

#include "btBulletDynamicsCommon.h"

#include "Globals.h"
#include "Shader.h"
#include "Audio.h"
#include "GUI.h"
#include "ECS.h"
#include "Entity.h"
#include "Camera.h"
#include "Model.h"
#include "Wire.h"
#include "Particle.h"
#include "Physics.h"
#include "Skybox.h"
#include "SkeletalModel.h"
#include "Skeleton.h"
#include "Light.h"
#include "Animator.h"
#include "Input.h"
#include "ParticleRenderer.h"
#include "LightSystem.h"
#include "ParticleSystem.h"
#include "Transform.h"
#include "QuadRenderer.h"

const double PI = 3.1415926535897932384626433832795028841971693993751058209;

// todo move to particle class?
struct {
	bool operator()(Particle a, Particle b) const { return glm::length(Globals::get().camera->getPosition() - a.getTranslation()) > glm::length(Globals::get().camera->getPosition() - b.getTranslation()); }
} Less;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

glm::quat quat_axis_angle(const double& xx, const double& yy, const double& zz, const double& a)
{
    // Here we calculate the sin( theta / 2) once for optimization
    double factor = sin(a / 2.0);

    // Calculate the x, y and z of the quaternion
    double x = xx * factor;
    double y = yy * factor;
    double z = zz * factor;

    // Calcualte the w value by cos( theta / 2 )
    double w = cos(a / 2.0);

    return glm::quat(w, x, y, z);
}

void linkModelShaderUniforms(Shader& shader) {
    shader.Activate();

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(Globals::get().lightSpaceMatrix));

    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_2D, Globals::get().depthMap);
    glUniform1i(glGetUniformLocation(shader.ID, "shadowMap"), 6);

    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Globals::get().depthCubeMap);
    glUniform1i(glGetUniformLocation(shader.ID, "shadowCubeMap"), 5);

    glUniform1f(glGetUniformLocation(shader.ID, "far_plane"), Globals::get().far_plane);
}

class Scene {
    public:
        virtual int loadResources(GLFWwindow* window) = 0;
        virtual int tick(GLFWwindow* window) = 0;
        virtual int drawFrame(GLFWwindow* window, double frameTime) = 0;
        virtual int cleanup() = 0;

        static void renderScene() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        void setupCallbacks(GLFWwindow* window) {
            glfwSetFramebufferSizeCallback(window, winFun);

            glfwSetKeyCallback(window, keyFun);

            glfwSetCursorPosCallback(window, curFun);
        }

        void (*winFun)(GLFWwindow*, int, int);
        void (*keyFun)(GLFWwindow*, int, int, int, int);
        void (*curFun)(GLFWwindow*, double, double);

        double delta = 1.0 / 64.0;

    private:
};

class MainMenu : public Scene {
    public:

        Shader* textProgram;

        MainMenu() {
            winFun = [](GLFWwindow* window, int width, int height) {
                // Define the portion of the window used for OpenGL rendering.
                glViewport(0, 0, width, height);
                Globals::get().screenWidth = width == 0 ? 1 : width;
                Globals::get().screenHeight = height == 0 ? 1 : height;

                Globals::get().camera->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

                Globals::get().handCam->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

                };

            keyFun = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (action == GLFW_RELEASE) {
                    Input::get().setValue(key, false);
                    return;
                }

                Input::get().setValue(key, true);
                switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_F10:
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
                    break;
                case GLFW_KEY_F9:
                    glfwSetWindowMonitor(window, NULL, 100, 100, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
                    break;
                }
                };

            curFun = [](GLFWwindow* window, double xpos, double ypos) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                };
        }

        int loadResources(GLFWwindow* window) override {
            ECS::get();
            GUI::get();
            Globals::get();
            LightSystem::get();
            ParticleSystem::get();
            Physics::get();
            Input::get();
            Audio::get();

            textProgram = new Shader("textVert.glsl", "textFrag.glsl");

            return 1;
        }

        int tick(GLFWwindow* window) override {
            return 1;
        }

        int drawFrame(GLFWwindow* window, double frameTime) override {
            renderScene();

            glEnable(GL_BLEND);
            GUI::get().RenderText(*textProgram, "Obelisk Engine", (Globals::get().screenWidth / 2) - 150.0f, Globals::get().screenHeight - (Globals::get().screenHeight / 10), 0.75f, glm::vec3(1.f, 1.f, 1.f));
            GUI::get().RenderText(*textProgram, "Test Room", (Globals::get().screenWidth / 2) - 150.0f, Globals::get().screenHeight - (Globals::get().screenHeight / 6), 0.75f, glm::vec3(1.f, 1.f, 1.f));
            glDisable(GL_BLEND);

            glfwSwapBuffers(window);
            return 1;
        }

        int cleanup() override {
            // careful with these! not well written![BROKEN ATM]
            LightSystem::destruct();
            ParticleSystem::destruct();
            Audio::destruct();
            GUI::destruct();
            ECS::destruct();
            Physics::destruct();
            Globals::destruct();
            Input::destruct();
            return 1;
        }

    private:
};

class FoldAnim : public Scene {
    public:

        QuadRenderer* quadSys = new QuadRenderer();

        Shader* quadProgram;

        FoldAnim() {
            winFun = [](GLFWwindow* window, int width, int height) {
                // Define the portion of the window used for OpenGL rendering.
                glViewport(0, 0, width, height);
                Globals::get().screenWidth = width == 0 ? 1 : width;
                Globals::get().screenHeight = height == 0 ? 1 : height;

                Globals::get().camera->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

                Globals::get().handCam->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

                };

            keyFun = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (action == GLFW_RELEASE) {
                    Input::get().setValue(key, false);
                    return;
                }

                Input::get().setValue(key, true);
                switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_X:
                    Globals::get().cursorLocked = !Globals::get().cursorLocked;
                    break;
                case GLFW_KEY_F10:
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
                    break;
                case GLFW_KEY_F9:
                    glfwSetWindowMonitor(window, NULL, 100, 100, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
                    break;
                }
                };

            curFun = [](GLFWwindow* window, double xpos, double ypos) {
                if (Globals::get().cursorLocked == true) {

                    float sensitivity = 1.0f; // mouse sens

                    double deltaAngle = 0.0005; // base angle we move in

                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                    double xMiddle = (double)Globals::get().screenWidth / 2.0;
                    double yMiddle = (double)Globals::get().screenHeight / 2.0;

                    double xOffset = xMiddle - xpos;
                    double yOffset = yMiddle - ypos;

                    Globals::get().rotX += xOffset * sensitivity * deltaAngle;
                    Globals::get().rotY += yOffset * sensitivity * deltaAngle;

                    glfwSetCursorPos(window, xMiddle, yMiddle);
                }
                else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                };
        }

        int loadResources(GLFWwindow* window) override {
            ECS::get();
            GUI::get();
            Globals::get();
            LightSystem::get();
            ParticleSystem::get();
            Physics::get();
            Input::get();
            Audio::get();

            quadProgram = new Shader("quadVert.glsl", "quadFrag.glsl");

            Quad q = Quad();
            // multiply first rot last when multiplying quats!
            glm::quat rot1 = quat_axis_angle(1.0, 0.0, 0.0, PI / 2);
            glm::quat rot2 = quat_axis_angle(0.0, 1.0, 0.0, PI / 4);
            glm::quat originQuat = rot2 * rot1;

            q.t.setRotation(originQuat);
            quadSys->quads.push_back(q);

            Globals::get().camera->setPosition(glm::vec3(0.0f, 10.0f, 20.0f));
            Globals::get().camera->setOrientation(glm::normalize(glm::vec3(0.0f, -10.0f, -20.0f)));

            return 1;
        }

        int tick(GLFWwindow* window) override {
            {
                Globals::get().camera->setOrientation(glm::rotate(Globals::get().camera->getOrientation(), (float)Globals::get().rotX, Globals::get().camera->getUp()));

                glm::vec3 perpendicular = glm::normalize(glm::cross(Globals::get().camera->getOrientation(), Globals::get().camera->getUp()));
                // Clamps rotY so it doesn't glitch when looking directly up or down
                if (!((Globals::get().rotY > 0 && Globals::get().camera->getOrientation().y > 0.99f) || (Globals::get().rotY < 0 && Globals::get().camera->getOrientation().y < -0.99f)))
                    Globals::get().camera->setOrientation(glm::rotate(Globals::get().camera->getOrientation(), (float)Globals::get().rotY, perpendicular));

                Globals::get().rotX = 0.0;
                Globals::get().rotY = 0.0;
            }
            { // update cam pos
                if (!Globals::get().camLock) {
                    glm::vec3 proj = glm::rotate(Globals::get().camera->getOrientation(), glm::radians(90.0f), Globals::get().camera->getUp());
                    proj.y = 0.0f;
                    proj = glm::normalize(proj);

                    float moveSpeed = 1.0f; // position of camera move speed
                    float order = 10.0f;

                    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

                    if (Input::get().getValue(GLFW_KEY_W)) {
                        velocity += Globals::get().camera->getOrientation();
                    }
                    if (Input::get().getValue(GLFW_KEY_A)) {
                        velocity += proj;
                    }
                    if (Input::get().getValue(GLFW_KEY_S)) {
                        velocity -= Globals::get().camera->getOrientation();
                    }
                    if (Input::get().getValue(GLFW_KEY_D)) {
                        velocity -= proj;
                    }

                    if (glm::length(velocity) > 0) {
                        velocity = glm::normalize(velocity);

                        velocity.x *= delta * order * moveSpeed;
                        velocity.y *= delta * order * moveSpeed;
                        velocity.z *= delta * order * moveSpeed;
                    }

                    Globals::get().camera->setPosition(Globals::get().camera->getPosition() + velocity);
                }
            }
            return 1;
        }

        int drawFrame(GLFWwindow* window, double frameTime) override {
            renderScene();

            glDisable(GL_CULL_FACE);
            quadSys->DrawQuads(*quadProgram, *Globals::get().camera);
            glEnable(GL_CULL_FACE);

            glfwSwapBuffers(window);
            return 1;
        }

        int cleanup() override {
            // careful with these! not well written![BROKEN ATM]
            LightSystem::destruct();
            ParticleSystem::destruct(); //questioning this one..
            Audio::destruct();
            GUI::destruct();
            ECS::destruct();
            Physics::destruct();
            Globals::destruct();
            Input::destruct(); // do i ever need to destruct this one?
            return 1;
        }

    private:
};

class TestRoom : public Scene {
    public:
        double batRot = 0.0f;
        float bf = 0.0f;

        unsigned int cigCt = 1;

        unsigned int prevID = 0;

        bool cig_anim = false;
        bool is_smoking = false;
        float cig_anim_time = 0.0f;

        int cd = 0;

        unsigned int batEntID;
        unsigned int cigEntID;
        unsigned int characterID;

        unsigned int depthMapFBO;
        unsigned int depthMap;

        Skybox* sky;
        Shader* skyProgram;

        ParticleRenderer particleEmitter;

        QuadRenderer* quadSys = new QuadRenderer();

        Shader* textProgram;

        Shader* partProgram;

        Shader* quadProgram;

        Animator* mator;


        TestRoom(){
            winFun = [](GLFWwindow* window, int width, int height) {
                // Define the portion of the window used for OpenGL rendering.
                glViewport(0, 0, width, height);
                Globals::get().screenWidth = width == 0 ? 1 : width;
                Globals::get().screenHeight = height == 0 ? 1 : height;

                Globals::get().camera->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

                Globals::get().handCam->setDims(Globals::get().screenWidth, Globals::get().screenHeight);

                };

            keyFun = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (action == GLFW_RELEASE) {
                    Input::get().setValue(key, false);
                    return;
                }

                Input::get().setValue(key, true);
                switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_X:
                    Globals::get().cursorLocked = !Globals::get().cursorLocked;
                    break;
                case GLFW_KEY_Z:
                    Globals::get().camLock = !Globals::get().camLock;
                    break;
                case GLFW_KEY_O:
                    Globals::get().drawWires = !Globals::get().drawWires;
                    break;
                case GLFW_KEY_F10:
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
                    break;
                case GLFW_KEY_F9:
                    glfwSetWindowMonitor(window, NULL, 100, 100, Globals::get().screenWidth, Globals::get().screenHeight, GLFW_DONT_CARE);
                    break;
                }
                };

            curFun = [](GLFWwindow* window, double xpos, double ypos) {
                if (Globals::get().cursorLocked == true) {

                    float sensitivity = 1.0f; // mouse sens

                    double deltaAngle = 0.0005; // base angle we move in

                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                    double xMiddle = (double)Globals::get().screenWidth / 2.0;
                    double yMiddle = (double)Globals::get().screenHeight / 2.0;

                    double xOffset = xMiddle - xpos;
                    double yOffset = yMiddle - ypos;

                    Globals::get().rotX += xOffset * sensitivity * deltaAngle;
                    Globals::get().rotY += yOffset * sensitivity * deltaAngle;

                    glfwSetCursorPos(window, xMiddle, yMiddle);
                }
                else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                };
        };


        int loadResources(GLFWwindow* window) override {
            ECS::get();
            GUI::get();
            Globals::get();
            LightSystem::get();
            ParticleSystem::get();
            Physics::get();
            Input::get();
            Audio::get();

            textProgram = new Shader("textVert.glsl", "textFrag.glsl");

            renderScene();

            glEnable(GL_BLEND);
            GUI::get().RenderText(*textProgram, "Loading...", (Globals::get().screenWidth / 2) - 100.0f, Globals::get().screenHeight / 2, 1.0f, glm::vec3(1.f, 1.f, 1.f));
            glDisable(GL_BLEND);

            glfwSwapBuffers(window);

            quadProgram = new Shader("quadVert.glsl", "quadFrag.glsl");

            Quad q = Quad();
            quadSys->quads.push_back(q);

            // warning these need to be deleted!
            Globals::get().rigProgram = new Shader("rigVert.glsl", "mdlFrag.glsl");
            Globals::get().lightProgram = new Shader("rigVert.glsl", "lightFrag.glsl");
            Globals::get().animProgram = new Shader("animVert.glsl", "mdlFrag.glsl");
            Globals::get().noTexAnimProgram = new Shader("animVert.glsl", "noTexFrag.glsl");
            Globals::get().wireShader = new Shader("wireVert.glsl", "wireFrag.glsl");
            Globals::get().shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");
            Globals::get().animShadowShader = new Shader("animShadowVert.glsl", "shadowFrag.glsl");
            Globals::get().cellShader = new Shader("rigVert.glsl", "cellFrag.glsl");
            Globals::get().animCellShader = new Shader("animVert.glsl", "cellFrag.glsl");
            Globals::get().pointShadowShader = new Shader("pointShadowVert.glsl", "pointShadowFrag.glsl", "pointShadowGeom.glsl");
            Globals::get().animPointShadowShader = new Shader("animPointShadowVert.glsl", "pointShadowFrag.glsl", "pointShadowGeom.glsl");

            unsigned int entID;
            Entity e;
            Transform* trf;

            trf = new Transform();
            Model* panel = new Model("floor/floor.dae");
            entID = ECS::get().createEntity();
            trf->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));
            trf->setScale(glm::vec3(20.0f));
            ECS::get().addModel(entID, panel);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            e = ECS::get().getEntity(entID);
            e.surface_flag = 1;
            ECS::get().updateEntity(e);

            trf = new Transform();
            Model* lamp = new Model("lamp/lamp.dae");
            entID = ECS::get().createEntity();
            trf->setScale(glm::vec3(0.05f));
            trf->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));
            ECS::get().addModel(entID, lamp);
            ECS::get().addShader(entID, Globals::get().lightProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            ECS::get().addPhysBody(entID, Physics::get().addUnitBoxStaticBody(entID, 2.0f, 5.0f, 2.0f, -20.0f, 10.0f - 0.1f, 28.0f));
            ECS::get().addPhysTransform(entID, new Transform());
            ECS::get().addWireFrame(entID, 2.0f, 5.0f, 2.0f);
            e = ECS::get().getEntity(entID);
            e.light_flag = 1;
            ECS::get().updateEntity(e);

            trf = new Transform();
            Model* bench = new Model("bench/bench.dae");
            float CosHalfPi = sqrt(2.f) / 2.f;
            entID = ECS::get().createEntity();
            trf->setScale(glm::vec3(4.5f));
            trf->setTranslation(glm::vec3(0.0f, -2.5f, 0.0f));
            trf->setRotation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
            ECS::get().addModel(entID, bench);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            ECS::get().addPhysBody(entID, Physics::get().addUnitBoxStaticBody(entID, 2.0f, 2.5f, 5.0f, 5.0f, 5.0f + 2.5f, 28.f));
            ECS::get().addPhysTransform(entID, new Transform());
            ECS::get().addWireFrame(entID, 2.0f, 2.5f, 5.0f);

            trf = new Transform();
            Model* handBat = new Model("bat/bat.dae");
            entID = ECS::get().createEntity();
            batEntID = entID;
            trf->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
            trf->setTranslation(glm::vec3(2.0f, -2.0f, 0.0f));
            ECS::get().addModel(entID, handBat);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().handCam);
            ECS::get().addTransform(entID, trf);

            trf = new Transform();
            Model* cig = new Model("cig/cig.dae");
            entID = ECS::get().createEntity();
            cigEntID = entID;
            trf->setTranslation(glm::vec3(0.0f, -2.0f, -1.0f));
            trf->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
            ECS::get().addModel(entID, cig);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().handCam);
            ECS::get().addTransform(entID, trf);

            trf = new Transform();
            Model* dumpster = new Model("dumpster/dumpster.dae");
            entID = ECS::get().createEntity();
            trf->setScale(glm::vec3(6.5f));
            trf->setTranslation(glm::vec3(0.0f, -4.0f, 0.0f));
            trf->setRotation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
            ECS::get().addModel(entID, dumpster);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            ECS::get().addPhysBody(entID, Physics::get().addUnitBoxStaticBody(entID, 3.0f, 4.0f, 6.0f, 25.0f, 5.0f + 4.0f, 35.f));
            ECS::get().addPhysTransform(entID, new Transform());
            ECS::get().addWireFrame(entID, 3.0f, 4.0f, 6.0f);

            trf = new Transform();
            entID = ECS::get().createEntity();
            Model* cart = new Model("cart/ShoppingCart.dae");
            trf->setTranslation(glm::vec3(20.0f, 5.0f, 20.0f));
            trf->setScale(glm::vec3(7.f));
            ECS::get().addModel(entID, cart);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);

            trf = new Transform();
            entID = ECS::get().createEntity();
            Model* tent = new Model("tent/tent.dae");
            trf->setScale(glm::vec3(7.f));
            trf->setTranslation(glm::vec3(0.0f, -4.0f, 0.0f));
            ECS::get().addModel(entID, tent);
            ECS::get().addShader(entID, Globals::get().rigProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            ECS::get().addPhysBody(entID, Physics::get().addUnitBoxStaticBody(entID, 6.0f, 4.0f, 6.0f, -15.0f, 5.0f + 4.0f, 10.0f));
            ECS::get().addPhysTransform(entID, new Transform());
            ECS::get().addWireFrame(entID, 6.0f, 4.0f, 6.0f);


            trf = new Transform();
            entID = ECS::get().createEntity();
            SkeletalModel* wolf = new SkeletalModel("wolf/Wolf_One_dae.dae");
            Skeleton* wolfAnimation = new Skeleton("wolf/Wolf_One_dae.dae", wolf);
            Animator* animator = new Animator(wolfAnimation);
            trf->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
            trf->setTranslation(glm::vec3(10.0f, 5.0f, 10.0f));
            ECS::get().addSkModel(entID, wolf);
            ECS::get().addAnimator(entID, animator);
            ECS::get().addShader(entID, Globals::get().noTexAnimProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);


            trf = new Transform();
            entID = ECS::get().createEntity();
            SkeletalModel* sit = new SkeletalModel("sit/Sitting Clap.dae");
            Skeleton* sitAnimation = new Skeleton("sit/Sitting Clap.dae", sit);
            Animator* sitMator = new Animator(sitAnimation);
            trf->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
            trf->setTranslation(glm::vec3(5.0f, 5.0f, 28.f));
            trf->setRotation(glm::quat(CosHalfPi, 0.0f, -CosHalfPi, 0.0f));
            ECS::get().addSkModel(entID, sit);
            ECS::get().addAnimator(entID, sitMator);
            ECS::get().addShader(entID, Globals::get().animProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);


            trf = new Transform();
            entID = ECS::get().createEntity();
            SkeletalModel* walk = new SkeletalModel("jjong/Idle.dae");
            Skeleton* walkAnimation = new Skeleton("jjong/Idle.dae", walk);
            walkAnimation->addAnimation("jjong/Walking.dae", walk);
            mator = new Animator(walkAnimation);
            mator->QueueAnimation(1);
            trf->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
            trf->setTranslation(glm::vec3(15.0f, 5.0f, -5.0f));
            ECS::get().addSkModel(entID, walk);
            ECS::get().addAnimator(entID, mator);
            ECS::get().addShader(entID, Globals::get().animProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);

            Light* lampLight = new Light(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), glm::vec3(-20.0f, 13.2f, 28.0f));
            LightSystem::get().lights.push_back(lampLight);

            Light* light2 = new Light(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec3(-20.0f, 10.0f, -20.0f));
            LightSystem::get().lights.push_back(light2);


            trf = new Transform();
            entID = ECS::get().createEntity();
            Model* light = new Model("bulb/scene.gltf");
            trf->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
            trf->setTranslation(glm::vec3(-20.0f, 13.2f, 28.0f));
            ECS::get().addModel(entID, light);
            ECS::get().addShader(entID, Globals::get().lightProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            e = ECS::get().getEntity(entID);
            e.light_flag = 1;
            ECS::get().updateEntity(e);

            trf = new Transform();
            entID = ECS::get().createEntity();
            Model* lightSecond = new Model("bulb/scene.gltf");
            trf->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
            trf->setTranslation(glm::vec3(-20.0f, 10.0f, -20.0f));
            ECS::get().addModel(entID, lightSecond);
            ECS::get().addShader(entID, Globals::get().lightProgram);
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addTransform(entID, trf);
            e = ECS::get().getEntity(entID);
            e.light_flag = 1;
            ECS::get().updateEntity(e);

            lampLight->linkShader(*Globals::get().rigProgram);
            lampLight->linkShader(*Globals::get().animProgram);
            lampLight->linkShader(*Globals::get().noTexAnimProgram);
            lampLight->linkShader(*Globals::get().lightProgram);

            LightSystem::get().linkShader(*Globals::get().rigProgram);
            LightSystem::get().linkShader(*Globals::get().animProgram);
            LightSystem::get().linkShader(*Globals::get().noTexAnimProgram);
            LightSystem::get().linkShader(*Globals::get().lightProgram);

            //QUAD
            entID = ECS::get().createEntity();
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addWire(entID, new Wire(glm::vec3(60.0f, 50.0f, -60.0f), glm::vec3(-60.0f, 50.0f, -60.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(-60.0f, 50.0f, -60.0f), glm::vec3(-60.0f, 50.0f, 60.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(-60.0f, 50.0f, 60.0f), glm::vec3(60.0f, 50.0f, 60.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(60.0f, 50.0f, 60.0f), glm::vec3(60.0f, 50.0f, -60.0f)));
            ECS::get().addPhysBody(entID, Physics::get().addShape1(entID));
            ECS::get().addPhysTransform(entID, new Transform());

            //STAGING AXIS
            entID = ECS::get().createEntity();
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addWire(entID, new Wire(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f)));

            //RIGID BODY 1
            entID = ECS::get().createEntity();
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addWire(entID, new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            ECS::get().addPhysBody(entID, Physics::get().addShape2(entID));
             ECS::get().addPhysTransform(entID, new Transform());

            //RIGID BODY 2
            entID = ECS::get().createEntity();
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addWire(entID, new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            ECS::get().addPhysBody(entID, Physics::get().addShape3(entID));
            ECS::get().addPhysTransform(entID, new Transform());

            //CONTROLLABLE BODY
            entID = ECS::get().createEntity();
            characterID = entID;
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addWire(entID, new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            ECS::get().addPhysBody(entID, Physics::get().addShape4(entID));
            ECS::get().addPhysTransform(entID, new Transform());


            // CAPSULE
            entID = ECS::get().createEntity();
            ECS::get().addCamera(entID, Globals::get().camera);
            ECS::get().addWire(entID, new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            ECS::get().addPhysBody(entID, Physics::get().addShape5(entID));
            ECS::get().addPhysTransform(entID, new Transform());


            //TEST CIGS
            for (int i = 0; i < 5; i++) {
                entID = ECS::get().createEntity();
                ECS::get().addModel(entID, new Model("cig/cig.dae"));
                ECS::get().addShader(entID, Globals::get().rigProgram);
                ECS::get().addCamera(entID, Globals::get().camera);
                ECS::get().addTransform(entID, new Transform());
                ECS::get().addWire(entID, new Wire(glm::vec3(-0.06f, 0.0f, 0.0f), glm::vec3(0.06f, 0.0f, 0.0f)));
                ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -0.06f, 0.0f), glm::vec3(0.0f, 0.06f, 0.0f)));
                ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, 0.0f, -0.685f), glm::vec3(0.0f, 0.0f, 0.685f)));
                ECS::get().addPhysBody(entID, Physics::get().addShape6(entID));
                ECS::get().addPhysTransform(entID, new Transform());
                e = ECS::get().getEntity(entID);
                e.pickup_flag = 1;
                ECS::get().updateEntity(e);
            }

            //CROSSHAIR HACK (class required)
            entID = ECS::get().createEntity();
            ECS::get().addCamera(entID, Globals::get().handCam);
            ECS::get().addWire(entID, new Wire(glm::vec3(-0.04f, 0.0f, 0.0f), glm::vec3(0.04f, 0.0f, 0.0f)));
            ECS::get().addWire(entID, new Wire(glm::vec3(0.0f, -0.04f, 0.0f), glm::vec3(0.0f, 0.04f, 0.0f)));

            partProgram = new Shader("partVert.glsl", "partFrag.glsl");

            std::vector<std::string> faces =
            {
                    "models/skybox/right.jpg",
                    "models/skybox/left.jpg",
                    "models/skybox/top.jpg",
                    "models/skybox/bottom.jpg",
                    "models/skybox/front.jpg",
                    "models/skybox/back.jpg"
            };
            sky = new Skybox(faces);


            skyProgram = new Shader("skyVert.glsl", "skyFrag.glsl");


            glGenFramebuffers(1, &depthMapFBO);

            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float clampColor[] = { 1.0f,1.0f,1.0f,1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            Globals::get().depthMap = depthMap;

            float near_plane = 0.1f, far_plane = 200.0f;
            glm::mat4 lightProjection = glm::ortho(-60.0f, 60.0f, -60.0f, 60.0f, near_plane, far_plane);

            Globals::get().far_plane = far_plane;

            glm::mat4 lightView = glm::lookAt(glm::vec3(-4.0f, 10.0f, -6.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 lightSpaceMatrix = lightProjection * lightView;
            Globals::get().lightSpaceMatrix = lightSpaceMatrix;

            Globals::get().shadowShader->Activate();
            glUniformMatrix4fv(glGetUniformLocation(Globals::get().shadowShader->ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            Globals::get().animShadowShader->Activate();
            glUniformMatrix4fv(glGetUniformLocation(Globals::get().animShadowShader->ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            Globals::get().depthCubeMap = lampLight->lightShadow->getMap();

            return 1;
        }


        int tick(GLFWwindow* window) override {
            {
                Globals::get().camera->setOrientation(glm::rotate(Globals::get().camera->getOrientation(), (float)Globals::get().rotX, Globals::get().camera->getUp()));

                glm::vec3 perpendicular = glm::normalize(glm::cross(Globals::get().camera->getOrientation(), Globals::get().camera->getUp()));
                // Clamps rotY so it doesn't glitch when looking directly up or down
                if (!((Globals::get().rotY > 0 && Globals::get().camera->getOrientation().y > 0.99f) || (Globals::get().rotY < 0 && Globals::get().camera->getOrientation().y < -0.99f)))
                    Globals::get().camera->setOrientation(glm::rotate(Globals::get().camera->getOrientation(), (float)Globals::get().rotY, perpendicular));

                Globals::get().rotX = 0.0;
                Globals::get().rotY = 0.0;
            }
            { // character, delta
				if (Globals::get().camLock) {
					btRigidBody* body = character->getBody();

					glm::vec2 camOri = glm::vec2(Globals::get().camera->getOrientation().x, Globals::get().camera->getOrientation().z);

					glm::vec2 proj = glm::rotate(camOri, glm::radians(-90.0f));

					glm::vec2 vel = glm::vec2(0.0f);


					if (Input::get().getValue(GLFW_KEY_W)) {
						vel += camOri;
					}
					if (Input::get().getValue(GLFW_KEY_A)) {
						vel += proj;
					}
					if (Input::get().getValue(GLFW_KEY_S)) {
						vel -= camOri;
					}
					if (Input::get().getValue(GLFW_KEY_D)) {
						vel -= proj;
					}


					if (cd > 0) cd--;
					// code character->canJump()
					if (Input::get().getValue(GLFW_KEY_SPACE)) {
						if (cd == 0) {
							cd = 192;
							body->applyCentralImpulse(btVector3(0.0f, 9.0f, 0.0f));
						}
					}

					float accel;
					accel = delta * 60.0f;

					if (glm::length(vel) > 0) {
						vel = glm::normalize(vel);
						vel *= accel;
					}

					btVector3 linVel = body->getLinearVelocity();
					btVector3 horizVel = btVector3(linVel.getX() + vel.x, 0.0f, linVel.getZ() + vel.y);
					if (horizVel.length() > 12.0f) {
						horizVel = horizVel * (12.0f / horizVel.length());
					}
					linVel = btVector3(horizVel.getX(), linVel.getY(), horizVel.getZ());
					body->setLinearVelocity(linVel);
				}
			} // character.physicsProcess(delta)

			Physics::get().updateSim(delta); // regular time advance

			// move sim forward by delta
			ECS::get().updatePhysics(); // update entities with physics state
			{
				if (Globals::get().camLock) {
					btTransform trans;
					btRigidBody* body = character->getBody();
					body->getMotionState()->getWorldTransform(trans);
					glm::vec3 pos = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()) + 7.0f, float(trans.getOrigin().getZ()));
					Globals::get().camera->setPosition(pos);
				}
			}
			{
				if (Input::get().getValue(GLFW_KEY_RIGHT)) bf += 0.02f;
				if (Input::get().getValue(GLFW_KEY_LEFT)) bf -= 0.02f;
				if (bf < 0.0f) bf = 0.0f;
				if (bf > 1.0f) bf = 1.0f;
				mator->SetBlendFactor(bf);
			}
            { // update cam pos
                if (!Globals::get().camLock) {
                    glm::vec3 proj = glm::rotate(Globals::get().camera->getOrientation(), glm::radians(90.0f), Globals::get().camera->getUp());
                    proj.y = 0.0f;
                    proj = glm::normalize(proj);

                    float moveSpeed = 1.0f; // position of camera move speed
                    float order = 10.0f;

                    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

                    if (Input::get().getValue(GLFW_KEY_W)) {
                        velocity += Globals::get().camera->getOrientation();
                    }
                    if (Input::get().getValue(GLFW_KEY_A)) {
                        velocity += proj;
                    }
                    if (Input::get().getValue(GLFW_KEY_S)) {
                        velocity -= Globals::get().camera->getOrientation();
                    }
                    if (Input::get().getValue(GLFW_KEY_D)) {
                        velocity -= proj;
                    }

                    if (glm::length(velocity) > 0) {
                        velocity = glm::normalize(velocity);

                        velocity.x *= delta * order * moveSpeed;
                        velocity.y *= delta * order * moveSpeed;
                        velocity.z *= delta * order * moveSpeed;
                    }

                    Globals::get().camera->setPosition(Globals::get().camera->getPosition() + velocity);
                }
            }

			{ // do rayCast // raycast selection causes big hitching! how do we optimize?

				glm::vec3 ppp = Globals::get().camera->getPosition() + (Globals::get().camera->getOrientation() * 12.5f);

				btVector3 from = btVector3(Globals::get().camera->getPosition().x, Globals::get().camera->getPosition().y, Globals::get().camera->getPosition().z);
				btVector3 to = btVector3(ppp.x, ppp.y, ppp.z);
				btCollisionWorld::ClosestRayResultCallback rrc = btCollisionWorld::ClosestRayResultCallback(from, to);
				Physics::get().getDynamicsWorld()->rayTest(from, to, rrc);

				if (rrc.hasHit()) {
					btRigidBody* sel = btRigidBody::upcast(const_cast <btCollisionObject*>(rrc.m_collisionObject));
					unsigned int entID = Physics::get().m_EntityMap[sel];
					if (entID != 0) {
						if (prevID != 0 && prevID != entID) {
							Entity* prevEnt = ECS::get().getEntity(prevID);
							if (prevEnt) {
								prevEnt->resetBit(COMPONENT_BIT_STENCIL);
								ECS::get().unregisterComponent(prevEnt, COMPONENT_BIT_STENCIL);
							}
							prevID = 0;
						}
						if (Input::get().getValue(GLFW_KEY_E)) {
							if (ECS::get().getEntity(entID)->getType() == "pickup") {
								// only deletes wires, rigid body & not model / skel model
								cigCt++;
								ECS::get().deleteEntity(entID);
								prevID = 0;

							}
						}
						else if (!ECS::get().getEntity(entID)->m_surface) {
							Entity* selEnt = ECS::get().getEntity(entID);
							if (selEnt) {
								selEnt->setBit(COMPONENT_BIT_STENCIL);
								ECS::get().registerComponent(selEnt, COMPONENT_BIT_STENCIL);
								prevID = entID;
							}
						}
					}
				}
				else if (prevID != 0) {
					Entity* prevEnt = ECS::get().getEntity(prevID);
					if (prevEnt) {
						prevEnt->resetBit(COMPONENT_BIT_STENCIL);
						ECS::get().unregisterComponent(prevEnt, COMPONENT_BIT_STENCIL);
					}
					prevID = 0;
				}

			}
			{
				if (Input::get().getValue(GLFW_KEY_Y)) Audio::get().playAudio(1);
				if (Input::get().getValue(GLFW_KEY_U)) Audio::get().playAudio(0);
			}

			{ // particles
			// UPDATE so that it shows cig first then blows smoke after it dissapears
				if (Input::get().getValue(GLFW_KEY_P) && !is_smoking && cigCt > 0) {
					is_smoking = true;
					cig_anim = true;
					cig_anim_time = 0.0f;
					cigCt--;
				}

				if (is_smoking) {
					cig_anim_time += delta;
					if (cig_anim_time >= 2.0f) cig_anim = false;
					if (cig_anim_time >= 3.0f) is_smoking = false;
				}

				if (cig_anim) {
					cigEnt->m_visible = true;
				}
				else cigEnt->m_visible = false;


				if (cig_anim == false && is_smoking == true) {
					Particle p = Particle();
					p.setTranslation(Globals::get().camera->getPosition() - glm::vec3(0.f, 0.25f, 0.f));
					p.setScale(0.1f);
					p.vel = glm::normalize(Globals::get().camera->getOrientation());
					particleEmitter.particles.push_back(p);

					Particle p2 = Particle();
					p2.setTranslation(Globals::get().camera->getPosition() - glm::vec3(0.f, 0.25f, 0.f));
					p2.setScale(0.1f);
					p2.vel = glm::normalize(Globals::get().camera->getOrientation());
					particleEmitter.particles.push_back(p2);
				}

				// how slow is this? (update/cleanup particles)
				particleEmitter.updateParticles(delta);

				for (int i = 0; i < particleEmitter.particles.size(); i++) {
					if (particleEmitter.particles[i].life >= particleEmitter.particles[i].expire) {
						particleEmitter.particles.erase(particleEmitter.particles.begin() + i);
						i--;
					}
				}

			}
            return 1;
        }


        int drawFrame(GLFWwindow* window, double frameTime) override {
            { // pls do not make this a function
                batRot += frameTime;
                if (batRot > 2 * PI) {
                    batRot = batRot - (2 * PI);
                }
                float q1, q3;
                q1 = cos(batRot / 2);
                q3 = sin(batRot / 2);
                batEnt->transform->setRotation(glm::quat(q1, 0.0f, q3, 0.0f));
            }

            renderScene();

            ECS::get().advanceEntityAnimations(frameTime);

            { // directional shadow draw pass
                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
                glClear(GL_DEPTH_BUFFER_BIT);// clears this framebuffers depth bit!

                ECS::get().DrawEntityShadows();

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glViewport(0, 0, Globals::get().screenWidth, Globals::get().screenHeight);
            }

            LightSystem::get().RenderPointShadows();

            linkModelShaderUniforms(*Globals::get().rigProgram);
            linkModelShaderUniforms(*Globals::get().lightProgram);
            linkModelShaderUniforms(*Globals::get().animProgram);
            linkModelShaderUniforms(*Globals::get().noTexAnimProgram);
            ECS::get().DrawEntities();

            quadSys->DrawQuads(*quadProgram, *Globals::get().camera);

            sky->Draw(*skyProgram, *Globals::get().camera);

            ECS::get().DrawEntityStencils();

            batEnt->Draw(); // fix these so that they have unique lighting!
            cigEnt->Draw();

            glEnable(GL_BLEND);
            std::sort(particleEmitter.particles.begin(), particleEmitter.particles.end(), Less);
            particleEmitter.DrawParticles(*partProgram, *Globals::get().camera);
            glDisable(GL_BLEND);

            glEnable(GL_BLEND);
            GUI::get().RenderText(*textProgram, "Cigarettes: " + std::to_string(cigCt), 25.0f, 25.0f, 1.0f, glm::vec3(1.f, 1.f, 1.f));
            glDisable(GL_BLEND);

            glfwSwapBuffers(window);
            return 1;
        }


        int cleanup() override {
            delete sky;

            // careful with these! not well written![BROKEN ATM]
            LightSystem::destruct();
            ParticleSystem::destruct();
            Audio::destruct();
            GUI::destruct();
            ECS::destruct();
            Physics::destruct();
            Globals::destruct();
            Input::destruct();
            return 1;
        }

    private:
};


#endif
