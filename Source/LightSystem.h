#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H

#include <vector>

#include "Light.h"

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

    std::vector <Light> lights;

private:
    LightSystem(); // no public constructor
    ~LightSystem(); // no public destructor
    static LightSystem* instance; // declaration class variable

};

#endif
