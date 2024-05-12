#ifndef SCENE_H
#define SCENE_H

class Scene {
    public:
        virtual int run();
        virtual int loadResources();

        virtual void window_size_callback(GLFWwindow* window, int width, int height);
        virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        virtual void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

    private:
};

class TestRoom : Scene {
    TestRoom() = default;

    int run() override {

    }

    int loadResources() override {
        ECS::get();
        GUI::get();
        Globals::get();
        LightSystem::get();
        ParticleSystem::get();
        Physics::get();
        Input::get();
        Audio::get();

        Shader textProgram = Shader("textVert.glsl", "textFrag.glsl");

        renderScene();

        glEnable(GL_BLEND);
        GUI::get().RenderText(textProgram, "Loading...", (Globals::get().screenWidth/2)-100.0f, Globals::get().screenHeight/2, 1.0f, glm::vec3(1.f, 1.f, 1.f));
        glDisable(GL_BLEND);

        glfwSwapBuffers(window);

        Shader rigProgram = Shader("rigVert.glsl", "mdlFrag.glsl");
        Shader lightProgram = Shader("rigVert.glsl", "lightFrag.glsl");
        Shader animProgram = Shader("animVert.glsl", "mdlFrag.glsl");
        Shader noTexAnimProgram = Shader("animVert.glsl", "noTexFrag.glsl");
        Shader wireProgram = Shader("wireVert.glsl", "wireFrag.glsl");
        Globals::get().wireShader = &wireProgram;
        Shader shadowProgram = Shader("shadowVert.glsl", "shadowFrag.glsl");
        Globals::get().shadowShader = &shadowProgram;
        Shader animShadowProgram = Shader("animShadowVert.glsl", "shadowFrag.glsl");
        Globals::get().animShadowShader = &animShadowProgram;
        Shader cellProgram = Shader("rigVert.glsl", "cellFrag.glsl");
        Globals::get().cellShader = &cellProgram;
        Shader animCellProgram = Shader("animVert.glsl", "cellFrag.glsl");
        Globals::get().animCellShader = &animCellProgram;
        Shader pointShadowProgram = Shader("pointShadowVert.glsl","pointShadowFrag.glsl","pointShadowGeom.glsl");
        Globals::get().pointShadowShader = &pointShadowProgram;
        Shader animPointShadowProgram = Shader("animPointShadowVert.glsl", "pointShadowFrag.glsl", "pointShadowGeom.glsl");
        Globals::get().animPointShadowShader = &animPointShadowProgram;

        Entity* e;

        Model* panel = new Model("floor/floor.dae");
        e = ECS::get().linkEntity(new Entity(panel, &rigProgram, Globals::get().camera));
        e->transform->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));
        e->transform->setScale(glm::vec3(20.0f));
        e->m_surface = true;
        ECS::get().registerComponents(e);

        Model* lamp = new Model("lamp/lamp.dae");
        e = ECS::get().linkEntity(new Entity(lamp, &lightProgram, Globals::get().camera));
        e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 2.0f, 5.0f, 2.0f, -20.0f, 10.0f - 0.1f, 28.0f));
        e->transform->setScale(glm::vec3(0.05f));
        e->transform->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));
        e->addWireFrame(2.0f, 5.0f, 2.0f);
        e->setType("light");
        ECS::get().registerComponents(e);

        Model* bench = new Model("bench/bench.dae");
        float CosHalfPi = sqrt(2.f) / 2.f;
        e = ECS::get().linkEntity(new Entity(bench, &rigProgram, Globals::get().camera));
        e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 2.0f, 2.5f, 5.0f, 5.0f, 5.0f + 2.5f, 28.f)); // whole lot of maths
        e->transform->setScale(glm::vec3(4.5f));
        e->transform->setTranslation(glm::vec3(0.0f, -2.5f, 0.0f));
        e->transform->setRotation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
        e->addWireFrame(2.0f, 2.5f, 5.0f);
        ECS::get().registerComponents(e);

        Model* handBat = new Model("bat/bat.dae");
        Entity* batEnt = new Entity(handBat, &rigProgram, Globals::get().handCam);
        batEnt->transform->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
        batEnt->transform->setTranslation(glm::vec3(2.0f, -2.0f, 0.0f));
        ECS::get().registerComponents(batEnt);

        Model* cig = new Model("cig/cig.dae");
        Entity* cigEnt = new Entity(cig, &rigProgram, Globals::get().handCam);
        cigEnt->transform->setTranslation(glm::vec3(0.0f, -2.0f, -1.0f));
        cigEnt->transform->setRotation(glm::quat(0.0f, 0.0f, 1.0f, 0.0f));
        ECS::get().registerComponents(cigEnt);

        Model* dumpster = new Model("dumpster/dumpster.dae");
        e = ECS::get().linkEntity(new Entity(dumpster, &rigProgram, Globals::get().camera));
        e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 3.0f, 4.0f, 6.0f, 25.0f, 5.0f + 4.0f, 35.f)); // whole lot of maths
        e->transform->setScale(glm::vec3(6.5f));
        e->transform->setTranslation(glm::vec3(0.0f, -4.0f, 0.0f));
        e->transform->setRotation(glm::quat(CosHalfPi, 0.f, -CosHalfPi, 0.f));
        e->addWireFrame(3.0f, 4.0f, 6.0f);
        ECS::get().registerComponents(e);

        Model* cart = new Model("cart/ShoppingCart.dae");
        e = ECS::get().linkEntity(new Entity(cart, &rigProgram, Globals::get().camera));
        e->transform->setTranslation(glm::vec3(20.0f, 5.0f, 20.0f));
        e->transform->setScale(glm::vec3(7.f));
        ECS::get().registerComponents(e);

        Model* tent = new Model("tent/tent.dae");
        e = ECS::get().linkEntity(new Entity(tent, &rigProgram, Globals::get().camera));
        e->addBody(Physics::get().addUnitBoxStaticBody(e->getID(), 6.0f, 4.0f, 6.0f, -15.0f, 5.0f + 4.0f, 10.0f)); // whole lot of maths
        e->transform->setScale(glm::vec3(7.f));
        e->transform->setTranslation(glm::vec3(0.0f, -4.0f, 0.0f));
        e->addWireFrame(6.0f, 4.0f, 6.0f);
        ECS::get().registerComponents(e);

        SkeletalModel* wolf = new SkeletalModel("wolf/Wolf_One_dae.dae");
        Skeleton* wolfAnimation = new Skeleton("wolf/Wolf_One_dae.dae", wolf);
        Animator* animator = new Animator(wolfAnimation);
        e = ECS::get().linkEntity(new Entity(wolf, animator, &noTexAnimProgram, Globals::get().camera));
        e->transform->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
        e->transform->setTranslation(glm::vec3(10.0f, 5.0f, 10.0f));
        ECS::get().registerComponents(e);

        SkeletalModel* sit = new SkeletalModel("sit/Sitting Clap.dae");
        Skeleton* sitAnimation = new Skeleton("sit/Sitting Clap.dae", sit);
        Animator* sitMator = new Animator(sitAnimation);
        e = ECS::get().linkEntity(new Entity(sit, sitMator, &animProgram, Globals::get().camera));
        e->transform->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        e->transform->setTranslation(glm::vec3(5.0f, 5.0f, 28.f));
        e->transform->setRotation(glm::quat(CosHalfPi, 0.0f, -CosHalfPi, 0.0f));
        ECS::get().registerComponents(e);

        SkeletalModel* walk = new SkeletalModel("jjong/Idle.dae");
        Skeleton* walkAnimation = new Skeleton("jjong/Idle.dae", walk);
        walkAnimation->addAnimation("jjong/Walking.dae", walk);
        Animator* mator = new Animator(walkAnimation);
        mator->QueueAnimation(1);
        e = ECS::get().linkEntity(new Entity(walk, mator, &animProgram, Globals::get().camera));
        e->transform->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        e->transform->setTranslation(glm::vec3(15.0f, 5.0f, -5.0f));
        ECS::get().registerComponents(e);

        Light* lampLight = new Light(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), glm::vec3(-20.0f, 13.2f, 28.0f));
        LightSystem::get().lights.push_back(lampLight);

        Light* light2 = new Light(glm::vec4(0.5f,0.5f,0.5f,1.0f), glm::vec3(-20.0f, 10.0f, -20.0f));
        LightSystem::get().lights.push_back(light2);

        Model* light = new Model("bulb/scene.gltf");
        e = ECS::get().linkEntity(new Entity(light, &lightProgram, Globals::get().camera));
        e->transform->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        e->transform->setTranslation(glm::vec3(-20.0f, 13.2f, 28.0f));
        e->setType("light");
        ECS::get().registerComponents(e);

        Model* lightSecond = new Model("bulb/scene.gltf");
        e = ECS::get().linkEntity(new Entity(lightSecond, &lightProgram, Globals::get().camera));
        e->transform->setScale(glm::vec3(5.0f, 5.0f, 5.0f));
        e->transform->setTranslation(glm::vec3(-20.0f, 10.0f, -20.0f));
        e->setType("light");
        ECS::get().registerComponents(e);

        lampLight->linkShader(rigProgram);
        lampLight->linkShader(animProgram);
        lampLight->linkShader(noTexAnimProgram);
        lampLight->linkShader(lightProgram);

        LightSystem::get().linkShader(rigProgram);
        LightSystem::get().linkShader(animProgram);
        LightSystem::get().linkShader(noTexAnimProgram);
        LightSystem::get().linkShader(lightProgram);

        //QUAD
        e = ECS::get().linkEntity(new Entity(Globals::get().camera));
        e->addWire(new Wire(glm::vec3(60.0f, 50.0f, -60.0f), glm::vec3(-60.0f, 50.0f, -60.0f)));
        e->addWire(new Wire(glm::vec3(-60.0f, 50.0f, -60.0f), glm::vec3(-60.0f, 50.0f, 60.0f)));
        e->addWire(new Wire(glm::vec3(-60.0f, 50.0f, 60.0f), glm::vec3(60.0f, 50.0f, 60.0f)));
        e->addWire(new Wire(glm::vec3(60.0f, 50.0f, 60.0f), glm::vec3(60.0f, 50.0f, -60.0f)));
        e->addBody(Physics::get().addShape1(e->getID()));
        ECS::get().registerComponents(e);

        //STAGING AXIS
        e = ECS::get().linkEntity(new Entity(Globals::get().camera));
        e->addWire(new Wire(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 5.0f)));
        ECS::get().registerComponents(e);

        //RIGID BODY 1
        e = ECS::get().linkEntity(new Entity(Globals::get().camera));
        e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        e->addBody(Physics::get().addShape2(e->getID()));
        ECS::get().registerComponents(e);

        //RIGID BODY 2
        e = ECS::get().linkEntity(new Entity(Globals::get().camera));
        e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        e->addBody(Physics::get().addShape3(e->getID()));
        ECS::get().registerComponents(e);

        //CONTROLLABLE BODY
        Entity* character = ECS::get().linkEntity(new Entity(Globals::get().camera));
        character->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        character->addWire(new Wire(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        character->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        character->addBody(Physics::get().addShape4(character->getID()));
        ECS::get().registerComponents(character);

        // CAPSULE
        e = ECS::get().linkEntity(new Entity(Globals::get().camera));
        e->addWire(new Wire(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        e->addBody(Physics::get().addShape5(e->getID()));
        ECS::get().registerComponents(e);

        //TEST CIGS
        for (int i = 0; i < 5; i++) {
            e = ECS::get().linkEntity(new Entity(new Model("cig/cig.dae"), &rigProgram, Globals::get().camera));
            e->addWire(new Wire(glm::vec3(-0.06f, 0.0f, 0.0f), glm::vec3(0.06f, 0.0f, 0.0f)));
            e->addWire(new Wire(glm::vec3(0.0f, -0.06f, 0.0f), glm::vec3(0.0f, 0.06f, 0.0f)));
            e->addWire(new Wire(glm::vec3(0.0f, 0.0f, -0.685f), glm::vec3(0.0f, 0.0f, 0.685f)));
            e->addBody(Physics::get().addShape6(e->getID()));
            e->setType("pickup");
            ECS::get().registerComponents(e);
        }

        //CROSSHAIR HACK (class required)
        e = ECS::get().linkEntity(new Entity(Globals::get().handCam));
        e->addWire(new Wire(glm::vec3(-0.04f, 0.0f, 0.0f), glm::vec3(0.04f, 0.0f, 0.0f)));
        e->addWire(new Wire(glm::vec3(0.0f, -0.04f, 0.0f), glm::vec3(0.0f, 0.04f, 0.0f)));
        ECS::get().registerComponents(e);

        Shader partProgram = Shader("partVert.glsl", "partFrag.glsl");

        ParticleEmitter particleEmitter;

        std::vector<std::string> faces =
        {
                "models/skybox/right.jpg",
                "models/skybox/left.jpg",
                "models/skybox/top.jpg",
                "models/skybox/bottom.jpg",
                "models/skybox/front.jpg",
                "models/skybox/back.jpg"
        };
        Skybox* sky = new Skybox(faces);

        Shader skyProgram = Shader("skyVert.glsl", "skyFrag.glsl");

        // TODO: CSM, GUI class, Audio Class. Extra: Normal Mapping

        /* SHADOW MAP (DIRECTIONAL [DEFUNCT]) */

        unsigned int depthMapFBO;
        glGenFramebuffers(1, &depthMapFBO);

        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        unsigned int depthMap;
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

        shadowProgram.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shadowProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        animShadowProgram.Activate();
        glUniformMatrix4fv(glGetUniformLocation(animShadowProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        /* SHADOW MAP (DIRECTIONAL [DEFUNCT]) */

        // SHADOW MAP (POINT)
        Globals::get().depthCubeMap = lampLight->lightShadow->getMap();

    }

};

#endif
