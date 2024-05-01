#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>

#include "ParticleEmitter.h"

class ParticleSystem {
public:
    // defines an class operation that lets clients access its unique instance.
    static ParticleSystem& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new ParticleSystem;
        return *instance;
    }
    ParticleSystem(const ParticleSystem&) = delete; // rule of three
    ParticleSystem& operator=(const ParticleSystem&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    std::vector <ParticleEmitter> emitters;

private:
    ParticleSystem(); // no public constructor
    ~ParticleSystem(); // no public destructor
    static ParticleSystem* instance; // declaration class variable

};

#endif
