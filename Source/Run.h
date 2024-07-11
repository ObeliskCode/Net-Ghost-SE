#ifndef RUN_H
#define RUN_H

// #define BUILD_WINDOWS

#ifdef BUILD_WINDOWS
// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // remove console on windows??
#endif

#ifdef BUILD_WINDOWS
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glew32.lib")
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.h"

GLFWwindow *initApp()
{

    glfwInit();

    GLFWwindow *window = glfwCreateWindow(Globals::get().screenWidth, Globals::get().screenHeight, "Obelisk Engine", NULL, NULL); // windowed
    // GLFWwindow* window = glfwCreateWindow(Globals::get().screenWidth, Globals::get().screenHeight, "Obelisk Engine", glfwGetPrimaryMonitor(), NULL); // fullscreen
    if (window == NULL)
    {
        printf("Failed to create GLFW window!\n");
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit())
    {
        printf("Failed to initialize GLEW!.\n");
        return nullptr;
    }

    // refresh rate
    // glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0,0, Globals::get().screenWidth, Globals::get().screenHeight, 120);

    // enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // emable stencil test
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);

    // enable 8x MSAA
    glfwWindowHint(GLFW_SAMPLES, 8);
    glEnable(GL_MULTISAMPLE);

    // default gamma correction
    // glEnable(GL_FRAMEBUFFER_SRGB);

    // enable smooth shading vs flat
    glShadeModel(GL_SMOOTH);

    // Enable Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    // disable vsync if enabled
    glfwSwapInterval(0);

    // enable transparency function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    return window;
}

int run()
{
    GLFWwindow *window = initApp();

    TestRoom *dp = new TestRoom();
    Scene *bp = dp;
    bp->setupCallbacks(window);
    bp->loadResources(window);

    /* loop vars */
    double crntTime = 0.0;

    const double timeStart = glfwGetTime();

    double prevTime = timeStart;
    double timeDiff;
    unsigned int counter = 0;

    double lastFrame = timeStart;
    double deltaTime = 1.0 / 188.0;
    double frameTime = 0.0f;
    double lastTick = timeStart;
    double thisTick = 0.0;
    double delta;

    /* Main Game Loop */
    while (!glfwWindowShouldClose(window))
    {
        crntTime = glfwGetTime();

        /* FPS counter */
        timeDiff = crntTime - prevTime;
        counter++;
        if (timeDiff >= 1.0)
        {
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff / counter) * 1000);
            std::string newTitle = "Obelisk Engine - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(window, newTitle.c_str());
            prevTime = crntTime;
            counter = 0;
        }

        glfwPollEvents(); // get inputs

        frameTime = crntTime - lastFrame;
        lastFrame = crntTime;

        thisTick = glfwGetTime();
        delta = thisTick - lastTick;

        if (delta >= deltaTime)
        {
            lastTick = thisTick;
            bp->tick(window, delta);
        }

        bp->drawFrame(window, frameTime);
    }

    bp->cleanup();

    glfwTerminate();
    return 0;
}

#endif