#ifndef ECS_H
#define ECS_H

#include "Entity.h"
#include "Physics.h"
#include <map>

class ECS {
public:
    // defines an class operation that lets clients access its unique instance.
    static ECS& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new ECS;
        return *instance;
    }
    ECS(const ECS&) = delete; // rule of three
    ECS& operator=(const ECS&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    void addEntity(Entity* e);
    void updatePhysics();
    void DrawEntityShadows(float delta);
    void DrawEntityStencils();
    void DrawEntities();
    void deleteEntity(unsigned int ID);
    Entity* getEntity(unsigned int ID);

private:
    ECS() = default; // no public constructor
    ~ECS(); // no public destructor
    static ECS* instance; // declaration class variable

    std::map<int, Entity*> entds2; // change ds to unordered_map
};

#endif