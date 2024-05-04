#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <vector>

#include "Light.h"
#include "ECS.h"
#include "Globals.h"

class LightSystem {
public:
    // defines an class operation that lets clients access its unique instance.
    static LightSystem& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new LightSystem;
        return *instance;
    }
    LightSystem(const LightSystem&) = delete; // rule of three
    LightSystem& operator=(const LightSystem&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    std::vector <Light*> lights;

    void RenderPointShadows();

private:
    LightSystem() = default; // no public constructor
    ~LightSystem(); // no public destructor
    static LightSystem* instance; // declaration class variable

};

#endif
